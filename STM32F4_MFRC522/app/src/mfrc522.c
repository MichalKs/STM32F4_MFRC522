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

#define MFRC522_COMMAND_REG 0x01
#define MFRC522_COMIEN_REG 0x02
#define MFRC522_DIVIEN_REG 0x03
#define MFRC522_COMIRQ_REG 0x04
#define MFRC522_DIVIRQ_REG 0x05
#define MFRC522_ERROR_REG 0x06
#define MFRC522_STATUS1_REG 0x07
#define MFRC522_STATUS2_REG 0x08
#define MFRC522_FIFODATA_REG 0x09
#define MFRC522_FIFOLEVEL_REG 0x0a
#define MFRC522_WATERLEVEL_REG 0x0b
#define MFRC522_CONTROL_REG 0x0c
#define MFRC522_BITFRAMING_REG 0x0d
#define MFRC522_COLL_REG 0x0e

#define MFRC522_MODE_REG 0x11
#define MFRC522_TXMODE_REG 0x12
#define MFRC522_RXMODE_REG 0x13
#define MFRC522_TXCONTROL_REG 0x14
#define MFRC522_TXASK_REG 0x15
#define MFRC522_TXSEL_REG 0x16
#define MFRC522_RXSEL_REG 0x17
#define MFRC522_RXTHRESHOLD_REG 0x18
#define MFRC522_DEMOD_REG 0x19
#define MFRC522_MFTX_REG 0x1c





uint8_t MFRC522_ReadReg(uint8_t address);
void MFRC522_WriteReg(uint8_t address, uint8_t data);
void MFRC522_SoftReset(void);

void MFRC522_Init(void) {

  MFRC522_HAL_Init();

  for (int i = 0; i < 20; i++) {
    MFRC522_HAL_Transmit(0x00);
  }

  uint8_t temp;

  MFRC522_SoftReset();

  temp = MFRC522_ReadReg(MFRC522_COMMAND_REG);
  println("Got 0x%02x", temp);

  temp = MFRC522_ReadReg(MFRC522_COMMAND_REG);
  println("Got 0x%02x", temp);

  temp = MFRC522_ReadReg(MFRC522_COMIEN_REG);
  println("Got 0x%02x", temp);

  temp = MFRC522_ReadReg(MFRC522_COMIEN_REG);
  println("Got 0x%02x", temp);

  temp = MFRC522_ReadReg(MFRC522_DIVIEN_REG);
  println("Got 0x%02x", temp);

}

void MFRC522_WriteReg(uint8_t address, uint8_t data) {

  MFRC522_HAL_Select();

  // MSB = 0 for write
  // send register address
  MFRC522_HAL_Transmit(((address << 1) & 0x7f));
  MFRC522_HAL_Transmit(data);

  MFRC522_HAL_Deselect();
}

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

void MFRC522_SoftReset(void) {
  MFRC522_WriteReg(MFRC522_COMMAND_REG, 0x0f);
}





