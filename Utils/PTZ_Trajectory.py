import sys
import cv2
import numpy as np

# ------------------ 参数 ------------------
img_path   = "Utils/test.png"
out_w      = 640
out_h      = 480
EDGE_TH    = 150                   # Canny 阈值

# ------------------ 读图 + 边缘 ------------------
img = cv2.imread(img_path, cv2.IMREAD_GRAYSCALE)
if img is None:
    sys.exit('cannot open ' + img_path)
edges = cv2.Canny(img, EDGE_TH, EDGE_TH*2)

# ------------------ 缩放到目标分辨率 ------------------
edges = cv2.resize(edges, (out_w, out_h), interpolation=cv2.INTER_AREA)
_, bw = cv2.threshold(edges, 1, 1, cv2.THRESH_BINARY)   # 0/1 矩阵

MIN_AREA = 0
num_labels, labels, stats, _ = cv2.connectedComponentsWithStats(bw, connectivity=8)
for i in range(1, num_labels):
    if stats[i, cv2.CC_STAT_AREA] < MIN_AREA:
        bw[labels == i] = 0

cv2.imshow('edges', edges)
cv2.imshow('bw', bw*255)
cv2.waitKey(0)
# ------------------ 1) 打印 bitmap[] ------------------
bits = bw.flatten()
byte_len = (len(bits)+7)//8
byte_arr = np.packbits(bits, bitorder='little')
print('const uint8_t bitmap[] = {')
for i in range(0, len(byte_arr), 16):
    line = ', '.join(f'0x{b:02X}' for b in byte_arr[i:i+16])
    print('   ', line + (',' if i+16<len(byte_arr) else ''))
print('};')
print()

with open('bitmap.txt', 'w') as f:
    bits = bw.flatten()
    byte_arr = np.packbits(bits, bitorder='little')

    f.write('const uint8_t bitmap[] = {\n')
    for i in range(0, len(byte_arr), 16):
        line = ', '.join(f'0x{b:02X}' for b in byte_arr[i:i+16])
        f.write('   ' + line + (',' if i+16 < len(byte_arr) else '') + '\n')
    f.write('};\n')