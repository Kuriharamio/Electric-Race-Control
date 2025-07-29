# ======================
# 导入必要模块
# ======================
from collections import deque

import cv2
import numpy as np
import time
import datapack
import points_data
import Kalman
import handle_points
# ======================
# 设定的常量区
# ======================
img_w = 640  # 图像宽度
img_h = 480  # 图像高度
kernel_3 = np.ones((3, 3), np.uint8)
kernel_5 = np.ones((5, 5), np.uint8)

red_low = np.array([150, 0, 200])
red_high = np.array([10, 30, 255])

# 1) 自适应激光阈值 - 红色激光HSV颜色范围
RED_HSV_LOW = np.array([150, 0, 100])
RED_HSV_HIGH = np.array([10, 30, 255])

# 2) 动态亮度阈值
BRIGHT_BASE = 225  # 基准亮度值
BRIGHT_DELTA = 30  # 动态调整范围

# 3) 形态学处理
KERNEL_DILATE = np.ones((5, 5), np.uint8)
KERNEL_ERODE = np.ones((3, 3), np.uint8)

# 4) 预测算法参数
MAX_PREDICT_FRAMES = 25  # 最大连续预测帧数
MIN_TRACK_POINTS = 20  # 启用预测所需的历史点数量
PREDICT_RADIUS = 40  # 预测点搜索半径


# ======================
# 类区
# ======================

# ======================
# 变量区
# ======================
flag_1 = False

flag_state_1 = True
flag_state_2 = True
flag_state_3 = True
flag_state_4 = True
# 黑色框的内外框
blobs_black = None
blobs_black_index = 0
len_blobs_black = 0
# 最大的轮廓
blobs_max = None
blobs_max_index = 0
len_blobs_max = 0

middle_red = None

state = 2


screen_region, screen_region_plus, screen_mask = None, None, None
track_points = deque(maxlen=10)  # 跟踪点历史记录（最多10个点）
kf = Kalman.KalmanFilter()  # 卡尔曼滤波器实例
predict_count = 0  # 连续预测帧计数

best_approx = None

# 添加用于存储激光路径的列表
laser_path = []  # 存储完整的激光路径
path_file_name = "laser_path.txt"  # 路径文件名

# 添加用于循迹的变量
trace_points = []  # 存储从文件读取的路径点
trace_index = 0    # 当前循迹点索引
trace_target = None  # 当前目标点

# ======================
#  函数区域
# ======================

