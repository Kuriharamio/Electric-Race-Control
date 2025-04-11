from re import search

from machine import FPIOA
from machine import Pin
import time
# 实例化FPIOA
fpioa = FPIOA()
# 打印所有引脚配置
fpioa.help()

#设置引脚功能
fpioa.set_function(9, FPIOA.GPIO9)
pin=Pin(9,Pin.OUT,pull=Pin.PULL_NONE,drive=7))
#或者使用 pin.init(Pin.OUT,pull=Pin.PULL_NONE,drive=7)

fpioa.get_pin_func(pin)
fpioa.get_pin_num(func)

# 设置引脚输出电平
pin.value(1)
pin.on()
pin.high()

pin.value(0)
pin.off()
pin.low()
# 读取引脚输入电平
pin.value()

# 设置引脚模式
pin.mode(Pin.IN)
pin.mode(Pin.OUT)
#读取引脚模式
pin.mode()

#设置上下拉
pin.pull(Pin.PULL_UP)
pin.pull(Pin.PULL_DOWN)
#读取上下拉
pin.pull()

#设置驱动能力
pin.drive(7)
#读取驱动能力
pin.drive()

#休眠一定时间
seconds = 1
time.sleep(seconds)


#PWM输出
#注意通道 0、1 和 2 输出频率相同，通道 3、4 和 5 输出频率也相同
fpioa.set_function(43, FPIOA.PWM1)
beep_pwm = PWM(1, 4000, 50, enable=False)  # 1通道 频率4kHz 占空比50% 未打开
beep_pwm.enable(1)

#读取频率
beep_pwm.freq()
#读取占空比
beep_pwm.duty()
#结束后关闭PWM
beep_pwm.deinit()


#定时器的使用
from machine import Timer
#创建定时器
tim = Timer(-1) #表示使用软件定时器

#配置定时器
tim.init(period=1000, mode=Timer.PERIODIC, callback=lambda t:print("Hello World"))
#Timer.PERIODIC 周期定时
#Timer.ONE_SHOT 一次性定时
#callback 超时回调函数

#删除定时器
Timer.deinit()







#UART串口通信
from machine import UART
# 将指定引脚配置为 UART 功能
fpioa.set_function(11, FPIOA.UART2_TXD)
fpioa.set_function(12, FPIOA.UART2_RXD)

# 初始化UART2，波特率115200，8位数据位，无校验，1位停止位
uart = UART(UART.UART2, baudrate=115200, bits=UART.EIGHTBITS, parity=UART.PARITY_NONE, stop=UART.STOPBITS_ONE)

#读取数据
uart.read() #如果填写数字则读取指定长度的数据
uart.readline() #读取一行数据











#使用摄像头
from media.sensor import *
from media.display import *
from media.media import *
# 实例化摄像头
sensor = Sensor(id=2, width=1920, height=1080, fps=30)
#复位摄像头
sensor.reset()
#设置指定通道的输出图像尺寸
sensor.set_framesize(chn=CAM_CHN_ID_0, width=320, height=240)
#framesize会自动设置分辨率，设置完之后，不用设置输出图像尺寸

#配置指定通道输出图像格式
sensor.set_pixformat( Sensor.RGB888,chn=CAM_CHN_ID_0)
#RGB565
#RGB888
#GRAYSCALE

#sensor.set_hmirror(enable) #水平镜像
#sensor.set_vflip(enable)   #垂直镜像

#启动图像输出
sensor.run()

#停止图像输出
sensor.stop()

#获取图像一帧数据
sensor.snapshot(chn=CAM_CHN_ID_0)

#显示图像
Display.show_image(img)







