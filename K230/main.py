import math
import time
from machine import FPIOA, Pin
from media.sensor import *
from media.display import *
from media.media import *
import utime
import datapack

fpioa = FPIOA()

picture_width = 640
picture_height = 360

middle_x=-1
middle_y=-1

n = 0

n_2 = 0

x_rotation = 0

flag_rect = True

flag_rect_2 = True

state = 3

red_x = -1
red_y = -1

sorted_points = []
sorted_points_2 = []

vertex_threshold_x = 3  # 阈值可以根据实际情况调整
vertex_threshold_y = 3
corner_n=0
# 颜色阈值（红色）
red_threshold = (0, 100, 24, 120, -32, 30)
green_threshold = (0, 100, -90, -20, -20, 100)

black_threshold = (0, 30, -50, 50, -50, 50)

fpioa.set_function(20, FPIOA.GPIO20)
LED_G = Pin(20, Pin.OUT, pull=Pin.PULL_UP, drive=15)
LED_G.value(1)


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
        largest_blob = max(blobs, key=lambda b: b[2] * b[3], default=None)
        if largest_blob:
            return largest_blob

    return None

def detect_rects(img):
    img.histeq(adaptive=False)


    img.find_edges(image.EDGE_CANNY, threshold=(40, 75))
    img.dilate(3)
    img.erode(2)

    Display.show_image(img, picture_width, picture_height, layer=Display.LAYER_OSD2)
    rects = img.find_rects(roi=(10, 10, picture_width-20, picture_height-20), threshold=200000)

    if rects:
        # 使用生成器表达式过滤矩形，并找到最大面积的矩形
        #矩形不会特别靠近左上角和右上角，h，w足够大，h，w差值小

        valid_rects = (rect for rect in rects
                       if (area := rect.magnitude()) > 5000
                       and rect.x() > 20
                       and rect.y() > 20
                       and rect.h() > 180
                       and rect.w() > 180
                       and rect.y() + rect.h()< picture_height - 40
                       and abs(rect.h() - rect.w()) < 80)

        if valid_rects:

            # 使用max函数找到最大面积的矩形
            selected_rect = max(valid_rects,
                                key=lambda r: r.magnitude(),
                                default=None)
            return selected_rect

    return None

def detect_rectangles(img, threshold):
    img.binary([threshold], invert=False)
    img.dilate(1)
    img.erode(1)
    # 高斯模糊去噪
    #img.gaussian(2)


    # Display.show_image(img, 0, picture_height, layer=Display.LAYER_OSD1)
    rects = img.find_rects(roi = (90, 10, 480, 320),threshold=100000)
    if rects:
        print(rects)
        #如果检测到两个矩形以上
        if len(rects) >= 2:
        #找到最大的两个矩形（h*w）
            valid_rects = (rect for rect in rects
                           if (area := rect.magnitude()) > 5000
                           and rect.x() > 15
                           and rect.y() > 15
                           and rect.h() > 70
                           and rect.w() > 70
                           and rect.y() + rect.h()< picture_height - 40)

            # 将生成器转换为列表
            valid_rects_list = list(valid_rects)
            if len(valid_rects_list) >= 2:
                valid_rects_list = sorted(valid_rects_list, key=lambda r: r.h() * r.w(), reverse=True)[:2]

                # 判断矩形之间是否大的包围小的（用x,y,h,w判断）
                if (valid_rects_list[0].x() < valid_rects_list[1].x()
                    and valid_rects_list[0].y() < valid_rects_list[1].y()
                    and valid_rects_list[0].h() > valid_rects_list[1].h()
                    and valid_rects_list[0].w() > valid_rects_list[1].w()):
                    return valid_rects_list[0], valid_rects_list[1]



    return None, None

        # valid_rects = (rect for rect in rects
        #                if (area := rect.magnitude()) > 5000
        #                and rect.x() > 15
        #                and rect.y() > 15
        #                and rect.h() > 70
        #                and rect.w() > 70
        #                and rect.y() + rect.h()< picture_height - 40)
        # selected_rect = max(valid_rects,
        #                     key=lambda r: r.magnitude(),
        #                     default=None)
        # if selected_rect is not None:
        #     return selected_rect
