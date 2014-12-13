/**
 * @file    sim900.c
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

#include <sim900.h>
#include <fifo.h>
// HAL
#include <uart3.h>
#include <stdio.h>
#include <string.h>

#ifndef DEBUG
  #define DEBUG
#endif

#ifdef DEBUG
  #define print(str, args...) printf(""str"%s",##args,"")
  #define println(str, args...) printf("GSM--> "str"%s",##args,"\r\n")
#else
  #define print(str, args...) (void)0
  #define println(str, args...) (void)0
#endif

/**
 * @defgroup  SIM900 SIM900
 * @brief     Communication with SIM900 functions.
 */

/**
 * @addtogroup SIM900
 * @{
 */

#define SIM900_BUF_LEN     4096    ///< SIM900 buffer lengths
#define SIM900_TERMINATOR '\n'     ///< SIM900 frame terminator character

static uint8_t rxBuffer[SIM900_BUF_LEN]; ///< Buffer for received data.
static uint8_t txBuffer[SIM900_BUF_LEN]; ///< Buffer for transmitted data.

static FIFO_TypeDef rxFifo; ///< RX FIFO
static FIFO_TypeDef txFifo; ///< TX FIFO

static uint8_t gotFrame;  ///< Nonzero signals a new frame (number of received frames)

uint8_t SIM900_TxCallback(uint8_t* c);
void    SIM900_RxCallback(uint8_t c);

/**
 * @brief Initialize communication terminal interface.
 *
 * @param baud Required baud rate
 */
void SIM900_Init(uint32_t baud) {

  // pass baud rate
  // callback for received data and callback for
  // transmitted data
  SIM900_HAL_Init(baud, SIM900_RxCallback, SIM900_TxCallback);

  // Initialize RX FIFO
  rxFifo.buf = rxBuffer;
  rxFifo.len = SIM900_BUF_LEN;
  FIFO_Add(&rxFifo);

  // Initialize TX FIFO
  txFifo.buf = txBuffer;
  txFifo.len = SIM900_BUF_LEN;
  FIFO_Add(&txFifo);

}

/**
 * @brief Send a char to USART2.
 * @details This function can be called in stubs.c _write
 * function in order for printf to work
 *
 * @param c Char to send.
 */
void SIM900_Putc(uint8_t c) {
  // disable IRQ so it doesn't screw up FIFO count - leads to errors in transmission
  SIM900_HAL_IrqDisable;

  FIFO_Push(&txFifo,c); // Put data in TX buffer
  SIM900_HAL_TxEnable();  // Enable low level transmitter

  // enable IRQ again
  SIM900_HAL_IrqEnable;
}
/**
 * @brief Get a char from USART2
 * @return Received char.
 * @warning Blocking function! Waits until char is received.
 */
uint8_t SIM900_Getc(void) {

  uint8_t c;

  while (FIFO_IsEmpty(&rxFifo) == 1); // wait until buffer is not empty
  // buffer not empty => char was received

  FIFO_Pop(&rxFifo,&c); // Get data from RX buffer

  return c;
}
/**
 * @brief Get a complete frame from USART2 (nonblocking)
 * @param buf Buffer for data (data will be null terminated for easier string manipulation)
 * @param len Length not including terminator character
 * @retval 0 Received frame
 * @retval 1 No frame in buffer
 * @retval 2 Frame error
 * TODO Add maximum length checking so as not to overflow
 */
uint8_t SIM900_GetFrame(uint8_t* buf, uint8_t* len) {

  uint8_t c;
  *len = 0; // zero out length variable

  if (gotFrame) {
    while (1) {

      // no more data and terminator wasn't reached => error
      if (FIFO_IsEmpty(&rxFifo)) {
        *len = 0;
        println("Invalid frame");
        return 2;
      }
      FIFO_Pop(&rxFifo, &c);
      buf[(*len)++] = c;

      // if end of frame
      if (c == SIM900_TERMINATOR) {
        (*len)--; // length without terminator character
        buf[*len] = 0; // USART terminator character converted to NULL terminator

        // SIM900 sends empty lines - eliminate them
        if (*len == 1 && buf[0] == '\r') {
          len = 0;
          gotFrame--;
          return 1;
        }

        break;
      }

    }
    gotFrame--;
    return 0;

  } else {

    return 1;
  }

}
/**
 * @brief Send a zero terminated string to SIm900.
 *
 * @param buf String to send.
 */
void SIM900_PutFrame(char* buf) {

  uint16_t len = strlen(buf);

  for(int i = 0; i < len; i++) {
    SIM900_Putc(buf[i]);
  }

}

/**
 * @brief Callback for receiving data from PC.
 * @param c Data sent from lower layer software.
 */
void SIM900_RxCallback(uint8_t c) {

  uint8_t res = FIFO_Push(&rxFifo, c); // Put data in RX buffer

  // Checking res to ensure no buffer overflow occurred
  if ((c == SIM900_TERMINATOR) && (res == 0)) {
    gotFrame++;
  }
}
/**
 * @brief Callback for transmitting data to lower layer
 * @param c Transmitted data
 * @retval 0 There is no more data in buffer (stop transmitting)
 * @retval 1 Valid data in c
 */
uint8_t SIM900_TxCallback(uint8_t* c) {

  if (FIFO_Pop(&txFifo, c) == 0) { // If buffer not empty
    return 1;
  } else {
    return 0;
  }

}

/**
 * @}
 */
