import cv2
import numpy as np
from collections import deque

class KalmanFilter:
    """
    卡尔曼滤波器类，用于预测激光点位置
    """

    def __init__(self):
        """
        初始化卡尔曼滤波器参数
        状态向量包含位置和速度：[x, y, vx, vy]
        观测向量包含位置：[x, y]
        """
        self.kf = cv2.KalmanFilter(4, 2)
        # 测量矩阵 H：将状态向量映射到观测空间
        self.kf.measurementMatrix = np.array([[1, 0, 0, 0], [0, 1, 0, 0]], np.float32)
        # 状态转移矩阵 F：描述状态如何随时间变化
        self.kf.transitionMatrix = np.array([[1, 0, 1, 0], [0, 1, 0, 1], [0, 0, 1, 0], [0, 0, 0, 1]], np.float32)
        # 过程噪声协方差矩阵 Q
        self.kf.processNoiseCov = np.eye(4, dtype=np.float32) * 0.03
        # 测量噪声协方差矩阵 R
        self.kf.measurementNoiseCov = np.eye(2, dtype=np.float32) * 0.1
        # 后验误差协方差矩阵 P
        self.kf.errorCovPost = np.eye(4, dtype=np.float32)
        self.last_prediction = None

        # 预测算法参数
        self.MAX_PREDICT_FRAMES = 25  # 最大连续预测帧数
        self.MIN_TRACK_POINTS = 20  # 启用预测所需的历史点数量
        self.PREDICT_RADIUS = 40  # 预测点搜索半径

        self.track_points = deque(maxlen=10)  
        self.predict_count = 0   

    def predict(self, pt):
        """
        使用卡尔曼滤波预测激光点位置
        参数:
            pt: 当前检测到的点坐标 (x, y)
        返回:
            预测的点坐标 (x, y)
        """
        if pt is None:
            return None

        # 初始化和更新状态
        if self.last_prediction is None:
            # 第一次调用，初始化状态向量
            self.kf.statePost = np.array([[pt[0]], [pt[1]], [0], [0]], dtype=np.float32)
            self.last_prediction = pt
            return pt

        # 预测
        # 先进行预测步骤
        prediction = self.kf.predict()
        # 再使用观测值进行更新步骤
        self.kf.correct(np.array([[np.float32(pt[0])], [np.float32(pt[1])]]))

        pred_pt = (int(prediction[0]), int(prediction[1]))
        self.last_prediction = pred_pt
        return pred_pt
    
    def update(self, pt_data):
        # 处理检测结果
        if pt_data:
            # 检测到激光点
            cx, cy, area = pt_data
            pt = (cx, cy)
            self.track_points.append((cx, cy, area))
            self.predict_count = 0  # 重置预测计数
            # 更新卡尔曼滤波器
            predicted_pt = self.predict(pt)
        else:
            # 未检测到激光点，尝试使用预测
            self.predict_count += 1
            # 检查是否满足预测条件：
            # 1. 连续预测帧数不超过最大允许值（25帧）
            # 2. 历史跟踪点数量达到最小要求（20个点）
            if self.predict_count <= self.MAX_PREDICT_FRAMES and len(self.track_points) >= self.MIN_TRACK_POINTS:
                # 使用卡尔曼滤波预测激光点位置
                # 如果卡尔曼滤波器有上一次的预测结果，则使用该结果
                if self.last_prediction:
                    pt = self.last_prediction
                else:
                    # 使用最后一个已知点
                    pt = self.track_points[-1][:2]
                predicted_pt = pt
            else:
                pt = None
                predicted_pt = None
        # 画pt
        target = None
        if pt :
            target = pt
        elif predicted_pt:
            target = predicted_pt

        return target