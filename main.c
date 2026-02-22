/*
 * Copyright (C) 2026 Поздняков Алексей Васильевич
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 */

#include "sys.h"

int main(int argc, char *argv[]) {
  int flag = SystemSwitch(); flag = Help(argc, argv, flag);
  while (flag) {
    Delay_ms(20); const char* k = GetKey();
    if (k[0] == 27) {
      if (k[1] == K_NO) continue;
      if (k[1] == K_ESC) break; }
    ViewPort(k); Show(); } 
   return SystemSwitch(); }
