import cv2
import numpy as np
import Points_Utils

class MyDetector:
    """
    激光点检测 + 屏幕矩形检测 + 多边形轮廓工具
    """

    # ------------------------------------------------------------------
    # 构造：集中管理所有可调参数
    # ------------------------------------------------------------------
    def __init__(self):
        # 1) 自适应激光阈值 - 红色激光HSV颜色范围
        self.RED_HSV_LOW   = np.array([150, 0, 100])
        self.RED_HSV_HIGH  = np.array([10, 30, 255])

        # 2) 动态亮度阈值
        self.BRIGHT_BASE = 225  # 基准亮度值
        self.BRIGHT_DELTA = 30  # 动态调整范围

        # 3) 形态学处理
        self.KERNEL_DILATE = np.ones((5, 5), np.uint8)
        self.KERNEL_ERODE = np.ones((3, 3), np.uint8)

        # 4) 预测算法参数
        self.MAX_PREDICT_FRAMES = 25  # 最大连续预测帧数
        self.MIN_TRACK_POINTS = 20  # 启用预测所需的历史点数量
        self.PREDICT_RADIUS = 40  # 预测点搜索半径

        # 5)矩形检测用形态学核
        self.KERNEL_3 = cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3))
        self.KERNEL_5 = cv2.getStructuringElement(cv2.MORPH_RECT, (5, 5))

        self.screen_mask = None
        self.screen_region = None
        self.screen_region_plus = None

        # 黑色框的内外框
        self.blobs_black = None
        self.blobs_black_index = 0
        self.len_blobs_black = 0
        # 最大的轮廓
        self.blobs_max = None
        self.blobs_max_index = 0
        self.len_blobs_max = 0

    # ------------------------------------------------------------------
    # 激光检测
    # ------------------------------------------------------------------
    def detect_laser(self, bgr,  prev_point, debug=False):
        """
        激光点检测。
        返回: (x, y)  or None
        """
        hsv = cv2.cvtColor(bgr, cv2.COLOR_BGR2HSV)
        h, s, v = cv2.split(hsv)

        # 1. 动态亮度阈值（基于屏幕区域亮度）
        if self.screen_mask is not None:
            mask_area = cv2.countNonZero(self.screen_mask)
            if mask_area > 0:
                avg_brightness = cv2.mean(v, mask=self.screen_mask)[0]
                bright_thresh = max(self.BRIGHT_BASE, avg_brightness - self.BRIGHT_DELTA)
            else:
                bright_thresh = self.BRIGHT_BASE
        else:
            bright_thresh = self.BRIGHT_BASE

        # 2. 亮度掩码
        _, mask_white = cv2.threshold(v, bright_thresh, 255, cv2.THRESH_BINARY)

        # 3. 红色掩码（H 环形）
        mask_red1 = cv2.inRange(hsv, self.RED_HSV_LOW, np.array([180, 255, 255]))
        mask_red2 = cv2.inRange(hsv, np.array([0, self.RED_HSV_LOW[1], self.RED_HSV_LOW[2]]), self.RED_HSV_HIGH)
        mask_red = cv2.bitwise_or(mask_red1, mask_red2)

        # 4. 组合（亮度+颜色）
        mask_combined = cv2.bitwise_and(mask_red, mask_white)

        # 5. 形态学优化
        # mask_combined = cv2.dilate(mask_combined, self.KERNEL_DILATE, 1)
        # mask_combined = cv2.erode(mask_combined, self.KERNEL_ERODE, 1)

        # 6. 屏幕掩码
        if self.screen_mask is not None:
            mask_combined = cv2.bitwise_and(mask_combined, self.screen_mask)

        # 7. 基于上一帧的 ROI
        if prev_point is not None and len(prev_point) >= 2:
            x, y = prev_point[:2]
            roi_mask = np.zeros_like(mask_combined)
            cv2.circle(roi_mask, (x, y), self.PREDICT_RADIUS, 255, -1)
            mask_combined = cv2.bitwise_and(mask_combined, roi_mask)

        if debug:
            cv2.imshow("debug_mask", mask_combined)

        # 8. 计算质心
        # 查找所有白色像素的坐标
        white = np.where(mask_combined == 255)
        if len(white[0]) == 0 or len(white[0]) > 500:
            return None
        # 获取这些像素在亮度通道中的值作为权重
        weights = v[white]
        # 计算加权质心
        total_w = np.sum(weights)
        if total_w == 0:
            return None

        # 如果没有先前点信息，优先选择距离图像中心最近的点
        if prev_point is None:
            # 计算图像中心
            cx_img, cy_img = bgr.shape[1] // 2, bgr.shape[0] // 2
            # 计算每个白点到图像中心的距离
            dists = (white[1] - cx_img) ** 2 + (white[0] - cy_img) ** 2
            # 找到距离中心最近的点
            idx = np.argmin(dists)
            cx, cy = int(white[1][idx]), int(white[0][idx])
        else:
            cx = int(np.sum(white[1] * weights) / total_w)
            cy = int(np.sum(white[0] * weights) / total_w)

        return cx, cy, float(total_w / len(weights))

    # ------------------------------------------------------------------
    # 屏幕矩形检测
    # ------------------------------------------------------------------
    def detect_rects(self, bgr, num_interpolations=4, debug=False):
        """
        检测图像中的屏幕区域（矩形区域）
        参数:
            bgr: BGR格式的输入图像
            debug: 是否显示调试信息
        返回:
            屏幕区域的边界框 (x, y, w, h)，如果未检测到则返回None
        """
        if bgr is None:
            return None, None

        # 1) histeq（直方图均衡）- 增强图像对比度
        gray = cv2.cvtColor(bgr, cv2.COLOR_BGR2GRAY)
        gray = cv2.equalizeHist(gray)
        # 2) Canny 边缘检测 - 提取图像边缘, 先膨胀后腐蚀，连接断开的边缘
        edges = cv2.Canny(gray, 50, 255)
        edges = cv2.dilate(edges, self.KERNEL_5, 1)
        edges = cv2.erode(edges, self.KERNEL_3, 1)

        if debug:
            cv2.imshow("edges", edges)

        # 4) 找轮廓 -> 拟合矩形
        # 在ROI区域内查找轮廓
        h_e, w_e = edges.shape
        roi_x, roi_y, roi_w, roi_h = 10, 10, w_e - 20, h_e - 20
        roi_edges = edges[roi_y:roi_y + roi_h, roi_x:roi_x + roi_w]
        contours, _ = cv2.findContours(roi_edges,
                                   cv2.RETR_EXTERNAL,
                                   cv2.CHAIN_APPROX_SIMPLE)
        rects, approx_contours = [], []

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
        valid, valid_contours = [], []
        for i, (x, y, w, h) in enumerate(rects):
            area = w * h
            if area > 5000 and w > 180 and h > 180 and abs(w - h) < 80:
                valid.append((x, y, w, h, area))
                valid_contours.append(approx_contours[i])

        if not valid:
            print("no rectangles")
            return None, None
        
        # 找到面积最大的矩形及其对应的轮廓
        idx = max(range(len(valid)), key=lambda i: valid[i][4])
        x, y, w, h, _ = valid[idx]
        best_approx = valid_contours[idx]

        # 16 点插值
        best_points = best_approx.reshape(4, 2)
        # 初始点集
        current = list(best_points)
        for _ in range(num_interpolations):
            # 插值操作
            interpolated = Points_Utils.interpolate_points(current)
            # 合并原始点和插入的点
            merged = []
            for i in range(len(current)):
                merged.extend([current[i], interpolated[i]])
            current = merged
        current.append(current[0])
        # 修改返回格式为OpenCV轮廓格式 (16, 1, 2)
        contour = np.array(current, dtype=np.int32).reshape(-1, 1, 2)
        # 调试时绘制近似轮廓
        if debug:
            cv2.drawContours(bgr, [contour], -1, (0, 0, 255), 2)
            cv2.rectangle(bgr, (x, y), (x + w, y + h), (0, 255, 0), 2)
            cv2.imshow("bgr", bgr)

        return (x, y, w, h), contour

    # ------------------------------------------------------------------
    # 通用多边形检测
    # ------------------------------------------------------------------
    def find_binary_approx_polygon(self, gray, n_points, min_area=10, max_area=20000, epsilon_factor=0.02, debug=False):
        """
        找到图像中指定二值化后，且近似为n边形的轮廓
        input:
        gray: 灰度图像
        n_points: 多边形顶点数
        min_area: 最小面积阈值，默认为10像素
        max_area: 最大面积阈值，默认为20000像素
        epsilon_factor: 近似精度因子，默认为0.02

        output:
        polygons: 所有符合的近似多边形轮廓
        """
        # 二值化
        img = cv2.equalizeHist(gray)
        _, bin_img = cv2.threshold(img, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
        # 后处理 处理黑框中白色噪声
        bin_img = cv2.morphologyEx(bin_img, cv2.MORPH_CLOSE, self.KERNEL_3)

        if debug:
            cv2.imshow("img_binary", bin_img)
        # 找轮廓
        contours, _ = cv2.findContours(bin_img, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
        polygons = []
        for cnt in contours:
            # 根据最小面积过滤轮廓
            area = cv2.contourArea(cnt)
            if min_area <= area <= max_area:
                epsilon = epsilon_factor * cv2.arcLength(cnt, True)
                approx = cv2.approxPolyDP(cnt, epsilon, True)
                if len(approx) == n_points:
                    polygons.append(approx)
        return polygons

    # ------------------------------------------------------------------
    # 轮廓包含检测
    # ------------------------------------------------------------------
    def check_contour_containment(self, contours):
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
        for (x, y) in small_contour:
            if cv2.pointPolygonTest(large_contour, (float(x), float(y)), False) < 0:
                return False
        return True
    
    def create_screen_mask(self, frame, debug=False):
        """
        创建一个遮罩，用于将屏幕区域从图像中排除
        :param frame: 输入的图像
        :param screen_region: 屏幕区域
        :return: 遮罩
        """
        rect, self.blobs_max = self.detect_rects(frame, debug=False)  
        if self.blobs_max is not None and debug:
            cv2.drawContours(frame, [self.blobs_max], -1, (0, 255, 0), 2)
            cv2.imshow("result2", frame)
        else:
            return
        
        if rect:
            x, y, w, h = rect
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
            self.screen_region = np.array([
                [x,         y],
                [x + w,     y],
                [x + w, y + h],
                [x,     y + h]
            ], dtype=np.int32)
            center = self.screen_region.mean(axis=0)
            self.screen_region_plus = ( (self.screen_region - center) * 1.2 + center ).astype(np.int32)
            self.screen_region_plus[:, 0] = np.clip(self.screen_region_plus[:, 0], 0, frame.shape[1] - 1)
            self.screen_region_plus[:, 1] = np.clip(self.screen_region_plus[:, 1], 0, frame.shape[0] - 1)
            self.screen_mask = np.zeros(frame.shape[:2], dtype=np.uint8)
            cv2.fillPoly(self.screen_mask, [self.screen_region], 255)