# -------------------- 激光检测 --------------------
def enhanced_laser_detector(bgr, screen_mask=None, prev_point=None, show_mask=True):
    """
    增强型激光点检测函数
    参数:
        bgr: BGR格式的输入图像
        screen_mask: 屏幕区域掩码，限制检测范围
        prev_point: 上一帧检测到的点 (x, y, area)
        show_mask: 是否显示调试掩码
    返回:
        激光中心 (x, y, area)；未检测到返回 None
    """
    hsv = cv2.cvtColor(bgr, cv2.COLOR_BGR2HSV)
    h, s, v = cv2.split(hsv)

    # 1. 动态亮度阈值（基于屏幕区域亮度）
    if screen_mask is not None:
        mask_area = cv2.countNonZero(screen_mask)
        if mask_area > 0:
            # 计算屏幕区域的平均亮度
            avg_brightness = cv2.mean(v, mask=screen_mask)[0]
            # 根据平均亮度动态调整阈值
            bright_thresh = max(BRIGHT_BASE, avg_brightness - BRIGHT_DELTA)
        else:
            bright_thresh = BRIGHT_BASE
    else:
        bright_thresh = BRIGHT_BASE

    # 2. 亮度通道阈值处理 - 提取高亮区域
    _, mask_white = cv2.threshold(v, bright_thresh, 255, cv2.THRESH_BINARY)

    # 3. 红色通道处理 - 提取红色区域
    # 处理红色在HSV色彩空间中的环形特性（H范围0-180）
    mask_red1 = cv2.inRange(hsv, RED_HSV_LOW, np.array([180, 255, 255]))
    mask_red2 = cv2.inRange(hsv, np.array([0, RED_HSV_LOW[1], RED_HSV_LOW[2]]), RED_HSV_HIGH)
    mask_red = cv2.bitwise_or(mask_red1, mask_red2)

    # 4. 组合掩码（亮度+颜色）- 同时满足亮度和颜色条件
    mask_combined = cv2.bitwise_and(mask_red, mask_white)

    # 5. 形态学优化（当前被注释掉）
    # mask_combined = cv2.dilate(mask_combined, KERNEL_DILATE, iterations=1)
    # mask_combined = cv2.erode(mask_combined, KERNEL_ERODE, iterations=1)

    # 6. 应用屏幕掩码 - 限制检测区域在屏幕内
    if screen_mask is not None:
        mask_combined = cv2.bitwise_and(mask_combined, screen_mask)

    # 7. 在预测点附近创建ROI - 限制搜索范围提高准确性
    if prev_point is not None:
        if len(prev_point) >= 2:
            x, y = prev_point[:2]
            roi_mask = np.zeros_like(mask_combined)
            cv2.circle(roi_mask, (x, y), PREDICT_RADIUS, 255, -1)
            mask_combined = cv2.bitwise_and(mask_combined, roi_mask)

    if show_mask:
        cv2.imshow("debug_mask", mask_combined)

    # 查找所有白色像素的坐标
    white_pixels_coords = np.where(mask_combined == 255)

    if len(white_pixels_coords[0]) < 1 or len(white_pixels_coords[0]) > 500:
        return None

    # 获取这些像素在亮度通道中的值作为权重
    weights = v[white_pixels_coords]

    # 计算加权质心
    total_weight = np.sum(weights)

    if total_weight == 0:
        return None

        # 如果没有先前点信息，优先选择距离图像中心最近的点
    if prev_point is None:
        # 计算图像中心
        img_center_x = bgr.shape[1] // 2
        img_center_y = bgr.shape[0] // 2

        # 计算每个白点到图像中心的距离
        distances_to_center = np.sqrt(
            (white_pixels_coords[1] - img_center_x) ** 2 +
            (white_pixels_coords[0] - img_center_y) ** 2
        )

        # 找到距离中心最近的点
        closest_point_idx = np.argmin(distances_to_center)

        # 返回最近点的坐标
        cx = int(white_pixels_coords[1][closest_point_idx])
        cy = int(white_pixels_coords[0][closest_point_idx])
    else:
        # 有先前点信息时，使用加权平均计算质心
        cx = int(np.sum(white_pixels_coords[1] * weights) / total_weight)
        cy = int(np.sum(white_pixels_coords[0] * weights) / total_weight)

    return (cx, cy, float(total_weight / len(weights)))  # 返回平均亮度作为强度指标

    # ... existing code ...

    # # 8. 轮廓分析 - 查找并分析可能的激光点
    # contours, _ = cv2.findContours(mask_combined, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    # best_pt = None
    # best_score = -1
    #
    # for cnt in contours:
    #     area = cv2.contourArea(cnt)
    #     # 过滤不合理尺寸的轮廓（激光点应该很小）
    #     if area < 0 or area > 200:  # 合理的激光点尺寸范围
    #         continue
    #
    #     # 圆形度检测 - 激光点应该接近圆形
    #     perimeter = cv2.arcLength(cnt, True)
    #     if perimeter == 0:
    #         continue
    #     circularity = 4 * np.pi * area / (perimeter ** 2)
    #
    #     # 尺寸稳定性（与历史点比较）
    #     size_stability = 1.0
    #     # 检查prev_point是否有面积信息（确保prev_point存在且至少有3个元素）
    #     if prev_point is not None and len(prev_point) >= 3:
    #         prev_area = prev_point[2]  # 获取上一帧检测点的面积
    #         if prev_area > 0:  # 确保上一帧面积大于0（有效）
    #             # 计算当前面积与上一帧面积的相似度
    #             # 相似度 = min(当前面积, 上一帧面积) / max(当前面积, 上一帧面积)
    #             # 值越接近1表示面积变化越小，越稳定
    #             size_stability = min(area, prev_area) / max(area, prev_area)
    #
    #     # 综合评分 - 结合圆形度和尺寸稳定性
    #     score = circularity * size_stability
    #     # 如果当前轮廓得分高于之前最佳得分
    #     if score > best_score:
    #         best_score = score  # 更新最佳得分
    #         # 计算轮廓的矩，用于求取轮廓的质心
    #         M = cv2.moments(cnt)
    #         # 确保矩的有效性（m00不为0）
    #         if M["m00"] != 0:
    #             # 计算质心坐标
    #             cx = int(M["m10"] / M["m00"])  # X坐标
    #             cy = int(M["m01"] / M["m00"])  # Y坐标
    #             # 保存最佳点信息（坐标和面积）
    #             best_pt = (cx, cy, area)
    #
    # return best_pt


