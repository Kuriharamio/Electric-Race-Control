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
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X

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



/* Defines for PWM_MOTOR_R */
#define PWM_MOTOR_R_INST                                                   TIMA0
#define PWM_MOTOR_R_INST_IRQHandler                             TIMA0_IRQHandler
#define PWM_MOTOR_R_INST_INT_IRQN                               (TIMA0_INT_IRQn)
#define PWM_MOTOR_R_INST_CLK_FREQ                                       40000000
/* GPIO defines for channel 1 */
#define GPIO_PWM_MOTOR_R_C1_PORT                                           GPIOA
#define GPIO_PWM_MOTOR_R_C1_PIN                                    DL_GPIO_PIN_7
#define GPIO_PWM_MOTOR_R_C1_IOMUX                                (IOMUX_PINCM14)
#define GPIO_PWM_MOTOR_R_C1_IOMUX_FUNC               IOMUX_PINCM14_PF_TIMA0_CCP1
#define GPIO_PWM_MOTOR_R_C1_IDX                              DL_TIMER_CC_1_INDEX

/* Defines for PWM_MOTOR_L */
#define PWM_MOTOR_L_INST                                                   TIMA1
#define PWM_MOTOR_L_INST_IRQHandler                             TIMA1_IRQHandler
#define PWM_MOTOR_L_INST_INT_IRQN                               (TIMA1_INT_IRQn)
#define PWM_MOTOR_L_INST_CLK_FREQ                                       40000000
/* GPIO defines for channel 1 */
#define GPIO_PWM_MOTOR_L_C1_PORT                                           GPIOA
#define GPIO_PWM_MOTOR_L_C1_PIN                                   DL_GPIO_PIN_24
#define GPIO_PWM_MOTOR_L_C1_IOMUX                                (IOMUX_PINCM54)
#define GPIO_PWM_MOTOR_L_C1_IOMUX_FUNC               IOMUX_PINCM54_PF_TIMA1_CCP1
#define GPIO_PWM_MOTOR_L_C1_IDX                              DL_TIMER_CC_1_INDEX



/* Defines for ENCODER_TIMER */
#define ENCODER_TIMER_INST                                               (TIMG7)
#define ENCODER_TIMER_INST_IRQHandler                           TIMG7_IRQHandler
#define ENCODER_TIMER_INST_INT_IRQN                             (TIMG7_INT_IRQn)
#define ENCODER_TIMER_INST_LOAD_VALUE                                    (7499U)
/* Defines for PID_TIMER */
#define PID_TIMER_INST                                                   (TIMG6)
#define PID_TIMER_INST_IRQHandler                               TIMG6_IRQHandler
#define PID_TIMER_INST_INT_IRQN                                 (TIMG6_INT_IRQn)
#define PID_TIMER_INST_LOAD_VALUE                                        (7499U)
/* Defines for READ_TIMER */
#define READ_TIMER_INST                                                  (TIMG8)
#define READ_TIMER_INST_IRQHandler                              TIMG8_IRQHandler
#define READ_TIMER_INST_INT_IRQN                                (TIMG8_INT_IRQn)
#define READ_TIMER_INST_LOAD_VALUE                                        (499U)
/* Defines for PTZ_TIMER */
#define PTZ_TIMER_INST                                                   (TIMG0)
#define PTZ_TIMER_INST_IRQHandler                               TIMG0_IRQHandler
#define PTZ_TIMER_INST_INT_IRQN                                 (TIMG0_INT_IRQn)
#define PTZ_TIMER_INST_LOAD_VALUE                                        (9999U)



