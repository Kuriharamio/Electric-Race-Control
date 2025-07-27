from pathlib import Path
import cv2
import numpy as np
import matplotlib.pyplot as plt
from skimage.morphology import skeletonize
from scipy.spatial.distance import cdist
from collections import deque

# -------------------- CONFIG --------------------
CFG = dict(
    img=Path("Utils/qq.png"),
    size=(640, 480),          # (w, h)
    canny=(50, 100),          # (low, high)
    eps=0.5,
    gap=50,
    servo=(0, 4095, 0, 4095), # (x_min, x_max, y_min, y_max)
    vis=True,
)
# -----------------------------------------------

# ---------- 1. 读图 + Canny + 骨架 ----------
def load_edges(p: Path, size, canny_th):
    g = cv2.imread(str(p), cv2.IMREAD_GRAYSCALE)
    e = cv2.Canny(g, *canny_th)
    e = cv2.resize(e, size, cv2.INTER_AREA)
    e = skeletonize(e.astype(bool))
    return (e * 255).astype(np.uint8)

# ---------- 2. 轮廓 → 子路径 ----------
def contours_to_paths(edges):
    cnts, *_ = cv2.findContours(edges, cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)
    return [c.squeeze(1).tolist() for c in cnts]

# ---------- 3. 坐标映射 + 点去重 ----------
def _map(x, y, w, h, sx, sy):
    return int(sx[0] + x * (sx[1] - sx[0]) / (w - 1)), \
           int(sy[0] + y * (sy[1] - sy[0]) / (h - 1))

def clean_path(p, eps, w, h, sx, sy):
    prev = None
    out = []
    for x, y in p:
        pt = _map(x, y, w, h, sx, sy)
        if prev is None or np.hypot(pt[0] - prev[0], pt[1] - prev[1]) >= eps:
            out.append(pt)
            prev = pt
    return out

# ---------- 4. 后校验：全局去重 + 距离分割 ----------
def post_validate(paths, max_gap=100):
    seen = set()          # 记录全局已经出现过的点
    new_paths = []

    for sub in paths:
        uniq = []
        for pt in sub:
            if pt not in seen:
                seen.add(pt)
                uniq.append(pt)
        if len(uniq) < 1:     # 少于 2 个点，直接丢弃
            continue
        new_paths.append(uniq)

    # 2. 距离分割
    final_paths = []
    for sub in new_paths:
        seg = [sub[0]]
        for prev, nxt in zip(sub, sub[1:]):
            if np.hypot(nxt[0]-prev[0], nxt[1]-prev[1]) > max_gap:
                final_paths.append(seg)
                seg = [nxt]   # 开启新段
            else:
                seg.append(nxt)
        if len(seg) >= 2:      # 最后一段保留
            final_paths.append(seg)

    return final_paths

# ---------- 5. 最短空行程排序 ----------
def sort_paths_tsp(paths):
    """
    对子路径排序，使得激光头空行程最短。
    每条子路径可正向或反向，但不会丢失任何路径。
    """
    if not paths:
        return []

    n = len(paths)
    # 每条路径的两个端点
    starts = [p[0] for p in paths]
    ends = [p[-1] for p in paths]

    # 计算任意两条路径之间的四种连接方式的距离
    # dist[i][j] = 从 paths[i] 的尾点到 paths[j] 的首点的最小距离
    dist = np.full((n, n), np.inf)
    reverse_flag = [[False] * n for _ in range(n)]  # 记录是否反转 j 路径

    for i in range(n):
        for j in range(n):
            if i == j:
                continue
            # 四种连接方式
            d1 = np.linalg.norm(np.array(ends[i]) - np.array(starts[j]))  # i -> j
            d2 = np.linalg.norm(np.array(ends[i]) - np.array(ends[j]))    # i -> j_reversed
            d3 = np.linalg.norm(np.array(starts[i]) - np.array(starts[j]))# i_reversed -> j
            d4 = np.linalg.norm(np.array(starts[i]) - np.array(ends[j]))  # i_reversed -> j_reversed

            min_d = min(d1, d2, d3, d4)
            dist[i, j] = min_d

    # 贪心最近邻（从任意起点开始）
    visited = [False] * n
    order = []
    current = 0
    visited[current] = True
    order.append(current)

    for _ in range(n - 1):
        next_idx = None
        min_dist = np.inf
        for j in range(n):
            if not visited[j] and dist[current, j] < min_dist:
                min_dist = dist[current, j]
                next_idx = j
        if next_idx is None:
            break  # 应该不会发生
        order.append(next_idx)
        visited[next_idx] = True
        current = next_idx

    # 根据 order 重构路径
    new_paths = []
    for i in order:
        new_paths.append(paths[i])

    # 现在需要确定每条路径是否需要反转，使得首尾衔接
    # 使用动态规划确定最优连接方式（简化版：贪心检查）
    final_paths = [new_paths[0]]
    for i in range(1, len(new_paths)):
        prev_end = final_paths[-1][-1]
        cur_start = new_paths[i][0]
        cur_end = new_paths[i][-1]

        if np.linalg.norm(np.array(prev_end) - np.array(cur_start)) > \
           np.linalg.norm(np.array(prev_end) - np.array(cur_end)):
            final_paths.append(list(reversed(new_paths[i])))
        else:
            final_paths.append(new_paths[i])

    return final_paths

