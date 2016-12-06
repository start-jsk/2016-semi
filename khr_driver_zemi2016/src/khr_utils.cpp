#include "khr_driver/khr_utils.h"
#include <cmath>

KondoInstance ki;

// Windowsで頭を動かしたときのコマンドを真似してみる。
// これで動く。
int windows_head_move() {
  ki.swap[0] = 0x0A;
  ki.swap[1] = 0x00;
  ki.swap[2] = 0x12;
  ki.swap[3] = 0x06;
  ki.swap[4] = 0x00;
  ki.swap[5] = 0x00;
  ki.swap[6] = 0xF5;
  ki.swap[7] = 0x20;
  ki.swap[8] = 0x00;
  ki.swap[9] = 0x37;
  return kondo_trx(&ki, 10, 4);
}

// 手を振るモーション
int shake_hand() {
  ki.swap[0] = 0x07;
  ki.swap[1] = 0x0C;
  ki.swap[2] = 0x80;
  ki.swap[3] = 0x1B;
  ki.swap[4] = 0x00;
  ki.swap[5] = 0x00;
  ki.swap[6] = 0xAE;
  return kondo_trx(&ki, 7, 4);
}

void error(KondoRef ki) {
  if(ki) {
    printf("%s", ki->error);
    kondo_close(ki);
  }
  exit(-1);
}

// RCB4のシステムレジスタ（RAMのアドレス$0000hから始まる16bit）を読み取る。
// 0-7bitはki.swap[3]に、8-15bitはki.swap[4]に格納される。
// return <  0 : エラー
//        == 5 : 成功
int read_system_register() {
  ki.swap[0] = 10;
  ki.swap[1] = RCB4_CMD_MOV;
  ki.swap[2] = RCB4_RAM_TO_COM;
  ki.swap[3] = 0x00;
  ki.swap[4] = 0x00;
  ki.swap[5] = 0x00;
  ki.swap[6] = 0x00;
  ki.swap[7] = 0x00;
  ki.swap[8] = 2;
  ki.swap[9] = kondo_checksum(&ki, 9);
  return kondo_trx(&ki, 10, 5);
}

// RCB4のRAMのシステムレジスタの指定したビットに値を書き込む。他のビットはそのまま。
// bit_num: 書き込みたいビット番号 (0~15)
// val:     書き込みたい値
// return <  0 : エラー
//        == 4 : 成功
//        other: バグ
int set_system_register(int bit_num, bool val) {
  if (bit_num < 0 || bit_num > 15) {
    //    ROS_WARN("[set_system_register] bit_num is out of range.");
    return -1;
  }
  char val_byte = val ?
      0x01 << (bit_num % 8) : // example: bit_num=2 -> val_byte = 0x00000100
    ~(0x01 << (bit_num % 8)); // example: bit_num=2 -> val_byte = 0x11111011
  read_system_register();
  char crt_register[2] = {ki.swap[2], ki.swap[3]};
  ki.swap[0] = 9;
  ki.swap[1] = RCB4_CMD_MOV;
  ki.swap[2] = RCB4_COM_TO_RAM;
  ki.swap[3] = 0x00;
  ki.swap[4] = 0x00;
  ki.swap[5] = 0x00;
  if (bit_num < 8) {
    ki.swap[6] = val ? (crt_register[0] | val_byte) : (crt_register[0] & val_byte);
    ki.swap[7] = crt_register[1];
  } else {
    ki.swap[6] = crt_register[0];
    ki.swap[7] = val ? (crt_register[1] | val_byte) : (crt_register[1] & val_byte);
  }
  ki.swap[8] = kondo_checksum(&ki, 8);
  return kondo_trx(&ki, 9, 4);
}

int set_ics_switch(bool val) {
  return set_system_register(RCB4_SYS_RGST_ICS, val);
}

int init_servo() {
  // set serial servo register
  for (int servo_num = 0; servo_num < 22; servo_num++ ) {
    if (servo_num == 1 || servo_num / 2 == 3 || servo_num / 2 == 5 || servo_num / 2 == 6)
      { continue; }
    unsigned short ram_addr = 0x0090 + (0x0014 * servo_num);
    copy_and_register_servo_register(ram_addr,  servo_num);
  }
  // set ics switch on
  return set_ics_switch(true);
}


// windowsから一度ROMに書き込んでおくと設定がコピーできる。
// idが同じでポートが違うサーボを一つだけ動かすこともこれで可能になる。
int copy_serial_servo_register_from_rom(unsigned short ram_addr, int servo_num) {
  unsigned long rom_addr = 0x00626 + (servo_num * 20);
  // ROS_INFO("rom_addr = %x", rom_addr);
  ki.swap[0]  = 11;                               // data size
  ki.swap[1]  = 0x00;                             // MOV
  ki.swap[2]  = 0x03;                             // ROM -> RAM
  ki.swap[3]  = (unsigned char)(ram_addr >> 0);   // RAM addr: lower byte
  ki.swap[4]  = (unsigned char)(ram_addr >> 8);   //           upper byte
  ki.swap[5]  = 0x00;                             // (fixed)
  ki.swap[6]  = (unsigned char)(rom_addr >>  0);  // ROM addr  0- 7 bit
  ki.swap[7]  = (unsigned char)(rom_addr >>  8);  //           8-15 bit
  ki.swap[8]  = (unsigned char)(rom_addr >> 16);  //          16-23 bit
  ki.swap[9]  = 20;                               // reply data size
  ki.swap[10] = kondo_checksum(&ki, 10);          // checksum
  return kondo_trx(&ki, 11, 4);
}

int register_servo_register_addr(unsigned short register_addr, int ics_num) {
  unsigned short ics_addr = 0x0044 + (2 * ics_num);
  ki.swap[0] = 9;
  ki.swap[1] = RCB4_CMD_MOV;
  ki.swap[2] = RCB4_COM_TO_RAM;
  ki.swap[3] = (unsigned char)(ics_addr >> 0);
  ki.swap[4] = (unsigned char)(ics_addr >> 8);
  ki.swap[5] = 0x00;
  ki.swap[6] = (unsigned char)(register_addr >> 0);
  ki.swap[7] = (unsigned char)(register_addr >> 8);
  ki.swap[8] = kondo_checksum(&ki, 8);
  return kondo_trx(&ki, 9, 4);
}

int copy_and_register_servo_register(unsigned short ram_addr, int servo_num) {
  copy_serial_servo_register_from_rom(ram_addr, servo_num);
  return register_servo_register_addr(ram_addr, servo_num);
}
