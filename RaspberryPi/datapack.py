import serial
import struct


# 帧头定义
FRAME_HEADER_1 = 0xA5
FRAME_HEADER_2 = 0x5A



# 状态机状态定义
STATE_IDLE = 0          # 空闲状态
STATE_HEADER_1 = 1      # 已接收第一个帧头
STATE_HEADER_2 = 2      # 已接收第二个帧头
STATE_LENGTH = 3        # 已接收长度字段
STATE_DATA = 4          # 正在接收数据
STATE_CHECKSUM = 5      # 已接收校验位

context_state = STATE_IDLE
data_len = 0
data = []


def init_serial():
    ser = serial.Serial(port='/dev/ttyCH341USB0', baudrate=9600, timeout=1)
    return ser
def calculate_float_bcc(data):
    """
    计算浮点数列表的BCC校验值
    :param data: 浮点数列表
    :return: BCC校验值
    """
    bcc = 0
    for f in data:
        # 将浮点数转换为字节并计算BCC
        for byte in struct.pack('>f', f):
            bcc ^= byte
    return bcc


# 数据包格式：帧头 + 数据长度 +  数据 + 校验位
def create_packet(data):
    """
    创建一个数据包。
    :param data: 要发送的原始数据（浮点数、整数或字符列表）
    :return: 打包后的数据包
    """
    # 计算数据长度
    data_len = len(data)
    # 将data中的整数转化为浮点数
    data = [float(x) for x in data]
    # 计算bcc校验位
    bcc = calculate_float_bcc(data)

    # 按照格式打包数据包
    packet = struct.pack(f">BBB{data_len}fB", FRAME_HEADER_1, FRAME_HEADER_2, data_len * 4, *data, bcc)

    return packet


def send_packet(ser, data):
    """
    创建并发送数据包
    :param ser: 串口对象
    :param data: 要发送的数据列表
    :return: None
    """
    # 创建数据包
    packet = create_packet(data)
    # 发送数据包
    ser.write(packet)
    # 确保数据被发送出去
    ser.flush()

def receive_state(ser):
    """
    接收状态数据
    :param ser: 串口对象
    :return: 接收到的状态数据
    """
    #是否有数据
    if ser.in_waiting > 0:
        data = ser.read(1)
        numeric_data = ord(data)
        print(numeric_data)
        return numeric_data

    return 0


def receive_data(ser):
    global context_state, data_len, data
    while ser.in_waiting:
        if context_state == STATE_IDLE:
            # 等待帧头1
            header1 = ser.read(1)
            if header1 and ord(header1) == FRAME_HEADER_1:
                context_state = STATE_HEADER_1
            elif not header1:
                return None

        elif context_state == STATE_HEADER_1:
            # 等待帧头2
            header2 = ser.read(1)
            if header2 and ord(header2) == FRAME_HEADER_2:
                context_state = STATE_HEADER_2
            elif not header2:
                context_state = STATE_IDLE
                return None
            else:
                context_state = STATE_IDLE

        elif context_state == STATE_HEADER_2:
            # 读取数据长度
            len_byte = ser.read(1)
            if len_byte:
                data_len = ord(len_byte)
                context_state = STATE_LENGTH
            else:
                context_state = STATE_IDLE
                return None

        elif context_state == STATE_LENGTH:
            # 读取数据
            # 判断长度
            if ser.in_waiting >= data_len:
                data_bytes = ser.read(data_len)
                if len(data_bytes) == data_len:
                    try:
                        float_count = data_len // 4  # 每个浮点数占4个字节
                        if float_count * 4 == data_len:  # 确保数据长度是4的倍数
                            data_format = f">{float_count}f"  # ">f"表示大端序浮点数
                            data = list(struct.unpack(data_format, data_bytes))
                            context_state = STATE_DATA
                        else:
                            context_state = STATE_IDLE
                            data_len = 0
                            return None
                    except Exception as e:
                        context_state = STATE_IDLE
                        data_len = 0
                        return None
                else:
                    context_state = STATE_IDLE
                    data_len = 0
                    return None
            else:
                # 数据还没到齐，继续等待
                return None

        elif context_state == STATE_DATA:
            # 读取校验位
            bcc_byte = ser.read(1)
            if bcc_byte:
                try:
                    calculated_bcc = calculate_float_bcc(data)
                    received_bcc = ord(bcc_byte)
                    context_state = STATE_IDLE  # 重置状态机
                    data_len = 0

                    if calculated_bcc == received_bcc:
                        return data  # 校验成功，返回数据
                    else:
                        print("接收数据失败：校验和不匹配")
                        return None
                except Exception as e:
                    context_state = STATE_IDLE
                    data_len = 0
                    return None
            else:
                return None

    # 没有完整数据包
    return None




# def receive_data(ser):
#     """
#     接收一定格式的数据  数据包格式：帧头 + 数据长度 +  数据 + 校验位
#     :param ser: 串口对象
#     :return: 接收到的状态数据
#     """
#     while ser.in_waiting:
#
#         # 检查是否有足够的数据
#         if ser.in_waiting < 3:  # 至少需要帧头(2字节) + 长度字段(1字节)
#             return None
#
#         # 读取第一帧头
#         header1 = ser.read(1)
#         if not header1 or ord(header1) != FRAME_HEADER_1:
#             return None
#
#         # 读取第二帧头
#         header2 = ser.read(1)
#         if not header2 or ord(header2) != FRAME_HEADER_2:
#             return None
#
#         # 读取数据长度
#         len_data = ser.read(1)
#         if not len_data:
#             return None
#
#         data_len = ord(len_data)
#
#         # 检查缓冲区是否有足够的数据
#         if ser.in_waiting < data_len + 1:  # 数据 + 校验位
#             return None
#
#         # 读取数据
#         data_bytes = ser.read(data_len)
#         if len(data_bytes) != data_len:
#             return None
#
#         # 读取校验位
#         bcc_byte = ser.read(1)
#         if not bcc_byte:
#             return None
#
#         # 解析数据
#         try:
#             # 根据数据长度计算浮点数个数（每个浮点数4字节）
#             float_count = data_len // 4
#             data_format = f">{float_count}f"
#             data = list(struct.unpack(data_format, data_bytes))
#
#             # 计算校验值
#             calculated_bcc = calculate_float_bcc(data)
#             received_bcc = ord(bcc_byte)
#
#             # 验证校验值
#             if calculated_bcc != received_bcc:
#                 print(f"校验失败: 计算值={calculated_bcc}, 接收值={received_bcc}")
#                 return None
#
#             return data
#         except Exception as e:
#             print(f"数据解析错误: {e}")
#             return None