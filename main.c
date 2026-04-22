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
      if (flag) { Print(Cconvas,AltBufOff); Print(CorangeIB," Created by Alexey Pozdnyakov "); flag = Off;
                  Print(Corange," in 07.02.2026 version 7.99 email: avp70ru@mail.ru https://github.com/AVPscan\n"); } } }
  return flag; }

int main(int argc, char *argv[]) {
  Cell c_argc = (Cell)argc, flag = SystemSwitch(); flag = Help(c_argc, argv, flag);
  if (flag) { uint16_t size = (uint16_t)((SizeVram + 1048575)/1048576), control = Window(-Cgold), menu = Window(-CgoldIB), mc = Window(-CgoldIB);
              WinData(menu, "%0mCreate\nLoad\nSave\nExit\n", K_Ctrl_C, K_Ctrl_L, K_Ctrl_S, K_Ctrl_E); WinSet(menu, K_ESC);
              WinData(mc, "%mU colour\nB bold\nN inverse\n", K_Ctrl_U, K_Ctrl_B, K_Ctrl_N); WinSet(mc, K_Ctrl_C);
              WinData(control, " %-3Mb %3 %06c:%06c ", CblueB, CblueB); WinView(control, -2, -2);
              while (ViewPort()) { WinData(control, "dbdd", size, VP.Mode, VP.X, VP.Y); Delay_ms(Fps); } }
  return (int)SystemSwitch(); }
