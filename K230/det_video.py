import os
import ujson
import aicube
from libs.PipeLine import ScopedTiming
from libs.Utils import *
from media.sensor import *
from media.display import *
from media.media import *
import nncase_runtime as nn
import ulab.numpy as np
import image
import gc
import utime
import datapack
import find_line

#显示像素大小 不改
DISPLAY_WIDTH = ALIGN_UP(480, 16)
DISPLAY_HEIGHT = 270
OUT_RGB888P_WIDTH = ALIGN_UP(480, 16)
OUT_RGB888P_HEIGH = 270

#各种阈值 不必要不用改 现在只用了red
red_threshold = [(0, 60, 17, 127, -40, 40)]
black_threshold = [(0, 30, -30, 10, -50, 50)]
blue_threshold = [(5, 95, -30, 30, -30, 30)]

#模型位置 不改
root_path="/sdcard/mp_deployment_source/"
config_path=root_path+"deploy_config.json"
deploy_conf={}
debug_mode=1

#模式：负责做判断：寻红线还是目标检测
STATE_REDLINE = 0
STATE_SIGN = 1
state_now = 1

# 计算图像缩放参数 不改
def two_side_pad_param(input_size,output_size):
    ratio_w = output_size[0] / input_size[0]  # 宽度缩放比例
    ratio_h = output_size[1] / input_size[1]   # 高度缩放比例
    ratio = min(ratio_w, ratio_h)  # 取较小的缩放比例
    new_w = int(ratio * input_size[0])  # 新宽度
    new_h = int(ratio * input_size[1])  # 新高度
    dw = (output_size[0] - new_w) / 2  # 宽度差
    dh = (output_size[1] - new_h) / 2  # 高度差
    top = int(round(dh - 0.1))
    bottom = int(round(dh + 0.1))
    left = int(round(dw - 0.1))
    right = int(round(dw - 0.1))
    return top, bottom, left, right,ratio

#读取模型配置文件 不改
def read_deploy_config(config_path):
    # 打开JSON文件以进行读取deploy_config
    with open(config_path, 'r') as json_file:
        try:
            # 从文件中加载JSON数据
            config = ujson.load(json_file)
        except ValueError as e:
            print("JSON 解析错误:", e)
    return config

#处理目标检测得到的目标 要自己写
def choose_box(det_boxes):
    #只检测到一个 直接返回
    if(len(det_boxes)==1):
        return det_boxes[0]

    #过滤易出现的值
    # filtered_boxes = [box for box in det_boxes if box[0] != "forward"]
    filtered_boxes = [box for box in det_boxes] 

    if not filtered_boxes:
        # 如果过滤后没有检测框，返回置信度最高的原始检测框
        return max(det_boxes, key=lambda x: x[1])

    # 返回过滤后置信度最高的检测框
    return max(filtered_boxes, key=lambda x: x[1])