def sort_corners(rect):
    if rect is None:
        return None

     # 计算矩形中心
    center_x = rect.x() + rect.w() / 2
    center_y = rect.y() + rect.h() / 2

    # 获取矩形的角点
    corner_rect = rect.corners()

    # 按照每个点相对于中心的角度进行排序
    sorted_points = sorted(corner_rect, key=lambda p: (math.atan2(p[1] - center_y, p[0] - center_x)))

    # 调整顺序以确保顺时针排列
    if sorted_points[0][0] > sorted_points[1][0]:
        sorted_points.reverse()

    print(sorted_points)
    return sorted_points

def add_points(sorted_points, step_distance=10):

    # 创建一个函数来按指定距离插值
    def interpolate_points(points, distance):
        interpolated = []
        for i in range(len(points)):
            p1 = points[i]
            p2 = points[(i + 1) % len(points)]

            # 计算两点间距离
            total_distance = math.sqrt((p2[0] - p1[0])**2 + (p2[1] - p1[1])**2)

            # 如果两点间距离小于或等于指定距离，则不需要插值
            if total_distance <= distance:
                interpolated.append((p1[0], p1[1]))
                continue

            # 计算需要插入的点数
            num_points = int(total_distance / distance)
            if num_points == 0:
                num_points = 1

            # 计算方向向量
            dx = p2[0] - p1[0]
            dy = p2[1] - p1[1]

            # 计算单位向量
            norm = math.sqrt(dx*dx + dy*dy)
            unit_dx = dx / norm
            unit_dy = dy / norm

            # 插入点
            interpolated.append((p1[0], p1[1]))
            for j in range(1, num_points):
                new_x = int(p1[0] + unit_dx * distance * j)
                new_y = int(p1[1] + unit_dy * distance * j)
                interpolated.append((new_x, new_y))

        return interpolated

    # 初始点集
    current_points = sorted_points.copy()

    # 按指定距离插值一次
    current_points = interpolate_points(current_points, step_distance)

    # 确保最终的点集是闭合的
    current_points.append(current_points[0])
    current_points = [list(p) for p in current_points]
    print(current_points)

    return current_points

def sort_corners_and_add_points(rect):
    if rect is None:
        return None

    # 计算矩形中心
    center_x = rect.x() + rect.w() / 2
    center_y = rect.y() + rect.h() / 2

    # 获取矩形的角点
    corner_rect = rect.corners()

    # 按照每个点相对于中心的角度进行排序
    sorted_points = sorted(corner_rect, key=lambda p: (math.atan2(p[1] - center_y, p[0] - center_x)))

    # 调整顺序以确保顺时针排列
    if sorted_points[0][0] > sorted_points[1][0]:
        sorted_points.reverse()

    # 确定需要进行的插值次数
    num_interpolations = 4

    # 创建一个函数来执行一次插值操作
    def interpolate_points(points):
        interpolated = []
        for i in range(len(points)):
            p1 = points[i]
            p2 = points[(i + 1) % len(points)]
            # 线性插值计算插入点的坐标
            mid_x = (int)((p1[0] + p2[0]) / 2)
            mid_y = (int)((p1[1] + p2[1]) / 2)
            interpolated.append((mid_x, mid_y))
        return interpolated

    # 创建一个函数来合并原始点和插入的点
    def merge_points(original, interpolated):
        merged = []
        for i in range(len(original)):
            merged.append(original[i])
            merged.append(interpolated[i])
        return merged

    # 初始点集
    current_points = sorted_points.copy()

    # 进行多次插值
    for _ in range(num_interpolations):
        # 插值操作
        interpolated = interpolate_points(current_points)
        # 合并原始点和插入的点
        merged_points = []
        for i in range(len(current_points)):
            merged_points.append(current_points[i])
            merged_points.append(interpolated[i])
        current_points = merged_points

    # 确保最终的点集是闭合的
    current_points.append(current_points[0])
    current_points = [list(p) for p in current_points]

    return current_points


