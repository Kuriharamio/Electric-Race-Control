def fit_quadrilateral(poly):
    """
    把任意多边形 poly 拟合为四边形
    poly: (N,1,2) 的 OpenCV 轮廓
    返回: 4×2 的 ndarray，顺时针排列
    """
    pts = poly[:, 0, :].astype(np.float32)          # (N,2)

    # 1. PCA 找主轴，旋转
    mean, eigvec = cv2.PCACompute(pts, mean=None)
    angle = np.arctan2(eigvec[0, 1], eigvec[0, 0])
    ca, sa = np.cos(angle), np.sin(angle)
    R = np.array([[ca, sa], [-sa, ca]])
    pts_rot = (pts - mean) @ R.T                    # 旋转后坐标

    # 2. 按极角四等分并取均值点
    angles = np.arctan2(pts_rot[:, 1], pts_rot[:, 0])
    bins = np.floor((angles + np.pi) / (np.pi / 2)) % 4
    quad = []
    for k in range(4):
        idx = np.where(bins == k)[0]
        quad.append(pts[idx].mean(axis=0) if len(idx) else pts[0])
    quad = np.array(quad, np.float32)

    # 3. 顺时针排序
    center = quad.mean(axis=0)
    angles = np.arctan2(quad[:, 1] - center[1], quad[:, 0] - center[0])
    quad = quad[np.argsort(angles)]

    return np.int0(np.round(quad))

def detect_screen_region(bgr, min_area_ratio=0.3):
    """
    检测画面中最大的白色区域（屏幕）
    返回：屏幕区域的四个顶点坐标 (多边形) 和对应的掩码
    """
    gray = cv2.cvtColor(bgr, cv2.COLOR_BGR2GRAY)
    _, mask = cv2.threshold(gray, SCREEN_BRIGHTNESS_TH, 255, cv2.THRESH_BINARY)
    
    # 形态学操作去除噪声
    mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, np.ones((5,5), np.uint8))
    mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, np.ones((15,15), np.uint8))
    
    # 查找轮廓
    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    if not contours:
        return None, None
    
    # 找到最大轮廓
    max_contour = max(contours, key=cv2.contourArea)
    
    # 计算轮廓面积占整个画面的比例
    area_ratio = cv2.contourArea(max_contour) / (bgr.shape[0] * bgr.shape[1])
    if area_ratio < min_area_ratio:
        return None, None
    
    # 多边形近似
    epsilon = 0.03 * cv2.arcLength(max_contour, True)
    approx = cv2.approxPolyDP(max_contour, epsilon, True)

    quad = fit_quadrilateral(approx) 
    
    # 创建屏幕掩码
    screen_mask = np.zeros_like(mask)
    cv2.drawContours(screen_mask, [quad], -1, 255, -1)
    
    return quad, screen_mask