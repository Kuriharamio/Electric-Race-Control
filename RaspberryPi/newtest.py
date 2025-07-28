import cv2
import numpy as np

# -------------------- 可调参数 --------------------
# 1) 激光颜色HSV阈值
RED_HSV_LOW  = np.array([150,   0, 170])
RED_HSV_HIGH = np.array([ 10,  30, 255])

BLACK_HSV_LOW  = np.array([40, 30,  0])
BLACK_HSV_HIGH = np.array([60, 60, 30])


# 2) 激光中心亮度阈值
BRIGHT_LOW  = 230          # V 通道下限，几乎纯白
BRIGHT_HIGH = 255

# 3) 面积/圆度过滤
MIN_AREA = -1               # 激光点像素面积
MAX_AREA = 100 
CIRCULARITY_TH = 0.0       # 圆度 = 4πA/P²，接近 1 表示圆

# 4) 形态学核
KERNEL = np.ones((3, 3), np.uint8)

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
# -------------------- 激光检测函数 --------------------
def laser_detector(bgr, screen_mask=None,  show_mask=True):
    """
    返回激光中心 (x, y)；未检测到返回 None
    """
    hsv = cv2.cvtColor(bgr, cv2.COLOR_BGR2HSV)

    # 1) 提取"过曝白核"
    _, _, v = cv2.split(hsv)
    mask_white = cv2.inRange(v, BRIGHT_LOW, BRIGHT_HIGH)

    # 2) 先做白核膨胀，再与红晕交集
    mask_white_dil = cv2.dilate(mask_white, KERNEL, iterations=2)

    # 3) 提取"极浅红晕"
    mask_red1 = cv2.inRange(hsv, RED_HSV_LOW, np.array([180, 255, 255]))
    mask_red2 = cv2.inRange(hsv, np.array([0, RED_HSV_LOW[1], RED_HSV_LOW[2]]), RED_HSV_HIGH)
    mask_red = cv2.bitwise_or(mask_red1, mask_red2)

    # 4) 与红晕交集
    mask_candidate = cv2.bitwise_and(mask_red, mask_white_dil)

    if cv2.countNonZero(mask_candidate) == 0:
        mask_candidate = mask_white_dil
    
    # 应用屏幕掩码（如果提供）
    if screen_mask is not None:
        mask_candidate = cv2.bitwise_and(mask_candidate, screen_mask)

    if show_mask:
        cv2.imshow("debug_mask", mask_candidate)

    contours, _ = cv2.findContours(mask_candidate, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    best = None
    best_score = 0
    for cnt in contours:
        area = cv2.contourArea(cnt)
        if area > best_score:
            best_score = area
            best = cnt

    if best is None:
        return None

    M = cv2.moments(best)
    cx = int(M["m10"] / M["m00"])
    cy = int(M["m01"] / M["m00"])
    
    return cx, cy

# -------------------- 主程序 --------------------
def main():
    cap = cv2.VideoCapture("D:\\Tool\\ti\\mspm0_sdk_1_30_00_03\\examples\\nortos\\LP_MSPM0G3507\\driverlib\\7_PTZ\\output.avi")
    if not cap.isOpened():
        raise RuntimeError("无法打开视频")
    
    screen_region, screen_region_plus, screen_mask = None, None, None
    
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
            # 检测激光点
            pt = laser_detector(frame, screen_mask, show_mask=True)
            # 处理检测结果
            if pt:
                color = (0, 255, 0) if pt else (0, 0, 255)
                cv2.circle(frame, (int(pt[0]), int(pt[1])), 4, color, -1)
                cv2.putText(frame, f"{pt[0]:.0f},{pt[1]:.0f}", 
                            (int(pt[0]) + 8, int(pt[1]) - 8),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.6, color, 2)
                cv2.putText(frame, "Detected" if pt else "Predicted", 
                            (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, color, 2)
                
        # 绘制屏幕区域
        if screen_region is not None:
            cv2.polylines(frame, [screen_region], True, (0, 255, 255), 2)

        if screen_region_plus is not None:
            cv2.polylines(frame, [screen_region_plus], True, (255, 0, 255), 2)

        cv2.imshow("Red Laser Dot Detection", frame)
        if cv2.waitKey(50) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()