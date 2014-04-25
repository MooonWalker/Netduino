////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Microsoft Corporation. All rights reserved.
//  Implementation for STM32: Copyright (c) Oberon microsystems, Inc.
//  Portions Copyright (c) Secret Labs LLC and the Netduino community. All rights reserved.
//
//  *** Flash Driver ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include "STM32_Flash.h"
#if defined(PLATFORM_ARM_STM32F4_ANY)
#include "..\stm32f4xx.h"
#elif defined(PLATFORM_ARM_STM32F2_ANY)
#include "..\stm32f2xx.h"
#else
#include "..\stm32f10x.h"
#endif


#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
    static const unsigned int STM32_FLASH_KEY1 = 0x45670123;
    static const unsigned int STM32_FLASH_KEY2 = 0xcdef89ab;
#else
    static const int STM32_FLASH_KEY1 = 0x45670123;
    static const int STM32_FLASH_KEY2 = 0xcdef89ab;
#endif


//--//

    /////////////////////////////////////////////////////////
    // Description:
    //    Initializes a given block device for use
    // 
    // Input:
    //
    // Returns:
    //   true if succesful; false if not
    //
    // Remarks:
    //    No other functions in this interface may be called
    //    until after Init returns.
    //
BOOL __section(SectionForFlashOperations)STM32_Flash_Driver::ChipInitialize( void* context )
{
    NATIVE_PROFILE_HAL_DRIVERS_FLASH();
    return TRUE;
}

    /////////////////////////////////////////////////////////
    // Description:
    //    Initializes a given block device for use
    // 
    // Returns:
    //   true if succesful; false if not
    //
    // Remarks:
    //   De initializes the device when no longer needed
    //
BOOL __section(SectionForFlashOperations)STM32_Flash_Driver::ChipUnInitialize( void* context )
{
    NATIVE_PROFILE_HAL_DRIVERS_FLASH();
    return TRUE;
}

    /////////////////////////////////////////////////////////
    // Description:
    //    Gets the information describing the device
    //
const BlockDeviceInfo* __section(SectionForFlashOperations)STM32_Flash_Driver::GetDeviceInfo( void* context )
{
    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG*)context;
    
    return config->BlockConfig.BlockDeviceInformation;    
}

    /////////////////////////////////////////////////////////
    // Description:
    //    Reads data from a set of sectors
    //
    // Input:
    //    StartSector - Starting Sector for the read
    //    NumSectors  - Number of sectors to read
    //    pSectorBuff - pointer to buffer to read the data into.
    //                  Must be large enough to hold all of the data
    //                  being read.
    //
    // Returns:
    //   true if succesful; false if not
    //
    // Remarks:
    //   This function reads the number of sectors specified from the device.
    //   
BOOL  __section(SectionForFlashOperations)STM32_Flash_Driver::Read( void* context, ByteAddress StartSector, UINT32 NumBytes, BYTE * pSectorBuff)
{
    // XIP device does not need to read into a buffer
    NATIVE_PROFILE_HAL_DRIVERS_FLASH();

    if (pSectorBuff == NULL) return FALSE;

    CHIP_WORD* ChipAddress = (CHIP_WORD *)StartSector;
    CHIP_WORD* EndAddress  = (CHIP_WORD *)(StartSector + NumBytes);
    CHIP_WORD *pBuf        = (CHIP_WORD *)pSectorBuff;

    while(ChipAddress < EndAddress)
    {
        *pBuf++ = *ChipAddress++;
    }

    return TRUE;
}


    /////////////////////////////////////////////////////////
    // Description:
    //    Writes data to a set of sectors
    //
    // Input:
    //    StartSector - Starting Sector for the write
    //    NumSectors  - Number of sectors to write
    //    pSectorBuff - pointer to data to write.
    //                  Must be large enough to hold complete sectors
    //                  for the number of sectors being written.
    //
    // Returns:
    //   true if succesful; false if not
    //
    // Remarks:
    //   This function reads the number of sectors specified from the device.
    //   