def caculate_error(x, y, corner, n, transition_threshold = 10):
    if corner is None or n >= len(corner):
        return 0, 0, False

    current_point = corner[n]
    # 计算到当前点的偏差
    error_x = current_point[0] - x
    error_y = current_point[1] - y

    # 判断是否临近当前点
    distance_to_current = ((x - current_point[0])**2 + (y - current_point[1])**2)**0.5

    if distance_to_current <= transition_threshold and n < len(corner) - 1:
        # 临近当前点，并且不是最后一个点，考虑下一个点的影响
        next_point = corner[n + 1]
        # 计算到下一个点的偏差
        error_to_next_x = next_point[0] - x
        error_to_next_y = next_point[1] - y

        # 根据距离当前点的远近，设置一个权重，距离越近，下一个点的影响权重越大
        weight = 1 - (distance_to_current / transition_threshold)
        # 混合当前点和下一个点的偏差
        error_x = error_x * (1 - weight) + error_to_next_x * weight
        error_y = error_y * (1 - weight) + error_to_next_y * weight

    # 判断是否到达当前点
    if distance_to_current <= transition_threshold * 0.5:
        # 到达当前点
        flag = True
    else:
        flag = False

    print(f'raw_error_x:{current_point[0] - x},raw_error_y:{-current_point[1] + y}')
    return error_x, error_y, flag

def caculate_error_2point(x, y, mid_x, mid_y):
    if mid_x == -1 or mid_y == -1:
        return 0, 0, False

    # 计算误差
    error_x = mid_x - x
    error_y = mid_y - y

    if abs(error_x) > vertex_threshold_x or abs(error_y) > vertex_threshold_y:
        flag = False
    else:
        flag = True

    return error_x, error_y, flag

def find_closest_blob(img, rect):
    target_x, target_y = int(rect.x() + rect.w() / 2) , int(rect.y() + rect.h() / 2) # 目标点坐标
    #保证target_x和target_y在范围内
    target_x = max(0, min(img.width() - 1, target_x))
    target_y = max(0, min(img.height() - 1, target_y))

    # 计算roi的边界
    roi_x = max(0, target_x - 40)
    roi_y = max(0, target_y - 40)
    roi_w = min(80, img.width() - roi_x)
    roi_h = min(80, img.height() - roi_y)

    # 设置roi
    roi = (roi_x, roi_y, roi_w, roi_h)


    # 查找blobs
    blobs = img.find_blobs([(240, 255)], roi=roi,
                           pixels_threshold=8, area_threshold=4)



    # 修改后的代码（带注释说明）
    if blobs:
        closest_blob = min(blobs,
                           key=lambda b: ((b[5] - target_x) ** 2 + (b[6] - target_y) ** 2),
                           default=None)

        if closest_blob:
            return closest_blob.cx(), closest_blob.cy()
    #如果没找到色块，就返回目标原定的位置
    return target_x, target_y


def get_state(uart):
    tem = uart.read(1)
    if tem:
        if tem[0] <4:
            state = tem[0]
            #print("state:", state)
            return state

    return 0
    #如果读取到数据，就把数据传给state



try:
    # 示例调用
    sensor, clock, success = initialize_camera_and_display()
    if success:
        print("success")
    else:
        print("fail")
    uart = datapack.Uart_Init()
    while True:
        #更新当前时间（毫秒）
        clock.tick()
        # 捕获通道0的图像
        img_raw = sensor.snapshot(chn=CAM_CHN_ID_0)
        img_raw.rotation_corr(x_rotation = x_rotation)
        # 显示捕获的图像
        # 在屏幕右上角显示处理后的图像

        # 图像处理放到这里
        #--------开始--------
        # 颜色块检测
        red_blob = detect_largest_blob(img_raw, red_threshold)
        if red_blob:
            red_x, red_y = red_blob[5], red_blob[6]

        tem = get_state(uart)
        if tem:
            state = tem
            print(state)
            # # 图像转换
        img_black_rect = img_raw.to_grayscale(copy=False)


        if state == 1:

            # 矩形检测
            if flag_rect:
                rect_black = detect_rects(img_black_rect)
                if rect_black:
                    sorted_points = sort_corners_and_add_points(rect_black)
                    # 中心点检测
                    middle_x, middle_y = find_closest_blob(img_black_rect, rect_black)
                    flag_rect = False
                    LED_G.value(0)
                #未检测到：修改x_rotation的数值(-20到30)
