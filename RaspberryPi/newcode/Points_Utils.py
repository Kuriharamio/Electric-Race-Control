import numpy as np
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

    # 排序
    outer_points = sort_points_clockwise(outer_points)
    inner_points = sort_points_clockwise(inner_points)

    # 使第一个点对齐
    reference_point = outer_points[0]

    # 计算该点与inner_points中每个点的距离
    distances = np.sqrt(np.sum((inner_points - reference_point) ** 2, axis=1))

    # 找到距离最近的点的索引
    closest_index = np.argmin(distances)

    # 重新排列inner_points，使得最近的点成为第一个点
    inner_points = np.roll(inner_points, -closest_index, axis=0)

    # 求中值点
    center_points = (outer_points + inner_points) / 2

    # 确定需要进行的插值次数
    num_interpolations = 4

    # 初始点集
    current_points = center_points.copy()

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
    current_points.append(current_points[0])


    # 修改返回格式为OpenCV轮廓格式 (, 1, 2)
    contour = np.array(current_points, dtype=np.int32)
    contour = contour.reshape((len(contour), 1, 2))
    return contour


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


def sort_points_clockwise(points):
    """
    将 (n,2) 的 numpy 数组按照顺时针或逆时针顺序排列

    参数:
        points: (n, 2) 的 numpy 数组，包含 n 个点的坐标
        clockwise: bool，True 表示顺时针，False 表示逆时针，默认为顺时针

    返回:
        排序后的 (n, 2) numpy 数组
    """
    # 计算中心点
    center = np.mean(points, axis=0)
    angles = np.arctan2(points[:, 1] - center[1], points[:, 0] - center[0])

    sorted_indices = np.argsort(angles)

    return points[sorted_indices]