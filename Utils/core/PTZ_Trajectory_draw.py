# import cv2
# import numpy as np

# # -------- 伺服行程 --------
# SERVO = dict(x=(0, 4095), y=(0, 4095))   # 对应激光头的物理行程
# CANVAS = (40, 160)                      # 窗口分辨率
# num = "5"
# output_name = f"Utils\PTZ_h\path{num}.h"
# def px2servo(px, py):
#     """把像素坐标映射成伺服坐标"""
#     sx = SERVO['x'][0] + px * (SERVO['x'][1] - SERVO['x'][0]) / (CANVAS[0] - 1)
#     sy = SERVO['y'][0] + py * (SERVO['y'][1] - SERVO['y'][0]) / (CANVAS[1] - 1)
#     return int(round(sx)), int(round(sy))

# # -------- 全局变量 --------
# drawing = False            # 是否正在画
# current_stroke = []        # 当前正在画的单条轨迹
# all_strokes = []           # 所有轨迹

# def mouse_cb(event, x, y, flags, param):
#     global drawing, current_stroke
#     if event == cv2.EVENT_LBUTTONDOWN:
#         drawing = True
#         current_stroke = [(x, y)]
#     elif event == cv2.EVENT_MOUSEMOVE and drawing:
#         current_stroke.append((x, y))
#     elif event == cv2.EVENT_LBUTTONUP:
#         drawing = False
#         if len(current_stroke) > 1:
#             all_strokes.append(current_stroke)

# # ---- 模拟加工 ----
# def simulate_gcode(paths,
#                    canvas_wh=(640, 480),
#                    feed_ms=15,
#                    laser_size=2):
#     w, h = canvas_wh
#     bg = np.zeros((h, w, 3), dtype=np.uint8)
#     cx, cy = w // 2, h // 2
#     cv2.line(bg, (cx - 30, cy), (cx + 30, cy), (50, 50, 50), 1)
#     cv2.line(bg, (cx, cy - 30), (cx, cy + 30), (50, 50, 50), 1)

#     def servo2pix(xy):
#         sx, sy = SERVO['x'], SERVO['y']
#         x = int((xy[0] - sx[0]) * (w - 1) / (sx[1] - sx[0]))
#         y = int((xy[1] - sy[0]) * (h - 1) / (sy[1] - sy[0]))
#         return x, y

#     base = bg.copy()
#     for sub in paths:
#         pts = np.array([servo2pix(p) for p in sub], np.int32)
#         cv2.polylines(base, [pts], isClosed=False, color=(0, 0, 255), thickness=1)

#     win = "Laser Simulation (ESC to quit)"
#     cv2.imshow(win, base)

#     cnt = 0
#     for sub in paths:
#         cnt += 1
#         for nxt_pt in sub:
#             nxt = servo2pix(nxt_pt)
#             frame = base.copy()
#             if cnt%2 == 0:
#                 cv2.circle(frame, nxt, laser_size, (255, 180, 255), -1) 
#             else:
#                 cv2.circle(frame, nxt, laser_size, (255, 255, 255), -1)
#             cv2.imshow(win, frame)
#             if cv2.waitKey(feed_ms) & 0xFF == 27:
#                 return
#             pen = nxt
#         cv2.waitKey(1)  

#     cv2.waitKey(0)
#     cv2.destroyAllWindows()


# # -------- 主程序 --------
# img = np.ones((*reversed(CANVAS), 3), dtype=np.uint8) * 255
# cv2.namedWindow('Draw')
# cv2.setMouseCallback('Draw', mouse_cb)

# while True:
#     vis = img.copy()
#     # 实时绘制当前正在画的线
#     if len(current_stroke) > 1:
#         pts = np.array(current_stroke, np.int32)
#         cv2.polylines(vis, [pts], False, (0, 0, 255), 2)
#     # 绘制历史轨迹
#     for stroke in all_strokes:
#         pts = np.array(stroke, np.int32)
#         cv2.polylines(vis, [pts], False, (0, 0, 0), 2)
#     cv2.imshow('Draw', vis)