#               else:
#                   x_rotation = x_rotation + 1
#                   if x_rotation > 20:
#                       x_rotation = -20

            #计算并且传输误差
            if middle_x != -1 and middle_y != -1:
                error_x, error_y, flag = caculate_error(red_x, red_y, sorted_points, n)
                x = red_x
                y = red_y
                corner = sorted_points
                current_point = corner[n]
                # 计算到当前点的偏差
                error_x = current_point[0] - x
                error_y = current_point[1] - y

                if abs(error_x) > vertex_threshold_x or abs(error_y) > vertex_threshold_y:
                    flag = False
                else:
                    flag = True

                if error_x != -9999 and error_y != -9999:

                    datapack.send_packet(uart, [0.12 ,0.18, -error_y, 0.12, 0.18, error_x])
                    print(error_x, error_y)
                    #print(sorted_points)
                    if flag:
                        n = n + 1


            if middle_x != -1 and middle_y != -1:
                img_raw.draw_cross(middle_x, middle_y, size=10, thickness=5)
            if sorted_points:
                for point in sorted_points:
                    img_raw.draw_circle(point[0], point[1], 1, color=(0, 255, 0), thickness=1)

        elif state == 2:
            n=0
            n_2=0
            # 矩形检测
            if flag_rect:
                rect_black = detect_rects(img_black_rect)
                if rect_black:
                    sorted_points = sort_corners_and_add_points(rect_black)
                    # 中心点检测
                    middle_x, middle_y = find_closest_blob(img_black_rect, rect_black)
                    flag_rect = False
                    LED_G.value(0)

            if middle_x != -1 and middle_y != -1:
                error_x, error_y, flag =  caculate_error_2point(red_x, red_y, middle_x, middle_y)
                if error_x != -9999 and error_y != -9999:
                    datapack.send_packet(uart, [0.05 ,0.125, -error_y, 0.05, 0.125, error_x])
                    print(error_x, error_y)

            if middle_x != -1 and middle_y != -1:
                img_raw.draw_cross(middle_x, middle_y, size=10, thickness=5)

        elif state == 3:
            img_2 = img_raw.copy()

            if flag_rect_2:
                rect_rectangles_big, rect_rectangles_small = detect_rectangles(img_2, black_threshold)
                if rect_rectangles_big and rect_rectangles_small:
#                    #* 按距离
#                    sorted_points_big = sort_corners(rect_rectangles_big)
#                    sorted_points_small = sort_corners(rect_rectangles_small)
#                    #把以上的点求均值放入sorted_points_2中
#                    for i in range(len(sorted_points_big)):
#                        sorted_points_2.append((int((sorted_points_big[i][0] + sorted_points_small[i][0]) / 2),  int((sorted_points_big[i][1] + sorted_points_small[i][1]) / 2)))

#                    sorted_points_2 = add_points(sorted_points_2, 9)

                    #* 二分插值
                    sorted_points_big = sort_corners_and_add_points(rect_rectangles_big)
                    sorted_points_small = sort_corners_and_add_points(rect_rectangles_small)

                    #把以上的点求均值放入sorted_points_2中
                    for i in range(len(sorted_points_big)):
                        sorted_points_2.append((int((sorted_points_big[i][0] + sorted_points_small[i][0]) / 2),
                                                int((sorted_points_big[i][1] + sorted_points_small[i][1]) / 2),
                                                #  (sorted_points_big[i][2] + sorted_points_small[i][2]) / 2,
                                                #  (sorted_points_big[i][3] + sorted_points_small[i][3])/ 2
                                                ))

                    sorted_points_2 = [list(p) for p in sorted_points_2]
                    sorted_points_small = [list(p) for p in sorted_points_small]
                    for i in range(len(sorted_points_2) - 1):
                        if i % 8  == 0 :
                            sorted_points_2[i][0] = int(0.4 * sorted_points_small[i][0] + 0.6 * sorted_points_big[i][0])
                            sorted_points_2[i][1] = int(0.4 * sorted_points_small[i][1] + 0.6 * sorted_points_big[i][1])

                    flag_rect_2 = False
                    LED_G.value(0)
            #如果以已经成功检测完毕rect_rectangles，则进行下一步操作
            if sorted_points_2:

                if n_2 is len(sorted_points_2) - 1:
                    vertex_threshold_x = 2
                    vertex_threshold_y = 2

                error_x, error_y, flag = caculate_error(red_x, red_y, sorted_points_2, n_2)

                if error_x != -9999 and error_y != -9999:
                    # sin = ((sorted_points_2[n_2][3]))
                    # cos = ((sorted_points_2[n_2][2]))
