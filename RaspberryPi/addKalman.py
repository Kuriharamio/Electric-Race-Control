import cv2
import numpy as np
from collections import deque

# 1) 自适应激光阈值
RED_HSV_LOW  = np.array([150,   0, 170]) 
RED_HSV_HIGH = np.array([ 10,  30, 255])

# 2) 动态亮度阈值
BRIGHT_BASE = 225           # 基准亮度值
BRIGHT_DELTA = 30           # 动态调整范围

# 3) 形态学处理
KERNEL_DILATE = np.ones((5,5), np.uint8)
KERNEL_ERODE = np.ones((3,3), np.uint8)

# 4) 预测算法参数
MAX_PREDICT_FRAMES = 25      # 最大连续预测帧数
MIN_TRACK_POINTS = 20        # 启用预测所需的历史点数量
PREDICT_RADIUS = 30         # 预测点搜索半径
# -------------------- 屏幕检测函数 --------------------
def detect_rects(bgr, debug=False):
    if bgr is None:
        return None

    # 1) histeq（直方图均衡）
    gray = cv2.cvtColor(bgr, cv2.COLOR_BGR2GRAY)
    gray = cv2.equalizeHist(gray)

    # 2) Canny 边缘
    edges = cv2.Canny(gray, 45, 50)

    # 3) dilate -> erode （相当于 openmv 的 dilate(2) & erode(2)）
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3))
    edges = cv2.dilate(edges, kernel, iterations=2)
    edges = cv2.erode(edges, kernel, iterations=2)

    if debug:
        cv2.imshow("edges", edges)

    # 4) 找轮廓 -> 拟合矩形
    h, w = edges.shape
    roi_x, roi_y, roi_w, roi_h = 10, 10, w - 20, h - 20
    roi_edges = edges[roi_y:roi_y + roi_h, roi_x:roi_x + roi_w]

    contours, _ = cv2.findContours(roi_edges,
                                   cv2.RETR_EXTERNAL,
                                   cv2.CHAIN_APPROX_SIMPLE)

    rects = []
    for cnt in contours:
        # 多边形近似，保证是四边形
        epsilon = 0.02 * cv2.arcLength(cnt, True)
        approx = cv2.approxPolyDP(cnt, epsilon, True)
        if len(approx) != 4:
            continue

        # 最小外接矩形（带旋转）
        rect = cv2.minAreaRect(approx)
        box = cv2.boxPoints(rect)
        box = np.int0(box)

        # 转成 axis-aligned 的 x,y,w,h
        x, y, w_bb, h_bb = cv2.boundingRect(box)
        rects.append((x + roi_x, y + roi_y, w_bb, h_bb))

    # 5) 过滤与选最大
    valid = []
    for (x, y, w, h) in rects:
        area = w * h
        if (area > 5000 and
            w > 180 and
            h > 180 and
            abs(w - h) < 80):
            valid.append((x, y, w, h, area))

    if not valid:
        print("no rectangles")
        return None

    x, y, w, h, _ = max(valid, key=lambda r: r[4])
    return (x, y, w, h)

# -------------------- 卡尔曼滤波器 --------------------
class KalmanFilter:
    def __init__(self):
        self.kf = cv2.KalmanFilter(4, 2)
        self.kf.measurementMatrix = np.array([[1,0,0,0],[0,1,0,0]], np.float32)
        self.kf.transitionMatrix = np.array([[1,0,1,0],[0,1,0,1],[0,0,1,0],[0,0,0,1]], np.float32)
        self.kf.processNoiseCov = np.eye(4, dtype=np.float32) * 0.03
        self.kf.measurementNoiseCov = np.eye(2, dtype=np.float32) * 0.1
        self.kf.errorCovPost = np.eye(4, dtype=np.float32)
        self.last_prediction = None
        
    def predict(self, pt):
        if pt is None:
            return None
            
        # 初始化和更新状态
        if self.last_prediction is None:
            self.kf.statePost = np.array([[pt[0]], [pt[1]], [0], [0]], dtype=np.float32)
            self.last_prediction = pt
            return pt
            
        # 预测
        prediction = self.kf.predict()
        self.kf.correct(np.array([[np.float32(pt[0])], [np.float32(pt[1])]]))
        
        pred_pt = (int(prediction[0]), int(prediction[1]))
        self.last_prediction = pred_pt
        return pred_pt