/* Defines for UART_0 */
#define UART_0_INST                                                        UART0
#define UART_0_INST_IRQHandler                                  UART0_IRQHandler
#define UART_0_INST_INT_IRQN                                      UART0_INT_IRQn
#define GPIO_UART_0_RX_PORT                                                GPIOA
#define GPIO_UART_0_TX_PORT                                                GPIOA
#define GPIO_UART_0_RX_PIN                                        DL_GPIO_PIN_11
#define GPIO_UART_0_TX_PIN                                        DL_GPIO_PIN_10
#define GPIO_UART_0_IOMUX_RX                                     (IOMUX_PINCM22)
#define GPIO_UART_0_IOMUX_TX                                     (IOMUX_PINCM21)
#define GPIO_UART_0_IOMUX_RX_FUNC                      IOMUX_PINCM22_PF_UART0_RX
#define GPIO_UART_0_IOMUX_TX_FUNC                      IOMUX_PINCM21_PF_UART0_TX
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
#define UART_1_BAUD_RATE                                                (115200)
#define UART_1_IBRD_40_MHZ_115200_BAUD                                      (21)
#define UART_1_FBRD_40_MHZ_115200_BAUD                                      (45)
/* Defines for UART_2 */
#define UART_2_INST                                                        UART3
#define UART_2_INST_IRQHandler                                  UART3_IRQHandler
#define UART_2_INST_INT_IRQN                                      UART3_INT_IRQn
#define GPIO_UART_2_RX_PORT                                                GPIOA
#define GPIO_UART_2_TX_PORT                                                GPIOA
#define GPIO_UART_2_RX_PIN                                        DL_GPIO_PIN_13
#define GPIO_UART_2_TX_PIN                                        DL_GPIO_PIN_14
#define GPIO_UART_2_IOMUX_RX                                     (IOMUX_PINCM35)
#define GPIO_UART_2_IOMUX_TX                                     (IOMUX_PINCM36)
#define GPIO_UART_2_IOMUX_RX_FUNC                      IOMUX_PINCM35_PF_UART3_RX
#define GPIO_UART_2_IOMUX_TX_FUNC                      IOMUX_PINCM36_PF_UART3_TX
#define UART_2_BAUD_RATE                                                  (9600)
#define UART_2_IBRD_10_MHZ_9600_BAUD                                        (65)
#define UART_2_FBRD_10_MHZ_9600_BAUD                                         (7)
/* Defines for UART_3 */
#define UART_3_INST                                                        UART2
#define UART_3_INST_IRQHandler                                  UART2_IRQHandler
#define UART_3_INST_INT_IRQN                                      UART2_INT_IRQn
#define GPIO_UART_3_RX_PORT                                                GPIOB
#define GPIO_UART_3_TX_PORT                                                GPIOB
#define GPIO_UART_3_RX_PIN                                        DL_GPIO_PIN_16
#define GPIO_UART_3_TX_PIN                                        DL_GPIO_PIN_15
#define GPIO_UART_3_IOMUX_RX                                     (IOMUX_PINCM33)
#define GPIO_UART_3_IOMUX_TX                                     (IOMUX_PINCM32)
#define GPIO_UART_3_IOMUX_RX_FUNC                      IOMUX_PINCM33_PF_UART2_RX
#define GPIO_UART_3_IOMUX_TX_FUNC                      IOMUX_PINCM32_PF_UART2_TX
#define UART_3_BAUD_RATE                                                  (9600)
#define UART_3_IBRD_40_MHZ_9600_BAUD                                       (260)
#define UART_3_FBRD_40_MHZ_9600_BAUD                                        (27)





/* Defines for ADC_GRAY_SCALE */
#define ADC_GRAY_SCALE_INST                                                 ADC0
#define ADC_GRAY_SCALE_INST_IRQHandler                           ADC0_IRQHandler
#define ADC_GRAY_SCALE_INST_INT_IRQN                             (ADC0_INT_IRQn)
#define ADC_GRAY_SCALE_ADCMEM_ADC_CH0                         DL_ADC12_MEM_IDX_0
#define ADC_GRAY_SCALE_ADCMEM_ADC_CH0_REF         DL_ADC12_REFERENCE_VOLTAGE_VDDA
#define ADC_GRAY_SCALE_ADCMEM_ADC_CH0_REF_VOLTAGE                                      -1 // VDDA cannot be determined
#define GPIO_ADC_GRAY_SCALE_C0_PORT                                        GPIOA
#define GPIO_ADC_GRAY_SCALE_C0_PIN                                DL_GPIO_PIN_27

