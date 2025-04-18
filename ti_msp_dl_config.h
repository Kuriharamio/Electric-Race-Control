/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the LP_MSPM0G3507
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_LP_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)



#define CPUCLK_FREQ                                                     80000000



/* Defines for PWM_MOTOR_L */
#define PWM_MOTOR_L_INST                                                   TIMA0
#define PWM_MOTOR_L_INST_IRQHandler                             TIMA0_IRQHandler
#define PWM_MOTOR_L_INST_INT_IRQN                               (TIMA0_INT_IRQn)
#define PWM_MOTOR_L_INST_CLK_FREQ                                       40000000
/* GPIO defines for channel 0 */
#define GPIO_PWM_MOTOR_L_C0_PORT                                           GPIOA
#define GPIO_PWM_MOTOR_L_C0_PIN                                    DL_GPIO_PIN_8
#define GPIO_PWM_MOTOR_L_C0_IOMUX                                (IOMUX_PINCM19)
#define GPIO_PWM_MOTOR_L_C0_IOMUX_FUNC               IOMUX_PINCM19_PF_TIMA0_CCP0
#define GPIO_PWM_MOTOR_L_C0_IDX                              DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_PWM_MOTOR_L_C1_PORT                                           GPIOA
#define GPIO_PWM_MOTOR_L_C1_PIN                                    DL_GPIO_PIN_7
#define GPIO_PWM_MOTOR_L_C1_IOMUX                                (IOMUX_PINCM14)
#define GPIO_PWM_MOTOR_L_C1_IOMUX_FUNC               IOMUX_PINCM14_PF_TIMA0_CCP1
#define GPIO_PWM_MOTOR_L_C1_IDX                              DL_TIMER_CC_1_INDEX

/* Defines for PWM_MOTOR_R */
#define PWM_MOTOR_R_INST                                                   TIMA1
#define PWM_MOTOR_R_INST_IRQHandler                             TIMA1_IRQHandler
#define PWM_MOTOR_R_INST_INT_IRQN                               (TIMA1_INT_IRQn)
#define PWM_MOTOR_R_INST_CLK_FREQ                                       40000000
/* GPIO defines for channel 0 */
#define GPIO_PWM_MOTOR_R_C0_PORT                                           GPIOB
#define GPIO_PWM_MOTOR_R_C0_PIN                                    DL_GPIO_PIN_2
#define GPIO_PWM_MOTOR_R_C0_IOMUX                                (IOMUX_PINCM15)
#define GPIO_PWM_MOTOR_R_C0_IOMUX_FUNC               IOMUX_PINCM15_PF_TIMA1_CCP0
#define GPIO_PWM_MOTOR_R_C0_IDX                              DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_PWM_MOTOR_R_C1_PORT                                           GPIOA
#define GPIO_PWM_MOTOR_R_C1_PIN                                   DL_GPIO_PIN_24
#define GPIO_PWM_MOTOR_R_C1_IOMUX                                (IOMUX_PINCM54)
#define GPIO_PWM_MOTOR_R_C1_IOMUX_FUNC               IOMUX_PINCM54_PF_TIMA1_CCP1
#define GPIO_PWM_MOTOR_R_C1_IDX                              DL_TIMER_CC_1_INDEX



/* Defines for ENCODER */
#define ENCODER_INST                                                     (TIMG7)
#define ENCODER_INST_IRQHandler                                 TIMG7_IRQHandler
#define ENCODER_INST_INT_IRQN                                   (TIMG7_INT_IRQn)
#define ENCODER_INST_LOAD_VALUE                                          (2499U)
/* Defines for PID */
#define PID_INST                                                         (TIMG6)
#define PID_INST_IRQHandler                                     TIMG6_IRQHandler
#define PID_INST_INT_IRQN                                       (TIMG6_INT_IRQn)
#define PID_INST_LOAD_VALUE                                              (2499U)
/* Defines for ADC_BUTTON */
#define ADC_BUTTON_INST                                                  (TIMG0)
#define ADC_BUTTON_INST_IRQHandler                              TIMG0_IRQHandler
#define ADC_BUTTON_INST_INT_IRQN                                (TIMG0_INT_IRQn)
#define ADC_BUTTON_INST_LOAD_VALUE                                      (62499U)