# -------------------- 激光检测 --------------------
def enhanced_laser_detector(bgr, screen_mask=None, prev_point=None, show_mask=True):
    """
    返回激光中心 (x, y)；未检测到返回 None
    """
    hsv = cv2.cvtColor(bgr, cv2.COLOR_BGR2HSV)
    h, s, v = cv2.split(hsv)
    
    # 1. 动态亮度阈值（基于屏幕区域亮度）
    if screen_mask is not None:
        mask_area = cv2.countNonZero(screen_mask)
        if mask_area > 0:
            avg_brightness = cv2.mean(v, mask=screen_mask)[0]
            bright_thresh = max(BRIGHT_BASE, avg_brightness - BRIGHT_DELTA)
        else:
            bright_thresh = BRIGHT_BASE
    else:
        bright_thresh = BRIGHT_BASE
    
    # 2. 亮度通道阈值处理
    _, mask_white = cv2.threshold(v, bright_thresh, 255, cv2.THRESH_BINARY)
    
    # 3. 红色通道处理
    mask_red1 = cv2.inRange(hsv, RED_HSV_LOW, np.array([180, 255, 255]))
    mask_red2 = cv2.inRange(hsv, np.array([0, RED_HSV_LOW[1], RED_HSV_LOW[2]]), RED_HSV_HIGH)
    mask_red = cv2.bitwise_or(mask_red1, mask_red2)
    
    # 4. 组合掩码（亮度+颜色）
    mask_combined = cv2.bitwise_and(mask_red, mask_white)
    
    # 5. 形态学优化
    # mask_combined = cv2.dilate(mask_combined, KERNEL_DILATE, iterations=1)
    # mask_combined = cv2.erode(mask_combined, KERNEL_ERODE, iterations=1)
    
    # 6. 应用屏幕掩码
    if screen_mask is not None:
        mask_combined = cv2.bitwise_and(mask_combined, screen_mask)
    
    # 7. 在预测点附近创建ROI
    if prev_point is not None:
        if len(prev_point) >= 2:
            x, y = prev_point[:2]
            roi_mask = np.zeros_like(mask_combined)
            cv2.circle(roi_mask, (x, y), PREDICT_RADIUS, 255, -1)
            mask_combined = cv2.bitwise_and(mask_combined, roi_mask)
    
    if show_mask:
        cv2.imshow("debug_mask", mask_combined)
    
    # 8. 轮廓分析
    contours, _ = cv2.findContours(mask_combined, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    best_pt = None
    best_score = -1
    
    for cnt in contours:
        area = cv2.contourArea(cnt)
        if area < 0 or area > 200:  # 合理的激光点尺寸范围
            continue
            
        # 圆形度检测
        perimeter = cv2.arcLength(cnt, True)
        if perimeter == 0:
            continue
        circularity = 4 * np.pi * area / (perimeter ** 2)
        
        # 尺寸稳定性（与历史点比较）
        size_stability = 1.0
        # 检查prev_point是否有面积信息
        if prev_point is not None and len(prev_point) >= 3:
            prev_area = prev_point[2]
            if prev_area > 0:
                size_stability = min(area, prev_area) / max(area, prev_area)
        
        # 综合评分
        score = circularity * size_stability
        
        if score > best_score:
            best_score = score
            M = cv2.moments(cnt)
            if M["m00"] != 0:
                cx = int(M["m10"] / M["m00"])
                cy = int(M["m01"] / M["m00"])
                best_pt = (cx, cy, area)
    
    return best_pt

# -------------------- 主程序 --------------------
def main():
    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        raise RuntimeError("error")
    
    # 初始化跟踪状态
    screen_region, screen_region_plus, screen_mask = None, None, None
    track_points = deque(maxlen=10)  
    kf = KalmanFilter()              
    predict_count = 0                
    
    print("按 q 退出")
    while True:
        ret, frame = cap.read()
        if not ret:
            break

        if screen_mask is None:
            # 屏幕检测
            rect = detect_rects(frame, debug=False)  
            if rect:
                x, y, w, h = rect
                cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
                screen_region = np.array([
                    [x,         y],
                    [x + w,     y],
                    [x + w, y + h],
                    [x,     y + h]
                ], dtype=np.int32)
                center = screen_region.mean(axis=0)
                screen_region_plus = ( (screen_region - center) * 1.2 + center ).astype(np.int32)
                screen_region_plus[:, 0] = np.clip(screen_region_plus[:, 0], 0, frame.shape[1] - 1)
                screen_region_plus[:, 1] = np.clip(screen_region_plus[:, 1], 0, frame.shape[0] - 1)
                screen_mask = np.zeros(frame.shape[:2], dtype=np.uint8)
                cv2.fillPoly(screen_mask, [screen_region], 255)
        else:
            # 激光点检测
            prev_point = track_points[-1] if track_points else None
            prev_coord = prev_point[:2] if prev_point else None
            pt_data = enhanced_laser_detector(
                frame, 
                screen_mask, 
                prev_point=prev_point,
                show_mask=True
            )
            
            # 处理检测结果
            if pt_data:
                # 检测到激光点
                cx, cy, area = pt_data
                pt = (cx, cy)
                track_points.append((cx, cy, area))
                predict_count = 0
                
                # 更新卡尔曼滤波器
                predicted_pt = kf.predict(pt)
            else:
                # 未检测到激光点，尝试使用预测
                predict_count += 1
                
                if predict_count <= MAX_PREDICT_FRAMES and len(track_points) >= MIN_TRACK_POINTS:
                    # 使用卡尔曼滤波预测
                    if kf.last_prediction:
                        pt = kf.last_prediction
                    else:
                        # 使用最后一个已知点
                        pt = track_points[-1][:2]
                    predicted_pt = pt
                else:
                    pt = None
                    predicted_pt = None
                
            # 绘制激光点
            if pt:
                cv2.circle(frame, pt, 3, (0, 255, 0), -1)
                
                if predicted_pt and predict_count > 0:
                    cv2.circle(frame, predicted_pt, 3, (0, 255, 0), -1)
                    cv2.putText(frame, "Predicted", 
                            (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 255), 2)
                
                cv2.putText(frame, f"{pt[0]:.0f},{pt[1]:.0f}", 
                        (pt[0] + 10, pt[1] - 10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)
                cv2.putText(frame, "Detected", 
                        (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)
            elif predicted_pt:
                cv2.circle(frame, predicted_pt, 3, (0, 255, 0), -1)
                cv2.putText(frame, f"{predicted_pt[0]:.0f},{predicted_pt[1]:.0f}", 
                        (predicted_pt[0] + 10, predicted_pt[1] - 10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 255), 2)
                cv2.putText(frame, "Predicted", 
                        (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 255), 2)
        
        # 绘制屏幕区域
        if screen_region is not None:
            cv2.polylines(frame, [screen_region], True, (0, 255, 255), 2)

        if screen_region_plus is not None:
            cv2.polylines(frame, [screen_region_plus], True, (255, 0, 255), 2)
        

        
        # 绘制历史轨迹
        for i in range(1, len(track_points)):
            pt1 = track_points[i-1][:2]
            pt2 = track_points[i][:2]
            cv2.line(frame, pt1, pt2, (0, 200, 0), 2)
        
        cv2.imshow("Enhanced Laser Detection", frame)
        if cv2.waitKey(50) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()