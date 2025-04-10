#摄像头读取
import time,  sys

from media.sensor import *
from media.display import *
from media.media import *

import utime


picture_width = 960
picture_height = 540
sensor = None

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
    sensor.set_framesize(width=picture_width, height=picture_height, chn=CAM_CHN_ID_0)
    # 设置通道0的输出像素格式为RGB888
    sensor.set_pixformat(Sensor.RGB565, chn=CAM_CHN_ID_0)
    # 使用IDE的帧缓冲区作为显示输出
    Display.init(Display.VIRT, width=1920, height=1080, to_ide=True)
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
        img_gray=img.to_grayscale()

        # 这里可以插入各种图像处理逻辑，例如二值化、直方图均衡化、滤波等
        # 当前示例仅仅直接显示原图，不做任何操作

        #--------结束--------

        # 在屏幕右上角显示处理后的图像
        Display.show_image(img_gray, picture_width, 0,layer = Display.LAYER_OSD1)

        # 打印帧率到控制台


        print("fps=", clock.fps())

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


