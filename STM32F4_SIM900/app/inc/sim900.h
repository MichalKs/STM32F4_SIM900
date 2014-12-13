/**
 * @file    sim900.h
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
#ifndef INC_SIM900_H_
#define INC_SIM900_H_

#include <inttypes.h>

void SIM900_Init(uint32_t baud);
uint8_t SIM900_GetFrame(uint8_t* buf, uint8_t* len);
void SIM900_PutFrame(char* buf);

#endif /* INC_SIM900_H_ */
