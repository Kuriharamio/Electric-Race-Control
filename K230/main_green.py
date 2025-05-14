import math
import time
from machine import FPIOA,Pin
from media.sensor import *
from media.display import *
from media.media import *
import utime
import datapack
import ulab.numpy as np


# 初始化舵机
fpioa = FPIOA()

picture_width = 640
picture_height = 360

error_x = 0
error_y = 0

error_flag = 0

green_x = -1
green_y = -1

red_x = -1
red_y = -1

vertex_threshold_x = 10
vertex_threshold_y = 10
no_detect_cnt = 0 # 跟随次数

# 颜色阈值
red_threshold = (0, 100, 24, 120, -32, 32)
green_threshold = (0, 100, -90, -17, -20, 100)

black_threshold = (0, 30, -50, 50, -50, 50)


fpioa.set_function(20, FPIOA.GPIO20)
LED_G = Pin(20, Pin.OUT, pull=Pin.PULL_UP, drive=15)
LED_G.value(1)


kalman_init_flag = False
dt = 0.055  # 时间步长，根据实际采样频率确定
# 状态转移矩阵，假设状态包括位置（x,） y和速度（vx, vy）
F = np.array([[1, 0, dt, 0],
            [0, 1, 0, dt],
            [0, 0, 1, 0],
            [0, 0, 0, 1]])
# 测量矩阵，只测量位置
H = np.array([[1, 0, 0, 0],
            [0, 1, 0, 0]])
# 过程噪声协方差矩阵
Q = np.eye(4) * 0.01  # 这个值可以根据实际情况调整，代表过程噪声的大小
# 测量噪声协方差矩阵
R = np.eye(2) * 0.1  # 同样根据实际情况调整，代表测量噪声的大小
# 初始状态估计协方差矩阵
P = np.eye(4)
# 初始状态估计（位置和速度，如果开始时速度为 0，则可以这样初始化）
x = np.array([[0], [0], [0], [0]])



def initialize_camera_and_display():
    try:
        # 构造一个具有默认配置的摄像头对象
        sensor = Sensor(id=2, width=1920, height=1080)
        # 重置摄像头sensor
        sensor.reset()

        # 设置水平镜像垂直镜像
        sensor.set_vflip(True)
        sensor.set_hmirror(True)

        # 设置通道0的输出尺寸
        sensor.set_framesize(width=picture_width, height=picture_height, chn=CAM_CHN_ID_0)
        # 设置通道0的输出像素格式为RGB565
        sensor.set_pixformat(Sensor.RGB565, chn=CAM_CHN_ID_0)

        # 使用IDE的帧缓冲区作为显示输出
        Display.init(Display.VIRT, width=1920, height=1080, to_ide=True, osd_num=2)
        # 初始化媒体管理器
        MediaManager.init()
        # 启动传感器
        sensor.run()
        # 构造clock
        clock = utime.clock()

        return sensor, clock, True

    except BaseException as e:
        print(f"初始化失败: {e}")
        return None, None, False


def detect_largest_blob(img, color_threshold):
    """
    检测图像中最大颜色块并记录坐标
    :param img: 输入图像对象
    :param color_threshold: 颜色阈值元组，例如(30, 100, 15, 127, 15, 127)
    """
    blobs = img.find_blobs([color_threshold], pixels_threshold=8, area_threshold=4)

    if blobs:
        # 使用max函数找到最大颜色块（按宽度*高度计算面积）
        largest_blob = max(blobs, key=lambda b: b[4], default=None)

        if largest_blob:
            return largest_blob
    return None

def detect_largest_color(img, color_threshold):

    img.binary([color_threshold], invert=False)
    #img.erode(1)
    img.dilate(2)
    blobs = img.find_blobs([(90,100,-10,10,-10,10)], pixels_threshold=10, area_threshold=4)
    if blobs:

        # 使用max函数找到最大颜色块（按宽度*高度计算面积）
        largest_blob = max(blobs, key=lambda b: b[4], default=None)

        if largest_blob:
            return largest_blob

    return None


def get_state(uart):
    tem = uart.read(1)
    if tem:

        state = tem[0]
        #print("state:", state)
        return state
    return 0

# 卡尔曼滤波参数初始化
def kalman_filter_init():
    global dt, F, H, Q, R, P, x
    dt = 0.1  # 时间步长，根据实际采样频率确定
    # 状态转移矩阵，假设状态包括位置（x,） y和速度（vx, vy）
    F = np.array([[1, 0, dt, 0],
                [0, 1, 0, dt],
                [0, 0, 1, 0],
                [0, 0, 0, 1]])
    # 测量矩阵，只测量位置
    H = np.array([[1, 0, 0, 0],
                [0, 1, 0, 0]])
    # 过程噪声协方差矩阵
    Q = np.eye(4) * 0.01  # 这个值可以根据实际情况调整，代表过程噪声的大小
    # 测量噪声协方差矩阵
    R = np.eye(2) * 5  # 同样根据实际情况调整，代表测量噪声的大小
    # 初始状态估计协方差矩阵
    P = np.eye(4)
    # 初始状态估计（位置和速度，如果开始时速度为 0，则可以这样初始化）
    x = np.array([[0], [0], [0], [0]])

