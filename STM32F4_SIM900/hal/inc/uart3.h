/**
 * @file    uart3.h
 * @brief	  
 * @date    13 gru 2014
 * @author  Michal Ksiezopolski
 *
 *
 * @verbatim
 * Copyright (c) 2014 Michal Ksiezopolski.
 * All rights reserved. This program and the
 * accompanying materials are made available
 * under the terms of the GNU Public License
 * v3.0 which accompanies this distribution,
 * and is available at
 * http://www.gnu.org/licenses/gpl.html
 * @endverbatim
 */
#ifndef INC_UART3_H_
#define INC_UART3_H_

#include <stm32f4xx.h>

/**
 * @defgroup  USART2 USART2
 * @brief     USART2 low level functions
 */

/**
 * @addtogroup USART2
 * @{
 */

void    UART3_Init(uint32_t baud, void(*rxCb)(uint8_t), uint8_t(*txCb)(uint8_t*));
void    UART3_TxEnable(void);

// HAL functions for use in higher level
#define SIM900_HAL_Init       UART3_Init
#define SIM900_HAL_TxEnable   UART3_TxEnable
#define SIM900_HAL_IrqEnable  NVIC_EnableIRQ(USART3_IRQn);
#define SIM900_HAL_IrqDisable NVIC_DisableIRQ(USART3_IRQn);

/**
 * @}
 */

#endif /* INC_UART3_H_ */