/* Defines for ADC_BUTTON */
#define ADC_BUTTON_INST                                                     ADC1
#define ADC_BUTTON_INST_IRQHandler                               ADC1_IRQHandler
#define ADC_BUTTON_INST_INT_IRQN                                 (ADC1_INT_IRQn)
#define ADC_BUTTON_ADCMEM_ADC_CH0                             DL_ADC12_MEM_IDX_0
#define ADC_BUTTON_ADCMEM_ADC_CH0_REF            DL_ADC12_REFERENCE_VOLTAGE_VDDA
#define ADC_BUTTON_ADCMEM_ADC_CH0_REF_VOLTAGE                                      -1 // VDDA cannot be determined
#define GPIO_ADC_BUTTON_C8_PORT                                            GPIOA
#define GPIO_ADC_BUTTON_C8_PIN                                    DL_GPIO_PIN_22



/* Defines for LED: GPIOB.22 with pinCMx 50 on package pin 21 */
#define REMINDER_LED_PORT                                                (GPIOB)
#define REMINDER_LED_PIN                                        (DL_GPIO_PIN_22)
#define REMINDER_LED_IOMUX                                       (IOMUX_PINCM50)
/* Defines for BUZZ: GPIOA.29 with pinCMx 4 on package pin 36 */
#define REMINDER_BUZZ_PORT                                               (GPIOA)
#define REMINDER_BUZZ_PIN                                       (DL_GPIO_PIN_29)
#define REMINDER_BUZZ_IOMUX                                       (IOMUX_PINCM4)
/* Defines for LAZER: GPIOB.13 with pinCMx 30 on package pin 1 */
#define REMINDER_LAZER_PORT                                              (GPIOB)
#define REMINDER_LAZER_PIN                                      (DL_GPIO_PIN_13)
#define REMINDER_LAZER_IOMUX                                     (IOMUX_PINCM30)
/* Port definition for Pin Group ENCODER_R */
#define ENCODER_R_PORT                                                   (GPIOB)

/* Defines for R_A: GPIOB.17 with pinCMx 43 on package pin 14 */
// groups represented: ["ENCODER_L","ENCODER_R"]
// pins affected: ["L_A","L_B","R_A","R_B"]
#define GPIO_MULTIPLE_GPIOB_INT_IRQN                            (GPIOB_INT_IRQn)
#define GPIO_MULTIPLE_GPIOB_INT_IIDX            (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define ENCODER_R_R_A_IIDX                                  (DL_GPIO_IIDX_DIO17)
#define ENCODER_R_R_A_PIN                                       (DL_GPIO_PIN_17)
#define ENCODER_R_R_A_IOMUX                                      (IOMUX_PINCM43)
/* Defines for R_B: GPIOB.18 with pinCMx 44 on package pin 15 */
#define ENCODER_R_R_B_IIDX                                  (DL_GPIO_IIDX_DIO18)
#define ENCODER_R_R_B_PIN                                       (DL_GPIO_PIN_18)
#define ENCODER_R_R_B_IOMUX                                      (IOMUX_PINCM44)
/* Port definition for Pin Group ENCODER_L */
#define ENCODER_L_PORT                                                   (GPIOB)