def kalman_filter(a, b):
    global F, H, Q, R, P, x, kalman_init_flag
    if kalman_init_flag is False:
        x[0][0] = a
        x[1][0] = b
        kalman_init_flag = True
        return a, b
    # 获取测量值（红色色块中心坐标）
    z = np.array([[a], [b]])

    # 预测步骤
    # 状态预测
    x_pred = np.dot(F, x)
    # 误差协方差预测
    P_pred = np.dot(np.dot(F, P), F.T) + Q

    # 更新步骤
    # 计算卡尔曼增益
    S = np.dot(np.dot(H, P_pred), H.T) + R
    K = np.dot(np.dot(P_pred, H.T), np.linalg.inv(S))

    # 状态更新
    x = x_pred + np.dot(K, (z - np.dot(H, x_pred)))

    # 误差协方差更新
    P = P_pred - np.dot(np.dot(K, H), P_pred)

    # 使用卡尔曼滤波后的坐标
    x_filtered = x[0][0]
    y_filtered = x[1][0]

    return int(x_filtered), int(y_filtered)


try:
    # 示例调用
    sensor, clock, success = initialize_camera_and_display()
    if success:
        print("success")
    else:
        print("fail")
    uart = datapack.Uart_Init()
    while True:
        # 更新当前时间（毫秒）
        clock.tick()
        # 捕获通道0的图像
        img_raw = sensor.snapshot(chn=CAM_CHN_ID_0)
        green_x = -1
        green_y = -1
        red_x =-1
        red_y = -1

        # 图像处理放到这里
        # --------开始--------

        #滤波
        #img_raw.gaussian(2)
        # 颜色块检测
        green_blob = detect_largest_blob(img_raw, green_threshold)
        red_blob = detect_largest_blob(img_raw, red_threshold)

        if green_blob:
            green_x, green_y = green_blob[5], green_blob[6]

        if red_blob:
            red_x, red_y = kalman_filter(red_blob[5], red_blob[6])

        radius = 19
        img_raw.draw_circle(red_x, red_y, radius, color=(0, 255, 0), thickness=1)
        Display.show_image(img_raw, 0, 0, layer=Display.LAYER_OSD0)

        if red_x > 0 and red_y > 0 and green_x > 0 and green_y > 0:
            error_x = red_x - green_x
            error_y = red_y - green_y
        else:
            no_detect_cnt += 1
            print("no detect !")
            datapack.send_packet(uart,[0 ,0, 0, 0, 0, 0, 0])
            if no_detect_cnt > 10:
                no_detect_cnt = 0
                datapack.send_packet(uart,[0 ,0, 0, 0, 0, 0, 1])
                print("no detect , but overlap !")
            print(clock.fps())
            continue

        if abs(error_x) < vertex_threshold_x and abs(error_y) < vertex_threshold_y and red_x > 0 and red_y > 0 and green_x > 0 and green_y > 0:
            # datapack.send_packet(uart,[0.06 ,0.15, 0, 0.06 ,0.15, 0, 1])
            datapack.send_packet(uart,[0.0 , 1.0, -error_y, 0.0, 1.0, error_x, 1])
            print("stop")
            print(clock.fps())
            continue

        # print("move")
        if error_x**2 + error_y**2 <= radius**2 and red_x > 0 and red_y > 0 and green_x > 0 and green_y > 0:
            error_flag = 1
        else:
            error_flag = 0

        if error_x < 0:
            error_x = -(abs(error_x) ** 0.5)
        else:
            error_x = (abs(error_x) ** 0.5)

        if error_y < 0:
            error_y = -(abs(error_y) ** 0.5)
        else:
            error_y = (abs(error_y) ** 0.5)

        print("error_x:", error_x, "error_y:", error_y)
        if error_flag:
            datapack.send_packet(uart,[0.0 , 1.0, -error_y, 0.0, 1.0, error_x, 1])
        else:
            datapack.send_packet(uart,[0.0 , 1.0, -error_y, 0.0, 1.0, error_x, 0])


        #print(get_state(uart))
        # 绘制颜色块的中心十字

#        #,,,,,,,,,,,,,,,,,,,,,,,,,
#        img_b1 = img_raw.copy()
#        blobs_red = detect_largest_color(img_b1, red_threshold)

#        error_red_x, error_red_y = 0, 0
#        if blobs_red:
#            error_red_x, error_red_y = blobs_red[5] - red_x, blobs_red[6] - red_y

#        Display.show_image(img_b1, picture_width, 0, layer=Display.LAYER_OSD1)

#        img_b2 = img_raw.copy()
#        blobs_green = detect_largest_color(img_b2, green_threshold)

#        error_green_x, error_green_y = 0, 0
#        if blobs_green:
#             error_green_x, error_green_y = blobs_green[5] - green_x, blobs_green[6] - green_y

#        Display.show_image(img_b2, 0, picture_height, layer=Display.LAYER_OSD2)
#        #,,,,,,,,,,,,,,,,,,,,,,,,,

#        if red_x != -1 and red_y != -1:
#            img_raw.draw_cross(red_x, red_y, size=10, thickness=5)

#        if green_x != -1 and green_y != -1:
#            img_raw.draw_cross(green_x, green_y, size=10, thickness=5)


        # 打印帧率到控制台
        print(clock.fps())


except KeyboardInterrupt as e:
    print("用户停止: ", e)
except BaseException as e:
    print(f"异常: {e}")
finally:
    datapack.send_packet(uart,[0 ,0, 0, 0, 0, 0, 0])
    # 停止传感器运行
    if isinstance(sensor, Sensor):
        sensor.stop()
    # 反初始化显示模块
    Display.deinit()
    time.sleep_ms(100)
    # 释放媒体缓冲区
    MediaManager.deinit()
