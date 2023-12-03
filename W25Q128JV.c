/*
 * W25Q128JV.c (FLASH interface)
 *
 * - December 3, 2023
 * - Authors: Anthony, Haidang
 *
 * SUBJECT TO CHANGE; WIP
 */

#include "W25Q128JV.h"

void* find_page(uint32_t page) {
  if (page < MIN_PAGES) { //MAX_PAGES comparison not needed b/c uint16_t cannot hold more
    return NULL;
  }
  uint8_t block = page / PAGES_PER_BLOCK;
  page = page % PAGES_PER_BLOCK;
  uint8_t sector = page / PAGES_PER_SECTOR;
  page = page % PAGES_PER_SECTOR;

  void* memPtr = (uint32_t*) (
    (block * BLOCK_SIZE) +
    (sector * SECTOR_SIZE) +
    (page * PAGE_SIZE)
  );

  return memPtr;
}

bool sector_write(uint16_t sector, uint8_t* buffer) {
  if (qspi_get_status() == QSPI_BUSY) {
    return false;
  }
  if (sector > MAX_SECTOR) {
    return false;
  }
  sector *= SECTOR_SIZE; //convert to pages

  for (int i = 0; i < 16; i++, sector++) {
    write_enable();
    page_write(sector, &buffer);
    buffer += 256;
  }
}

bool sector_read(uint16_t sector, uint8_t* buffer) {
  if (qspi_get_status() == QSPI_BUSY) {
    return false;
  }
  if (sector > MAX_SECTOR) {
    return false;
  }
  sector *= SECTOR_SIZE;

  for (int i = 0; i < 16; i++, sector++) {
    write_enable();
    page_write(sector, &buffer);
    buffer += 256;
  }
}

bool sector_erase(uint16_t sector) {
  if (qspi_get_status() == QSPI_BUSY) {
    return false;
  }

  if (sector > MAX_SECTOR) {
    return false;
  }
  sector *= PAGES_PER_SECTOR; //convert to page
  uint32_t address = (uint32_t)find_page(page);
  if (address == NULL) {
    return false;
  }

  write_enable();

  qspi_set_command(
      QSPI_INDIRECT_WRITE,
      QSPI_1_WIRE,
      QSPI_1_WIRE,
      QSPI_UNUSED,
      0,
      QSPI_UNUSED,
      false
  );
  qspi_send_command(
      QSPI_SECTOR_ERASE,
      address,
      0,
      0,
      QSPI_1_WIRE,
      QSPI_TIMEOUT_PERIOD
  );

  while (qspi_get_status() == QSPI_BUSY);

  return true;
}
bool page_write(uint16_t page, uint8_t* buffer) {
  if (qspi_get_status() == QSPI_BUSY) {
    return false;
  }

  uint32_t address = (uint32_t)find_page(page);
  if (address == NULL) {
    return false;
  }
  address = address & PAGE_MASK;

  write_enable();

  qspi_set_command(
      QSPI_INDIRECT_WRITE,
      QSPI_1_WIRE,
      QSPI_1_WIRE,
      QSPI_UNUSED,
      0,
      QSPI_1_WIRE,
      false
  );
  qspi_send_command(
      QSPI_PAGE,
      address,
      256,
      &buffer,
      1,
      QSPI_TIMEOUT_PERIOD
  );

  while (qspi_get_status() == QSPI_BUSY);

  return true;
}

bool page_read(uint16_t page, uint8_t* buffer) {
  if (get_qspi_status() == QSPI_BUSY) {
    return 1;
  }

  uint32_t address = (uint32_t)find_page(page);
  if (address == NULL) {
    return false;
  }
  address = address & PAGE_MASK;

  qspi_set_command(
    QSPI_INDIRECT_READ,
    QSPI_1_WIRE,
    QSPI_1_WIRE,
    QSPI_UNUSED,
    0,
    QSPI_1_WIRE,
    false
  );
  qspi_send_command(
    0x03,
    page_find(page_num),
    8,
    &buffer,
    0,
    QSPI_TIMEOUT_PERIOD
  );

  return 1;
}


bool write_enable() {
  if (qspi_get_status() == QSPI_BUSY) {
        return 1;
  }
  qspi_set_commmand(
      QSPI_INDIRECT_WRITE,
      QSPI_1_WIRE,
      QSPI_UNUSED,
      QSPI_UNUSED,
      0,
      QSPI_UNUSED,
      false
  );
  qspi_send_command(
      QSPI_WRITE_ENABLE,
      QSPI_UNUSED,
      0,
      0,
      0,
      QSPI_TIMEOUT_PERIOD
  );

  qspi_set_commmand(
      QSPI_AUTOMATIC_POLLING,
      QSPI_1_WIRE,
      QSPI_UNUSED,
      QSPI_UNUSED,
      0,
      QSPI_UNUSED,
      false
  );
  qspi_status_polling(
      0,
      0x05,
      QSPI_WRITE_REGISTER,
      QSPI_WRITE_REGISTER,
      QSPI_TIMEOUT_PERIOD
  );
  while (qspi_get_status() == QSPI_BUSY);
  return 0;
}

bool quad_enable() {
  if (qspi_get_status() == QSPI_BUSY) {
    return false;
  }

  uint8_t register_two;
  read_register_two(&register_two);

  if ( (register_two & QSPI_QUAD_REGISTER) == QSPI_QUAD_REGISTER) {
    return 0;
  }

  register_two |= (1 << 1);

  qspi_set_command(
      QSPI_INDIRECT_WRITE,
      QSPI_1_WIRE,
      QSPI_UNUSED,
      QSPI_UNUSED,
      0,
      QSPI_1_WIRE,
      false
  );
  qspi_send_command(
      QSPI_WRITE_REGISTER_TWO,
      0,
      1,
      &register_two,
      1,
      QSPI_TIMEOUT_PERIOD
  );

  qspi_set_command(
      QSPI_INDIRECT_READ,
      QSPI_1_WIRE,
      QSPI_UNUSED,
      QSPI_UNUSED,
      0,
      QSPI_UNUSED,
      QSPI_TIMEOUT_PERIOD
  );
  qspi_status_poll(
      0,
      QSPI_READ_REGISTER_TWO,
      QSPI_QUAD_REGISTER,
      QSPI_QUAD_REGISTER,
      QSPI_TIMEOUT_PERIOD
  );

  while (qspi_get_status() == QSPI_BUSY);

  return 0;
}

void read_register_two(uint8_t* ptr_register_two) {
  if (qspi_get_status() == QSPI_BUSY) {
    return 1;
  }

  qspi_set_command(
      QSPI_INDIRECT_READ,
      QSPI_1_WIRE,
      QSPI_UNUSED,
      QSPI_UNUSED,
      0,
      QSPI_1_WIRE,
      false
  );
  qspi_send_command(
      QSPI_READ_REGISTER_TWO,
      0,
      1,
      ptr_register_two,
      0,
      QSPI_TIMEOUT_PERIOD
  );
}

bool wait_for_flash() {
  while (get_flash_status() != 0) {}
  return 0;
};

uint8_t get_flash_status() {
  if (get_qspi_status() == QSPI_BUSY) {
    return 1;
  }

  qspi_set_command(
    QSPI_INDIRECT_READ,
    QSPI_1_WIRE,
    QSPI_UNUSED,
    QSPI_UNUSED,
    0,
    QSPI_UNUSED,
    false
  );

  uint8_t register_one;
  qspi_send_command(
    0x05,
    0x00,
    8,
    &register_one,
    0,
    QSPI_TIMEOUT_PERIOD
  );
  return register_one & (1 << 0);
};

