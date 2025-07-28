# ======================
# 导入必要模块
# ======================
import cv2
import numpy as np
# import onnxruntime as ort
import time
# import datapack


# ======================
# 设定的常量区
# ======================
img_w = 640  # 图像宽度
img_h = 480  # 图像高度
kernel_3 = np.ones((3, 3), np.uint8)
kernel_5 = np.ones((5, 5), np.uint8)

red_low = np.array([150, 0, 200])
red_high = np.array([10, 30, 255])


# ======================
# 变量区
# ======================
flag_1 = False

flag_state_1 = True

flag_state_2 = True

#黑色框的内外框
blobs_black = None
blobs_black_index = 0

middle_red =  None

state = 2


# ======================
#  函数区域
# ======================



def find_approx_polygon(img, n_points, min_area=10, epsilon_factor=0.02, show = False):
    """
    找到图像中近似为n边形的轮廓
    input:
    img: 灰度图像
    n_points: 多边形顶点数
    min_area: 最小面积阈值，默认为10像素
    epsilon_factor: 近似精度因子，默认为0.02

    output:
    polygons: 所有符合的近似多边形轮廓
    """
    # Canny边缘检测
    img_canny = cv2.Canny(img, 50, 100)

    # 后处理
    #img_close = cv2.morphologyEx(img_canny, cv2.MORPH_CLOSE, kernel_5)

    if show:
        cv2.imshow("img_canny", img_canny)

    # 找轮廓
    contours, hierarchy = cv2.findContours(img_canny, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    polygons = []
    for contour in contours:
        # 根据最小面积过滤轮廓
        if cv2.contourArea(contour) >= min_area:
            # 计算轮廓周长
            perimeter = cv2.arcLength(contour, True)
            # 设置近似精度
            epsilon = epsilon_factor * perimeter
            # 近似多边形
            approx = cv2.approxPolyDP(contour, epsilon, True)

            # 检查近似多边形是否具有指定的顶点数
            if len(approx) == n_points:
                polygons.append(approx)



    return polygons


# ... existing code ...

def find_color_blob(img, hsv_low, hsv_high, max_area=200, show=False):
    """
    找到指定hsv颜色的色块轮廓
    input:
    img:BGR彩色图像
    hsv_low: 低阈值
    hsv_high: 高阈值
    min_area: 最小面积阈值，默认为10像素

    output:
    contour:所有符合的轮廓
    """

    #模糊
    # img = cv2.GaussianBlur(img, (5, 5), 0)
    # 转为HSV
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)

    # 处理H值跨越边界的情况（如红色检测：H在170到10之间）
    if hsv_low[0] > hsv_high[0]:
        # 创建两个阈值范围：一个从hsv_low[0]到180，另一个从0到hsv_high[0]
        hsv_low1 = np.array([hsv_low[0], hsv_low[1], hsv_low[2]])
        hsv_high1 = np.array([180, hsv_high[1], hsv_high[2]])
        hsv_low2 = np.array([0, hsv_low[1], hsv_low[2]])
        hsv_high2 = np.array([hsv_high[0], hsv_high[1], hsv_high[2]])

        # 分别进行二值化
        img_hsv1 = cv2.inRange(hsv, hsv_low1, hsv_high1)
        img_hsv2 = cv2.inRange(hsv, hsv_low2, hsv_high2)

        # 合并两个结果
        img_hsv = cv2.bitwise_or(img_hsv1, img_hsv2)
    else:
        # HSV的二值化
        img_hsv = cv2.inRange(hsv, hsv_low, hsv_high)

    # 后处理
    # 使白色噪声消除

    #img_hsv = cv2.morphologyEx(img_hsv, cv2.MORPH_CLOSE, kernel_3)
    img_hsv = cv2.dilate(img_hsv, kernel_5)

    if show:
        cv2.imshow("img_hsv", img_hsv)

    # 找轮廓
    contours, hierarchy = cv2.findContours(img_hsv, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    # 根据最小面积过滤轮廓
    filtered_contours = [contour for contour in contours if cv2.contourArea(contour) <= max_area]

    return filtered_contours


# ... existing code ...


def find_binary_approx_polygon(img, n_points, min_area=10, max_area=20000, epsilon_factor=0.02, show = False):
    """
    找到图像中指定二值化后，且近似为n边形的轮廓
    input:
    img: 灰度图像
    n_points: 多边形顶点数
    min_area: 最小面积阈值，默认为10像素
    epsilon_factor: 近似精度因子，默认为0.02

    output:
    polygons: 所有符合的近似多边形轮廓
    """
    #二值化
    ret, img_binary = cv2.threshold(img, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)

    # 后处理 处理黑框中白色噪声
    img_binary = cv2.morphologyEx(img_binary, cv2.MORPH_CLOSE, kernel_3)




    if show:
        cv2.imshow("img_binary", img_binary)


    #找轮廓
    contours,  hierarchy = cv2.findContours(img_binary, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
    polygons = []
    for contour in contours:
        # 根据最小面积过滤轮廓
        if cv2.contourArea(contour) >= min_area and cv2.contourArea(contour) <= max_area:
            # 计算轮廓周长
            perimeter = cv2.arcLength(contour, True)
            # 设置近似精度
            epsilon = epsilon_factor * perimeter
            # 近似多边形
            approx = cv2.approxPolyDP(contour, epsilon, True)

            # 检查近似多边形是否具有指定的顶点数
            if len(approx) == n_points:
                polygons.append(approx)



    return polygons


def check_contour_containment(contours):
    """
    判断两个四边形轮廓是否包含关系（较大轮廓包含较小轮廓）

    参数:
        contours: 包含两个四边形轮廓的列表，每个轮廓是4个点的数组

    返回:
        bool: 如果较小轮廓完全在较大轮廓内则返回True，否则返回False
    """
    # 确保输入是两个轮廓
    if len(contours) != 2:
        return False


    large_contour = contours[0].reshape(-1, 2).astype(np.float32)  # 较大轮廓的4个点
    small_contour = contours[1].reshape(-1, 2)  # 较小轮廓的4个点

    # 检查较小轮廓的所有点是否都在较大轮廓内或边界上
    for i in range(len(small_contour)):
        # 获取点坐标并转换为元组
        x, y = small_contour[i]
        pt = (float(x), float(y))

        # 检测点是否在轮廓内
        #   +1 -> 内部
        #   -1 -> 外部
        #   0  -> 边界
        if cv2.pointPolygonTest(large_contour, pt, False) < 0:
            return False

    return True

def find_center_contour_clockwise(outer_contour, inner_contour):
    """
    找到两个四边形轮廓的顺时针方向
    参数:
        outer_contour: 较大轮廓的4个点
        inner_contour: 较小轮廓的4个点

    返回:
        数组：按逆时针排序的（4，2）
    """
    output = []
    if outer_contour.shape[0] != 4 or inner_contour.shape[0] != 4:
        return output
    outer_points = outer_contour.reshape(4, 2)
    inner_points = inner_contour.reshape(4, 2)

    #排序
    outer_points = sort_points_clockwise(outer_points)
    inner_points = sort_points_clockwise(inner_points)

    #使第一个点对齐
    reference_point = outer_points[0]

    # 计算该点与inner_points中每个点的距离
    distances = np.sqrt(np.sum((inner_points - reference_point) ** 2, axis=1))

    # 找到距离最近的点的索引
    closest_index = np.argmin(distances)

    # 重新排列inner_points，使得最近的点成为第一个点
    inner_points = np.roll(inner_points, -closest_index, axis=0)

    #求中值点
    center_points = (outer_points + inner_points)/2

    tem_center_points_1 = []
    # 注意这里应该是 range(len(center_points)) 而不是 range(len(center_points)-1)
    for i in range(len(center_points)):
        tem_center_points_1.append(center_points[i])
        tem_center_points_1.append(np.mean([center_points[i], center_points[(i + 1) % 4]], axis=0))

    tem_center_points_2 = []
    # 注意这里应该是 range(len(tem_center_points_1)) 而不是 range(len(tem_center_points_1)-1)
    for i in range(len(tem_center_points_1)):
        tem_center_points_2.append(tem_center_points_1[i])
        tem_center_points_2.append(np.mean([tem_center_points_1[i], tem_center_points_1[(i + 1) % 8]], axis=0))

    # 修改返回格式为OpenCV轮廓格式 (16, 1, 2)
    contour = np.array(tem_center_points_2, dtype=np.int32)
    contour = contour.reshape((16, 1, 2))

    # print(tem_center_points_2)

    return contour


def sort_points_clockwise(points):
    """
    将 (n,2) 的 numpy 数组按照顺时针或逆时针顺序排列

    参数:
        points: (n, 2) 的 numpy 数组，包含 n 个点的坐标
        clockwise: bool，True 表示顺时针，False 表示逆时针，默认为顺时针

    返回:
        排序后的 (n, 2) numpy 数组
    """
    #计算中心点
    center = np.mean(points, axis=0)
    angles = np.arctan2(points[:, 1] - center[1], points[:, 0] - center[0])

    sorted_indices = np.argsort(angles)

    return points[sorted_indices]


def detect_laser_point_v2(gray, show_process=False):
    """
    另一种激光点检测方法：使用Blob检测
    """

    # 创建SimpleBlobDetector参数
    params = cv2.SimpleBlobDetector_Params()

    # 设置阈值
    params.minThreshold = 200
    params.maxThreshold = 255

    # 设置面积过滤器
    params.filterByArea = True
    params.minArea = 1
    params.maxArea = 100

    # 设置圆形度过滤器
    params.filterByCircularity = True
    params.minCircularity = 0.1

    # 设置凸度过滤器
    params.filterByConvexity = True
    params.minConvexity = 0.1

    # 设置惯性比过滤器
    params.filterByInertia = True
    params.minInertiaRatio = 0.01

    # 创建检测器
    detector = cv2.SimpleBlobDetector_create(params)

    # 检测blobs
    keypoints = detector.detect(gray)

    if show_process:
        # 绘制检测到的blobs
        im_with_keypoints = cv2.drawKeypoints(frame, keypoints, np.array([]), (0, 0, 255),
                                              cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)
        cv2.imshow("Blobs", im_with_keypoints)

    # 如果检测到blobs，返回第一个（最亮的）
    if keypoints:
        return (int(keypoints[0].pt[0]), int(keypoints[0].pt[1]))

    return None



# ======================
# 主函数
# ======================


# ======================
# 初始化
# ======================

# 打开摄像头
cap = cv2.VideoCapture("D:\\Tool\\ti\\mspm0_sdk_1_30_00_03\\examples\\nortos\\LP_MSPM0G3507\\driverlib\\7_PTZ\\output.avi")
# fgbg = cv2.createBackgroundSubtractorMOG2()
#打开串口
# ser = datapack.init_serial()


ret, frame = cap.read()
time.sleep(0.1)
ret, frame = cap.read()
time.sleep(0.1)
# ======================
# 主循环
# ======================

while True:

    ret, frame = cap.read()

    #time======================
    time_start = time.time()


    if not ret:
        break

    # ======================
    # 统一的图像处理
    # ======================
    # 灰度图
    img_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)




    # img_canny = cv2.Canny(img_gray, 100, 150)
    # cv2.imshow("img_gray_1", img_gray)
    #
    # cv2.imshow("img_canny", img_canny)



    # ======================
    # 只执行一次的代码
    # ======================
    if flag_1:
        # 这里可以放一些只需要执行一次的初始化代码
        print("1")


        # flag_1 = False


    # ======================
    # 获取状态
    # ======================
    #state = datapack.receive_state(ser)
    #print(state)

    # ======================
    # 状态1逻辑
    # ======================
    if state == 1:
        print("State 1 logic")


        # ======================
        # 只执行一次的代码
        # ======================
        # if flag_state_1:
        #     # 这里可以放一些只需要执行一次的初始化代码
        #     print("State 1 initialized")
        #     flag_state_1 = False


    # ======================
    # 状态2逻辑
    # ======================
    elif state == 2:


        # ======================
        # 只执行一次的代码
        # ======================
        if flag_state_2:
            # 这里可以放一些只需要执行一次的初始化代码
            contours = find_binary_approx_polygon(img_gray, 4, min_area=100, epsilon_factor=0.02, show=False)
            #排序找到面积最大的两个框
            if len(contours) >= 2:
                contours = sorted(contours, key=lambda x: cv2.contourArea(x), reverse=True)[:2]

                #判断是否包含
                if check_contour_containment(contours):

                    #保存contours，完成任务
                    blobs_black = find_center_contour_clockwise(contours[0], contours[1])
                    # print(blobs_black)

                    flag_state_2 = False

                    #画图
                    cv2.drawContours(frame, contours, -1, (0, 255, 0), 2)
                    cv2.drawContours(frame, [blobs_black], -1, (255, 0, 255), 2)
                    cv2.imshow("result",  frame)


        # ======================
        # 总是执行的代码
        # ======================

        if blobs_black is not None:
            #找激光位置：
            contours_red = find_color_blob(frame, red_low, red_high, show=True)
            #找最大的轮廓
            if len(contours_red) > 0:
                #排除坐标靠近四周的轮廓
                contours_red = [contour for contour in contours_red if 50 < contour[0][0][0] < frame.shape[1] - 50
                                and 50 < contour[0][0][1] < frame.shape[0] - 50]

                if len(contours_red) > 0:
                    contour_red = max(contours_red, key=cv2.contourArea)
                    (x, y, w, h) = cv2.boundingRect(contour_red)
                    middle_red = (x + w / 2, y + h / 2)
                    #输出轮廓area面积
                    print("area:", cv2.contourArea(contour_red))



                    # 画图
                    cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 0, 255), 3)

                    #计算和blobs_black的差值
                    if blobs_black_index < 16:
                        error_red = np.array(middle_red) - blobs_black[blobs_black_index][0]
                        print(error_red)

                        #发送数据给舵机控制
                        #datapack.send_packet(ser, [0, 0])
                        # datapack.send_packet(ser, [error_red[0], error_red[1]])


                    else:
                        error_red = np.array(middle_red) - blobs_black[3][0]
                        print(error_red)

                        #发送数据给舵机控制
                        # datapack.send_packet(ser, [error_red[0], error_red[1]])

                    #如果误差值绝对值小于阈值，则认为已经到达目标
                    if error_red[0]**2 + error_red[1]**2 < 25 and blobs_black_index < 16:
                        blobs_black_index = (blobs_black_index + 1)











    # ======================
    # 状态3逻辑
    # ======================
    # elif state == 3:
    #     print("State 3 logic")

        # ======================
        # 只执行一次的代码
        # ======================
        # if flag_state_1:
        #     # 这里可以放一些只需要执行一次的初始化代码
        #     print("State 1 initialized")
        #     flag_state_1 = False



    # ======================
    # 显示图像
    # ======================

    cv2.imshow("frame", frame)
    #cv2.imshow("img_binary", img_binary)
    # cv2.imshow("img_canny", img_canny)
    # cv2.imshow("img_erode", img_erode)
    # time======================
    time_end = time.time()
    #print(f"{time_end - time_start:.3f} ")

    if cv2.waitKey(10) & 0xFF == ord('q'):
        break

# ======================
# 释放资源
# ======================
cap.release()
cv2.destroyAllWindows()



# ======================
# 主函数入口
# ======================


