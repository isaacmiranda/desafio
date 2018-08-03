/**********************************************************************************
 * 
 *                       NOTICE
 *   
 *   Modify by Cedar
 *   If you have any question, please feel free to write to me ^_^
 *   xuesong5825718@gmail.com
 *                                                                        2013-12-7
 *
 *
 *********************************************************************************/

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
* OneWireEEPROM is free software: you can redistribute it and/or modify
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

#include "OneWireEEPROM.h"

#ifdef DEBUG
#define PRINT(x, ...) do {} while(0)
#else
#define PRINT(x, ...)
#endif

//bool _useParasiticPower;
//bool _useCRC;
static int _memsize;
static int _pages;
static int _eeprom_id;
static bool active;
BYTE _ROMCode[8];

#if    (1 == USEADDRESS)
static BYTE ROMaddress[8] = 
{
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
}
#endif
static bool OneWireEEPROM_Initialize();

static void wait_us(int tick)
{
    volatile int _tick = tick;
    volatile int i = 0;
    const int    max = 10;

    while(_tick--)
    {
        for(i = 0; i < max; i++)
        {
            ; // DoNothing
        }
    }
}

static void wait_ms(volatile int tick)
{
    while(tick--)
    {
        wait_us(1000);
    }
}

static void ResetAndAddress(void)
{
    OneWireCRC_reset();                 // reset device
    if (USEADDRESS) {
        OneWireCRC_matchROM(_ROMCode);  // select which device to talk to
    } else {
        OneWireCRC_skipROM();           // broadcast
    }
}

void OneWireEEPROM_Init(void)
{
    int i = 0;

    OneWireCRC_init();

    //_useCRC=crcOn;
    //USEADDRESS=useAddr;
    //_useParasiticPower=parasitic;
#if (DSTYPE == DS2433)
    _pages = DS2433PAGES;
    _eeprom_id =DS2433EEPROM_ID;
#elif (DSTYPE == DS28EC20)
    _pages =DS28EC20PAGES;
    _eeprom_id =DS28EC20EEPROM_ID;
#else
#error "Please define DS Device type first!"
#endif

    _memsize = _pages * PAGESIZE;

    for (i = 0; i < ADDRESS_SIZE; i++)
    {
#if (0 != USEADDRESS)
        _ROMCode[i]=ROMaddress[i];
#else
        _ROMCode[i]=0;
#endif
    }

    active=0;
    OneWireEEPROM_Initialize();

}

bool OneWireEEPROM_Active(void)
{
    return (active);
}

static bool OneWireEEPROM_Initialize()
{

    int OneWireFound;
    int OneWireSameAddress;
    int i;
    BYTE _dummyaddress[8];

#ifdef DEBUG
    if (USEADDRESS) {
        PRINT("\r\nScan for device with address ");
        for (i = 0; i < ADDRESS_SIZE; i++) {
            PRINT("%x ", (int)_ROMCode[i]);
        }
        PRINT("\r\n");
    }
#endif

    OneWireSameAddress=0;

    OneWireCRC_resetSearch();
    do {
        OneWireFound=(OneWireCRC_search(_dummyaddress));
        if (OneWireFound) {

#ifdef DEBUG
            if (!USEADDRESS) {
                PRINT("Device found with Address = ");
                for (i = 0; i < ADDRESS_SIZE; i++) {
                    PRINT("%x ", (int)_dummyaddress[i]);
                }
            }
#endif

            OneWireSameAddress=1;

            if (USEADDRESS) {
                for (i = 0; i < ADDRESS_SIZE; i++) {
                    if (!((OneWireSameAddress) && (_ROMCode[i] ==_dummyaddress[i])))
                        OneWireSameAddress=0;
                }
            } else {
                for (i = 0; i < ADDRESS_SIZE; i++) {
                    _ROMCode[i] =_dummyaddress[i];
                }
            }

#ifdef DEBUG
            if (OneWireSameAddress) {
                PRINT("-> Address valid!\r\n");
            }
#endif

        } else {

#ifdef DEBUG
            PRINT("No more addresses.\r\n");
#endif
            OneWireCRC_resetSearch();
            wait_ms(250);  //500
        }
    } while (OneWireFound && !OneWireSameAddress);

    if (!OneWireSameAddress) {
#ifdef DEBUG
        PRINT("-> No Valid ROM Code found.\r\n");
#endif
        return false;
    }
    if (OneWireCRC_crc8(_ROMCode, ADDRESS_CRC_BYTE) != _ROMCode[ADDRESS_CRC_BYTE]) { // check address CRC is valid
#ifdef DEBUG
        PRINT("CRC is not valid!\r\n");
#endif
        wait_ms(100);

        return false;
    }

    if (_ROMCode[0] !=  _eeprom_id) {
#ifdef DEBUG
        PRINT("Device is not a OneWireEEPROM_ID device.\r\n");
#endif
        wait_ms(100);
        return false;

    } else {
#ifdef DEBUG
        PRINT("OneWireEEPROM present and correct.\r\n");
#endif
    }
    active=1;
    return true;
}

