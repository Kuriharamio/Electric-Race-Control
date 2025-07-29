import cv2
import numpy as np
import time

class MyCamera:
    def __init__(self):
        self.cap = cv2.VideoCapture(0)
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

    def wait_stable(self):
        print("等待摄像头稳定...")
        for i in range(15):
            ret, frame = self.cap.read()
            if not ret:
                print("无法读取帧")
                break
            time.sleep(0.05)  # 50ms延时
        print("摄像头已稳定")

    def get_frame(self):
        ret, frame = self.cap.read()
        return frame
    
    def release(self):
        self.cap.release()

    
    