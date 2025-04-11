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

picture_width = 960
picture_height = 540
sensor = None
middle_flag=True
middle_x=0
middle_y=0

# 颜色阈值（红色）
color_threshold = [(19, 33, 20, 127, 1, 26)]


# def find_middle_spot(img):
#     global middle_flag, middle_x, middle_y  # 声明使用全局变量
#
#     if middle_flag:
#         blobs = img.find_blobs([(240, 255)], roi=(400, 200, 160, 140),
#                                pixels_threshold=50,
#                                area_threshold=4)
#         if blobs:
#             for blob in blobs:
#                 if blob[4] < 200:
#                     middle_x = blob[5]
#                     middle_y = blob[6]
#                     print(blob)
#                     middle_flag = False



def find_line_spot(img):

    if line_flag:
        lines = img.find_lines(roi=(400, 200, 160, 140), pixels_threshold=50,
                               area_threshold=4, merge=True)
        if lines:
            for line in lines:
                img.draw_line(line.line(), color=(1, 147, 230), thickness=3)  # 绘制线段
                print(line)
                line_flag = False
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
    sensor = Sensor(id=2,width=960, height=540)
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
    sensor.set_pixformat(Sensor.GRAYSCALE, chn=CAM_CHN_ID_0)

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
        img = sensor.snapshot(chn=CAM_CHN_ID_0)




        Display.show_image(img, 0, 0, layer = Display.LAYER_OSD0)
        # 显示捕获的图像
        # 在屏幕右上角显示处理后的图像

        # 图像处理放到这里
        #--------开始--------


        img.histeq(adaptive=False)
        Display.show_image(img, picture_width, 0, layer = Display.LAYER_OSD2)



        #轮廓检测
        img.find_edges(image.EDGE_CANNY, threshold=(40, 80))

        img.dilate(2)
        find_middle_spot(img)
        # # 寻找高亮斑点
        # if middle_flag:
        #     blobs = img.find_blobs([(240, 255)],roi=(400, 200, 160, 140),
        #                            pixels_threshold=50,
        #                            area_threshold=4)
        #
        #     if blobs:
        #         for blob in blobs:
        #             if blob[4]<200:
        #                 middle_x=blob[5]
        #                 middle_y=blob[6]
        #                 print(blob)
        #                 middle_flag=False

        print(middle_x,middle_y)

        #        for l in img.find_line_segments(merge_distance = 100, max_theta_diff = 15):

#            img.draw_line(l.line(), color = (255, 0, 0), thickness=2)
#            print(l)

#        else:
#            print("No")








#        rects=img.find_rects(threshold=1000)
#        if rects:
#            for rect in rects:
#                # 若想获取更详细的四个顶点，可使用 rect.corners()，该函数会返回一个有四个元祖的列表，每个元组代表矩形的四个顶点，从左上角开始，按照顺时针排序。
#                # img.draw_rectangle(rect.rect(), color=(1, 147, 230), thickness=3)  # 绘制线段
#                print(rect)
#        else:
#            print("no rect")


        # 这里可以插入各种图像处理逻辑，例如二值化、直方图均衡化、滤波等
        # 当前示例仅仅直接显示原图，不做任何操作

        #--------结束--------

        # 在屏幕右上角显示处理后的图像

        Display.show_image(img, 0, picture_height, layer = Display.LAYER_OSD1)


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