bool OneWireEEPROM_WriteMemory(uint8_t* Source, uint16_t Address, uint8_t Size) {
    uint8_t _ES; //store Endif Address , datastatus
    uint8_t B;
    int i = 0;

    if (Address<_memsize) {
        uint8_t _TA1=(uint8_t)(Address & 0x00FF);
        uint8_t _TA2=(uint8_t)((Address & 0xFF00)>>8);

        if ((Size<=PAGESIZE) && ((Size+Address)<=_memsize)) {

            PRINT ("\r\nWriting to OneWireEEPROM %i Bytes",Size);

            ResetAndAddress();
            OneWireCRC_writeByte(WRITESCRATCHPAD);
            OneWireCRC_writeByte(_TA1); //begin address T7..T0
            OneWireCRC_writeByte(_TA2); //begin address T15..T8

            //write _memPage to scratchpad

            for (i = 0; i < Size; i++) {
                OneWireCRC_writeByte(Source[i]);
                PRINT ("%X ",Source[i]);
            }

            PRINT ("\r\nTA1=%X",_TA1);
            PRINT ("\r\nTA2=%X\r\n",_TA2);

            //read and check data in scratchpad
            ResetAndAddress();
            OneWireCRC_writeByte(READSCRATCHPAD);
            B=OneWireCRC_readByte();
            if (B != _TA1) { //check TA1, return if bad
                PRINT("\r\nWrite error in TA1 %X - %X\r\n",B,_TA1);
                return false;
            }
            B=OneWireCRC_readByte();
            if (B != _TA2) { //check TA2, return if bad
                PRINT("\r\nWrite error in TA2 %X - %X\r\n",B,_TA2);
                return false;
            }
            _ES = OneWireCRC_readByte(); // ES Register

            //check data written
            for (i = 0; i < Size; i++) {
                B=OneWireCRC_readByte();
                if (B != Source[i]) { //return if bad
                    PRINT("\r\nWrite error in scratchpad on %i %X<->%X\r\n",i,B ,Source[i]);
                    return false;
                }
            }

            PRINT("\r\nES=%X\r\n",_ES);

            //issue copy with auth data
            wait_ms(10);
            ResetAndAddress();
            OneWireCRC_writeByte(COPYSCRATCHPAD);
            OneWireCRC_writeByte(_TA1);
            OneWireCRC_writeByte(_TA2);
            OneWireCRC_writeByte(_ES); //pull-up!
            wait_ms(10); //10ms min strong pullup delay -> time needed to copy from scratchpad to memory

            OneWireCRC_reset();

            PRINT ("\r\nData written\r\n");

            return true;
        } else {
            PRINT ("\r\nTrying to write more then %i bytes-> %i\r\n",PAGESIZE,Size);
            return false;
        }
    } else {
        PRINT ("\r\nAddress %X not available, EEPROM isn't that bigg..-> %i\r\n",Address);
        return false;
    }
}

int OneWireEEPROM_ReadMemory(uint8_t* Destination, uint16_t Address, uint16_t Size) {
    uint8_t tmpReader;
    //  bool readFF = 0;
    int memPtr;

    if (Address<_memsize) {

        uint8_t _TA1=(uint8_t)(Address & 0x00FF);
        uint8_t _TA2=(uint8_t)((Address & 0xFF00)>>8);

        if ((Size+Address)<=_memsize) {

            ResetAndAddress();
            OneWireCRC_writeByte(READMEMORY);
            OneWireCRC_writeByte(_TA1);
            OneWireCRC_writeByte(_TA2);

            PRINT ("\r\nReading... TA1=%X TA2=%X",_TA1, _TA2);

            for (memPtr = 0; memPtr < Size; memPtr++) {
                tmpReader = OneWireCRC_readByte();
                //   if (tmpReader == 0xff & !readFF)
                //       readFF = 1;
                //   else if (tmpReader == 0xff & readFF)
                // 0xff read twice, hopefully EoF as we break here :)
                //       break;

                Destination[memPtr] = tmpReader;
            }
            PRINT ("-> %i byte(s)\r\n",memPtr);
            return memPtr;

        }  else {
            PRINT ("\r\nTrying to read outside MEMORY -> %i\r\n",Address+Size);
            return 0;
        }
    } else {
        PRINT ("\r\nAddress %X not available, EEPROM isn't that bigg..-> %i\r\n",Address);
        return 0;
    }
}

