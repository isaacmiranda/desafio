/*
* OneWireEEPROM. Library for Maxim One-Wire EEPROM.
*
* see http://www.maxim-ic.com
*
* DS2433
* DS28EC20
*
* Copyright (C) <2011> Wim De Roeve <wim312@gmail.com>
*
* Uses the OneWireCRC library. http://mbed.org/users/snatch59/programs/OneWireCRC/gpdz56
*
* OneWire EEPROM is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* OneWireEEPROM is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _OneWireEEPROM_H
#define _OneWireEEPROM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "OneWireCRC.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>


//  OneWire info
#define ADDRESS_SIZE      8
#define ADDRESS_CRC_BYTE  7

// OneWire device-id
#define DS2433EEPROM_ID   0x23
#define DS28EC20EEPROM_ID 0x43

// OneWireEEPROM related
#define WRITESCRATCHPAD   0x0F
#define READSCRATCHPAD    0xAA
#define COPYSCRATCHPAD    0x55
#define READMEMORY        0xF0

#define PAGESIZE          0x20  // 32 bytes for each page
#define DS2433PAGES       0x10  // 16 pages 
#define DS28EC20PAGES     0x50  // 80 pages 

#define USEADDRESS        0

/*
DS2433 is a 4096 bit EEPROM
  4096 bits (512 bytes) in 16 pages of 256 bits (32 bytes)

ADDRESS 32-BYTE intermediate storage scratchpad
0x0000 to 0x001F   32 byte finale storage EEPROM    PAGE 0
0x0020 to 0x003F   32 byte finale storage EEPROM    PAGE 1
0x0040 to 0x01DF   32 byte finale storage EEPROM    PAGE 2 to PAGE 14
0x01E0 to 0x01FF   32 byte finale storage EEPROM    PAGE 15

DS28EC20 is a 20480-bit EEPROM
 20480 bits (2560 bytes) in 80 pages of 256 bits (32 bytes)

ADDRESS 32-BYTE intermediate storage scratchpad
0x0000 to 0x001F   32 byte finale storage EEPROM    PAGE 0
0x0020 to 0x003F   32 byte finale storage EEPROM    PAGE 1
0x0040 to 0x09DF   32 byte finale storage EEPROM    PAGE 2 to PAGE 78
0x09E0 to 0x09FF   32 byte finale storage EEPROM    PAGE 79

*/

#define DS2433   1
#define DS28EC20 2

#define DSTYPE  DS2433

extern void OneWireEEPROM_Init(void);

extern bool OneWireEEPROM_Active(void);

extern bool OneWireEEPROM_WriteMemory(uint8_t* Source, uint16_t Address, uint8_t Size);

extern int OneWireEEPROM_ReadMemory(uint8_t* Destination, uint16_t Address, uint16_t Size);

extern bool OneWireEEPROM_WriteWord(uint16_t v, uint16_t Address);

extern bool OneWireEEPROM_WriteByte(uint8_t b, uint16_t Address);

extern uint16_t OneWireEEPROM_ReadWord(uint16_t Address);

extern uint8_t OneWireEEPROM_ReadByte(uint8_t Address);

#ifdef __cplusplus
}
#endif

#endif
