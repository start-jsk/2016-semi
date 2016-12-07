#ifndef __KHR_UTILS_H__
#define __KHR_UTILS_H__

#include <string>
extern "C" {
#include "khr_driver/rcb4.h"
}
#define RCB4_SYS_RGST_ICS 0
#define RCB4_SYS_RGST_LED 15
#define SERVO_FREE_POSITION 0x8000
#define SERVO_HOLD_POSITION 0x7FFF
#define SERVO_NEWTRAL_POSITION 7500
#define KHR_DOF 22

int windows_head_move();
int shake_hand();
void error(KondoRef ki);
int read_system_register();
int set_system_register(int bit_num, bool val);
int set_ics_switch(bool val);
int init_servo();
int register_servo_register_addr(unsigned short register_addr, int ics_num);
int copy_and_register_servo_register(unsigned short ram_addr, int servo_num);

#endif  // __KHR_UTILS_H__
