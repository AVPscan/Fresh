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
  Cell c_argc = (Cell)argc; Cell flag = SystemSwitch(); flag = Help(c_argc, argv, flag);
  if (flag) {
    while (ViewPort()) {
      Show();
      Delay_ms(20); } }
   return (int)SystemSwitch(); }
