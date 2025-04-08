from machine import UART

from machine import FPIOA

import struct
# 实例化FPIOA
fpioa = FPIOA()

# 将指定引脚配置为 UART 功能
fpioa.set_function(11, FPIOA.UART2_TXD)
fpioa.set_function(12, FPIOA.UART2_RXD)
# 初始化UART
uart = UART(UART.UART2, baudrate=115200, bits=UART.EIGHTBITS, parity=UART.PARITY_NONE, stop=UART.STOPBITS_ONE)

# 定义帧头
FRAME_HEADER = 0xAA

# 数据包格式：帧头 + 数据长度 + 数据 + 校验位
def create_packet(data):
    """
    创建一个数据包。
    :param data: 要发送的原始数据（字节列表）
    :return: 打包后的数据包
    """
    # 计算数据长度
    data_len = len(data)

    # 计算BCC校验值
    bcc = calculate_bcc(data)

    # 按照格式打包数据包
    packet = struct.pack(f"<BB{data_len}B", FRAME_HEADER, data_len, *data, bcc)
    return packet


def calculate_bcc(data):
    """
    计算BCC校验值。
    :param data: 数据字节列表
    :return: BCC校验值
    """
    bcc = 0
    for byte in data:
        bcc ^= byte
    return bcc


def send_packet(uart, data):
    """
    发送数据包。
    :param uart: UART对象
    :param data: 要发送的原始数据（字节列表）
    """
    packet = create_packet(data)
    uart.write(packet)


def receive_packet(uart):
    """
    接收并解析数据包。
    :param uart: UART对象
    :return: 解析后的数据（如果成功），否则返回None
    """
    # 读取帧头
    header = uart.read(1)
    if not header or header[0] != FRAME_HEADER:
        return None

    # 读取数据长度
    data_len_bytes = uart.read(1)
    if not data_len_bytes:
        return None
    data_len = data_len_bytes[0]

    # 读取数据部分
    data = uart.read(data_len)
    if not data or len(data) != data_len:
        return None

    # 读取校验位
    bcc_received = uart.read(1)
    if not bcc_received:
        return None
    bcc_received = bcc_received[0]

    # 计算BCC校验值
    bcc_calculated = calculate_bcc(data)
    if bcc_received != bcc_calculated:
        print("BCC error")
        return None

    return data


## 示例使用
# if __name__ == "__main__":
#     # 发送数据
#     data_to_send = [0x01, 0x02, 0x03]
#     send_packet(uart, data_to_send)
#     print("Packet sent:", data_to_send)
#
#     # 接收数据
#     received_data = receive_packet(uart)
#     if received_data:
#         print("Packet received:", list(received_data))