BOOL __section(SectionForFlashOperations)STM32_Flash_Driver::Write(void* context, ByteAddress Address, UINT32 NumBytes, BYTE * pSectorBuff, BOOL ReadModifyWrite)
{
    NATIVE_PROFILE_PAL_FLASH();

    // Read-modify-write is used for FAT filesystems only
    if (ReadModifyWrite) return FALSE;
    
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
	while (FLASH->SR & FLASH_SR_BSY);
#else
    // turn on HSI clock
    RCC->CR |= RCC_CR_HSION;
    while(!(RCC->CR & RCC_CR_HSIRDY));
#endif
    
    if (FLASH->CR & FLASH_CR_LOCK) { // unlock
        FLASH->KEYR = STM32_FLASH_KEY1;
        FLASH->KEYR = STM32_FLASH_KEY2;
    }
        
    CHIP_WORD* ChipAddress = (CHIP_WORD *)Address;
    CHIP_WORD* EndAddress  = (CHIP_WORD *)(Address + NumBytes);
    CHIP_WORD *pBuf        = (CHIP_WORD *)pSectorBuff;

    // enable programming
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
    FLASH->CR = FLASH_PSIZE_WORD | FLASH_CR_PG;
#else
    FLASH->CR = FLASH_CR_PG;
#endif
    
    while(ChipAddress < EndAddress) {
        if (*ChipAddress != *pBuf) {
            // write data
            *ChipAddress = *pBuf;
            // wait for completion
            while (FLASH->SR & FLASH_SR_BSY);
            // check
            if (*ChipAddress != *pBuf) {
                debug_printf( "Flash_WriteToSector failure @ 0x%08x, wrote 0x%08x, read 0x%08x\r\n", (UINT32)ChipAddress, *pBuf, *ChipAddress );
                return FALSE;
            }
        }
        ChipAddress++;
        pBuf++;
    }
    
    // reset & lock the controller
    FLASH->CR = FLASH_CR_LOCK;

#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
#else
    // stop HSI clock
    RCC->CR &= ~RCC_CR_HSION;
#endif

    return TRUE;
}

BOOL __section(SectionForFlashOperations)STM32_Flash_Driver::Memset(void* context, ByteAddress Address, UINT8 Data, UINT32 NumBytes)
{
    NATIVE_PROFILE_PAL_FLASH();

    // used for FAT filesystems only
    return FALSE;

}


BOOL __section(SectionForFlashOperations)STM32_Flash_Driver::GetSectorMetadata(void* context, ByteAddress SectorStart, SectorMetadata* pSectorMetadata)
{
    // no metadata
    return FALSE;
}

BOOL __section(SectionForFlashOperations)STM32_Flash_Driver::SetSectorMetadata(void* context, ByteAddress SectorStart, SectorMetadata* pSectorMetadata)
{
    // no metadata
    return FALSE;
}


    /////////////////////////////////////////////////////////
    // Description:
    //    Check a block is erased or not.
    // 
    // Input:
    //    BlockStartAddress - Logical Sector Address
    //
    // Returns:
    //   true if it is erassed, otherwise false
    //
    // Remarks:
    //    Check  the block containing the sector address specified.
    //    
BOOL __section(SectionForFlashOperations)STM32_Flash_Driver::IsBlockErased( void* context, ByteAddress BlockStart, UINT32 BlockLength )
{
    NATIVE_PROFILE_HAL_DRIVERS_FLASH();
    
    CHIP_WORD* ChipAddress = (CHIP_WORD *) BlockStart;
    CHIP_WORD* EndAddress  = (CHIP_WORD *)(BlockStart + BlockLength);
    
    while(ChipAddress < EndAddress)
    {
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
        if(*ChipAddress != 0xFFFFFFFF)
#else
        if(*ChipAddress != 0xFFFF)
#endif
        {
            return FALSE;
        }
        ChipAddress++;
    }

    return TRUE;
}


    /////////////////////////////////////////////////////////
    // Description:
    //    Erases a block
    // 
    // Input:
    //    Address - Logical Sector Address
    //
    // Returns:
    //   true if succesful; false if not
    //
    // Remarks:
    //    Erases the block containing the sector address specified.
    //    
