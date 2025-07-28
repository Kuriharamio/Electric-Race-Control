import serial
import struct


# 帧头定义
FRAME_HEADER_1 = 0xA5
FRAME_HEADER_2 = 0x5A

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
