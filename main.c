/*
 * Copyright (C) 2026 Поздняков Алексей Васильевич
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 */

#include <stdio.h>
#include <string.h>
#include "sys.h"
  
void help(void) { Print(CorangeB,"Created by Alexey Pozdnyakov in 02.2026 version 2.21\n");
                  Print(Cgold,"email: avp70ru@mail.ru https://github.com/AVPscan/Fresh"); }

int main(int argc, char *argv[]) {
  int flag = SystemSwitch(); size_t size = 256; char *buf = TakeTb(&size);
  int16_t c,r,x,y,wx,wy,mk,mx,my;
  if (argc > 1) { if (strcmp(argv[1], "-?") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0) help();
                  flag = 0; }
  while (flag) {
    Delay_ms(20); const char* k = GetKey();
    if (k[0] == 27) {
      if (k[1] == K_NO) continue;
      if (k[1] == K_ESC) break; }
    ViewPort(k); Getlib(&c,&r,&x,&y,&wx,&wy,&mk,&mx,&my);
    Print(Ccurrent,Home); snprintf(buf, size, "%d %d %d %d %d %d         \n", c, r, x + wx, y + wy, mx, my);
    Print(Cblue,buf); snprintf(buf, size, "%d %d %d %d %d        ", x, wx, y, wy, mk); Print(Cgreen,buf); } 
   return SystemSwitch(); }