#if 0
void OneWireEEPROM_ShowMemory(int PageFrom, int PageTo) {
    int Size;
    int MemSize;
    uint8_t *MemAll;
    int i = 0;
    int j = 0;

    if (PageFrom<0) {
        PageFrom=0;
    }

    if (PageTo>=_pages) {
        PageTo=_pages-1;
    }

    if ((PageFrom>=0) && (PageFrom<_pages) && (PageTo>=0) && (PageTo<_pages)) {

        MemSize=(PageTo-PageFrom+1)*PAGESIZE;

        MemAll = (uint8_t*) malloc(MemSize);

        if (MemAll!=NULL) {
            PRINT ("\r\nRead Page(s) from EEPROM %i to %i -> ", PageFrom,PageTo);

            Size=ReadMemory(MemAll,PageFrom*PAGESIZE,MemSize);

            PRINT("%i bytes\r\n ",Size);

            for (j=PageFrom;j<=PageTo;j++) {
                PRINT ("\r\nPage %2i ->",j);
                for (i=0;i<PAGESIZE;i++) {
                    if ((j*32+i)<= Size)
                        PRINT("%2X ",MemAll[j*32+i]);
                }
                if ((j*32)>Size)
                    break;
            }
            PRINT ("\r\n");

            free(MemAll);
        } else
            PRINT ("\r\nNOT enough memory to display EEPROM content !!\r\n");
    }
}
#endif

bool OneWireEEPROM_WriteWord(uint16_t v, uint16_t Address) {
    bool ok=false;
    int it=0;
    uint8_t Mem[2];

    Mem[0]=(uint8_t)(v & 0x00FF);
    Mem[1]=(uint8_t)((v & 0xFF00)>>8);

    do {
        ok = OneWireEEPROM_WriteMemory(Mem,Address,2);
        it++;
    } while ((ok==false) && (it<5));

#ifdef DEBUG
    if (it>1) {
        PRINT (" EEPROM WriteWord %i times ->",it);
        if (ok)
            PRINT("ok\r\n");
        else
            PRINT("not ok\r\n");
    }
#endif

    return ok;
}

bool OneWireEEPROM_WriteByte(uint8_t b, uint16_t Address) {
    bool ok=false;
    int it=0;
    uint8_t Mem[1];

    Mem[0]=(uint8_t)(b & 0x00FF);

    do {
        ok= OneWireEEPROM_WriteMemory(Mem,Address,1);
        it++;
    } while ((ok==false) && (it<5));

#ifdef DEBUG
    if (it>1) {
        PRINT (" EEPROM WriteByte %i times ->",it);
        if (ok)
            PRINT("ok\r\n");
        else
            PRINT("not ok\r\n");
    }
#endif

    return ok;
}

uint16_t OneWireEEPROM_ReadWord(uint16_t Address) {
    int size;
    int it=0;
    uint8_t Mem[2];

    do {
        size=OneWireEEPROM_ReadMemory(Mem,Address,2);
        it++;
    } while ((size==0) && (it<5));

#ifdef DEBUG
    PRINT ("%i %i \r\n",Mem[0],Mem[1]);
    if (it>1) {
        PRINT (" EEPROM ReadWord %i times ->",it);
        if (size>0)
            PRINT("ok\r\n");
        else
            PRINT("not ok\r\n");
    }
#endif

    return (uint16_t)Mem[0] + (uint16_t) (Mem[1]<<8);
}

uint8_t OneWireEEPROM_ReadByte(uint8_t Address) {
    int size;
    int it=0;
    uint8_t Mem[1];

    do {
        size= OneWireEEPROM_ReadMemory(Mem,Address,1);
        it++;
    } while ((size==0) && (it<5));

#ifdef DEBUG
    PRINT ("%i \r\n",Mem[0]);
    if (it>1) {
        PRINT (" EEPROM ReadByte %i times ->",it);
        if (size>0)
            PRINT("ok\r\n");
        else
            PRINT("not ok\r\n");
    }
#endif

    return (uint8_t) Mem[0];
}
