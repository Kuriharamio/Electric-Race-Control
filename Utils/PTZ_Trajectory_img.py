import cv2
import matplotlib.pyplot as plt
import numpy as np
from skimage.morphology import skeletonize

IMG_PATH  = "Utils/EDC.png"
OUT_W, OUT_H = 640, 480
EDGE_TH = 50
VIS = True 

img  = cv2.imread(IMG_PATH, cv2.IMREAD_GRAYSCALE)
edges = cv2.Canny(img, EDGE_TH, EDGE_TH*2)
edges = cv2.resize(edges, (OUT_W, OUT_H), interpolation=cv2.INTER_AREA)

edges = skeletonize(edges.astype(bool))
edges = (edges * 255).astype(np.uint8)       # 转回 0/255
cv2.imshow('edges', edges)

contours, _ = cv2.findContours(edges, cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)
subpaths_px = [cnt.squeeze(1).tolist() for cnt in contours]


epsilon = 2 # 像素级误差阈值，可调（越大点越少）
subpaths_dp = []
for cnt in contours:
    approx = cv2.approxPolyDP(cnt, epsilon, closed=False)  # 简化
    subpaths_dp.append(approx.squeeze(1).tolist())

subpaths_px = subpaths_dp   # 替换原列表


import numpy as np
from scipy.spatial.distance import cdist

dup_thresh = 5.0   # 像素级距离阈值（可调）

# 1) 计算每条路径的首点、长度
starts = np.array([p[0] for p in subpaths_px])   # (N,2)
lens   = np.array([len(p) for p in subpaths_px])

# 2) 距离矩阵，True 表示接近
D = cdist(starts, starts, metric='euclidean')
dup_mask = D < dup_thresh
dup_mask[np.diag_indices_from(dup_mask)] = False   # 自己不算

# 3) 标记要保留的索引
keep = np.ones(len(subpaths_px), dtype=bool)
for i in range(len(subpaths_px)):
    if not keep[i]:
        continue
    # 找到所有与 i 接近的路径索引
    near = np.where(dup_mask[i])[0]
    if len(near) == 0:
        continue
    # 在接近组里选最长那条
    group = np.append(i, near)
    best  = group[np.argmax(lens[group])]
    # 其余全部删除
    keep[group[group != best]] = False

# 4) 过滤结果
subpaths_px = [subpaths_px[i] for i in np.where(keep)[0]]



print(f"去重后子路径条数：{len(subpaths_px)}")

if VIS:
    plt.figure(figsize=(6,4))
    for p in subpaths_px:
        xs, ys = zip(*p)
        plt.plot(xs, ys, linewidth=0.8)
    plt.gca().set_aspect('equal')
    plt.title(f"{len(subpaths_px)} sub-paths")
    plt.savefig("subpaths.png", bbox_inches='tight')
    plt.show()

X_MIN_SERVO = 1947
X_MAX_SERVO = 2147
Y_MIN_SERVO = 1800
Y_MAX_SERVO = 1900
def map_x(x): return int(X_MIN_SERVO + x * (X_MAX_SERVO - X_MIN_SERVO) / (OUT_W-1))
def map_y(y): return int(Y_MIN_SERVO + y * (Y_MAX_SERVO - Y_MIN_SERVO) / (OUT_H-1))




subpaths = []
for sp in subpaths_px:
    subpaths.append([(map_x(x), map_y(y)) for x, y in sp])


with open("subpaths.h", "w") as f:
    f.write('#include <stdint.h>\n\n')
    f.write(f'const uint16_t subpath_cnt = {len(subpaths)};\n')
    offset = 0
    lens = [len(p) for p in subpaths]
    total = sum(lens)
    f.write(f'const uint16_t subpath_lens[{len(subpaths)}] = {{ {",".join(map(str,lens))} }};\n')
    f.write(f'const int16_t subpath_points[{total}][2] = {{\n')
    for p in subpaths:
        for x,y in p:
            f.write(f'  {{{x},{y}}},\n')
    f.write('};\n')
print("已生成 subpaths.h：", len(subpaths), "条子路径，共", total, "点")