def merge_nearby_paths(paths, merge_gap=50):
    """
    将终点与起点距离 < merge_gap 的路径合并（可翻转）
    paths: list[list[(x, y)]]
    return: list[list[(x, y)]]  合并后的路径
    """
    if not paths:
        return []

    merged = [list(paths[0])]          # 至少保留第一条
    for cur in paths[1:]:
        last = merged[-1]
        last_end = last[-1]
        cur_start = cur[0]
        cur_end = cur[-1]

        # 计算两种连接方式的距离
        d_normal = np.linalg.norm(np.array(last_end) - np.array(cur_start))
        d_flip   = np.linalg.norm(np.array(last_end) - np.array(cur_end))

        if min(d_normal, d_flip) <= merge_gap:
            # 可以合并
            if d_flip < d_normal:
                cur = list(reversed(cur))   # 翻转后更短
            merged[-1].extend(cur)          # 拼接到上一段
        else:
            # 不能合并，新开一条
            merged.append(list(cur))
    return merged
# ---------- 6. 加工过程可视化 ----------
def simulate_gcode(paths,
                   canvas_wh=(640, 480),
                   feed_ms=15,
                   laser_size=2):
    w, h = canvas_wh
    bg = np.zeros((h, w, 3), dtype=np.uint8)
    cx, cy = w // 2, h // 2
    cv2.line(bg, (cx - 30, cy), (cx + 30, cy), (50, 50, 50), 1)
    cv2.line(bg, (cx, cy - 30), (cx, cy + 30), (50, 50, 50), 1)

    def servo2pix(xy):
        sx, sy = CFG["servo"][:2], CFG["servo"][2:]
        x = int((xy[0] - sx[0]) * (w - 1) / (sx[1] - sx[0]))
        y = int((xy[1] - sy[0]) * (h - 1) / (sy[1] - sy[0]))
        return x, y

    base = bg.copy()
    for sub in paths:
        pts = np.array([servo2pix(p) for p in sub], np.int32)
        cv2.polylines(base, [pts], isClosed=False, color=(0, 0, 255), thickness=1)

    win = "Laser Simulation (ESC to quit)"
    cv2.imshow(win, base)

    cnt = 0
    for sub in paths:
        cnt += 1
        for nxt_pt in sub:
            nxt = servo2pix(nxt_pt)
            frame = base.copy()
            if cnt%2 == 0:
                cv2.circle(frame, nxt, laser_size, (255, 180, 255), -1) 
            else:
                cv2.circle(frame, nxt, laser_size, (255, 255, 255), -1)
            cv2.imshow(win, frame)
            if cv2.waitKey(feed_ms) & 0xFF == 27:
                return
            pen = nxt
        cv2.waitKey(1)  

    cv2.waitKey(0)
    cv2.destroyAllWindows()

# ---------- 7. 主流程 ----------
def main(cfg):
    edges = load_edges(cfg["img"], cfg["size"], cfg["canny"])
    paths = contours_to_paths(edges)
    paths = [clean_path(p, cfg["eps"], *cfg["size"], cfg["servo"][:2], cfg["servo"][2:]) for p in paths]
    paths = post_validate(paths,cfg["gap"]) 
    paths = sort_paths_tsp(paths) 
    paths = merge_nearby_paths(paths)  
    # ---- 导出 C header ----
    lens = [len(p) for p in paths]
    total = sum(lens)
    with open("subpaths.h", "w") as f:
        f.write('#include <stdint.h>\n\n')
        f.write(f'const uint16_t subpath_cnt = {len(paths)};\n')
        offset = 0
        lens = [len(p) for p in paths]
        total = sum(lens)
        f.write(f'const uint16_t subpath_lens[{len(paths)}] = {{ {",".join(map(str,lens))} }};\n')
        f.write(f'const int16_t subpath_points[{total}][2] = {{\n')
        for p in paths:
            for x,y in p:
                f.write(f'  {{{x},{y}}},\n')
        f.write('};\n')
    print("已生成 subpaths.h：", len(paths), "条子路径，共", total, "点")

    # ---- 模拟加工 ----
    if cfg["vis"]:
        simulate_gcode(paths,
                       canvas_wh=cfg["size"],
                       feed_ms=1,
                       laser_size=3)

if __name__ == "__main__":
    main(CFG)