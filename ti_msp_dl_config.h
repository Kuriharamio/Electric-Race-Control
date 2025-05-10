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



/* Defines for PWM_SERVO_DOWN */
#define PWM_SERVO_DOWN_INST                                                TIMA0
#define PWM_SERVO_DOWN_INST_IRQHandler                          TIMA0_IRQHandler
#define PWM_SERVO_DOWN_INST_INT_IRQN                            (TIMA0_INT_IRQn)
#define PWM_SERVO_DOWN_INST_CLK_FREQ                                     2500000
/* GPIO defines for channel 0 */
#define GPIO_PWM_SERVO_DOWN_C0_PORT                                        GPIOA
#define GPIO_PWM_SERVO_DOWN_C0_PIN                                 DL_GPIO_PIN_8
#define GPIO_PWM_SERVO_DOWN_C0_IOMUX                             (IOMUX_PINCM19)
#define GPIO_PWM_SERVO_DOWN_C0_IOMUX_FUNC             IOMUX_PINCM19_PF_TIMA0_CCP0
#define GPIO_PWM_SERVO_DOWN_C0_IDX                           DL_TIMER_CC_0_INDEX

/* Defines for PWM_SERVO_UP */
#define PWM_SERVO_UP_INST                                                  TIMG8
#define PWM_SERVO_UP_INST_IRQHandler                            TIMG8_IRQHandler
#define PWM_SERVO_UP_INST_INT_IRQN                              (TIMG8_INT_IRQn)
#define PWM_SERVO_UP_INST_CLK_FREQ                                       1250000
/* GPIO defines for channel 0 */
#define GPIO_PWM_SERVO_UP_C0_PORT                                          GPIOA
#define GPIO_PWM_SERVO_UP_C0_PIN                                   DL_GPIO_PIN_7
#define GPIO_PWM_SERVO_UP_C0_IOMUX                               (IOMUX_PINCM14)
#define GPIO_PWM_SERVO_UP_C0_IOMUX_FUNC              IOMUX_PINCM14_PF_TIMG8_CCP0
#define GPIO_PWM_SERVO_UP_C0_IDX                             DL_TIMER_CC_0_INDEX



/* Defines for PID_TIMER */
#define PID_TIMER_INST                                                   (TIMG0)
#define PID_TIMER_INST_IRQHandler                               TIMG0_IRQHandler
#define PID_TIMER_INST_INT_IRQN                                 (TIMG0_INT_IRQn)
#define PID_TIMER_INST_LOAD_VALUE                                        (5999U)
/* Defines for ADC_BUTTON_TIMER */
#define ADC_BUTTON_TIMER_INST                                            (TIMA1)
#define ADC_BUTTON_TIMER_INST_IRQHandler                        TIMA1_IRQHandler
#define ADC_BUTTON_TIMER_INST_INT_IRQN                          (TIMA1_INT_IRQn)
#define ADC_BUTTON_TIMER_INST_LOAD_VALUE                                  (999U)



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





/* Defines for ADC_BUTTON */
#define ADC_BUTTON_INST                                                     ADC0
#define ADC_BUTTON_INST_IRQHandler                               ADC0_IRQHandler
#define ADC_BUTTON_INST_INT_IRQN                                 (ADC0_INT_IRQn)
#define ADC_BUTTON_ADCMEM_0                                   DL_ADC12_MEM_IDX_0
#define ADC_BUTTON_ADCMEM_0_REF                  DL_ADC12_REFERENCE_VOLTAGE_VDDA
#define ADC_BUTTON_ADCMEM_0_REF_VOLTAGE                                       -1 // VDDA cannot be determined
#define GPIO_ADC_BUTTON_C0_PORT                                            GPIOA
#define GPIO_ADC_BUTTON_C0_PIN                                    DL_GPIO_PIN_27



/* Port definition for Pin Group REMINDER */
#define REMINDER_PORT                                                    (GPIOA)

/* Defines for LED: GPIOA.14 with pinCMx 36 on package pin 7 */
#define REMINDER_LED_PIN                                        (DL_GPIO_PIN_14)
#define REMINDER_LED_IOMUX                                       (IOMUX_PINCM36)
/* Defines for BUZZ: GPIOA.2 with pinCMx 7 on package pin 42 */
#define REMINDER_BUZZ_PIN                                        (DL_GPIO_PIN_2)
#define REMINDER_BUZZ_IOMUX                                       (IOMUX_PINCM7)



/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_PWM_SERVO_DOWN_init(void);
void SYSCFG_DL_PWM_SERVO_UP_init(void);
void SYSCFG_DL_PID_TIMER_init(void);
void SYSCFG_DL_ADC_BUTTON_TIMER_init(void);
void SYSCFG_DL_UART_0_init(void);
void SYSCFG_DL_ADC_BUTTON_init(void);

void SYSCFG_DL_SYSTICK_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
