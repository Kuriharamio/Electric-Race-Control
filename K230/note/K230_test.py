from machine import FPIOA
from machine import Pin
from machine import PWM
import time
from machine import Timer
# 实例化FPIOA
fpioa = FPIOA()
# 打印所有引脚配置
fpioa.help()

led_i=0

# while True:
#     for i in range(8):
#         # 按位操作设置LED状态
#         LED_R.value((i >> 2) & 0x01)  # 二进制第三位控制红灯
#         LED_G.value((i >> 1) & 0x01)  # 二进制第二位控制绿灯
#         LED_B.value( i       & 0x01)  # 二进制第一位控制蓝灯
#         time.sleep(0.5)  # 每个状态保持1秒
#     print("hhh")

#设置引脚功能
fpioa.set_function(62, FPIOA.GPIO62)
fpioa.set_function(20, FPIOA.GPIO20)
fpioa.set_function(63, FPIOA.GPIO63)
fpioa.set_function(53, FPIOA.GPIO53)
fpioa.set_function(43, FPIOA.PWM1)

Button= Pin(53, Pin.IN, pull=Pin.PULL_DOWN)
LED_R = Pin(62, Pin.OUT, pull=Pin.PULL_UP, drive=15)
LED_G = Pin(20, Pin.OUT, pull=Pin.PULL_UP, drive=15)
LED_B = Pin(63, Pin.OUT, pull=Pin.PULL_UP, drive=15)
beep_pwm = PWM(1, 4000, 0, enable=False)  # 默认频率4kHz,占空比50%

beep_on = False
debounce_delay=50
last_press_time =0

LED_R.value(1)
LED_G.value(1)
LED_B.value(1)
beep_pwm.enable(1)


def led_toggle(timer):
    global led_i
    LED_R.value((led_i >> 2) & 0x01)  # 二进制第三位控制红灯
    LED_G.value((led_i >> 1) & 0x01)  # 二进制第二位控制绿灯
    LED_B.value( led_i       & 0x01)  # 二进制第一位控制蓝灯
    led_i=led_i+1
    if led_i>7:
        led_i=0

#创建定时器
tim1 = Timer(-1) #表示使用软件定时器

#配置定时器

tim1.init(period=500, mode=Timer.PERIODIC, callback=led_toggle)\
#主循环
while True:
    button_state = Button.value()  # 获取当前按键状态
    current_time = time.ticks_ms()  # 获取当前时间（单位：毫秒）

    # 检测按键从未按下(0)到按下(1)的变化（上升沿）
    if button_state == 1 and button_last_state == 0:
        # 检查按键是否在消抖时间外
        if current_time - last_press_time > debounce_delay:
            # 切换LED的状态
            if beep_on:
                beep_pwm.duty(0)
            else:
                beep_pwm.duty(50)

            beep_on = not beep_on


    # 更新上次按键状态
    button_last_state = button_state

    # 简单延时，防止主循环过于频繁
    time.sleep_ms(10)



# 叫完了释放PWM
beep_pwm.deinit()
