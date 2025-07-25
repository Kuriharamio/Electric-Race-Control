epsilon = 0.1  # 像素级误差阈值，可调（越大点越少）
subpaths_dp = []
for cnt in contours:
    approx = cv2.approxPolyDP(cnt, epsilon, closed=False)  # 简化
    subpaths_dp.append(approx.squeeze(1).tolist())

subpaths_px = subpaths_dp   # 替换原列表