import numpy as np
import math
from scipy.interpolate import CubicHermiteSpline

def generate_trajectory(poses, num_points=30, 
                                                linear_resistance_coeff=0.1, 
                                                angular_resistance_coeff=0.5):
    if len(poses) < 2:
        return None

    # 时间参数基于路径点数量
    n = len(poses)
    t = np.linspace(0, 1, n)
    
    # 提取坐标并计算平滑导数
    x = [p[0] for p in poses]
    y = [p[1] for p in poses]
    theta = [p[2] for p in poses]
    
    # 计算平滑导数
    derivatives_x, derivatives_y = calculate_smooth_derivatives(poses)
    
    # 创建分段三次埃尔米特样条
    try:
        cs_x = CubicHermiteSpline(t, x, derivatives_x)
        cs_y = CubicHermiteSpline(t, y, derivatives_y)
    except ValueError:
        return None
    
    # 生成等间距时间参数
    t_new = np.linspace(0, 1, num_points)
    
    # 插值得到基本轨迹
    x_new = cs_x(t_new)
    y_new = cs_y(t_new)
    
    # 计算原始导数（速度分量）
    dx_dt = cs_x.derivative(1)(t_new)
    dy_dt = cs_y.derivative(1)(t_new)
    
    # 应用线性阻力
    velocity = np.sqrt(dx_dt**2 + dy_dt**2)
    resistance_x = -linear_resistance_coeff * dx_dt * velocity
    resistance_y = -linear_resistance_coeff * dy_dt * velocity
    
    # 调整速度（保持原始位置）
    adjusted_dx = dx_dt + resistance_x * (1.0/num_points)
    adjusted_dy = dy_dt + resistance_y * (1.0/num_points)
    
    # 计算角度
    theta_new = np.arctan2(adjusted_dy, adjusted_dx)
    
    # 应用角速度阻力
    angular_velocity = np.gradient(theta_new, 1.0/num_points)
    angular_resistance = -angular_resistance_coeff * angular_velocity * np.abs(angular_velocity)
    theta_new += angular_resistance * (1.0/num_points)
    
    # 确保经过原始路径点
    for i in range(n):
        idx = int(i * (num_points-1)/(n-1))
        x_new[idx] = x[i]
        y_new[idx] = y[i]
        theta_new[idx] = theta[i]
    
    return np.column_stack((x_new, y_new, theta_new))

def calculate_smooth_derivatives(poses):
    """计算平滑导数，保证中间点处导数连续"""
    n = len(poses)
    derivatives_x = []
    derivatives_y = []
    
    for i in range(n):
        if i == 0:
            # 起始点使用初始方向
            dx = math.cos(poses[i][2])
            dy = math.sin(poses[i][2])
        elif i == n-1:
            # 终点使用最终方向
            dx = math.cos(poses[i][2])
            dy = math.sin(poses[i][2])
        else:
            # 中间点使用加权平均方向
            prev_vec = (poses[i][0]-poses[i-1][0], poses[i][1]-poses[i-1][1])
            next_vec = (poses[i+1][0]-poses[i][0], poses[i+1][1]-poses[i][1])
            
            # 计算加权平均值（根据相邻段长度）
            weight_prev = math.hypot(*next_vec)
            weight_next = math.hypot(*prev_vec)
            
            avg_dx = (weight_prev*prev_vec[0] + weight_next*next_vec[0]) / (weight_prev + weight_next)
            avg_dy = (weight_prev*prev_vec[1] + weight_next*next_vec[1]) / (weight_prev + weight_next)
            
            # 保持原始方向的影响
            blend_factor = 0.3  # 保留30%的原始方向
            original_dx = math.cos(poses[i][2])
            original_dy = math.sin(poses[i][2])
            
            dx = (1-blend_factor)*avg_dx + blend_factor*original_dx
            dy = (1-blend_factor)*avg_dy + blend_factor*original_dy
            
            # 归一化
            norm = math.hypot(dx, dy)
            dx /= norm
            dy /= norm
            
        derivatives_x.append(dx)
        derivatives_y.append(dy)
    
    return derivatives_x, derivatives_y


if __name__ == "__main__":
    # 角度为弧度，使用 math.pi
    poses = [
        (0.0, 0.0, 0.0),
        (0.2, 0.1, 0.0),
        (0.5, 0.2, 0.0),
        (0.8, 0.1, 0.0),
        (1.0, 0.0, 0.0)
    ]
    
    trajectory = generate_trajectory(
        poses, 
        num_points=30,
        linear_resistance_coeff=0.1,
        angular_resistance_coeff=0.05
    )
    
    if trajectory is not None:
        print("\nfloat trajectory[NUM_POINTS][2] = {")
        for point in trajectory:
            print(f"    {{{point[0]:.6f}, {point[1]:.6f}}},")
        print("};")
        import matplotlib.pyplot as plt
        plt.figure(figsize=(10, 6))
        
        plt.plot(trajectory[:, 0], trajectory[:, 1], 'b-', label='Trajectory')
        plt.plot([p[0] for p in poses], [p[1] for p in poses], 'ro', label='Waypoints')
        
        for i in range(0, len(trajectory), 10):
            x, y, theta = trajectory[i]
            dx = math.cos(theta) * 0.05
            dy = math.sin(theta) * 0.05
            plt.arrow(x, y, dx, dy, head_width=0.02, fc='g', ec='g')
        
        plt.axis('equal')
        plt.legend()
        plt.title('Trajectory')
        plt.xlabel('X (m)')
        plt.ylabel('Y (m)')
        plt.show()
    else:
        print("轨迹生成失败")