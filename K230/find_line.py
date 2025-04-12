
from media.sensor import *
from media.display import *
from media.media import *
import utime

red_threshold = [(0, 60, 17, 127, -40, 40)]
black_threshold = [(0, 30, -30, 10, -50, 50)]
blue_threshold = [(5, 95, -30, 30, -30, 30)]
def initialize_camera_and_display():

    # 初始化并配置sensor
    sensor = Sensor(id=2, width=480, height=270)
    sensor.reset()
    # 设置水平镜像垂直镜像
    sensor.set_vflip(True)
    sensor.set_hmirror(True)
    # 通道0直接给到显示VO，格式为RGB565
    sensor.set_framesize(width = 480, height = 270)
    sensor.set_pixformat(Sensor.RGB565, chn=CAM_CHN_ID_0)
    # 通道2给到AI做算法处理，格式为RGB888
    sensor.set_framesize(width = 480 , height = 270, chn=CAM_CHN_ID_2)
    sensor.set_pixformat(PIXEL_FORMAT_RGB_888_PLANAR, chn=CAM_CHN_ID_2)


    # 设置为LT9611显示，默认1920x1080
    Display.init(Display.LT9611, to_ide = True)
    #创建OSD图像
    osd_img = image.Image(480, 270, image.ARGB8888)
    # media初始化
    MediaManager.init()
    # 启动sensor
    sensor.run()
    clock = utime.clock()

    return sensor,osd_img,clock



def Get_Line_Error(img):
    #分为9个部分获取统计数据，放在数组中
    sta = [0]*18
    sta_mean_max = 10
    sta_mean_max_index = 9
    for i in range(18):
        sta[i] = img.get_statistics(red_threshold, roi=(i*25+15, 120, 25 ,100)).mean()
    for i in range(18):
        if sta[i] > sta_mean_max:
            sta_mean_max = sta[i]
            sta_mean_max_index = i

    return sta_mean_max_index*0.1-0.9