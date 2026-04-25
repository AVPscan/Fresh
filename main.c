/* 
 * Fresh (C) 2026 A.Pozdnyakov GPLv3 - see LICENSE
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 */

#include "sys.h"

Cell Help(Cell argc, char *argv[], Cell flag) {
  if (argc > 1) {
    if (MemCmp(argv[1], "-?", 2) == 0 || MemCmp(argv[1], "-h", 2) == 0 || MemCmp(argv[1], "-help", 5) == 0) {
      if (flag) { Print(Cconvas,AltBufOff); Print(CorangeIB," Created by Alexey Pozdnyakov "); flag = Off;
                  Print(Corange," in 07.02.2026 version 8.03 email: avp70ru@mail.ru https://github.com/AVPscan\n"); } } }
  return flag; }

void show(void) { static uint8_t flag = Off;
  if (flag) { WinView(Convas.WinCurrent, -2, -2); --flag; }
  else { WinView(Convas.WinCurrent, 0, 0); ++flag; } }
void body(void) { WinData(Convas.WinCurrent, "%2fbdd", VP.Mode, VP.X, VP.Y); }

int main(int argc, char *argv[]) {
  Cell c_argc = (Cell)argc, flag = SystemSwitch(); flag = Help(c_argc, argv, flag);
  if (flag) { uint16_t size = (uint16_t)((SizeVram + 1048575)/1048576), control = Window(-Cgold), menu = Window(-CgoldIB);
              WinData(control, " %-3dMb %3 %06c:%06c ", size, CblueB, CblueB); WinView(control, -2, -2); WinSet(control, K_Ctrl_K, show);
              WinData(menu, "%0mCreate\nLoad\nSave\nExit\n", K_Ctrl_C, K_Ctrl_L, K_Ctrl_S, K_Ctrl_E); WinSet(menu, K_ESC);
              WinSet(control, Off, body);
              while (ViewPort()) Delay_ms(Fps); }
  return (int)SystemSwitch(); }
