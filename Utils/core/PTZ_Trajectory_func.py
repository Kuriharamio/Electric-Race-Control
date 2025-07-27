import numpy as np
import matplotlib.pyplot as plt

FUNC      = lambda x: 1 * np.sin(1 * x)
X_MIN     = 0.0
X_MAX     = 6.28
POINTS    = 200 
OUT_FILE  = "subfunc.h"

X_MIN_SERVO = 0
X_MAX_SERVO = 4095
Y_MIN_SERVO = 0
Y_MAX_SERVO = 4095

def map_x(x):
    return int(X_MIN_SERVO + (x - X_MIN) * (X_MAX_SERVO - X_MIN_SERVO) / (X_MAX - X_MIN))

def map_y(y):
    y_min = min(FUNC(X_MIN), FUNC(X_MAX), *FUNC(np.linspace(X_MIN, X_MAX, 100)))
    y_max = max(FUNC(X_MIN), FUNC(X_MAX), *FUNC(np.linspace(X_MIN, X_MAX, 100)))
    return int(Y_MIN_SERVO + (y - y_min) * (Y_MAX_SERVO - Y_MIN_SERVO) / (y_max - y_min))

def dedup_points(path, eps=1.0):
    if not path:
        return []
    cleaned = [path[0]]
    for p in path[1:]:
        dx = p[0] - cleaned[-1][0]
        dy = p[1] - cleaned[-1][1]
        if dx == dy == 0:          # 完全重合
            continue
        if np.hypot(dx, dy) < eps: # 太近
            continue
        cleaned.append(p)
    return cleaned

xs = np.linspace(X_MIN, X_MAX, POINTS)
ys = [FUNC(x) for x in xs]
path = [(map_x(x), map_y(y)) for x, y in zip(xs, ys)]

path = dedup_points(path, eps=2.0)

plt.plot(xs, ys)
plt.title("Function Preview")
plt.gca().set_aspect('equal')
plt.show()

with open(OUT_FILE, "w") as f:
    f.write('#include <stdint.h>\n\n')
    f.write(f'const uint16_t func_point_cnt = {len(path)};\n')
    f.write('const int16_t func_path[][2] = {\n')
    for x, y in path:
        f.write(f'  {{{x},{y}}},\n')
    f.write('};\n')
print("已生成", OUT_FILE, "共", len(path), "点")