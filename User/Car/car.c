#include "car.h"

/**
 * @brief 创建小车对象
 * 
 * @return Class_Car* 
 */
Class_Car *create_car(void)
{
    Class_Car *car = (Class_Car *)malloc(sizeof(Class_Car));

    // 创建电机对象
   
    // 创建PID对象

    // 函数指针赋值
    
    return car;
}

/**
 * @brief 小车初始化
 * 
 * @param self 
 */
void Car_Init(Class_Car *self)
{
    // 参数赋值

    // 初始化电机

    // 初始化PID

    // 初始化完成标志位
}


/**
 * @brief 小车定时器回调函数
 * 
 * @param self 
 */ 
void Car_TIM_PID_PeriodElapsedCallback(Class_Car *self)
{

}
