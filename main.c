/*
 * Copyright (C) 2026 Поздняков Алексей Васильевич
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 */

#include "sys.h"

Cell Help(Cell argc, char *argv[], Cell flag) {
  if (argc > 1) {
    if (MemCmp(argv[1], "-?",2) == 0 || MemCmp(argv[1], "-h",2) == 0 || MemCmp(argv[1], "-help",5) == 0) {
      if (flag) { Print(Cdefault,AltBufOff); Print(CorangeBI," Created by Alexey Pozdnyakov "); flag = 0;
                  Print(Corange," in 07.02.2026 version 7.04 email: avp70ru@mail.ru https://github.com/AVPscan\n"); } } }
  return flag; }

int main(int argc, char *argv[]) {
  Cell c_argc = (Cell)argc, flag = SystemSwitch(); flag = Help(c_argc, argv, flag);
  if (flag) { Window(0, Cgreen, 0, 0); WData(0, "Память под Vram %03cdMbytes\n", Corange, (uint16_t)((SizeVram + 1048575)/1048576));
              Window(1, Cgold, 0, -1); WConst(1, " %3 %06c:%06c \n", Cblue, Cblue); WSet(1, -2, -2);
              while (ViewPort()) { Show(); Delay_ms(Free); } }
  return (int)SystemSwitch(); }
