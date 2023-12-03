/*
 * W25Q128JV.h (FLASH interface)
 *
 * - December 3, 2023
 *    Author: Anthony
 *
 * SUBJECT TO CHANGE; WIP
 */

#ifndef REALOP1_PERIPHERALS_FLASH_H_
#define REALOP1_PERIPHERALS_FLASH_H_

#include "../../system_config/QSPI/QSPI.h"


//Register and mask macros
#define QSPI_READ_REGISTER_TWO    0x35
#define QSPI_WRITE_REGISTER_TWO   0x31
#define QSPI_QUAD_REGISTER        0b00000010
#define QSPI_WRITE_ENABLE         0x06
#define QSPI_WRITE_REGISTER       0b00000010
#define QSPI_READ_DATA            0x03
#define QSPI_PAGE                 0x20
#define PAGE_MASK                 0x11111100
#define QSPI_READ_DATA            0x03

//Memory organization macros
#define BLOCK_SIZE                65536
#define SECTOR_SIZE               4096
#define PAGE_SIZE                 256

#define PAGES_PER_BLOCK           256
#define PAGES_PER_SECTOR          16

#define MIN_PAGES                 0
#define MAX_PAGES                 65535
#define MIN_SECTOR                0
#define MAX_SECTOR                4095


/**
 * Converts a page number (0 ~ 65535) to its memory address in the FLASH.
 * Specifying a page greater than 65535 will override page 65535.
 *
 * @returns Void pointer to the address, or null if unsuccessful
 */
void* find_page(uint32_t page);


/**
 * Writes one sector
 *
 * @param sector   # of sector (0 ~ 4095) to write to. Do not use address
 * @param buffer   Buffer to take data from. Ensure appropriate size (4096 bytes)
 *
 * @returns Whether write was successfully completed
 */
bool sector_write(uint16_t sector, uint8_t* buffer);


/**
 * Reads one sector
 *
 * @param sector   # of sector (0 ~ 4095) to read from. Do not use address
 * @param buffer   Buffer to put data into. Ensure appropriate size (4096 bytes)
 *
 * @returns Whether read was successfully completed
 */
bool sector_read(uint16_t sector, uint8_t* buffer);


/**
 * Erases one sector
 *
 * @param sector   # of sector (0 ~ 4095) to erase. Do not use address
 *
 * @returns Whether erase was successfully completed
 */
bool sector_erase(uint16_t sector);


/**
 * Writes one page
 *
 * @param page     # of page (0 ~ 65535) to write to. Do not use address
 * @param buffer   Buffer to take data from. Ensure appropriate size (256 bytes)
 *
 * @returns Whether write was successfully completed
 */
bool page_write(uint16_t page, uint8_t* buffer);


/**
 * Reads one page
 *
 * @param page     # of page (0 ~ 65536) to read from. Do not use address
 * @param buffer   Buffer to put data into. Ensure appropriate size (256 bytes)
 *
 * @returns Whether read was successfully completed
 */
bool page_read(uint16_t page, uint8_t* buffer);

/**
 * Reads a variable number of bytes
 *
 * @param size     # of bytes to read
 * @param page     Page to start reading from. Do not use address
 * @param buffer   Buffer to put data into. Ensure appropriate size (variable)
 *
 * @returns Whether read was successfully completed
 */
bool variable_read(uint16_t size, uint32_t page, uint8_t* buffer);

/**
 * Writes a variable number of bytes
 * WARNING - uses large amount of memory. >256 bytes!
 *
 * @param size     # of bytes to write
 * @param page     Page to start writing from. Do not use address
 * @param buffer   Buffer to take data from. Ensure appropriate size (variable)
 */
bool variable_write(uint16_t size, uint32_t page, uint8_t* buffer);

/**
 * Writes '1' to write_enable bit on register one. i.e. enables write functionality
 * Is already built into functions that need it
 *
 * @returns whether write was successfully enabled
 */
bool write_enable();


/**
 * Writes '1' to quad_enable bit on register two. i.e. enables QSPI functionality
 * Is mostly unnecessary for currently written functions (12-3-2023)
 *
 * @returns whether quad was successfully enabled
 */
bool quad_enable();


/**
 * Reads the contents of register two
 *
 * @param ptr_register_two   Reads the contents of register two into argued pointer
 */
void read_register_two(uint8_t* ptr_register_two);


/**
 * Waits for get_flash_status() to return 0. i.e. waits until flash is available
 *
 * @returns 0 when done
 */
bool wait_for_flash();


/**
 * Reads register one's busy flag. i.e. finds whether flash is currently busy
 *
 * @returns masked register one (00000010 if busy, 00000000 if not)
 */
uint8_t get_flash_status();


#endif
