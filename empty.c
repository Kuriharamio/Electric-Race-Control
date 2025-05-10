#include "User/config.h"

#include "BSP/board.h"
#include "BSP/uart.h"

#include "Base_Modules/reminder.h"
#include "Base_Modules/servo.h"
#include "Base_Modules/k230_serial.h"
#include "Base_Modules/adc_button.h"



// 上舵机向上
void Servo_UP_UP(void)
{
    pClass_Servo servo = Get_Servo_INST(SERVO_UP_INDEX);
    if (servo->is_inited){
         servo->Set_Angle(servo, servo->Now_Angle - 0.03);
         servo->Begin_Angle = servo->Now_Angle;
    }
       
}

// 上舵机向下
void Servo_UP_DOWN(void)
{
    pClass_Servo servo = Get_Servo_INST(SERVO_UP_INDEX);
    if (servo->is_inited){
        servo->Set_Angle(servo, servo->Now_Angle + 0.03);
        servo->Begin_Angle = servo->Now_Angle;
    }
}

// 下舵机向左
void Servo_DOWN_LEFT(void)
{
    pClass_Servo servo = Get_Servo_INST(SERVO_DOWN_INDEX);
    if (servo->is_inited){
        servo->Set_Angle(servo, servo->Now_Angle + 0.03);
        servo->Begin_Angle = servo->Now_Angle;
    }
        
}

// 下舵机向右
void Servo_DOWN_RIGHT(void)
{
    pClass_Servo servo = Get_Servo_INST(SERVO_DOWN_INDEX);
    if (servo->is_inited){
        servo->Set_Angle(servo, servo->Now_Angle - 0.03);
        servo->Begin_Angle = servo->Now_Angle;
    }
        
}

bool stop = false;
void Set_Servo_STOP(void){
    // BUZZ(ON);
    stop = !stop;
    Get_Servo_INST(SERVO_UP_INDEX)->STOP = stop;
    Get_Servo_INST(SERVO_DOWN_INDEX)->STOP = stop;
    
}

typedef enum
{
    Problem_None,
    Problem_1,
    Problem_2,
    Problem_3,
    Problem_4,
    Problem_5,
    Problem_6,
} PROBLEM;

PROBLEM Now_Problem = Problem_None;

//* 问题一：设置运动⽬标位置复位功能。执⾏此功能，红⾊光斑能从屏幕任意位置回到原点。光斑中⼼距原点误差≤2cm 
void Set_Problem_1(void)
{
    pClass_UART K230_UART = Get_UART_INST(K230_UART_INDEX);
    K230_UART->Send_Bit(K230_UART, 1);
    Now_Problem = Problem_1;
    // BUZZ(ON);
}

void Process_Problem_1(void)
{
  
}

//* 问题二：启动运动⽬标控制系统。红⾊光斑能在30 秒内沿屏幕四周边线顺时针移动⼀周，移动时光斑中⼼距边线距离≤2cm。
void Set_Problem_2(void)
{
    pClass_UART K230_UART = Get_UART_INST(K230_UART_INDEX);
    K230_UART->Send_Bit(K230_UART, 2);
    Now_Problem = Problem_2;
    // BUZZ(ON);
}

void Process_Problem_2(void)
{
  
}

// * 问题三：启动运动⽬标控制系统，红⾊光斑能在30 秒内沿胶带顺时针移动⼀周。 
void Set_Problem_3(void)
{
    pClass_UART K230_UART = Get_UART_INST(K230_UART_INDEX);
    K230_UART->Send_Bit(K230_UART, 3);
    Now_Problem = Problem_3;
    // BUZZ(ON);
}

void Process_Problem_3(void)
{

}

// * 问题四：
void Set_Problem_4(void)
{
    pClass_UART K230_UART = Get_UART_INST(K230_UART_INDEX);
    K230_UART->Send_Bit(K230_UART, 4);
    Now_Problem = Problem_4;
    // BUZZ(ON);
}


int main(void)
{
    board_init();

    // float pid_reinit_last = -1.0f;
    // float pid_reinit = -1.0f;

    pClass_Servo servo_up = create_Servo(SERVO_UP_INDEX);
    servo_up->Init(servo_up, PWM_SERVO_UP_INST, GPIO_PWM_SERVO_UP_C0_IDX, 25000, 180, 80);

    pClass_Servo servo_down = create_Servo(SERVO_DOWN_INDEX);
    servo_down->Init(servo_down, PWM_SERVO_DOWN_INST, GPIO_PWM_SERVO_DOWN_C0_IDX, 50000, 270, 135);

    float buzzer = 0.0f;
    int buzzer_cnt = 0;
    pClass_UART K230_UART = Create_UART(K230_UART_INDEX);
    K230_UART->Init(K230_UART, K230_RX_LEN_MAX, 7);
    K230_UART->Configure_Callback(K230_UART, K230_Rx_Callback);
    // K230_UART->Bind_Param_With_Id(K230_UART, 0, &(pid_reinit));
    K230_UART->Bind_Param_With_Id(K230_UART, 0, &(servo_up->PID->K_P));
    K230_UART->Bind_Param_With_Id(K230_UART, 1, &(servo_up->PID->K_I));
    K230_UART->Bind_Param_With_Id(K230_UART, 2, &(servo_up->Error));
    K230_UART->Bind_Param_With_Id(K230_UART, 3, &(servo_down->PID->K_P));
    K230_UART->Bind_Param_With_Id(K230_UART, 4, &(servo_down->PID->K_I));
    K230_UART->Bind_Param_With_Id(K230_UART, 5, &(servo_down->Error));
    K230_UART->Bind_Param_With_Id(K230_UART, 6, &(buzzer));

    // 按键配置
    pClass_ADCButton ADC_Button = Create_ADCButton();
    ADC_Button->Init(ADC_Button, ADC_BUTTON_INST, ADC_BUTTON_INST_INT_IRQN, ADC_BUTTON_ADCMEM_0);
    ADC_Button->Configure_Callback(ADC_Button, BUTTON_1, Set_Servo_STOP, softwareReset, NULL);
    ADC_Button->Configure_Callback(ADC_Button, BUTTON_2, Set_Problem_1, NULL, Servo_UP_UP);
    ADC_Button->Configure_Callback(ADC_Button, BUTTON_3, Set_Problem_2, NULL, Servo_DOWN_LEFT);
    ADC_Button->Configure_Callback(ADC_Button, BUTTON_4, Set_Problem_3, NULL, Servo_DOWN_RIGHT);
    ADC_Button->Configure_Callback(ADC_Button, BUTTON_5, Set_Problem_4, NULL, Servo_UP_DOWN);

    while (1)
    {   
        if(buzzer){
            BUZZ(ON);
        }
        if(BUZZ_STATE == ON){
            delay_ms(10);
            BUZZ(OFF);
        }
        switch(Now_Problem){
            case Problem_1:
                Process_Problem_1();
                Now_Problem = Problem_None;
                break;
            case Problem_2:
                Process_Problem_2();
                Now_Problem = Problem_None;
                break;
            case Problem_3:
                Process_Problem_3();
                Now_Problem = Problem_None;
                break;
            default:
                if(!stop)
                    LED(TOGGLE);
                break;
        }
        delay_ms(100);
    }
}