#                    kp_x = 1.2
#                    ki_x = 1.8
#                    kp_y = 1.2
#                    ki_y = 1.8
                    kp_x = 0.2
                    ki_x = 2.6
                    kp_y = 0.2
                    ki_y = 2.6

                    careful_list = [13, 14, 15, 16, 17, 18, 29, 30, 31, 32, 33, 45, 46, 47, 48, 49]

                    for i in range(len(careful_list)):
                        if n_2 is careful_list[i]:
                            kp_x = 0.1
                            ki_x = 1.1
                            kp_y = 0.1
                            ki_y = 1.1
                            break


                    error_x = float(error_x/10)
                    error_y = float(error_y/10)

                    print(f'error_x:{error_x},error_y:{-error_y}')
                    if n_2 is 0:
                        datapack.send_packet(uart, [0.03, 1.5, -error_y, 0.03, 1.5, error_x])
                    else:
                        datapack.send_packet(uart, [kp_y, ki_y, -error_y , kp_x, ki_x, error_x])
#                    else:
#                        datapack.send_packet(uart, [0, 2, 0, 0, 2, 0])

                    #print(sorted_points_2)
                    #对于n_2不同的情况，发送不同的数据包
                    # if n_2 == 0:
                    #     datapack.send_packet(uart, [kp_y, ki_y, -error_y, kp_x, ki_x, error_x])
                    # elif n_2 == 1:
                    #     datapack.send_packet(uart, [kp_y, ki_y, -error_y, kp_x, ki_x, error_x])
                    #     # datapack.send_packet(uart, [0.06 * sorted_points_2[1][2], 11.2 * sorted_points_2[1][2], -error_y, 0.060 * sorted_points_2[1][3], 10 * sorted_points_2[1][3], error_x])
                    # elif n_2 == 2:
                    #     datapack.send_packet(uart, [kp_y, ki_y, -error_y, kp_x, ki_x, error_x])
                    #     # datapack.send_packet(uart, [0.06 * sorted_points_2[2][2], 11.2 * sorted_points_2[2][2], -error_y, 0.060 * sorted_points_2[2][3], 10 * sorted_points_2[2][3], error_x])
                    # elif n_2 == 3:
                    #     datapack.send_packet(uart, [kp_y, ki_y, -error_y, kp_x, ki_x, error_x])
                    #     # datapack.send_packet(uart, [0.06 * sorted_points_2[3][2], 11.2 * sorted_points_2[3][2], -error_y, 0.060 * sorted_points_2[3][3], 10 * sorted_points_2[3][3], error_x])
                    # else:
                    #     datapack.send_packet(uart, [kp_y, ki_y, -error_y, kp_x, ki_x, error_x])
                    #     # datapack.send_packet(uart, [0.06 * sorted_points_2[4][2], 11.2 * sorted_points_2[4][2], -error_y, 0.060 * sorted_points_2[4][3], 10 * sorted_points_2[4][3], error_x])


                    # datapack.send_packet(uart, [0.0011 ,0.11, -error_y, 0.0009, 0.11, error_x])
                    #datapack.send_packet(uart, [0.0006, 0.112, -error_y, 0.0004, 0.15, error_x])

                    if flag and n_2 <= len(sorted_points_2) - 1:
                        n_2 = n_2 + 1
#                        if n_2 is len(sorted_points_2) - 2:
#                            n_2 = 0


            if sorted_points_2:
                for point in sorted_points_2:
                    img_raw.draw_circle(point[0], point[1], 2, color=(0, 255, 0), thickness=1)

#        # 绘制颜色块的中心十字
        img_raw.draw_cross(red_x, red_y, size=5, thickness=2)
        Display.show_image(img_raw, 0, 0, layer=Display.LAYER_OSD0)

        # 打印帧率到控制台
#        print(clock.fps())

except KeyboardInterrupt as e:
    print("用户停止: ", e)
except BaseException as e:
    print(f"异常: {e}")
finally:
    datapack.send_packet(uart, [0, 2, 0, 0, 2, 0])
    # 停止传感器运行
    if isinstance(sensor, Sensor):
        sensor.stop()
    # 反初始化显示模块
    Display.deinit()
    time.sleep_ms(100)
    # 释放媒体缓冲区
    MediaManager.deinit()