#主函数运行的函数
# #主函数运行的函数
def detection():
    #AI模块初始化 不改
    print("det_infer start")
    # 使用json读取内容初始化部署变量
    deploy_conf=read_deploy_config(config_path)
    kmodel_name=deploy_conf["kmodel_path"]
    labels=deploy_conf["categories"]
    confidence_threshold= deploy_conf["confidence_threshold"]
    nms_threshold = deploy_conf["nms_threshold"]
    img_size=deploy_conf["img_size"]
    num_classes=deploy_conf["num_classes"]
    color_four=get_colors(num_classes)
    nms_option = deploy_conf["nms_option"]
    model_type = deploy_conf["model_type"]
    if model_type == "AnchorBaseDet":
        anchors = deploy_conf["anchors"][0] + deploy_conf["anchors"][1] + deploy_conf["anchors"][2]
    kmodel_frame_size = img_size
    frame_size = [OUT_RGB888P_WIDTH,OUT_RGB888P_HEIGH]
    strides = [8,16,32]
    # 计算padding值
    top, bottom, left, right,ratio=two_side_pad_param(frame_size,kmodel_frame_size)
    # 初始化kpu
    kpu = nn.kpu()
    kpu.load_kmodel(root_path+kmodel_name)
    # 初始化ai2d
    ai2d = nn.ai2d()
    ai2d.set_dtype(nn.ai2d_format.NCHW_FMT,nn.ai2d_format.NCHW_FMT,np.uint8, np.uint8)
    ai2d.set_pad_param(True, [0,0,0,0,top,bottom,left,right], 0, [114,114,114])
    ai2d.set_resize_param(True, nn.interp_method.tf_bilinear, nn.interp_mode.half_pixel )
    ai2d_builder = ai2d.build([1,3,OUT_RGB888P_HEIGH,OUT_RGB888P_WIDTH], [1,3,kmodel_frame_size[1],kmodel_frame_size[0]])
    rgb888p_img = None
    ai2d_input_tensor = None
    data = np.ones((1,3,kmodel_frame_size[1],kmodel_frame_size[0]),dtype=np.uint8)
    ai2d_output_tensor = nn.from_numpy(data)
    # ___________________________________


    # 初始化摄像头和显示~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    #调用外部两个py文件
    sensor, osd_img, clock = find_line.initialize_camera_and_display()
    uart = datapack.Uart_Init()
    # 初始化摄像头和显示~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    # 主循环
    while  True:
        with ScopedTiming("total",debug_mode > 0):
            # 判断目前的状态，决定执行哪部分代码
            if state_now == STATE_REDLINE:
                # 寻红线代码
                # 获取图像~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                # 更新当前时间（毫秒）
                clock.tick()
                # 捕获通道0的图像
                img_raw = sensor.snapshot(chn=CAM_CHN_ID_0)
                img_find_redline = img_raw.copy()
                # 获取图像~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

                # 处理图像~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                # 寻找红色色块并显示
                img_raw.binary(red_threshold, invert=False)
                Display.show_image(img_raw, 600, 600, layer = Display.LAYER_OSD0)
                # 用红色拟合直线并显示，得到误差数据
                error = find_line.Get_Line_Error(img_find_redline)#find_line.py里
                Display.show_image(img_find_redline, 600, 0, layer = Display.LAYER_OSD1)
                # 处理图像~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

                # --------发送数据接收数据--------
                datapack.send_packet(uart, [error])
                print("Packet sent:", [error])
            # --------结束--------


            elif state_now == STATE_SIGN:
                # 目标检测
                # 获取图像~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                rgb888p_img = sensor.snapshot(chn=CAM_CHN_ID_2)

                img_raw = sensor.snapshot(chn=CAM_CHN_ID_0)


                # 获取图像~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


                # 处理图像~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

                # 跑模型 不改
                if rgb888p_img.format() == image.RGBP888:
                    ai2d_input = rgb888p_img.to_numpy_ref()
                    ai2d_input_tensor = nn.from_numpy(ai2d_input)
                    # 使用ai2d进行预处理
                    ai2d_builder.run(ai2d_input_tensor, ai2d_output_tensor)
                    # 设置模型输入
                    kpu.set_input_tensor(0, ai2d_output_tensor)
                    # 模型推理
                    kpu.run()
                    # 获取模型输出
                    results = []
                    for i in range(kpu.outputs_size()):
                        out_data = kpu.get_output_tensor(i)
                        result = out_data.to_numpy()
                        result = result.reshape((result.shape[0]*result.shape[1]*result.shape[2]*result.shape[3]))
                        del out_data
                        results.append(result)
                    # 使用aicube模块封装的接口进行后处理
                    det_boxes = aicube.anchorbasedet_post_process( results[0], results[1], results[2], kmodel_frame_size, frame_size, strides, num_classes, confidence_threshold, nms_threshold, anchors, nms_option)
                    # 绘制结果
                    osd_img.clear()
                    # det_boxes 是目标的框
                    if det_boxes:

                        #想办法消去易混绕的值 前面定义的函数
                        det_boxe = choose_box(det_boxes)

                        # 画图 不改
                        x1, y1, x2, y2 = det_boxe[2],det_boxe[3],det_boxe[4],det_boxe[5]
                        x=int(x1 * DISPLAY_WIDTH // OUT_RGB888P_WIDTH)
                        y=int(y1 * DISPLAY_HEIGHT // OUT_RGB888P_HEIGH)
                        w = int((x2 - x1) * DISPLAY_WIDTH // OUT_RGB888P_WIDTH)
                        h = int((y2 - y1) * DISPLAY_HEIGHT // OUT_RGB888P_HEIGH)
                        osd_img.draw_rectangle(x, y, w, h, color=color_four[det_boxe[0]][1:])
                        text=labels[det_boxe[0]] + " " + str(round(det_boxe[1],2))
                        osd_img.draw_string_advanced(x,y-40,32,text, color=color_four[det_boxe[0]][1:])

                    Display.show_image(osd_img, 0, 0, Display.LAYER_OSD3)
                    Display.show_image(img_raw, 0, 0, Display.LAYER_OSD2)



                    gc.collect()
                rgb888p_img = None

                # 处理图像~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


    # 离开while 不改
    del ai2d_input_tensor
    del ai2d_output_tensor
    #停止摄像头输出
    sensor.stop()
    #去初始化显示设备
    Display.deinit()
    #释放媒体缓冲区
    MediaManager.deinit()
    gc.collect()
    time.sleep(1)
    nn.shrink_memory_pool()
    print("det_infer end")
    return 0

if __name__=="__main__":
    detection()
