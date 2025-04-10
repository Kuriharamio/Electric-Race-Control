import struct
from machine import UART
from machine import FPIOA

# 实例化FPIOA
fpioa = FPIOA()

# 将指定引脚配置为 UART 功能
fpioa.set_function(11, FPIOA.UART2_TXD)
fpioa.set_function(12, FPIOA.UART2_RXD)
# 初始化UART
uart = UART(UART.UART2, baudrate=115200, bits=UART.EIGHTBITS, parity=UART.PARITY_NONE, stop=UART.STOPBITS_ONE)

#发送数据打包
class uart_sendpack():
    def __init__(self,packmsg,data_format,header_format):
        self.msg=packmsg
        self.sendformat=data_format
        self.headerformat=header_format
        self.sendlen=struct.calcsize(data_format)
        self.headerlen=struct.calcsize(header_format)


    def calculate_BCC(self,datalist):
        ref=0
        for b in datalist:
            ref ^= b
        return ref&0xFF


    def get_pack_list(self):
        #打包帧头部分
        header_values=list(self.msg["header"].values())
        header_bytes=struct.pack(self.headerformat,*header_values)

        #打包数据部分
        data_values=list(self.msg["data"].values())
        data_bytes=struct.pack(self.sendformat,*data_values)

        #计算校验位
        bcc=self.calculate_BCC(data_bytes+header_bytes)

        #返回打包后的数据
        return header_bytes+data_bytes+bytes([bcc])



#解包接收数据类
class uart_recvpack():
    def __init__(self,packmsg,header_format,data_format):
        self.msg=packmsg
        self.recvformat=data_format
        self.headerformat=header_format
        self.recvlen=struct.calcsize(data_format)
        self.headerlen=struct.calcsize(header_format)

    #计算校验位
    def calculate_BCC(self,datalist):
        ref=0
        for b in datalist:
            ref ^= b
        return ref&0xFF


    #完成数据包解包
    def unpack_value(self,datalist):

        try:
            if len(datalist)!=self.recvlen+self.headerlen+1:
                return  False

            headerbytes=datalist[:self.headerlen]
            databytes=datalist[self.headerlen:-1]
            bcc_recv=datalist[-1]

            #计算接收字节流校验位
            bcc=self.calculate_BCC(headerbytes+databytes)

            if bcc_recv!=bcc:
                return False

            # 解包 header 和 data
            headervalues = struct.unpack(self.headerformat, bytes(headerbytes))
            datavalues = struct.unpack(self.recvformat, bytes(databytes))

            # 更新 msg 字典
            self.msg["header"].update(zip(self.msg["header"].keys(), headervalues))
            self.msg["data"].update(zip(self.msg["data"].keys(), datavalues))

            return True
        except Exception as e:
            print("解包异常:", e)
            return False




