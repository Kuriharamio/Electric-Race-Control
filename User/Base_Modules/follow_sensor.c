#include "Base_Modules/follow_sensor.h"
#ifdef USE_GRAY_SENSOR
static Class_GraySensor _Gray_Sensor = {0};

unsigned short white[8] = {1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000};
unsigned short black[8] = {800, 800, 800, 800, 800, 800, 800, 800};

pClass_GraySensor Create_GraySensor(void)
{
    pClass_GraySensor sensor = &_Gray_Sensor;

    // 函数指针赋值
    sensor->Init_With_Params = GraySensor_Init_With_Params; // 带校准参数的初始化
    sensor->Init_Without_Params = GraySensor_Init_Without_Params; // 首次初始化
    sensor->Update = GraySensor_Update; // 更新传感器数据

    sensor->is_inited = false; // 初始状态未就绪

    return sensor;
}

pClass_GraySensor Get_Sensor_Handle(void)
{
    return &_Gray_Sensor;
}

void Get_ADC_Data(pClass_GraySensor this)
{
    DL_ADC12_enableConversions(ADC_GRAY_SCALE_INST);          
    DL_ADC12_startConversion(ADC_GRAY_SCALE_INST);

    this->ADC_Value += DL_ADC12_getMemResult(ADC_GRAY_SCALE_INST, ADC_GRAY_SCALE_ADCMEM_ADC_CH0); 

}

/* 函数功能：采集8个通道的模拟值并进行均值滤波 */
void Get_Analog_Value(pClass_GraySensor this, uint8_t filter_times)
{
    unsigned char i, j;

    // 遍历8个传感器通道（3位地址线组合）
    for (i = 0; i < 8; i++)
    {
        // 通过地址线组合切换传感器通道（注意取反逻辑）
        Switch_Address_0(!(i & 0x01)); // 地址线0，对应bit0
        Switch_Address_1(!(i & 0x02)); // 地址线1，对应bit1
        Switch_Address_2(!(i & 0x04)); // 地址线2，对应bit2

        // 每个通道采集8次ADC值进行均值滤波
        for (j = 0; j < filter_times; j++)
        {
            Get_ADC_Data(this); 
        }

        if (!Direction)
            this->Analog_value[i] = this->ADC_Value / filter_times; // 计算平均值
        else
            this->Analog_value[7 - i] = this->ADC_Value / filter_times; // 计算平均值
        this->ADC_Value = 0;                                            // 重置累加器
    }
}

/* 函数功能：将模拟值转换为数字信号（二值化处理） */
void Convert_Analog_To_Digital(pClass_GraySensor this)
{
    for (int i = 0; i < 8; i++)
    {
        if (this->Analog_value[i] > this->Gray_white[i])
        {
            this->Digtal |= (1 << i); // 超过白阈值置1（白色）
            this->Digital_value[i] = 0;
        }
        else if (this->Analog_value[i] < this->Gray_black[i])
        {
            this->Digtal &= ~(1 << i); // 低于黑阈值置0（黑色）
            this->Digital_value[i] = 1;
        }
        // 中间灰度值保持原有状态
    }
}

/* 函数功能：归一化ADC值到指定范围 */
void Normalize_Analog_Values(pClass_GraySensor this)
{
    for (int i = 0; i < 8; i++)
    {
        // unsigned short n;
        // // 计算归一化值（减去黑电平后缩放）
        // if (this->Analog_value[i] < this->Calibrated_black[i])
        //     n = 0; // 低于黑电平归零
        // else
        //     n = (this->Analog_value[i] - this->Calibrated_black[i]) * this->Normal_factor[i];

        // // 限幅处理
        // if (n > this->bits)
        // {
        //     n = this->bits;
        // }
        // this->Normal_value[i] = n;
        this->Normal_value[i] = 1 - (float)(this->Analog_value[i]) / (float)(this->bits);
    }
}

/* 函数功能：传感器结构体初始化（首次初始化）
   参数说明：sensor - 传感器结构体指针 */
void GraySensor_Init_Without_Params(pClass_GraySensor this)
{
    // 清零所有校准数据和状态
    memset(this->Calibrated_black, 0, 16);
    memset(this->Calibrated_white, 0, 16);
    memset(this->Normal_value, 0, 16);
    memset(this->Analog_value, 0, 16);



    // 初始化归一化系数
    for (int i = 0; i < 8; i++)
    {
        this->Normal_factor[i] = 0.0;
    }

    this->k1 = 1.0f;
    this->k2 = 0.5f;
    this->k3 = 0.25f;
    this->k4 = 0.125f;

    // 初始化状态变量
    this->undetected = false;
    this->Digtal = 0;
    this->Time_out = 0;
    this->Tick = 0;
    this->ADC_Flag = false; // ADC采样标志


    this->is_inited = false; // 标记未完成校准
}

