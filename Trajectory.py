import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import CubicHermiteSpline

def generate_trajectory(start, end, num_points=100, scale=1.0):
    """
    Generates a smooth trajectory between start and end poses using Cubic Hermite Splines.

    Parameters:
    - start: tuple of (x, y, yaw) for the starting pose
    - end: tuple of (x, y, yaw) for the ending pose
    - num_points: number of points in the trajectory
    - scale: scaling factor for derivatives

    Returns:
    - trajectory: list of (x, y) tuples
    """
    x0, y0, yaw0 = start
    x1, y1, yaw1 = end

    # Parameter t
    t = [0, 1]

    # Calculate derivatives based on yaw
    dx0 = np.cos(yaw0) * scale
    dy0 = np.sin(yaw0) * scale
    dx1 = np.cos(yaw1) * scale
    dy1 = np.sin(yaw1) * scale

    # Create Hermite splines for x and y
    spline_x = CubicHermiteSpline(t, [x0, x1], [dx0, dx1])
    spline_y = CubicHermiteSpline(t, [y0, y1], [dy0, dy1])

    # Sample points
    ts = np.linspace(0, 1, num_points)
    xs = spline_x(ts)
    ys = spline_y(ts)

    trajectory = list(zip(xs, ys))
    return trajectory

def print_c_style_array(trajectory):
    """
    Prints the trajectory points in C-style array format.

    Parameters:
    - trajectory: list of (x, y) tuples
    """
    print("float trajectory[NUM_POINTS][2] = {")
    i = 0
    for point in trajectory:
        i += 1
        print(f"    {{{point[0]:.3f}, {point[1]:.3f}}},")
    print("};")
    print(f"size: {i}")

def plot_trajectory(trajectories, labels, colors):
    """
    Plots multiple trajectories.

    Parameters:
    - trajectories: list of trajectories, each a list of (x, y) tuples
    - labels: list of labels for each trajectory
    - colors: list of colors for each trajectory
    """
    plt.figure(figsize=(10, 10))
    for traj, label, color in zip(trajectories, labels, colors):
        xs, ys = zip(*traj)
        plt.plot(xs, ys, label=label, color=color)
        plt.scatter(xs[0], ys[0], marker='o', color=color)  # Start point
        plt.scatter(xs[-1], ys[-1], marker='x', color=color)  # End point
    plt.axis('equal')
    plt.grid(True)
    plt.legend()
    plt.title('Smooth Trajectories with Smooth Left and Right Turns')
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.show()

def main():
    # Example 1: Smooth Left Turn (90 degrees)
    start_left = (0, 0, 0)  # Facing right (0 radians)
    end_left = (1, 1, np.pi/2)  # Facing up (90 degrees)
    trajectory_left = generate_trajectory(start_left, end_left, num_points=100, scale=3.2)

    # Example 2: Smooth Right Turn (90 degrees)
    start_right = (0, 0, 0)  # Facing right (0 radians)
    end_right = (1, -1, -np.pi/2)  # Facing down (-90 degrees)
    trajectory_right = generate_trajectory(start_right, end_right, num_points=100, scale=3.2)

    # Plot trajectories
    plot_trajectory(
        [trajectory_left, trajectory_right],
        ['Smooth Left Turn', 'Smooth Right Turn'],
        ['blue', 'red']
    )

    # Print C-style array for Left Turn
    print("// Smooth Left Turn Trajectory Points")
    print_c_style_array(trajectory_left)
    print("\n// Smooth Right Turn Trajectory Points")
    print_c_style_array(trajectory_right)

if __name__ == "__main__":
    main()