#图像的绘制
# 创建绘制的图像
img = image.Image(width, height, image.ARGB8888)
#文字
img.draw_string_advanced(10, 10, 32, "你好世界", color=(255, 0, 0))
#画线
img.draw_line(10, 10, 100, 100, color=(255, 0, 0))
#画矩形
img.draw_rectangle(20, 20, 50, 30, color=(0, 255, 0), thickness=2)
#画圆
img.draw_circle(60, 60, 30, color=(0, 0, 255), thickness=3)
#画十字交叉
img.draw_cross(40, 40, color=(255, 255, 0), size=10, thickness=2)
#画箭头线
img.draw_arrow(10, 10, 100, 100, color=(255, 0, 0), thickness=2)
#画椭圆
img.draw_ellipse(60, 60, 30, 20, color=(0, 0, 255), thickness=3)
#图像画另一个图像
image.draw_image(img, x, y, alpha=128, scale=1.0)
#绘制关键点
keypoints = [(30, 30, 0), (50, 50, 90), (70, 70, 180)]
img.draw_keypoints(keypoints, size=10, color=(255, 255, 0), thickness=2)
#洪水填充算法
img.flood_fill(30, 30, color=(255, 0, 0), threshold=30, invert=False, clear_background=False)
#画字符串
img.draw_string(10, 10, "Hello OpenMV", color=(255, 255, 255), scale=2)



#图像转换
img_gray=img.to_grayscale(copy=False)

img_rgb565=img.to_rgb565(copy=False)

#图像复制
img_copy=img.copy(roi=(0, 0, 1920, 1080))



#图像处理
#对比度拉伸
img.histeq(adaptive=False)

#gamma矫正
img.gamma_corr(3)
#当 Gamma 值 >1 时，图像的中间灰度变亮，高亮区域对比度变强
#当 Gamma 值 <1 时，图像的中间灰度变暗，整体对比会变弱

#二值化
img.binary([(120, 255)],invert=False)

#轮廓检测
img.find_edges(image.EDGE_CANNY, threshold=(50, 80))
img.find_edges(image.EDGE_SIMPLE, threshold=(50, 255))

#拉普拉斯边缘检测
src_img.laplacian(2,mul=0.2) #2指5x5内核

image.gaussian(2)

image.bilateral(2)

#腐蚀和膨胀
img.erode(2)
img.dilate(2)

#线段检测
lines=img.find_line_segments(roi, merge_distance=0, max_theta_diffience=15)
#参数说明：
# roi：ROI区域，默认为整个图像
# merge_distance：合并距离，默认为0
# max_theta_diffience：最大角度差，默认为15
for line in lines:
    img.draw_line(line.line(), color=(1, 147, 230), thickness=3)  # 绘制线段
    print(line)

#矩形检测
rects=img.find_rects(roi, threshold=10000)
#参数说明：
# roi：ROI区域，默认为整个图像
# threshold：阈值，默认为10000
for rect in rects:
# 若想获取更详细的四个顶点，可使用 rect.corners()，该函数会返回一个有四个元祖的列表，每个元组代表矩形的四个顶点，从左上角开始，按照顺时针排序。
    img.draw_rectangle(rect.rect(), color=(1, 147, 230), thickness=3)  # 绘制线段
    print(rect)

#圆形检测
circles=img.find_circles(roi, x_stride=2, y_stride=1, threshold=10000)


#利用FOG替换像素
img.find_hog()


#NCC模板匹配
img.find_template(template, threshold=0.8, roi=(0, 0, 1920, 1080), step=2, search=SEARCH_EX)
#template 是一个小图像，需要与大图像进行匹配（灰度图像）

#特征匹配
img.find_features(template, threshold=0.8, scale=1.0, roi=(0, 0, 1920, 1080))





#打开和关闭的顺序
# 使用IDE的帧缓冲区作为显示输出
Display.init(Display.VIRT, width=1920, height=1080, to_ide=True)
#初始化媒体管理器
MediaManager.init()
#启动传感器
sensor.run()


#关闭传感器
sensor.stop()
#反初始化显示模块
Display.deinit()
#释放媒体缓冲区
MediaManager.deinit()




#WLAN网络连接
import network
# 启用 STA 模式并连接到 WiFi 接入点
nic = network.WLAN(network.STA_IF)
nic.active(True)
nic.connect('your-ssid', 'your-password')
# 配置完成后，即可像往常一样使用 socket


#定时器

