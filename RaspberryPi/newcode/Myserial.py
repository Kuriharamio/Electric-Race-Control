from rclpy.node import Node
import serial
import struct
from collections import namedtuple



class MySerial(Node):
    def __init__(self):
        super().__init__("Serial_node")
        self.timer = self.create_timer(0.001, self.timer_callback)
        self.ser = serial.Serial(port='/dev/ttyCH341USB0', baudrate=9600, timeout=1)
        self.ser.flush()

        self.float_list = []
        self._rx_buf = bytearray()
        self.FloatFrame = namedtuple('FloatFrame',
                        ['Servo_x_Pos',
                         'Servo_y_Pos',
                         'Task_ID'])
    
    def timer_callback(self):
        """
        协议：帧头 2 字节 + len 1 字节 + len*4 字节 float 数据（大端） + bcc 1 字节
        """
        if self.ser.in_waiting == 0:
            return
        # 1. 先把串口缓存一次性读完，避免逐字节 read 的阻塞
        raw = self.ser.read(self.ser.in_waiting)
        self._rx_buf.extend(raw)
        # 2. 在缓冲区里找完整帧
        while True:
            # 至少需要 5 字节（2 帧头 + 1 len + 0 数据 + 1 bcc）
            if len(self._rx_buf) < 5:
                return
            # 2-1 找帧头
            try:
                idx = self._rx_buf.index(0xA5)
            except ValueError:
                # 整个缓冲区都没有 0xA5，全部丢掉
                self._rx_buf.clear()
                return
            if idx + 1 >= len(self._rx_buf):
                # 只有一个 0xAA，后面字节还没到
                self._rx_buf = self._rx_buf[idx:]
                return
            if self._rx_buf[idx + 1] != 0x5A:
                # 不是 0xA5 0x5A
                self._rx_buf = self._rx_buf[idx + 1:]
                continue
            # 2-2 校验长度字段
            data_len = self._rx_buf[idx + 2]   # len 字段：float 个数
            frame_len = 2 + 1 + 4 * data_len + 1
            if idx + frame_len > len(self._rx_buf):
                # 帧还没收全，留着等下一轮
                self._rx_buf = self._rx_buf[idx:]
                return
            # 2-3 截取完整帧
            frame = self._rx_buf[idx: idx + frame_len]
            # 2-4 计算 BCC（对数据字段做异或）
            data_field = frame[3:-1]
            bcc = 0
            for b in data_field:
                bcc ^= b
            if bcc != frame[-1]:
                # 校验失败，丢掉这一帧，继续找
                self._rx_buf = self._rx_buf[idx + 1:]
                continue
            # 2-5 解析 float（大端 -> Python float）
            if data_len != len(self.FloatFrame._fields):
                self.get_logger().warn(
                    f'Expected {len(self.FloatFrame._fields)} floats, got {data_len}. Frame dropped.')
                self._rx_buf = self._rx_buf[idx + frame_len:]
                continue

            float_list = list(struct.unpack(f'>{data_len}f', data_field))
            # 更新命名元组
            self.latest_frame = self.FloatFrame(*float_list)
            # 2-6 打印
            self.print_data()
            # 2-7 把已处理部分清掉
            self._rx_buf = self._rx_buf[idx + frame_len:]

    def write(self, data):
        self.ser.write(data)

    def read(self, size=1):
        return self.ser.read(size)
    def close(self):
        if self.ser.is_open:
            self.ser.close()

    def send_packet(self, data):
        """
        创建并发送数据包
        :param ser: 串口对象
        :param data: 要发送的数据列表
        :return: None
        """
        packet = self.create_packet(data)
        self.ser.write(packet)

    def print_data(self):
        self.get_logger().info(f'{self.latest_frame}')

    def get_latest_frame(self):
        return self.latest_frame


    def create_packet(self, data, FRAME_HEADER_1=0xA5, FRAME_HEADER_2=0x5A):
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
        bcc = self.calculate_float_bcc(data)
        # 按照格式打包数据包
        packet = struct.pack(f">BBB{data_len}fB", FRAME_HEADER_1, FRAME_HEADER_2, data_len * 4, *data, bcc)
        return packet
    
    def calculate_float_bcc(self, data):
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

