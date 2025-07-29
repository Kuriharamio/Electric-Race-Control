import numpy as np



def load_path_data(filename):
    """
    从txt文件加载路径数据
    """
    points = []
    with open(filename, 'r') as f:
        for line in f:
            parts = line.strip().split(',')
            if len(parts) == 3:
                idx, x, y = int(parts[0]), int(parts[1]), int(parts[2])
                points.append((x, y))
    return np.array(points)


def remove_consecutive_duplicates(points, tolerance=0):
    """
    移除连续重复或过于接近的点

    Args:
        points: 点坐标数组
        tolerance: 容差值，两点间距离小于此值则认为是重复点

    Returns:
        优化后的点数组
    """
    if len(points) <= 1:
        return points

    optimized_points = [points[0]]  # 保留第一个点

    for i in range(1, len(points)):
        # 计算当前点与上一个保留点的距离
        prev_point = optimized_points[-1]
        curr_point = points[i]

        distance = np.sqrt((curr_point[0] - prev_point[0]) ** 2 + (curr_point[1] - prev_point[1]) ** 2)

        # 如果距离大于容差值，则保留该点
        if distance > tolerance:
            optimized_points.append(curr_point)

    return np.array(optimized_points)


def optimize_path(filename, tolerance=2.0):
    """
    优化路径数据

    Args:
        filename: 输入文件名
        tolerance: 最小点间距离容差

    Returns:
        原始点和优化后点的数组
    """
    # 加载原始路径数据
    original_points = load_path_data(filename)

    # 移除连续重复或过于接近的点
    optimized_points = remove_consecutive_duplicates(original_points, tolerance)

    return original_points, optimized_points


def save_optimized_path(points, filename):
    """
    保存优化后的路径数据
    """
    with open(filename, 'w') as f:
        for i, point in enumerate(points):
            f.write(f"{i},{int(point[0])},{int(point[1])}\n")




def main():
    # 输入文件名
    input_filename = "laser_path.txt"
    output_filename = "optimized_laser_path.txt"

    # 优化路径数据
    original_points, optimized_points = optimize_path(input_filename, tolerance=1.0)

    # 保存优化后的路径
    save_optimized_path(optimized_points, output_filename)

    # 打印统计信息
    print(f"原始点数: {len(original_points)}")
    print(f"优化后点数: {len(optimized_points)}")
    print(f"减少点数: {len(original_points) - len(optimized_points)}")
    print(f"优化比例: {(1 - len(optimized_points) / len(original_points)) * 100:.2f}%")



    print(f"\n优化后的路径已保存到 {output_filename}")


if __name__ == "__main__":
    main()