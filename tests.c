/*
 * tests.c (FLASH testing)
 *
 * - Jan 26, 2024
 *   Author: Anthony Surkov
 *
 */

#include "W25Q128JV.h"

#define flash_JEDEC  0x9F


/*
 * 02-03-24
 * Read JEDEC ID - Verification of reading capability
 * Expected cout: EFh 9Fh ABh
 * Actual cout:   xxx xxx xxx (verify at home)
 *
 * Notes: Discovered and fixed error in qspi.c/qspi.h
 */
void read_jedecID() {
  uint8_t id_receiver[3] = { 0 };

  printMsg("init id_receiver var: \n\r");
  for (int i = 0; i < 3; i++) {
    printMsg("%x", id_receiver[i], " ");
  }
  printMsg("\n\r");

  qspi_setCommand(
      QSPI_FMODE_INDIRECT_READ,
      QSPI_1_WIRE,
      QSPI_UNUSED,
      QSPI_UNUSED,
      0,
      1,
      false
  );
  qspi_sendCommand(
      flash_JEDEC,
      QSPI_UNUSED,
      3,
      &id_receiver;
      QSPI_READ,
      QSPI_TIMEOUT_PERIOD
  );

  printMsg("final id_receiver var: \n\r");
  for (int i = 0; i < 3; i++) {
    printMsg("%x", id_receiver[i], " ");
  }
}

//WRITTEN FXNS, UNTESTED:

//READ_PAGE
void test_readPage() {
  uint8_t buffer[256] = { 2 };
  flash_writePage(0, buffer);
  uint8_t newBuffer[256] = { 0 };

  for (int i = 0; i < 256; i++) {
    printMsg("%u ", newbuffer[i], "\n\r");
  }
  printMsg("\n\r");

  flash_readPage(0, newBuffer);
  for (int i = 0; i < 256; i++) {
    printMsg(newbuffer[i], "%d\n\r");
  }
}

//QUAD_ENABLE
//READ REGISTER TWO
//(test separately)

void printBinary(uint8_t byte) {
  for (int i = 7; i >= 0; i--) {
    printMsg("%c", (byte & (1 << i)) ? '1' : '0');
  }
  printMsg("\n\r");
}
void test_quadEnable() {

  uint8_t register_two;
  flash_readRegisterTwo(&register_two);
  printBinary(register_two);

  flash_quadEnable();
  flash_readRegisterTwo(&register_two);
  printBinary(register_two);
}

//WRITE_ENABLE
//READ REGISTER ONE
//(test separately)

void qspi_readRegisterOne(uint8_t* register_one) {
  if (qspi_getStatus() == QSPI_BUSY) {
    printMsg("busy!\n\r");
  }
  qspi_setCommand(
      QSPI_FMODE_INDIRECT_READ,
      QSPI_1_WIRE,
      QSPI_UNUSED,
      QSPI_UNUSED,
      0,
      QSPI_UNUSED,
      false
  );
  qspi_sendCommand(
      0x05, //read register 1
      QSPI_UNUSED,
      1,
      register_one,
      1,
      QSPI_TIMEOUT_PERIOD
  );
}
void test_writeEnable() {
  uint8_t register_one = 0;
  qspi_readRegisterOne(&register_one);
  flash_writeEnable();
  printBinary(register_one);

  qspi_readRegisterOne(&register_one);
  printBinary(register_one);
}