def detect_rects(bgr, debug=False):
    """
    检测图像中的屏幕区域（矩形区域）
    参数:
        bgr: BGR格式的输入图像
        debug: 是否显示调试信息
    返回:
        屏幕区域的边界框 (x, y, w, h)，如果未检测到则返回None
    """
    if bgr is None:
        return None

    # 1) histeq（直方图均衡）- 增强图像对比度
    gray = cv2.cvtColor(bgr, cv2.COLOR_BGR2GRAY)
    gray = cv2.equalizeHist(gray)

    # 2) Canny 边缘检测 - 提取图像边缘
    edges = cv2.Canny(gray, 50, 255)

    # 3) dilate -> erode （相当于 openmv 的 dilate(2) & erode(2)）
    # 形态学操作，先膨胀后腐蚀，连接断开的边缘
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3))
    edges = cv2.dilate(edges, kernel_5, iterations=1)
    edges = cv2.erode(edges, kernel_3, iterations=1)

    if debug:
        cv2.imshow("edges", edges)

    # 4) 找轮廓 -> 拟合矩形
    # 在ROI区域内查找轮廓
    h, w = edges.shape
    roi_x, roi_y, roi_w, roi_h = 10, 10, w - 20, h - 20
    roi_edges = edges[roi_y:roi_y + roi_h, roi_x:roi_x + roi_w]

    contours, _ = cv2.findContours(roi_edges,
                                   cv2.RETR_EXTERNAL,
                                   cv2.CHAIN_APPROX_SIMPLE)
    rects = []
    approx_contours = []  # 存储对应的近似轮廓

    for cnt in contours:
        # 多边形近似，保证是四边形
        epsilon = 0.01 * cv2.arcLength(cnt, True)
        approx = cv2.approxPolyDP(cnt, epsilon, True)
        if len(approx) != 4:
            continue

        # 将approx坐标从ROI坐标系转换到原始图像坐标系
        approx_global = approx.copy()
        approx_global[:, :, 0] += roi_x
        approx_global[:, :, 1] += roi_y

        # 直接使用approx计算axis-aligned边界框
        x, y, w_bb, h_bb = cv2.boundingRect(approx_global)
        rects.append((x, y, w_bb, h_bb))
        approx_contours.append(approx_global)  # 保存对应的轮廓

    # 5) 过滤与选最大
    # 根据面积、尺寸等条件过滤候选矩形，选择最大的一个
    valid = []
    valid_contours = []  # 存储对应的轮廓
    for i, (x, y, w, h) in enumerate(rects):
        area = w * h
        if (area > 5000 and
                w > 180 and
                h > 180 and
                abs(w - h) < 80):
            valid.append((x, y, w, h, area))
            valid_contours.append(approx_contours[i])  # 保存对应的轮廓

    if not valid:
        print("no rectangles")
        return None, None

    # 找到面积最大的矩形及其对应的轮廓
    max_index = max(range(len(valid)), key=lambda i: valid[i][4])
    x, y, w, h, _ = valid[max_index]
    best_approx = valid_contours[max_index]  # 获取对应的轮廓

    best_points = best_approx.reshape(4, 2)

    #确定插值的次数
    num_interpolations = 4

    # 初始点集
    current_points = best_points.copy()

    # 进行多次插值
    for _ in range(num_interpolations):
        # 插值操作
        interpolated = handle_points.interpolate_points(current_points)
        # 合并原始点和插入的点
        merged_points = []
        for i in range(len(current_points)):
            merged_points.append(current_points[i])
            merged_points.append(interpolated[i])
        current_points = merged_points
    current_points.append(current_points[0])


    # 修改返回格式为OpenCV轮廓格式 (16, 1, 2)
    contour = np.array(current_points, dtype=np.int32)
    contour = contour.reshape((len(contour), 1, 2))

    # 调试时绘制近似轮廓
    if debug:
        cv2.drawContours(bgr, [contour], -1, (0, 0, 255), 2)
        cv2.rectangle(bgr, (x, y), (x + w, y + h), (0, 255, 0), 2)
        cv2.imshow("bgr", bgr)
    return (x, y, w, h), contour