/* Defines for UART_0 */
#define UART_0_INST                                                        UART0
#define UART_0_INST_IRQHandler                                  UART0_IRQHandler
#define UART_0_INST_INT_IRQN                                      UART0_INT_IRQn
#define GPIO_UART_0_RX_PORT                                                GPIOA
#define GPIO_UART_0_TX_PORT                                                GPIOA
#define GPIO_UART_0_RX_PIN                                         DL_GPIO_PIN_1
#define GPIO_UART_0_TX_PIN                                         DL_GPIO_PIN_0
#define GPIO_UART_0_IOMUX_RX                                      (IOMUX_PINCM2)
#define GPIO_UART_0_IOMUX_TX                                      (IOMUX_PINCM1)
#define GPIO_UART_0_IOMUX_RX_FUNC                       IOMUX_PINCM2_PF_UART0_RX
#define GPIO_UART_0_IOMUX_TX_FUNC                       IOMUX_PINCM1_PF_UART0_TX
#define UART_0_BAUD_RATE                                                  (9600)
#define UART_0_IBRD_40_MHZ_9600_BAUD                                       (260)
#define UART_0_FBRD_40_MHZ_9600_BAUD                                        (27)
/* Defines for UART_1 */
#define UART_1_INST                                                        UART1
#define UART_1_INST_IRQHandler                                  UART1_IRQHandler
#define UART_1_INST_INT_IRQN                                      UART1_INT_IRQn
#define GPIO_UART_1_RX_PORT                                                GPIOA
#define GPIO_UART_1_TX_PORT                                                GPIOA
#define GPIO_UART_1_RX_PIN                                         DL_GPIO_PIN_9
#define GPIO_UART_1_TX_PIN                                        DL_GPIO_PIN_17
#define GPIO_UART_1_IOMUX_RX                                     (IOMUX_PINCM20)
#define GPIO_UART_1_IOMUX_TX                                     (IOMUX_PINCM39)
#define GPIO_UART_1_IOMUX_RX_FUNC                      IOMUX_PINCM20_PF_UART1_RX
#define GPIO_UART_1_IOMUX_TX_FUNC                      IOMUX_PINCM39_PF_UART1_TX
#define UART_1_BAUD_RATE                                                  (9600)
#define UART_1_IBRD_40_MHZ_9600_BAUD                                       (260)
#define UART_1_FBRD_40_MHZ_9600_BAUD                                        (27)
/* Defines for UART_2 */
#define UART_2_INST                                                        UART3
#define UART_2_INST_IRQHandler                                  UART3_IRQHandler
#define UART_2_INST_INT_IRQN                                      UART3_INT_IRQn
#define GPIO_UART_2_RX_PORT                                                GPIOB
#define GPIO_UART_2_RX_PIN                                         DL_GPIO_PIN_3
#define GPIO_UART_2_IOMUX_RX                                     (IOMUX_PINCM16)
#define GPIO_UART_2_IOMUX_RX_FUNC                      IOMUX_PINCM16_PF_UART3_RX
#define UART_2_BAUD_RATE                                                  (9600)
#define UART_2_IBRD_80_MHZ_9600_BAUD                                       (520)
#define UART_2_FBRD_80_MHZ_9600_BAUD                                        (53)





/* Defines for adckey */
#define adckey_INST                                                         ADC0
#define adckey_INST_IRQHandler                                   ADC0_IRQHandler
#define adckey_INST_INT_IRQN                                     (ADC0_INT_IRQn)
#define adckey_ADCMEM_key                                     DL_ADC12_MEM_IDX_0
#define adckey_ADCMEM_key_REF                    DL_ADC12_REFERENCE_VOLTAGE_VDDA
#define adckey_ADCMEM_key_REF_VOLTAGE                                         -1 // VDDA cannot be determined
#define GPIO_adckey_C0_PORT                                                GPIOA
#define GPIO_adckey_C0_PIN                                        DL_GPIO_PIN_27



/* Port definition for Pin Group REMINDER */
#define REMINDER_PORT                                                    (GPIOB)

/* Defines for LED: GPIOB.22 with pinCMx 50 on package pin 21 */
#define REMINDER_LED_PIN                                        (DL_GPIO_PIN_22)
#define REMINDER_LED_IOMUX                                       (IOMUX_PINCM50)
/* Defines for BUZZ: GPIOB.27 with pinCMx 58 on package pin 29 */
#define REMINDER_BUZZ_PIN                                       (DL_GPIO_PIN_27)
#define REMINDER_BUZZ_IOMUX                                      (IOMUX_PINCM58)
/* Port definition for Pin Group ENCODER_LF */
#define ENCODER_LF_PORT                                                  (GPIOB)