/* Defines for L_A: GPIOB.21 with pinCMx 49 on package pin 20 */
#define ENCODER_L_L_A_IIDX                                  (DL_GPIO_IIDX_DIO21)
#define ENCODER_L_L_A_PIN                                       (DL_GPIO_PIN_21)
#define ENCODER_L_L_A_IOMUX                                      (IOMUX_PINCM49)
/* Defines for L_B: GPIOB.11 with pinCMx 28 on package pin 63 */
#define ENCODER_L_L_B_IIDX                                  (DL_GPIO_IIDX_DIO11)
#define ENCODER_L_L_B_PIN                                       (DL_GPIO_PIN_11)
#define ENCODER_L_L_B_IOMUX                                      (IOMUX_PINCM28)
/* Defines for STBY: GPIOB.14 with pinCMx 31 on package pin 2 */
#define MOTOR_DRV_STBY_PORT                                              (GPIOB)
#define MOTOR_DRV_STBY_PIN                                      (DL_GPIO_PIN_14)
#define MOTOR_DRV_STBY_IOMUX                                     (IOMUX_PINCM31)
/* Defines for L_IN1: GPIOB.1 with pinCMx 13 on package pin 48 */
#define MOTOR_DRV_L_IN1_PORT                                             (GPIOB)
#define MOTOR_DRV_L_IN1_PIN                                      (DL_GPIO_PIN_1)
#define MOTOR_DRV_L_IN1_IOMUX                                    (IOMUX_PINCM13)
/* Defines for L_IN2: GPIOB.0 with pinCMx 12 on package pin 47 */
#define MOTOR_DRV_L_IN2_PORT                                             (GPIOB)
#define MOTOR_DRV_L_IN2_PIN                                      (DL_GPIO_PIN_0)
#define MOTOR_DRV_L_IN2_IOMUX                                    (IOMUX_PINCM12)
/* Defines for R_IN1: GPIOA.16 with pinCMx 38 on package pin 9 */
#define MOTOR_DRV_R_IN1_PORT                                             (GPIOA)
#define MOTOR_DRV_R_IN1_PIN                                     (DL_GPIO_PIN_16)
#define MOTOR_DRV_R_IN1_IOMUX                                    (IOMUX_PINCM38)
/* Defines for R_IN2: GPIOB.19 with pinCMx 45 on package pin 16 */
#define MOTOR_DRV_R_IN2_PORT                                             (GPIOB)
#define MOTOR_DRV_R_IN2_PIN                                     (DL_GPIO_PIN_19)
#define MOTOR_DRV_R_IN2_IOMUX                                    (IOMUX_PINCM45)
/* Port definition for Pin Group GRAY_SCALE */
#define GRAY_SCALE_PORT                                                  (GPIOB)

/* Defines for AD0: GPIOB.23 with pinCMx 51 on package pin 22 */
#define GRAY_SCALE_AD0_PIN                                      (DL_GPIO_PIN_23)
#define GRAY_SCALE_AD0_IOMUX                                     (IOMUX_PINCM51)
/* Defines for AD1: GPIOB.26 with pinCMx 57 on package pin 28 */
#define GRAY_SCALE_AD1_PIN                                      (DL_GPIO_PIN_26)
#define GRAY_SCALE_AD1_IOMUX                                     (IOMUX_PINCM57)
/* Defines for AD2: GPIOB.27 with pinCMx 58 on package pin 29 */
#define GRAY_SCALE_AD2_PIN                                      (DL_GPIO_PIN_27)
#define GRAY_SCALE_AD2_IOMUX                                     (IOMUX_PINCM58)



/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_PWM_MOTOR_R_init(void);
void SYSCFG_DL_PWM_MOTOR_L_init(void);
void SYSCFG_DL_ENCODER_TIMER_init(void);
void SYSCFG_DL_PID_TIMER_init(void);
void SYSCFG_DL_READ_TIMER_init(void);
void SYSCFG_DL_PTZ_TIMER_init(void);
void SYSCFG_DL_UART_0_init(void);
void SYSCFG_DL_UART_1_init(void);
void SYSCFG_DL_UART_2_init(void);
void SYSCFG_DL_UART_3_init(void);
void SYSCFG_DL_ADC_GRAY_SCALE_init(void);
void SYSCFG_DL_ADC_BUTTON_init(void);

void SYSCFG_DL_SYSTICK_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
