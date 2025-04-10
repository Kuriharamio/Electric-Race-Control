import time
from machine import FPIOA, PWM
from media.sensor import *
from media.display import *
from media.media import *
import utime
# 初始化舵机
fpioa = FPIOA()
fpioa.set_function(42, FPIOA.PWM0)  # 舵机1
fpioa.set_function(52, FPIOA.PWM4)  # 舵机2

servo1 = PWM(0, 50, 0, enable=True)
servo2 = PWM(4, 50, 0, enable=True)

def angle_to_duty(angle):
    """将角度转换为占空比"""
    if angle < 0:
        angle = 0
    elif angle > 180:
        angle = 180
    pulse_width = 0.5 + (angle / 180) * 2
    duty = (pulse_width / 20) * 100
    return duty

def control_servo(servo, angle):
    """控制舵机转到指定角度"""
    duty = angle_to_duty(angle)
    servo.duty(duty)

# 初始化摄像头
sensor_id = 2
sensor = Sensor(id=sensor_id)
sensor.reset()
sensor.set_framesize(width=1920, height=1080, chn=CAM_CHN_ID_0)
sensor.set_pixformat(Sensor.RGB565, chn=CAM_CHN_ID_0)

# 初始化显示器
Display.init(Display.VIRT, width=1920, height=1080, to_ide=True)
MediaManager.init()
sensor.run()

# 颜色阈值（红色）
color_threshold = [(19, 33, 20, 127, 1, 26)]
#构造clock
clock = utime.clock()
try:
    while True:
        os.exitpoint()
        clock.tick()

        # 捕获图像
        img = sensor.snapshot(chn=CAM_CHN_ID_0)

        # 寻找颜色块
        blobs = img.find_blobs(color_threshold, area_threshold=2000)

        if blobs:
            print("1")
            # 获取最大的颜色块
            largest_blob = max(blobs, key=lambda b: b[2]*b[3])

            # 绘制颜色块的外接矩形和中心十字
            img.draw_rectangle(largest_blob[0:4])
            img.draw_cross(largest_blob[5], largest_blob[6], size=10, thickness=5)
            # 获取颜色块中心坐标
            cx, cy = largest_blob[5], largest_blob[6]

            # 计算舵机角度
            angle1 = int((cx / 1920) * 180)  # 水平方向
            angle2 = int((cy / 1080) * 180)  # 垂直方向

            # 控制舵机转动
            control_servo(servo1, angle1)
            control_servo(servo2, angle2)

        # 显示图像
        Display.show_image(img)
#        print(clock.fps())
#        time.sleep_ms(100)

except KeyboardInterrupt:
    # 释放资源
    servo1.deinit()
    servo2.deinit()
    sensor.stop()
    Display.deinit()
    MediaManager.deinit()