BOOL __section(SectionForFlashOperations)STM32_Flash_Driver::EraseBlock( void* context, ByteAddress Sector )
{
    NATIVE_PROFILE_HAL_DRIVERS_FLASH();

#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
	UINT32 sectorNumber = (Sector - FLASH_BASE) >> 14;
	if(sectorNumber >= 4) {
		sectorNumber = (sectorNumber >> 3) + 4;
	}
    
	while (FLASH->SR & FLASH_SR_BSY);
#else
    // turn on HSI clock
    RCC->CR |= RCC_CR_HSION;
    while(!(RCC->CR & RCC_CR_HSIRDY));
#endif
    
    if (FLASH->CR & FLASH_CR_LOCK) { // unlock
        FLASH->KEYR = STM32_FLASH_KEY1;
        FLASH->KEYR = STM32_FLASH_KEY2;
    }
        
    // enable erasing
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
	UINT32 cr = (sectorNumber << 3) | FLASH_CR_SER;
	FLASH->CR = cr;

	cr |= FLASH_CR_STRT;
	FLASH->CR = cr;
#else
    FLASH->CR = FLASH_CR_PER;
    // set sector address
    FLASH->AR = Sector;
    // start erase
    FLASH->CR = FLASH_CR_PER | FLASH_CR_STRT;
    // asure busy flag is set up (see STM32 errata)
    FLASH->CR = FLASH_CR_PER | FLASH_CR_STRT;
#endif
    // wait for completion
    while (FLASH->SR & FLASH_SR_BSY);
    
    // reset & lock the controller
    FLASH->CR = FLASH_CR_LOCK;

#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
#else
    // stop HSI clock
    RCC->CR &= ~RCC_CR_HSION;
#endif

    return TRUE;
}



    /////////////////////////////////////////////////////////
    // Description:
    //   Changes the power state of the device
    // 
    // Input:
    //    State   - true= power on; false = power off
    //
    // Remarks:
    //   This function allows systems to conserve power by 
    //   shutting down the hardware when the system is 
    //   going into low power states.
    //
void  __section(SectionForFlashOperations)STM32_Flash_Driver::SetPowerState( void* context, UINT32 State )
{
}


//--// ---------------------------------------------------

#pragma arm section code = "SectionForFlashOperations"

UINT32 __section(SectionForFlashOperations)STM32_Flash_Driver::MaxSectorWrite_uSec( void* context )
{
    NATIVE_PROFILE_PAL_FLASH();

    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG*)context;
    
    return config->BlockConfig.BlockDeviceInformation->MaxSectorWrite_uSec;
}


UINT32 __section(SectionForFlashOperations)STM32_Flash_Driver::MaxBlockErase_uSec( void* context )
{
    NATIVE_PROFILE_PAL_FLASH();
    
    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG*)context;

    return config->BlockConfig.BlockDeviceInformation->MaxBlockErase_uSec;
    
}


#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section rodata = "g_STM32_Flash_DeviceTable"
#endif

struct IBlockStorageDevice g_STM32_Flash_DeviceTable = 
{                          
    &STM32_Flash_Driver::ChipInitialize,
    &STM32_Flash_Driver::ChipUnInitialize,
    &STM32_Flash_Driver::GetDeviceInfo,
    &STM32_Flash_Driver::Read,
    &STM32_Flash_Driver::Write,
    &STM32_Flash_Driver::Memset,    
    &STM32_Flash_Driver::GetSectorMetadata,
    &STM32_Flash_Driver::SetSectorMetadata,
    &STM32_Flash_Driver::IsBlockErased,
    &STM32_Flash_Driver::EraseBlock,
    &STM32_Flash_Driver::SetPowerState,
    &STM32_Flash_Driver::MaxSectorWrite_uSec,
    &STM32_Flash_Driver::MaxBlockErase_uSec,    
};

#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section rodata 
#endif 






