from machine import FPIOA
from machine import Pin
from machine import PWM
import time
import _thread

# 实例化FPIOA
fpioa = FPIOA()
# 打印所有引脚配置
fpioa.help()
fpioa.set_function(46, FPIOA.PWM2)

pwm = PWM(2, 50, 0, enable=False)

pwm.duty(2.5)
pwm.enable(1)

while True:

    print("1")
    for i in range(25, 125, 10):
        pwm.duty(i*0.1)
        time.sleep_ms(1000)
    for i in range(125, 25, -10):
        pwm.duty(i*0.1)
        time.sleep_ms(1000)