/* 函数功能：传感器完整初始化（带校准参数）
   参数说明：
   sensor - 传感器结构体指针
   Calibrated_white - 校准白值数组
   Calibrated_black - 校准黑值数组 */
void GraySensor_Init_With_Params(pClass_GraySensor this, unsigned short *Calibrated_white, unsigned short *Calibrated_black)
{
    GraySensor_Init_Without_Params(this);

    this->bits = 4096.0;
    this->Time_out = 1;

    double Normal_Diff[8];
    unsigned short temp;

    for (int i = 0; i < 8; i++)
    {
        // 确保白值 > 黑值（必要时交换）
        if (Calibrated_black[i] >= Calibrated_white[i])
        {
            temp = Calibrated_white[i];
            Calibrated_white[i] = Calibrated_black[i];
            Calibrated_black[i] = temp;
        }

        // 计算灰度阈值（1:2和2:1分界点）
        this->Gray_white[i] = (Calibrated_white[i] * 2 + Calibrated_black[i]) / 3;
        this->Gray_black[i] = (Calibrated_white[i] + Calibrated_black[i] * 2) / 3;

        // 保存校准数据
        this->Calibrated_black[i] = Calibrated_black[i];
        this->Calibrated_white[i] = Calibrated_white[i];

        // 处理无效校准数据（全黑/全白/相等情况）
        if ((Calibrated_white[i] == 0 && Calibrated_black[i] == 0) ||
            (Calibrated_white[i] == Calibrated_black[i]))
        {
            this->Normal_factor[i] = 0.0; // 无效通道
            continue;
        }

        // 计算归一化系数
        Normal_Diff[i] = (double)Calibrated_white[i] - (double)Calibrated_black[i];
        this->Normal_factor[i] = this->bits / Normal_Diff[i];
    }
    this->is_inited = true; // 标记初始化完成
}

/* 函数功能：传感器主任务（无定时器版本）*/
void GraySensor_Update(pClass_GraySensor this)
{
    Get_Analog_Value(this, FILTER_SIZE); // 采集数据
    Convert_Analog_To_Digital(this); // 二值化处理
    Normalize_Analog_Values(this);   // 归一化处理

    int j = 0, black = 0;
    static int black_times = 0, white_times = 0;

    float now_error = 0;
    //* 读取数据
    for(int i = 0; i < 8; i++){
        if(this->Digital_value[i] == 1){
            black++;
        }
    }

    // if (this->undetected)
    // {
    //     this->k1 = 0.0f;
    //     if (this->Search_Direction == 1)
    //     {
    //         if (this->Digital_value[0] || this->Digital_value[1])
    //         {
    //             black = 1;
    //         }
    //         else
    //         {
    //             black = 0;
    //             for (int k = 2; k < 8; k++)
    //             {
    //                 this->Digital_value[k] = 0;
    //             }
    //         }
    //     }
    //     else if (this->Search_Direction == -1)
    //     {
    //         if(this->Digital_value[6] || this->Digital_value[7]){
    //             black = 1;
    //         }else{
    //             black = 0;
    //             for (int k = 0; k < 6; k++){
    //                 this->Digital_value[k] = 0;
    //             }
    //         }
    //     }
    // }else{

    // }

    // //* 判断急转方向
    // if (this->Digital_value[7] || this->Digital_value[6])
    // {
    //     this->Search_Direction = -1;
    // }
    // if (this->Digital_value[0] || this->Digital_value[1])
    // {
    //     this->Search_Direction = 1;
    // }


    // if (black == 8)
    // {
    //     black_times++;
    //     if (black_times > 2)
    //     {
    //         black_times = 0;
    //         //* 全黑状态处理
    //     }
    // }
    // else 
    if(black == 0){
        white_times++;
        // if (white_times > 100) // 防止线比传感器间隔细，导致误判为白色区域
        // {
        //     // this->Linear_Speed_Max = -1.5;
        //     // this->undetected = true;
        //     // this->Follow_Error = 80.0f * this->Search_Direction;
        // }
        if(white_times > 20){
            white_times = 0;
            //* 全白状态处理
            this->Finish = true;
        }
    }
    else
    {
        this->undetected = false;
        black_times = 0;
        white_times = 0;
        this->Linear_Speed_Max = 0.2f;
        this->Follow_Error = this->k1 * (this->Normal_value[0] - this->Normal_value[7]) + this->k2 * (this->Normal_value[1] - this->Normal_value[6]) + this->k3 * (this->Normal_value[2] - this->Normal_value[5]) + this->k4 * (this->Normal_value[3] - this->Normal_value[4]);
    
    }
    
}

#endif