def find_binary_approx_polygon(img, n_points, min_area=10, max_area=20000, epsilon_factor=0.02, show=False):
    """
    找到图像中指定二值化后，且近似为n边形的轮廓
    input:
    img: 灰度图像
    n_points: 多边形顶点数
    min_area: 最小面积阈值，默认为10像素
    epsilon_factor: 近似精度因子，默认为0.02

    output:
    polygons: 所有符合的近似多边形轮廓
    """
    # 二值化
    img = cv2.equalizeHist(img)
    ret, img_binary = cv2.threshold(img, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)

    # 后处理 处理黑框中白色噪声
    img_binary = cv2.morphologyEx(img_binary, cv2.MORPH_CLOSE, kernel_3)

    if show:
        cv2.imshow("img_binary", img_binary)

    # 找轮廓
    contours, hierarchy = cv2.findContours(img_binary, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
    polygons = []
    for contour in contours:
        # 根据最小面积过滤轮廓
        if cv2.contourArea(contour) >= min_area and cv2.contourArea(contour) <= max_area:
            # 计算轮廓周长
            perimeter = cv2.arcLength(contour, True)
            # 设置近似精度
            epsilon = epsilon_factor * perimeter
            # 近似多边形
            approx = cv2.approxPolyDP(contour, epsilon, True)

            # 检查近似多边形是否具有指定的顶点数
            if len(approx) == n_points:
                polygons.append(approx)

    return polygons


def check_contour_containment(contours):
    """
    判断两个四边形轮廓是否包含关系（较大轮廓包含较小轮廓）

    参数:
        contours: 包含两个四边形轮廓的列表，每个轮廓是4个点的数组

    返回:
        bool: 如果较小轮廓完全在较大轮廓内则返回True，否则返回False
    """
    # 确保输入是两个轮廓
    if len(contours) != 2:
        return False

    large_contour = contours[0].reshape(-1, 2).astype(np.float32)  # 较大轮廓的4个点
    small_contour = contours[1].reshape(-1, 2)  # 较小轮廓的4个点

    # 检查较小轮廓的所有点是否都在较大轮廓内或边界上
    for i in range(len(small_contour)):
        # 获取点坐标并转换为元组
        x, y = small_contour[i]
        pt = (float(x), float(y))

        # 检测点是否在轮廓内
        #   +1 -> 内部
        #   -1 -> 外部
        #   0  -> 边界
        if cv2.pointPolygonTest(large_contour, pt, False) < 0:
            return False

    return True



# ======================
# 主函数
# ======================


# ======================
# 初始化
# ======================

# 打开摄像头
cap = cv2.VideoCapture(0)

# 打开串口
ser = datapack.init_serial()

# 跳过前几帧并添加延时，让摄像头自动曝光稳定
print("等待摄像头稳定...")
for i in range(15):
    ret, frame = cap.read()
    if not ret:
        print("无法读取帧")
        break
    time.sleep(0.05)  # 50ms延时
print("摄像头已稳定")
# ======================
# 主循环
# ======================

while True:

    ret, frame = cap.read()

    # time======================
    time_start = time.time()

    if not ret:
        break

    # ======================
    # 统一的图像处理
    # ======================
    # 灰度图
    img_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # ======================
    # 只执行一次的代码
    # ======================
    if screen_mask is None:
        # 屏幕检测阶段
        rect, blobs_max = detect_rects(frame, debug=False)
        # 画出轮廓
        cv2.drawContours(frame, [blobs_max], -1, (0, 255, 0), 2)

        #cv2.imshow("result2", frame)
        if rect:
            x, y, w, h = rect
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
            # 定义屏幕区域的四个顶点
            screen_region = np.array([
                [x, y],
                [x + w, y],
                [x + w, y + h],
                [x, y + h]
            ], dtype=np.int32)
            # 创建一个稍微扩大一点的屏幕区域用于检测
            center = screen_region.mean(axis=0)
            screen_region_plus = ((screen_region - center) * 1.2 + center).astype(np.int32)
            screen_region_plus[:, 0] = np.clip(screen_region_plus[:, 0], 0, frame.shape[1] - 1)
            screen_region_plus[:, 1] = np.clip(screen_region_plus[:, 1], 0, frame.shape[0] - 1)
            # 创建屏幕区域掩码
            screen_mask = np.zeros(frame.shape[:2], dtype=np.uint8)
            cv2.fillPoly(screen_mask, [screen_region_plus], 255)

    # ======================
    # 获取状态
    # ======================
    #如果不是0就赋值
    # new_state = datapack.receive_state(ser)
    # if new_state:
    #     state = new_state
    #     print(state)

    #读取数据
 
    rec_data = datapack.receive_data(ser)
    print(ser.in_waiting)
    print(rec_data)
  #  if rec_data:
 #       print(rec_data)
    # ======================
    # 状态1逻辑
    # ======================
    if state == 1:
        if blobs_max is not None:

            len_blobs_max = len(blobs_max)
            # 激光点检测阶段 - 当屏幕区域已经检测到后，开始检测激光点
            # 获取上一帧检测到的点作为参考点
            prev_point = track_points[-1] if track_points else None
            # 从prev_point中提取坐标信息(x, y)
            prev_coord = prev_point[:2] if prev_point else None
            # 调用增强型激光检测函数检测当前帧中的激光点
            pt_data = enhanced_laser_detector(
                frame,  # 当前帧图像
                screen_mask,  # 屏幕区域掩码，限制检测范围
                prev_point=prev_point,  # 上一帧检测到的点，用于预测和ROI优化
                show_mask=False  # 显示调试掩码窗口
            )

            # 处理检测结果
            if pt_data:
                # 检测到激光点
                cx, cy, area = pt_data
                pt = (cx, cy)
                track_points.append((cx, cy, area))
                predict_count = 0  # 重置预测计数

                # 更新卡尔曼滤波器
                predicted_pt = kf.predict(pt)
            else:
                # 未检测到激光点，尝试使用预测
                predict_count += 1

                # 检查是否满足预测条件：
                # 1. 连续预测帧数不超过最大允许值（25帧）
                # 2. 历史跟踪点数量达到最小要求（20个点）
                if predict_count <= MAX_PREDICT_FRAMES and len(track_points) >= MIN_TRACK_POINTS:
                    # 使用卡尔曼滤波预测激光点位置
                    # 如果卡尔曼滤波器有上一次的预测结果，则使用该结果

                    if kf.last_prediction:
                        pt = kf.last_prediction
                    else:
                        # 使用最后一个已知点
                        pt = track_points[-1][:2]
                    predicted_pt = pt
                else:
                    pt = None
                    predicted_pt = None

            # 画pt
            if pt:
                # 计算和blobs_black的差值
                if blobs_max_index < len_blobs_max:
                    error_red = np.array(pt) - blobs_max[blobs_max_index][0]
                    print(error_red)

                    # 发送数据给舵机控制

                    datapack.send_packet(ser, [error_red[0], -error_red[1]])
                else:
                    error_red = np.array(pt) - blobs_max[0][0]
                    print(error_red)

                    # 发送数据给舵机控制
                    datapack.send_packet(ser, [0, 0])

                # 如果误差值绝对值小于阈值，则认为已经到达目标
                if error_red[0] ** 2 + error_red[1] ** 2 < 40 and blobs_max_index < len_blobs_max:
                    blobs_max_index = (blobs_max_index + 1)

                cv2.circle(frame, pt, 3, (0, 255, 0), -1)

        # ======================
        # 只执行一次的代码
        # ======================
        # if flag_state_1:
        #     # 这里可以放一些只需要执行一次的初始化代码

        #     flag_state_1 = False
    # ======================
    # 状态2逻辑
    # ======================
    elif state == 2:

        # ======================
        # 只执行一次的代码
        # ======================
        if flag_state_2:
            # 这里可以放一些只需要执行一次的初始化代码
            contours = find_binary_approx_polygon(img_gray, 4, min_area=100, epsilon_factor=0.015, show=False)
            # 排序找到面积最大的两个框
            if len(contours) >= 2:
                contours = sorted(contours, key=lambda x: cv2.contourArea(x), reverse=True)[:2]

                # 判断是否包含
                if check_contour_containment(contours):
                    # 保存contours，完成任务
                    blobs_black = handle_points.find_center_contour_clockwise(contours[0], contours[1])

                    flag_state_2 = False

                    # 画图
                    cv2.drawContours(frame, contours, -1, (0, 255, 0), 2)
                    cv2.drawContours(frame, [blobs_black], -1, (255, 0, 255), 2)
                    #cv2.imshow("result", frame)

        # ======================
        # 总是执行的代码
        # ======================

        if blobs_black is not None and screen_mask is not None:
            len_blobs_black = len(blobs_black)
            # print(len(blobs_black))

            # 激光点检测阶段 - 当屏幕区域已经检测到后，开始检测激光点
            # 获取上一帧检测到的点作为参考点
            prev_point = track_points[-1] if track_points else None
            # 从prev_point中提取坐标信息(x, y)
            prev_coord = prev_point[:2] if prev_point else None
            # 调用增强型激光检测函数检测当前帧中的激光点
            pt_data = enhanced_laser_detector(
                frame,  # 当前帧图像
                screen_mask,  # 屏幕区域掩码，限制检测范围
                prev_point=prev_point,  # 上一帧检测到的点，用于预测和ROI优化
                show_mask=False# 显示调试掩码窗口
            )

            # 处理检测结果
            if pt_data:
                # 检测到激光点
                cx, cy, area = pt_data
                pt = (cx, cy)
                track_points.append((cx, cy, area))
                predict_count = 0  # 重置预测计数

                # 更新卡尔曼滤波器
                predicted_pt = kf.predict(pt)
            else:
                # 未检测到激光点，尝试使用预测
                predict_count += 1

                # 检查是否满足预测条件：
                # 1. 连续预测帧数不超过最大允许值（25帧）
                # 2. 历史跟踪点数量达到最小要求（20个点）
                if predict_count <= MAX_PREDICT_FRAMES and len(track_points) >= MIN_TRACK_POINTS:
                    # 使用卡尔曼滤波预测激光点位置
                    # 如果卡尔曼滤波器有上一次的预测结果，则使用该结果

                    if kf.last_prediction:
                        pt = kf.last_prediction
                    else:
                        # 使用最后一个已知点
                        pt = track_points[-1][:2]
                    predicted_pt = pt
                else:
                    pt = None
                    predicted_pt = None

            # 画pt
            if pt:
                # 计算和blobs_black的差值
                if blobs_black_index < len_blobs_black:
                    error_red = np.array(pt) - blobs_black[blobs_black_index][0]
                    print(error_red)

                    # 发送数据给舵机控制
                    datapack.send_packet(ser, [error_red[0], -error_red[1]])
                else:
                    error_red = np.array(pt) - blobs_black[0][0]
                    print(error_red)

                    # 发送数据给舵机控制
                    datapack.send_packet(ser, [0, 0])

                # 如果误差值绝对值小于阈值，则认为已经到达目标
                if error_red[0] ** 2 + error_red[1] ** 2 < 40 and blobs_black_index < len_blobs_black:
                    blobs_black_index = (blobs_black_index + 1)

                cv2.circle(frame, pt, 3, (0, 255, 0), -1)
    # ======================
    # 状态3逻辑
    # ======================
    elif state == 3:


        # ======================
        # 只执行一次的代码
        # ======================
        if flag_state_3:
            print("开始记录激光路径...")
            laser_path = []  # 清空之前的路径记录
            flag_state_3 = False


        # ======================
        # 总是执行的代码
        # ======================
        if screen_mask is not None:
            # 激光点检测阶段 - 当屏幕区域已经检测到后，开始检测激光点
            # 获取上一帧检测到的点作为参考点
            prev_point = track_points[-1] if track_points else None
            # 从prev_point中提取坐标信息(x, y)
            prev_coord = prev_point[:2] if prev_point else None
            # 调用增强型激光检测函数检测当前帧中的激光点
            pt_data = enhanced_laser_detector(
                frame,  # 当前帧图像
                screen_mask,  # 屏幕区域掩码，限制检测范围
                prev_point=prev_point,  # 上一帧检测到的点，用于预测和ROI优化
                show_mask=True  # 显示调试掩码窗口
            )

            # 处理检测结果
            if pt_data:
                # 检测到激光点
                cx, cy, area = pt_data
                pt = (cx, cy)
                track_points.append((cx, cy, area))
                predict_count = 0  # 重置预测计数
                # 更新卡尔曼滤波器
                predicted_pt = kf.predict(pt)
                # 在图像上绘制检测到的点
                cv2.circle(frame, pt, 3, (0, 0, 255), -1)  # 红色点表示检测到的激光点

            else:
                # 未检测到激光点，尝试使用预测
                predict_count += 1

                # 检查是否满足预测条件：
                # 1. 连续预测帧数不超过最大允许值（25帧）
                # 2. 历史跟踪点数量达到最小要求（20个点）
                if predict_count <= MAX_PREDICT_FRAMES and len(track_points) >= MIN_TRACK_POINTS:
                    # 使用卡尔曼滤波预测激光点位置
                    # 如果卡尔曼滤波器有上一次的预测结果，则使用该结果

                    if kf.last_prediction:
                        pt = kf.last_prediction
                    else:
                        # 使用最后一个已知点
                        pt = track_points[-1][:2]
                    predicted_pt = pt
                else:
                    pt = None
                    predicted_pt = None
            #pt存在就加入到路径中
            if pt:
                laser_path.append(pt)
            # 如果路径中至少有两个点，绘制路径线
            if len(laser_path) > 1:
                for i in range(1, len(laser_path)):
                    cv2.line(frame, laser_path[i - 1], laser_path[i], (0, 255, 255), 1)

            # 显示当前已记录的路径点数量
            cv2.putText(frame, f"Path Points: {len(laser_path)}", (10, 30),
                        cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)

    elif state == 4:
        # ======================
        # 只执行一次的代码
        # ======================
        if flag_state_4:
            print("开始循迹模式...")
            # 加载循迹路径点
            trace_points = points_data.load_path_data("optimized_laser_path.txt")
            if len(trace_points) > 0:
                trace_index = 0
                trace_target = trace_points[trace_index]
                print(f"开始循迹，共 {len(trace_points)} 个点，目标点: {trace_target}")
            else:
                print("未加载到循迹点，无法开始循迹")
            flag_state_4 = False
        # ======================
        # 总是执行的代码
        # ======================
        if screen_mask is not None and len(trace_points) > 0:
            # 激光点检测阶段 - 检测当前激光点位置
            prev_point = track_points[-1] if track_points else None
            pt_data = enhanced_laser_detector(
                frame,
                screen_mask,
                prev_point=prev_point,
                show_mask=False
            )

            current_point = None
            if pt_data:
                # 检测到激光点
                cx, cy, area = pt_data
                current_point = (cx, cy)
                track_points.append((cx, cy, area))
                predict_count = 0
                # 更新卡尔曼滤波器
                predicted_pt = kf.predict(current_point)
                # 在图像上绘制当前激光点
                cv2.circle(frame, current_point, 3, (0, 0, 255), -1)
            else:
                # 未检测到激光点，尝试使用预测
                predict_count += 1
                if predict_count <= MAX_PREDICT_FRAMES and len(track_points) >= MIN_TRACK_POINTS:
                    if kf.last_prediction:
                        current_point = kf.last_prediction
                    else:
                        current_point = track_points[-1][:2] if track_points else None
                    # 在图像上绘制预测点
                    if current_point:
                        cv2.circle(frame, current_point, 3, (255, 0, 0), -1)
                else:
                    current_point = None

            # 如果有当前点和目标点，则计算误差并控制舵机
            if current_point and trace_target is not None:
                # 计算当前位置与目标点的误差
                error_x = trace_target[0] - current_point[0]
                error_y = trace_target[1] - current_point[1]
                error = np.array([error_x, error_y])

                # 显示当前目标点和误差
                cv2.putText(frame, f"Target: {trace_target}", (10, 30),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 1)
                cv2.putText(frame, f"Error: ({error_x:.1f}, {error_y:.1f})", (10, 50),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 1)

                # 发送误差数据给舵机控制
                datapack.send_packet(ser, [error_x, error_y])

                # 如果当前位置接近目标点（误差小于阈值），则移动到下一个点
                distance_to_target = np.sqrt(error_x ** 2 + error_y ** 2)
                if distance_to_target < 8:  # 阈值设为10像素
                    trace_index += 1
                    if trace_index < len(trace_points):
                        trace_target = trace_points[trace_index]
                        print(f"到达目标点，新目标点: {trace_target}")
                    else:
                        # 已完成所有点的循迹
                        trace_target = None
                        print("已完成所有点的循迹")

            # 绘制循迹路径
            if len(trace_points) > 1:
                for i in range(1, len(trace_points)):
                    cv2.line(frame, trace_points[i - 1], trace_points[i], (0, 255, 255), 1)

            # 绘制目标点
            if trace_target is not None:
                cv2.circle(frame, trace_target, 5, (0, 255, 0), 2)
                # 绘制当前位置到目标点的连线
                if current_point:
                    cv2.line(frame, current_point, trace_target, (255, 0, 0), 1)

            # 显示当前循迹进度
            if len(trace_points) > 0:
                progress = f"Progress: {trace_index + 1}/{len(trace_points)}"
                cv2.putText(frame, progress, (10, 70),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 1)


    # ======================
    # 显示图像
    # ======================
    if screen_region is not None:
        cv2.polylines(frame, [screen_region], True, (0, 255, 255), 2)

    if screen_region_plus is not None:
        cv2.polylines(frame, [screen_region_plus], True, (255, 0, 255), 2)

    #cv2.imshow("frame", frame)

    # time======================
    time_end = time.time()
    #print(f"{time_end - time_start:.3f} ")

    if cv2.waitKey(20) & 0xFF == ord('q'):
        break

# ======================
# 释放资源
# ======================
cap.release()
cv2.destroyAllWindows()

# 保存激光路径到文件
if state == 3 and len(laser_path) > 0:
    print(f"保存激光路径到 {path_file_name}")
    with open(path_file_name, 'w') as f:
        for i, point in enumerate(laser_path):
            f.write(f"{i},{point[0]},{point[1]}\n")
    print(f"已保存 {len(laser_path)} 个路径点")
elif state == 3:
    print("State 3: 没有记录到激光路径")

