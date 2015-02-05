/**
 * @file    mfrc522.c
 * @brief
 * @date    3 lut 2015
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


#include <spi1.h>
#include <stdio.h>


#define DEBUG

#ifdef DEBUG
#define print(str, args...) printf(""str"%s",##args,"")
#define println(str, args...) printf("RFID--> "str"%s",##args,"\r\n")
#else
#define print(str, args...) (void)0
#define println(str, args...) (void)0
#endif


#define MFRC522_HAL_Init         SPI1_Init
#define MFRC522_HAL_Select       SPI1_Select
#define MFRC522_HAL_Deselect     SPI1_Deselect
#define MFRC522_HAL_Transmit     SPI1_Transmit
#define MFRC522_HAL_ReadBuffer   SPI1_ReadBuffer
#define MFRC522_HAL_WriteBuffer  SPI1_WriteBuffer

// Page 0 registers - command and status
#define MFRC522_COMMAND_REG     0x01 ///< Starts and stops commands execution
#define MFRC522_COMIEN_REG      0x02 ///< Enabling IRQs
#define MFRC522_DIVIEN_REG      0x03
#define MFRC522_COMIRQ_REG      0x04
#define MFRC522_DIVIRQ_REG      0x05
#define MFRC522_ERROR_REG       0x06
#define MFRC522_STATUS1_REG     0x07
#define MFRC522_STATUS2_REG     0x08
#define MFRC522_FIFODATA_REG    0x09
#define MFRC522_FIFOLEVEL_REG   0x0a
#define MFRC522_WATERLEVEL_REG  0x0b
#define MFRC522_CONTROL_REG     0x0c
#define MFRC522_BITFRAMING_REG  0x0d
#define MFRC522_COLL_REG        0x0e

// Page 1 - communication
#define MFRC522_MODE_REG        0x11
#define MFRC522_TXMODE_REG      0x12
#define MFRC522_RXMODE_REG      0x13
#define MFRC522_TXCONTROL_REG   0x14
#define MFRC522_TXASK_REG       0x15
#define MFRC522_TXSEL_REG       0x16
#define MFRC522_RXSEL_REG       0x17
#define MFRC522_RXTHRESHOLD_REG 0x18
#define MFRC522_DEMOD_REG       0x19
#define MFRC522_MFTX_REG        0x1c
#define MFRC522_MFRX_REG        0x1d
#define MFRC522_SERIALSPEED_REG 0x1f

// Page 2 - configuration
#define MFRC522_CRCRESULT_REG_H 0x21
#define MFRC522_CRCRESULT_REG_L 0x22
#define MFRC522_MODWIDTH_REG    0x24
#define MFRC522_RFCFG_REG       0x26
#define MFRC522_GSN_REG         0x27
#define MFRC522_CWGSP_REG       0x28
#define MFRC522_MODGSP_REG      0x29
#define MFRC522_TMODE_REG       0x2a
#define MFRC522_TPRESCALER_REG  0x2b
#define MFRC522_RELOAD_REG_H    0x2c
#define MFRC522_RELOAD_REG_L    0x2d
#define MFRC522_TCNTRVAL_REG_H  0x2e
#define MFRC522_TCNTRVAL_REG_L  0x2f

// Page 3 - test
#define MFRC522_TESTSEL1_REG    0x31
#define MFRC522_TESTSEL2_REG    0x32
#define MFRC522_TESTPINEN_REG   0x33
#define MFRC522_TESTPINVAL_REG  0x34
#define MFRC522_TESTBUS_REG     0x35
#define MFRC522_AUTOTEST_REG    0x36
#define MFRC522_VERSION_REG     0x37
#define MFRC522_ANALOGTEST_REG  0x38
#define MFRC522_TESTDAC1_REG    0x39
#define MFRC522_TESTDAC2_REG    0x3a
#define MFRC522_TESTADC_REG     0x3b

/**
 *
 */
typedef enum {
  CMD_IDLE                = 0b0000,//!< CMD_IDLE
  CMD_MEM                 = 0b0001,//!< CMD_MEM
  CMD_GENERATE_RANDOM_ID  = 0b0010,//!< CMD_GENERATE_RANDOM_ID
  CMD_CALC_CRC            = 0b0011,//!< CMD_CALC_CRC
  CMD_TRANSMIT            = 0b0100,//!< CMD_TRANSMIT
  CMD_NO_CMD_CHANGE       = 0b0111,//!< CMD_NO_CMD_CHANGE
  CMD_RECEIVE             = 0b1000,//!< CMD_RECEIVE
  CMD_TRANSCEIVE          = 0b1100,//!< CMD_TRANSCEIVE
  CMD_MF_AUTHENT          = 0b1110,//!< CMD_MF_AUTHENT
  CMD_SOFT_RESET          = 0b1111,//!< CMD_SOFT_RESET
} MFRC522_Commands;

uint8_t MFRC522_ReadReg(uint8_t address);
void MFRC522_WriteReg(uint8_t address, uint8_t data);
void MFRC522_SoftReset(void);

/**
 * @brief Initialize communication with RFID reader
 */
void MFRC522_Init(void) {

  MFRC522_HAL_Init();

  for (int i = 0; i < 20; i++) {
    MFRC522_HAL_Transmit(0x00);
  }

  uint8_t temp;

  MFRC522_SoftReset();

  temp = MFRC522_ReadReg(MFRC522_COMMAND_REG);
  println("Command reg 0x%02x", temp);

  temp = MFRC522_ReadReg(MFRC522_VERSION_REG);
  println("Version 0x%02x", temp);


}
/**
 * @brief Write to a register
 * @param address
 * @param data
 */
void MFRC522_WriteReg(uint8_t address, uint8_t data) {

  MFRC522_HAL_Select();

  // MSB = 0 for write
  // send register address
  MFRC522_HAL_Transmit(((address << 1) & 0x7f));
  MFRC522_HAL_Transmit(data);

  MFRC522_HAL_Deselect();
}

/**
 * @brief Write buffer to a register
 * @param address
 * @param data
 * @param len
 */
void MFRC522_WriteBufReg(uint8_t address, uint8_t* data, uint8_t len) {

  MFRC522_HAL_Select();

  // MSB = 0 for write
  // send register address
  MFRC522_HAL_Transmit(((address << 1) & 0x7f));

  for (int i = 0; i < len; i++) {
    MFRC522_HAL_Transmit(data[i]);
  }

  MFRC522_HAL_Deselect();
}

/**
 * @brief Read a register
 * @param address
 * @return
 */
uint8_t MFRC522_ReadReg(uint8_t address) {

  uint8_t ret;

  MFRC522_HAL_Select();

  // MSB = 1 for read
  // send register address
  MFRC522_HAL_Transmit(((address << 1) & 0x7f) | 0x80);
  ret = MFRC522_HAL_Transmit(0x00);

  MFRC522_HAL_Deselect();

  return ret;
}

/**
 * @brief Execute soft reset
 */
void MFRC522_SoftReset(void) {
  MFRC522_WriteReg(MFRC522_COMMAND_REG, CMD_SOFT_RESET);
}





