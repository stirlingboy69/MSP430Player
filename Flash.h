#ifndef HAL_FLASH_H
#define HAL_FLASH_H

#include <stdint.h>

//******************************************************************************
// Defines
//******************************************************************************

#define FLASH_STATUS_OK     0
#define FLASH_STATUS_ERROR  1

/*******************************************************************************
 * \brief   Erase a single segment of the flash memory
 *
 * \param *Flash_ptr    Pointer into the flash segment to erase
 ******************************************************************************/
extern void Flash_SegmentErase(uint16_t *Flash_ptr);

/*******************************************************************************
 * \brief   Erase Check of the flash memory
 *
 * \param *Flash_ptr    Pointer into the flash segment to erase
 * \param len           give the len in word
 ******************************************************************************/
extern uint8_t Flash_EraseCheck(uint16_t *Flash_ptr, uint16_t len);

/*******************************************************************************
 * \brief   Write data into the flash memory (Byte format)
 *
 * \param *Data_ptr     Pointer to the Data to write
 * \param *Flash_ptr    Pointer into the flash to write data to
 * \param count         number of data to write
 ******************************************************************************/
extern void FlashWrite_8(uint8_t *Data_ptr, uint8_t *Flash_ptr, uint16_t count);

/*******************************************************************************
 * \brief   Write data into the flash memory (Word format)
 *
 * \param *Data_ptr     Pointer to the Data to write
 * \param *Flash_ptr    Pointer into the flash to write data to
 * \param count         number of data to write
 ******************************************************************************/
extern void FlashWrite_16(uint16_t *Data_ptr, uint16_t *Flash_ptr, uint16_t count);

/*******************************************************************************
 * \brief   Write data into the flash memory (Long format)
 *
 * \param *Data_ptr     Pointer to the Data to write
 * \param *Flash_ptr    Pointer into the flash to write data to
 * \param count         number of data to write
 ******************************************************************************/
extern void FlashWrite_32(uint32_t *Data_ptr, uint32_t *Flash_ptr, uint16_t count);

/*******************************************************************************
 * \brief   Fill data into the flash memory (Long format)
 *
 * \param value         Pointer to the Data to write
 * \param *Flash_ptr    pointer into the flash to write data to
 * \param count         number of data to write (= byte * 4)
 ******************************************************************************/
extern void FlashMemoryFill_32(uint32_t value, uint32_t *Flash_ptr, uint16_t count);

#define SegmentDPtr16 ((uint16_t*)(0x1000))
#define SegmentDPtr32 ((uint32_t*)(0x1000))
#define SegmentDPtr8 ((uint8_t*)(0x1000))

void write_Seg(uint8_t* Flash_ptr, uint8_t* pData, uint16_t count);

#endif /* HAL_FLASH_H */