#     key = cv2.waitKey(1) & 0xFF
#     if key == ord(' '):          # 空格：结束当前笔迹
#         if len(current_stroke) > 1:
#             all_strokes.append(current_stroke)
#         current_stroke = []
#     elif key == 13:              # Enter：结束全部手写
#         break
#     elif key == 27:              # Esc：放弃全部
#         all_strokes = []
#         break

# cv2.destroyAllWindows()

# # -------- 坐标转换 & 输出 --------
# paths = [[px2servo(x, y) for x, y in stroke] for stroke in all_strokes]

# lens = [len(p) for p in paths]
# total = sum(lens)
# with open(output_name, "w") as f:
#     f.write('#include <stdint.h>\n\n')
#     f.write(f'const uint16_t subpath_cnt_{num} = {len(paths)};\n')
#     offset = 0
#     lens = [len(p) for p in paths]
#     total = sum(lens)
#     f.write(f'const uint16_t subpath_lens_{num}[{len(paths)}] = {{ {",".join(map(str,lens))} }};\n')
#     f.write(f'const int16_t subpath_points_{num}[{total}][2] = {{\n')
#     for p in paths:
#         for x,y in p:
#             f.write(f'  {{{x},{y}}},\n')
#     f.write('};\n')
# print(f"已生成 {output_name}: ", len(paths), "条子路径，共", total, "点")
# simulate_gcode(paths,
#                 canvas_wh=CANVAS,
#                 feed_ms=1,
#                 laser_size=3)


import cv2
import numpy as np
from dataclasses import dataclass
from typing import List, Tuple

# ------------------ 可配置参数 ------------------
CANVAS_PX      = (640, 1280)          # 手写窗口分辨率 (w, h)
SERVO_RANGE     = (0, 4095)         # 激光头物理行程
OUTPUT_NUM      = "9"
OUTPUT_PATH     = f"Utils/PTZ_h/path{OUTPUT_NUM}.h"
# ------------------------------------------------

@dataclass
class Glyph:
    strokes: List[List[Tuple[int, int]]]  # 像素坐标

    # ---------- 内部工具 ----------
    def _bbox(self) -> Tuple[int, int, int, int]:
        """返回 (min_x, max_x, min_y, max_y)"""
        pts = [p for s in self.strokes for p in s]
        if not pts:      # 空画布
            return 0, 0, 0, 0
        xs, ys = zip(*pts)
        return min(xs), max(xs), min(ys), max(ys)

    def normalize_to_servo(self) -> List[List[Tuple[int, int]]]:
        min_x, max_x, min_y, max_y = self._bbox()
        if max_x == min_x:
            max_x += 1
        if max_y == min_y:
            max_y += 1

        # 计算缩放比例（取较小值，保持等比例）
        scale_x = (SERVO_RANGE[1] - SERVO_RANGE[0]) / (max_x - min_x)
        scale_y = (SERVO_RANGE[1] - SERVO_RANGE[0]) / (max_y - min_y)

        # 转换坐标
        servo_strokes = []
        for s in self.strokes:
            servo_s = []
            for x, y in s:
                sx = int(round((x - min_x) * scale_x))
                sy = int(round((y - min_y) * scale_y))
                # 确保严格落在 0-4095
                sx = max(0, min(4095, sx))
                sy = max(0, min(4095, sy))
                servo_s.append((sx, sy))
            servo_strokes.append(servo_s)
        return servo_strokes

    # ---------- 导出 C 头文件 ----------
    def dump_header(self, name: str, file_path: str):
        servo = self.normalize_to_servo()
        sub_cnt = len(servo)
        sub_lens = [len(s) for s in servo]
        total = sum(sub_lens)

        with open(file_path, 'w') as f:
            f.write('#include <stdint.h>\n\n')
            f.write(f'const uint16_t subpath_cnt_{name} = {sub_cnt};\n')
            f.write(f'const uint16_t subpath_lens_{name}[{sub_cnt}] = {{ {",".join(map(str, sub_lens))} }};\n')
            f.write(f'const int16_t subpath_points_{name}[{total}][2] = {{\n')
            for sub in servo:
                for x, y in sub:
                    f.write(f'  {{{x},{y}}},\n')
            f.write('};\n')
        print(f"已生成 {file_path}: {sub_cnt} 条子路径，共 {total} 点")

