#include "User/config.h"

#include "BSP/board.h"
#include "BSP/uart.h"

#include "Base_Modules/reminder.h"
#include "Base_Modules/servo.h"
#include "Base_Modules/k230_serial.h"



int main(void)
{
    board_init();

    pClass_Servo servo_up = create_Servo(SERVO_UP_INDEX);
    servo_up->Init(servo_up, PWM_SERVO_INST, GPIO_PWM_SERVO_C0_IDX, 50000, 180);
    // servo_up->Set_Angle(servo_up, 90);

    pClass_Servo servo_down = create_Servo(SERVO_DOWN_INDEX);
    servo_down->Init(servo_down, PWM_SERVO_INST, GPIO_PWM_SERVO_C1_IDX, 50000, 270);
    // servo_down->Set_Angle(servo_down, 90);

    pClass_UART K230_UART = Create_UART(K230_UART_INDEX);
    K230_UART->Init(K230_UART, K230_RX_LEN_MAX, 4);
    K230_UART->Configure_Callback(K230_UART, K230_Rx_Callback);
    K230_UART->Bind_Param_With_Id(K230_UART, 0, &(servo_up->PID->K_I));
    K230_UART->Bind_Param_With_Id(K230_UART, 1, &(servo_up->Error));
    K230_UART->Bind_Param_With_Id(K230_UART, 2, &(servo_down->PID->K_I));
    K230_UART->Bind_Param_With_Id(K230_UART, 3, &(servo_down->Error));

    while (1)
    {
        LED(TOGGLE);
        delay_ms(1000);
    }
}
