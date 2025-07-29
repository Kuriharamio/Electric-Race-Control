from Mycamera import MyCamera
from Myserial import MySerial
from Mykalman import KalmanFilter
from Mydetector import MyDetector
import Points_Utils

import time
import rclpy
import numpy as np
import cv2
import threading

DEBUG = False

def main():
    rclpy.init()
    serial = MySerial()
    spin_thread = threading.Thread(target=rclpy.spin, args=(serial,), daemon=True)
    spin_thread.start()


    camera = MyCamera()
    detector = MyDetector()
    kf = KalmanFilter()

    flag_state_1 = True
    flag_state_2 = True
    flag_state_3 = True
    flag_state_4 = True

    def Laser_step(frame, length, blobs_index, blobs):
        # 激光点检测阶段 - 当屏幕区域已经检测到后，开始检测激光点
        # 获取上一帧检测到的点作为参考点
        prev_point = kf.track_points[-1] if kf.track_points else None
        # 从prev_point中提取坐标信息(x, y)
        prev_coord = prev_point[:2] if prev_point else None
        # 调用增强型激光检测函数检测当前帧中的激光点
        pt_data = detector.detect_laser(
            frame,  # 当前帧图像
            prev_point=prev_point,  # 上一帧检测到的点，用于预测和ROI优化
            show_mask=False  # 显示调试掩码窗口
        )
        target = kf.update(pt_data)
        if target:
            # 计算和blobs_black的差值
            if blobs_index < length:
                error_red = np.array(target) - blobs[blobs_index][0]
                print(error_red)
                # 发送数据给舵机控制
                serial.send_packet([error_red[0], error_red[1]])
            else:
                error_red = np.array(target) - blobs[0][0]
                print(error_red)
                # 发送数据给舵机控制
                serial.send_packet([0, 0])
            if error_red[0] ** 2 + error_red[1] ** 2 < 40 and blobs_index < length:
                if serial.get_latest_frame().Task_ID == 1:
                    detector.blobs_max_index += 1
                elif serial.get_latest_frame().Task_ID == 2:
                    detector.blobs_black_index += 1
            cv2.circle(frame, target, 3, (0, 255, 0), -1)
        return frame

    while True:
        frame = camera.get_frame()
        serial_data = serial.get_latest_frame()
        time_start = time.time()
        # ======================
        # 统一的图像处理
        # ======================
        # 灰度图
        img_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        if detector.screen_mask is None:
            detector.create_screen_mask(frame=frame, debug=DEBUG)
            continue
        
        # =======================
        # 执行任务
        # =======================
        if serial_data.Task_ID == 1:
           if detector.blobs_max is not None:
                frame = Laser_step(frame, len(detector.blobs_max), detector.blobs_max_index, detector.blobs_max)

        elif serial_data.Task_ID == 2:
            # ======================
            # 只执行一次的代码
            # ======================
            if flag_state_2:
                # 这里可以放一些只需要执行一次的初始化代码
                contours = detector.find_binary_approx_polygon(img_gray, 4, min_area=100, epsilon_factor=0.015, debug=False)
                # 排序找到面积最大的两个框
                if len(contours) >= 2:
                    contours = sorted(contours, key=lambda x: cv2.contourArea(x), reverse=True)[:2]
                    # 判断是否包含
                    if detector.check_contour_containment(contours):
                        # 保存contours，完成任务
                        detector.blobs_black = Points_Utils.find_center_contour_clockwise(contours[0], contours[1])
                        flag_state_2 = False
                        if DEBUG:
                            # 画图
                            cv2.drawContours(frame, contours, -1, (0, 255, 0), 2)
                            cv2.drawContours(frame, [detector.blobs_black], -1, (255, 0, 255), 2)
                            cv2.imshow("result", frame)
            # ======================
            # 总是执行的代码
            # ======================
            if detector.blobs_black is not None and detector.screen_mask is not None:
                frame = Laser_step(frame, len(detector.blobs_black), detector.blobs_black_index, detector.blobs_black)

        elif serial_data.Task_ID == 3:
            pass
        elif serial_data.Task_ID == 4:
            pass
        
        if DEBUG:
            # ======================
            # 显示图像
            # ======================
            if detector.screen_region is not None:
                cv2.polylines(frame, [detector.screen_region], True, (0, 255, 255), 2)
            if detector.screen_region_plus is not None:
                cv2.polylines(frame, [detector.screen_region_plus], True, (255, 0, 255), 2)
            
            cv2.imshow("frame", frame)
            # ======================
            # 计时
            # ======================
            time_end = time.time()
            # print(f"{time_end - time_start:.3f} ")

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    serial.destroy_node()
    camera.release()
    rclpy.shutdown()


if __name__ == '__main__':
    main()