# RGB灯控制
# from machine import FPIOA
# from machine import Pin
# import time
#
# # 实例化FPIOA
# fpioa = FPIOA()
# # 打印所有引脚配置
# fpioa.help()
#
# #设置引脚功能
# fpioa.set_function(62, FPIOA.GPIO62)
# fpioa.set_function(20, FPIOA.GPIO20)
# fpioa.set_function(63, FPIOA.GPIO63)
#
# LED_R = Pin(62, Pin.OUT, pull=Pin.PULL_UP, drive=15)
# LED_G = Pin(20, Pin.OUT, pull=Pin.PULL_UP, drive=15)
# LED_B = Pin(63, Pin.OUT, pull=Pin.PULL_UP, drive=15)
# LED_R.value(0)
# LED_G.value(0)
# LED_B.value(0)
#
# while True:
#     for i in range(8):
#         # 按位操作设置LED状态
#         LED_R.value((i >> 2) & 0x01)  # 二进制第三位控制红灯
#         LED_G.value((i >> 1) & 0x01)  # 二进制第二位控制绿灯
#         LED_B.value( i       & 0x01)  # 二进制第一位控制蓝灯
#         time.sleep(0.5)  # 每个状态保持1秒
#     print("hhh")



#按键中断
# 主循环
# while True:
#     button_state = button.value()  # 获取当前按键状态
#     current_time = time.ticks_ms()  # 获取当前时间（单位：毫秒）
#
#     # 检测按键从未按下(0)到按下(1)的变化（上升沿）
#     if button_state == 1 and button_last_state == 0:
#         # 检查按键是否在消抖时间外
#         if current_time - last_press_time > debounce_delay:
#             # 切换LED的状态
#             if led_on:
#                 LED.high()  # 熄灭LED
#             else:
#                 LED.low()   # 点亮LED
#
#             led_on = not led_on  # 反转LED状态
#             last_press_time = current_time  # 更新按键按下时间
#
#     # 更新上次按键状态
#     button_last_state = button_state
#
#     # 简单延时，防止主循环过于频繁
#     time.sleep_ms(10)



def lckfb_save_jpg(img, filename, quality=95):
    """
    将图像压缩成JPEG后写入文件 (不依赖第一段 save_jpg/MediaManager.convert_to_jpeg 的写法)
    :param img:    传入的图像对象 (Sensor.snapshot() 得到)
    :param filename: 保存的目标文件名 (含路径)
    :param quality:  压缩质量 (1-100)
    """
    compressed_data = img.compress(quality=quality)

    with open(filename, "wb") as f:
        f.write(compressed_data)

    print(f"[INFO] 使用 lckfb_save_jpg() 保存完毕: {filename}")


# #摄像头读取
# import time,  sys
#
# from media.sensor import *
# from media.display import *
# from media.media import *
# import utime
#
#
# sensor = None
#
# try:
#     # 构造一个具有默认配置的摄像头对象
#     sensor = Sensor(id=2)
#     # 重置摄像头sensor
#     sensor.reset()
#
#     # 无需进行镜像翻转
#     # 设置水平镜像
#     # sensor.set_hmirror(False)
#     # 设置垂直翻转
#     # sensor.set_vflip(False)
#
#     # 设置通道0的输出尺寸为1920x1080
#     sensor.set_framesize(Sensor.FHD, chn=CAM_CHN_ID_0)
#     # 设置通道0的输出像素格式为RGB888
#     sensor.set_pixformat(Sensor.RGB888, chn=CAM_CHN_ID_0)
#
#     # 使用IDE的帧缓冲区作为显示输出
#     Display.init(Display.VIRT, width=1920, height=1080, to_ide=True)
#     # 初始化媒体管理器
#     MediaManager.init()
#     # 启动传感器
#     sensor.run()
#
#     #构造clock
#     clock = utime.clock()
#
#     while True:
#         #更新当前时间（毫秒）
#         clock.tick()
#         # 捕获通道0的图像
#         img = sensor.snapshot(chn=CAM_CHN_ID_0)
#         # 显示捕获的图像
#         Display.show_image(img)
#
#
#         print("fps=", clock.fps())
#
# except KeyboardInterrupt as e:
#     print("用户停止: ", e)
# except BaseException as e:
#     print(f"异常: {e}")
# finally:
#     # 停止传感器运行
#     if isinstance(sensor, Sensor):
#         sensor.stop()
#     # 反初始化显示模块
#     Display.deinit()
#     time.sleep_ms(100)
#     # 释放媒体缓冲区
#     MediaManager.deinit()