/* Defines for LF_A: GPIOB.12 with pinCMx 29 on package pin 64 */
// groups represented: ["ENCODER_LB","ENCODER_RF","ENCODER_RB","ENCODER_LF"]
// pins affected: ["LB_A","LB_B","RF_A","RF_B","RB_A","RB_B","LF_A","LF_B"]
#define GPIO_MULTIPLE_GPIOB_INT_IRQN                            (GPIOB_INT_IRQn)
#define GPIO_MULTIPLE_GPIOB_INT_IIDX            (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define ENCODER_LF_LF_A_IIDX                                (DL_GPIO_IIDX_DIO12)
#define ENCODER_LF_LF_A_PIN                                     (DL_GPIO_PIN_12)
#define ENCODER_LF_LF_A_IOMUX                                    (IOMUX_PINCM29)
/* Defines for LF_B: GPIOB.13 with pinCMx 30 on package pin 1 */
#define ENCODER_LF_LF_B_IIDX                                (DL_GPIO_IIDX_DIO13)
#define ENCODER_LF_LF_B_PIN                                     (DL_GPIO_PIN_13)
#define ENCODER_LF_LF_B_IOMUX                                    (IOMUX_PINCM30)
/* Port definition for Pin Group ENCODER_LB */
#define ENCODER_LB_PORT                                                  (GPIOB)

/* Defines for LB_A: GPIOB.17 with pinCMx 43 on package pin 14 */
#define ENCODER_LB_LB_A_IIDX                                (DL_GPIO_IIDX_DIO17)
#define ENCODER_LB_LB_A_PIN                                     (DL_GPIO_PIN_17)
#define ENCODER_LB_LB_A_IOMUX                                    (IOMUX_PINCM43)
/* Defines for LB_B: GPIOB.18 with pinCMx 44 on package pin 15 */
#define ENCODER_LB_LB_B_IIDX                                (DL_GPIO_IIDX_DIO18)
#define ENCODER_LB_LB_B_PIN                                     (DL_GPIO_PIN_18)
#define ENCODER_LB_LB_B_IOMUX                                    (IOMUX_PINCM44)
/* Port definition for Pin Group ENCODER_RF */
#define ENCODER_RF_PORT                                                  (GPIOB)

/* Defines for RF_A: GPIOB.8 with pinCMx 25 on package pin 60 */
#define ENCODER_RF_RF_A_IIDX                                 (DL_GPIO_IIDX_DIO8)
#define ENCODER_RF_RF_A_PIN                                      (DL_GPIO_PIN_8)
#define ENCODER_RF_RF_A_IOMUX                                    (IOMUX_PINCM25)
/* Defines for RF_B: GPIOB.9 with pinCMx 26 on package pin 61 */
#define ENCODER_RF_RF_B_IIDX                                 (DL_GPIO_IIDX_DIO9)
#define ENCODER_RF_RF_B_PIN                                      (DL_GPIO_PIN_9)
#define ENCODER_RF_RF_B_IOMUX                                    (IOMUX_PINCM26)
/* Port definition for Pin Group ENCODER_RB */
#define ENCODER_RB_PORT                                                  (GPIOB)

