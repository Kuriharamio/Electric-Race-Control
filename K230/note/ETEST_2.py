import math
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

picture_width = 480
picture_height = 270
sensor = None

middle_x=-1
middle_y=-1

corner_rect = None
x_y_w_h_rect = None
sorted_points = None
red_x = -1
red_y = -1

vertex_threshold = 10  # 阈值可以根据实际情况调整
corner_n=0
# 颜色阈值（红色）
color_threshold = (18, 57, 7, 87, -11, 127)


def caculate_error(x, y, corner_rect, n):
    if corner_rect is None:
        return -9999, -9999, False

    # 计算误差
    error_x = corner_rect[n][0] - x
    error_y = corner_rect[n][1] - y

    if abs(error_x) > vertex_threshold or abs(error_y) > vertex_threshold:
        flag=False
    else:
        flag=True
    return error_x, error_y, flag





def detect_rects(img):
    global corner_rect, x_y_w_h_rect, sorted_points
    rects = img.find_rects(threshold=100000)

    if rects:
        # 使用生成器表达式过滤矩形，并找到最大面积的矩形
        valid_rects = (rect for rect in rects
                       if (area := rect.magnitude()) > 5000
                       and rect.x() > 10
                       and rect.y() > 10
                       and rect.h() > 70
                       and rect.w() > 70)

        selected_rect = max(valid_rects,
                            key=lambda r: r.magnitude(),
                            default=None)

        if selected_rect:
            corner_rect = selected_rect.corners()
            x_y_w_h_rect = selected_rect.rect()

            # 计算矩形中心
            center_x = sum(p[0] for p in corner_rect) / 4
            center_y = sum(p[1] for p in corner_rect) / 4

            # 按照每个点相对于中心的角度进行排序
            sorted_points = sorted(corner_rect, key=lambda p: (math.atan2(p[1] - center_y, p[0] - center_x)))

            # 调整顺序以确保顺时针排列
            if sorted_points[0][0] > sorted_points[1][0]:  # 如果第一个点在第二个点的右边，说明是逆时针
                sorted_points.reverse()


def find_closest_blob(img):
    global middle_x, middle_y
    target_x, target_y = 240, 135  # 目标点坐标

    blobs = img.find_blobs([(240, 255)], roi=(200, 100, 80, 70),
                           pixels_threshold=20, area_threshold=4)

    # 修改后的代码（带注释说明）
    if blobs:
        # 使用min函数找到距离目标点最近的blob
        valid_blobs = (blob for blob in blobs if blob[4] < 200)
        closest_blob = min(valid_blobs,
                           key=lambda b: ((b[5] - target_x) ** 2 + (b[6] - target_y) ** 2),
                           default=None)

        if closest_blob:
            middle_x, middle_y = closest_blob[5], closest_blob[6]


def detect_largest_blob(img, color_threshold):
    """
    检测图像中最大颜色块并记录坐标
    :param img: 输入图像对象
    :param color_threshold: 颜色阈值元组，例如(30, 100, 15, 127, 15, 127)
    """
    global red_x, red_y

    blobs = img.find_blobs([color_threshold], pixels_threshold=20, area_threshold=4)

    if blobs:
        # 使用max函数找到最大颜色块（按宽度*高度计算面积）
        largest_blob = max(blobs, key=lambda b: b[2] * b[3], default=None)

        # # 绘制颜色块的外接矩形和中心十字
        # img.draw_rectangle(largest_blob[0:4])
        # img.draw_cross(largest_blob[5], largest_blob[6], size=10, thickness=5)

        if largest_blob:
            red_x, red_y = largest_blob[5], largest_blob[6]


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




try:
    # 构造一个具有默认配置的摄像头对象
    sensor = Sensor(id=2,width=480, height=270)
    # 重置摄像头sensor
    sensor.reset()

    # 无需进行镜像翻转
    # 设置水平镜像
    # sensor.set_hmirror(False)
    # 设置垂直翻转
    # sensor.set_vflip(False)

    # 设置通道0的输出尺寸
    sensor.set_framesize(width=picture_width, height=picture_height, chn=CAM_CHN_ID_0)
    # 设置通道0的输出像素格式为RGB888
    sensor.set_pixformat(Sensor.RGB565, chn=CAM_CHN_ID_0)

    # 使用IDE的帧缓冲区作为显示输出
    Display.init(Display.VIRT, width=1920, height=1080, to_ide=True, osd_num=2)
    # 初始化媒体管理器
    MediaManager.init()
    # 启动传感器
    sensor.run()

    #构造clock
    clock = utime.clock()

    while True:
        #更新当前时间（毫秒）
        clock.tick()
        # 捕获通道0的图像
        img_raw = sensor.snapshot(chn=CAM_CHN_ID_0)

        detect_largest_blob(img_raw, color_threshold)






        # 显示捕获的图像
        # 在屏幕右上角显示处理后的图像

        # 图像处理放到这里
        #--------开始--------

        # 图像转换
        img = img_raw.to_grayscale(copy=False)
        img.histeq(adaptive=False)




        #中心检测
        img.find_edges(image.EDGE_CANNY, threshold=(50, 80))
        img.dilate(3)

        find_closest_blob(img)


        Display.show_image(img, picture_width, 0, layer = Display.LAYER_OSD1)

        #矩形检测
        img.erode(2)

        detect_rects(img)


        error_x, error_y, flag = caculate_error(red_x, red_y, sorted_points, corner_n)
        if flag:
            corner_n=(corner_n+1)%4

        print(error_x, error_y, flag)
        print(corner_n)
        print(red_x, red_y)











        #寻找高亮斑点
#        find_closest_blob(img)
#        print(middle_x, middle_y)





        # 这里可以插入各种图像处理逻辑，例如二值化、直方图均衡化、滤波等
        # 当前示例仅仅直接显示原图，不做任何操作

        #--------结束--------
        #绘制红色斑点
        if red_x != -1 and red_y != -1:
            img_raw.draw_circle(red_x, red_y, 10, color=(255, 0, 0), thickness=5)


        # 绘制四条边（自动闭合）
        if corner_rect:
#            img_raw.draw_rectangle(x_y_w_h_rect, color=(0, 0, 255), thickness=5)
            print(sorted_points)
            for i in range(4):
                start_point = sorted_points[i]
                end_point = sorted_points[(i + 1) % 4]  # 最后一个点连接第一个点
                img_raw.draw_line(start_point[0], start_point[1],
                                  end_point[0], end_point[1],
                                  color=(0, 0, 255), thickness=3)
            # 在屏幕右上角显示处理后的图像


        #绘制中心点
        if middle_x != -1 and middle_y != -1:
            img_raw.draw_circle(middle_x, middle_y, 10, color=(0, 255, 0), thickness=5)


        Display.show_image(img, 0, 0, layer = Display.LAYER_OSD2)
        Display.show_image(img_raw, 0, picture_height, layer = Display.LAYER_OSD0)

        # 打印帧率到控制台
        print(clock.fps())




except KeyboardInterrupt as e:
    print("用户停止: ", e)
except BaseException as e:
    print(f"异常: {e}")
finally:
    # 停止传感器运行
    if isinstance(sensor, Sensor):
        sensor.stop()
    # 反初始化显示模块
    Display.deinit()
    time.sleep_ms(100)
    # 释放媒体缓冲区
    MediaManager.deinit()


