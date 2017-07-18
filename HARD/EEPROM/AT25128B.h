#ifndef  __AT25128B_H
#define  __AT25128B_H

#include "sys.h"





u8 ReadEepromByte(u32 Address);
u8 WriteEepromByte(u32 Address, u8 Value);
void ReadEepromBlock(u32 Address, u8 *Block, u32 Size);
u8 WriteEepromBlock(u32 Address, u8 *Block, u32 Size);

#endif 









