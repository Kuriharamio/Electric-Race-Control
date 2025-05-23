// #include "Base_Modules/k230_serial.h"
// #include "config.h"
// typedef enum
// {
//     Problem_None,
//     Problem_1,
//     Problem_2,
//     Problem_3,
//     Problem_4,
//     Problem_5,
//     Problem_6,
// } PROBLEM;

// PROBLEM Now_Problem = Problem_None;

// //* 问题一
// void Set_Problem_1(void)
// {
//     pClass_UART K230_UART = Get_UART_INST(K230_UART_INDEX);
//     K230_UART->Send_Bit(K230_UART, 1);
//     Now_Problem = Problem_1;
// }

// void Process_Problem_1(void)
// {
// }

// //* 问题二
// void Set_Problem_2(void)
// {
//     pClass_UART K230_UART = Get_UART_INST(K230_UART_INDEX);
//     K230_UART->Send_Bit(K230_UART, 2);
//     Now_Problem = Problem_2;
// }

// void Process_Problem_2(void)
// {
// }

// // * 问题三
// void Set_Problem_3(void)
// {
//     pClass_UART K230_UART = Get_UART_INST(K230_UART_INDEX);
//     K230_UART->Send_Bit(K230_UART, 3);
//     Now_Problem = Problem_3;
// }

// void Process_Problem_3(void)
// {
// }

// // * 问题四：
// void Set_Problem_4(void)
// {
//     pClass_UART K230_UART = Get_UART_INST(K230_UART_INDEX);
//     K230_UART->Send_Bit(K230_UART, 4);
//     Now_Problem = Problem_4;
// }