/* Defines for RB_A: GPIOB.21 with pinCMx 49 on package pin 20 */
#define ENCODER_RB_RB_A_IIDX                                (DL_GPIO_IIDX_DIO21)
#define ENCODER_RB_RB_A_PIN                                     (DL_GPIO_PIN_21)
#define ENCODER_RB_RB_A_IOMUX                                    (IOMUX_PINCM49)
/* Defines for RB_B: GPIOB.11 with pinCMx 28 on package pin 63 */
#define ENCODER_RB_RB_B_IIDX                                (DL_GPIO_IIDX_DIO11)
#define ENCODER_RB_RB_B_PIN                                     (DL_GPIO_PIN_11)
#define ENCODER_RB_RB_B_IOMUX                                    (IOMUX_PINCM28)
/* Defines for STBY_F: GPIOA.26 with pinCMx 59 on package pin 30 */
#define MOTOR_DRV_STBY_F_PORT                                            (GPIOA)
#define MOTOR_DRV_STBY_F_PIN                                    (DL_GPIO_PIN_26)
#define MOTOR_DRV_STBY_F_IOMUX                                   (IOMUX_PINCM59)
/* Defines for STBY_B: GPIOB.14 with pinCMx 31 on package pin 2 */
#define MOTOR_DRV_STBY_B_PORT                                            (GPIOB)
#define MOTOR_DRV_STBY_B_PIN                                    (DL_GPIO_PIN_14)
#define MOTOR_DRV_STBY_B_IOMUX                                   (IOMUX_PINCM31)
/* Defines for LF_IN1: GPIOB.15 with pinCMx 32 on package pin 3 */
#define MOTOR_DRV_LF_IN1_PORT                                            (GPIOB)
#define MOTOR_DRV_LF_IN1_PIN                                    (DL_GPIO_PIN_15)
#define MOTOR_DRV_LF_IN1_IOMUX                                   (IOMUX_PINCM32)
/* Defines for LF_IN2: GPIOB.16 with pinCMx 33 on package pin 4 */
#define MOTOR_DRV_LF_IN2_PORT                                            (GPIOB)
#define MOTOR_DRV_LF_IN2_PIN                                    (DL_GPIO_PIN_16)
#define MOTOR_DRV_LF_IN2_IOMUX                                   (IOMUX_PINCM33)
/* Defines for LB_IN1: GPIOA.15 with pinCMx 37 on package pin 8 */
#define MOTOR_DRV_LB_IN1_PORT                                            (GPIOA)
#define MOTOR_DRV_LB_IN1_PIN                                    (DL_GPIO_PIN_15)
#define MOTOR_DRV_LB_IN1_IOMUX                                   (IOMUX_PINCM37)
/* Defines for LB_IN2: GPIOA.16 with pinCMx 38 on package pin 9 */
#define MOTOR_DRV_LB_IN2_PORT                                            (GPIOA)
#define MOTOR_DRV_LB_IN2_PIN                                    (DL_GPIO_PIN_16)
#define MOTOR_DRV_LB_IN2_IOMUX                                   (IOMUX_PINCM38)
/* Defines for RF_IN1: GPIOB.6 with pinCMx 23 on package pin 58 */
#define MOTOR_DRV_RF_IN1_PORT                                            (GPIOB)
#define MOTOR_DRV_RF_IN1_PIN                                     (DL_GPIO_PIN_6)
#define MOTOR_DRV_RF_IN1_IOMUX                                   (IOMUX_PINCM23)
/* Defines for RF_IN2: GPIOB.7 with pinCMx 24 on package pin 59 */
#define MOTOR_DRV_RF_IN2_PORT                                            (GPIOB)
#define MOTOR_DRV_RF_IN2_PIN                                     (DL_GPIO_PIN_7)
#define MOTOR_DRV_RF_IN2_IOMUX                                   (IOMUX_PINCM24)
/* Defines for RB_IN1: GPIOB.0 with pinCMx 12 on package pin 47 */
#define MOTOR_DRV_RB_IN1_PORT                                            (GPIOB)
#define MOTOR_DRV_RB_IN1_PIN                                     (DL_GPIO_PIN_0)
#define MOTOR_DRV_RB_IN1_IOMUX                                   (IOMUX_PINCM12)
/* Defines for RB_IN2: GPIOB.1 with pinCMx 13 on package pin 48 */
#define MOTOR_DRV_RB_IN2_PORT                                            (GPIOB)
#define MOTOR_DRV_RB_IN2_PIN                                     (DL_GPIO_PIN_1)
#define MOTOR_DRV_RB_IN2_IOMUX                                   (IOMUX_PINCM13)



/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_PWM_MOTOR_L_init(void);
void SYSCFG_DL_PWM_MOTOR_R_init(void);
void SYSCFG_DL_ENCODER_init(void);
void SYSCFG_DL_PID_init(void);
void SYSCFG_DL_ADC_BUTTON_init(void);
void SYSCFG_DL_UART_0_init(void);
void SYSCFG_DL_UART_1_init(void);
void SYSCFG_DL_UART_2_init(void);
void SYSCFG_DL_adckey_init(void);

void SYSCFG_DL_SYSTICK_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
