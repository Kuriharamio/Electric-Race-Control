import math
import time
from machine import FPIOA, Pin
from media.sensor import *
from media.display import *
from media.media import *
import utime
fpioa = FPIOA()

#设置引脚功能(三个RGB灯)
fpioa.set_function(62, FPIOA.GPIO62)
fpioa.set_function(20, FPIOA.GPIO20)
fpioa.set_function(63, FPIOA.GPIO63)

LED_R = Pin(62, Pin.OUT, pull=Pin.PULL_UP, drive=15)
LED_G = Pin(20, Pin.OUT, pull=Pin.PULL_UP, drive=15)
LED_B = Pin(63, Pin.OUT, pull=Pin.PULL_UP, drive=15)
LED_R.value(1)
LED_G.value(1)
LED_B.value(1)

try:
    # 构造一个具有默认配置的摄像头对象
    sensor = Sensor(id=2,width=1920, height=1080)
    # 重置摄像头sensor
    sensor.reset()

    # 无需进行镜像翻转
    # 设置水平镜像
    # sensor.set_hmirror(False)
    # 设置垂直翻转
    # sensor.set_vflip(False)

    # 设置通道0的输出尺寸
    sensor.set_framesize(width=960, height=540, chn=CAM_CHN_ID_0)
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
        img_raw = sensor.snapshot(chn=CAM_CHN_ID_0)

        Display.show_image(img_raw, 0, 0, layer = Display.LAYER_OSD0)

        # 图像转换
        #对比度拉伸

#        img_raw.histeq(adaptive=False)
#        Display.show_image(img_raw, 960, 540, layer = Display.LAYER_OSD2)
        img_raw.binary([(95, 200)])
        Display.show_image(img_raw, 960, 0, layer = Display.LAYER_OSD1)

        # 逐行扫描处理图像
        height, width = img_raw.height(), img_raw.width()
        for y in range(height - 1, -1, -1):  # 从底向上逐行扫描
            startX = -1  # 记录黑色区域的起始位置
            endX = -1  # 记录黑色区域的结束位置

            for x in range(width):
                if img_raw.get_pixel(x, y) == 0:  # 找到黑色像素
                    if startX == -1:
                        startX = x  # 记录黑色区域的起始位置
                    endX = x  # 更新黑色区域的结束位置
                else:
                    if startX != -1:
                        # 检查是否为中间部分的黑色区域
                        if startX != 0 and endX != width - 1 and startX - endX>10:
                            midX = (startX + endX) // 2
                            img_raw.set_pixel(midX, y, 255)  # 将中线像素设置为白色
                            break



#        line = img_raw.get_regression([(0,0)])
#        if (line):

#            img_raw.draw_line(line.line(), color = 127,thickness=4)

#            print(line) #打印结果
        Display.show_image(img_raw, 0, 540, layer = Display.LAYER_OSD2)



        # 显示捕获的图像
        # 在屏幕右上角显示处理后的图像

        # 图像处理放到这里
        #--------开始--------
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
