#include <sstream>
extern "C" {
#include "khr_driver/rcb4.h"
}
#include <unistd.h>
#include <khr_driver/khr_utils.h>
#include <iostream>
#include <math.h>

extern KondoInstance ki;
unsigned char servo_gain[] = {0x01, 0x01, 0x01, 0x01, 0x01,
                              0x01, 0x01, 0x01, 0x01, 0x01,
                              0x01, 0x01, 0x01, 0x01, 0x01,
                              0x01, 0x01, 0x01, 0x01, 0x01,
                              0x01, 0x01};
unsigned char servo_gain_hard[] = {0x01, 0x04, 0x07, 0x07, 0x07,
                                   0x07, 0x02, 0x02, 0x01, 0x01,
                                   0x03, 0x03, 0x05, 0x05, 0x06,
                                   0x06, 0x03, 0x03, 0x03, 0x03,
                                   0x03, 0x03};

// argv[1]: 使用するKHRのindex（省略した場合は0）
int main(int argc, char **argv)
{
  char str[100];
  
  // open -------------------------------------------------------------------
  int ret;
  if (argc <= 1) {
    ret = kondo_init(&ki);
  } else {
    ret = kondo_init_index(&ki, atoi(argv[1]));
  }

  if (ret < 0) {
    printf("%s", ki.error);
    exit(-1);
  }
  ki.debug = false;

  // servo on
  init_servo();
  sleep(1);

  // move servo
  for (UINT sensor_idx = 0; sensor_idx < 4; sensor_idx++) {
    int sensor_val;
    kondo_read_analog((KondoRef)&ki, &sensor_val, sensor_idx + 1);
  }

  std::cout << "start" << std::endl;
  // khrに保存されているfunctionを呼び出す.

  while(1){
    fscanf(stdin, "%s", str);
    fprintf(stdout, "%s\n", str);
    if(!strcmp(str, "hogescrewdriver")){
      printf("OK\n");
      shake_hand();
      exit(0);
    }
    else
      printf("NG\n");
    //
  }
  // close ------------------------------------------------------------------
  ret = kondo_close(&ki);
  if (ret < 0)
    error(&ki);

  return 0;
}