# ------------------ 手写交互 ------------------
class Drawer:
    def __init__(self, canvas_px, win_name='Draw'):
        self.canvas_px = canvas_px
        self.win_name  = win_name
        self.glyph     = Glyph([])
        self.current   = []
        self.drawing   = False

        self.img = np.ones((*reversed(canvas_px), 3), dtype=np.uint8) * 255
        cv2.namedWindow(self.win_name)
        cv2.setMouseCallback(self.win_name, self._mouse_cb)

    def _mouse_cb(self, event, x, y, flags, param):
        if event == cv2.EVENT_LBUTTONDOWN:
            self.drawing = True
            self.current = [(x, y)]
        elif event == cv2.EVENT_MOUSEMOVE and self.drawing:
            self.current.append((x, y))
        elif event == cv2.EVENT_LBUTTONUP:
            self.drawing = False
            if len(self.current) > 1:
                self.glyph.strokes.append(self.current)
            self.current = []

    def run(self):
        while True:
            vis = self.img.copy()
            # 实时线
            if len(self.current) > 1:
                cv2.polylines(vis, [np.array(self.current, np.int32)], False, (0, 0, 255), 2)
            # 已画线
            for s in self.glyph.strokes:
                cv2.polylines(vis, [np.array(s, np.int32)], False, (0, 0, 0), 2)

            cv2.imshow(self.win_name, vis)
            k = cv2.waitKey(1) & 0xFF
            if k == ord('u') and self.glyph.strokes:   # 撤销
                self.glyph.strokes.pop()
            elif k == 13:                              # Enter 保存
                break
            elif k == 27:                              # Esc 放弃
                self.glyph.strokes.clear()
                break
        cv2.destroyAllWindows()

# ------------------ 模拟器 ------------------
def simulate_glyph(glyph: Glyph, canvas_wh=(640, 480), feed_ms=1):
    servo = glyph.normalize_to_servo()
    bg = np.zeros((*reversed(canvas_wh), 3), dtype=np.uint8)

    # 画十字中心线
    cx, cy = canvas_wh[0] // 2, canvas_wh[1] // 2
    cv2.line(bg, (cx - 30, cy), (cx + 30, cy), (50, 50, 50), 1)
    cv2.line(bg, (cx, cy - 30), (cx, cy + 30), (50, 50, 50), 1)

    # 把伺服坐标映射回像素
    def s2p(xy):
        lo, hi = SERVO_RANGE
        px = int((xy[0] - lo) * (canvas_wh[0] - 1) / (hi - lo))
        py = int((xy[1] - lo) * (canvas_wh[1] - 1) / (hi - lo))
        return px, py

    for sub in servo:
        pts = np.array([s2p(p) for p in sub], np.int32)
        cv2.polylines(bg, [pts], False, (0, 0, 255), 1)

    win = "Laser Simulation (ESC to quit)"
    cv2.imshow(win, bg)

    for sub in servo:
        for pt in sub:
            frame = bg.copy()
            cv2.circle(frame, s2p(pt), 2, (255, 255, 255), -1)
            cv2.imshow(win, frame)
            if cv2.waitKey(feed_ms) & 0xFF == 27:
                return
    cv2.waitKey(0)
    cv2.destroyAllWindows()

# ------------------ 主流程 ------------------
if __name__ == '__main__':
    dw = Drawer(CANVAS_PX)
    dw.run()
    if dw.glyph.strokes:
        dw.glyph.dump_header(OUTPUT_NUM, OUTPUT_PATH)
        simulate_glyph(dw.glyph)