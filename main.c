/* 
 * Fresh (C) 2026 A.Pozdnyakov GPLv3 - see LICENSE
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 */

#include <stdio.h> // временно snprintf -> WinData
#include "sys.h"

void body(void) {  // пока не дописано WinData,Render9
  char *b = var.dbuf + 512, *p = b; uint8_t l, v, w, j = 55, i = 8; Print(Base.Fone, Off, "\033[H"); while(i--) Print(Base.AF + Base.I[7-i],Off," ");
  Print(Base.Fone, Off, ""); Cell s = ((VRam.size + 1023) / 1024); i = 3; *p++ = '\n'; *p++ = 'v'; while(i--) { *p++ = (VP.Mode & (1 << i)) ? '1' : '0'; } 
  snprintf(p, 91, " %ld Kbyte %d %d %d %ld ", s, Base.CellP, Base.Win, Base.Colours, var.end - var.dbuf); i = (uint8_t)StrLen(b); Print((Base.Colours == 1) ? I0 : I2, aB, b);
  snprintf(b, 100, " Time %s             ", Base.T); if ((uint8_t)StrLen(b) > (j - i)) { *(b + j - i) = Off; }
  if ((uint8_t)StrLen(b) >= (TS.c - i)) { *(b + TS.c - On - i) = Off; } Print((Base.On) ? Base.Last : I5, Off, b); if (TS.r < 3) return;
  snprintf(b, 100, "\nH%d A%d D%d C%d x%d y%d %d %d b%d x%d y%d                         ", Base.Hz/10, Base.Apm, Base.Deep, Base.Colours, VP.X, VP.Y, VP.Xs, VP.Ys,
  Buf.Mkey, Buf.MX, Buf.MY); if (StrLen(b) > j) { *(b + j) = Off; } if (StrLen(b) >= TS.c) { *(b + TS.c) = Off; } Print((Base.Colours == 1) ? I0 : I7, aC | aD, b);
  if (TS.r < 4) { return; } if (Buf.pop > Buf.push) { i = PopKey(); } else { i = ShowKey(); } w = Buf.Dat; if (i || Buf.Count) { l = On + (w & b10); v = ((w>>2) & b10);
    w = (w & b5) ? On : Off; p = b; snprintf(p, 100, "\nKeys %d {%d:%d} Repeat %d lvm %d%d%d ", Key(), Buf.pop, Buf.push, Buf.Count, l, v, w); p += StrLen(p);
    if (!(w)) { *(p + l) = 0; while (l--) { *(p + l) = *(Buf.Key + l); } } else { w = *Buf.Key; snprintf(p, 10, "{%d}", w); } p += StrLen(p);
    snprintf(p, 100, "                                 "); if (StrLen(b) > j) { *(b + j) = Off; } if (StrLen(b) >= TS.c) *(b + TS.c) = Off;
    if (TS.r > 3) Print((Base.On) ? Base.Last : I5, aF | aI, b); } }
//void body(void) { WinData(VP.Wexe, "%1fb%d%d%s", VP.Mode, VP.X, VP.Y, Base.T); }
void sb(void) { if (!Base.On) { static uint8_t c = On; c = (c > 7) ? Off : c; SetBorder(Base.On, Base.AF + Base.I[c++]); } }
void tim(void) { static int16_t c = Off; c = (c > 511) ? Off : c; GenLast(++c); SetBorder(Base.On, Base.Border); }

void Init(void) { Keys(K_F1,K_Ctrl_DOW,K_Ctrl_LEF,K_Ctrl_UP,K_Ctrl_RIG,K_UP,K_LEF,K_DOW,K_RIG); Fresh(13, 2000, 10, 500, 250); Colour(7, 24);
  ugoc control = Window(On,I0, -2, -2), W1 = Window(Off,Rand(Base.Colours + On), Off, Off, 80, 24); WExec(control, K_NO, body);
  WData(control, " %+5dMb %+3 %06c:%06c %5c ", ((VRam.size + 1048575)/1048576), I0, I0, Base.Last); WExec(control, K_Ctrl_K, WSwitch);
  WSet(W1, On, On); WView(W1, On, On); Even(' ', Timer, K_ALT_TAB, K_ALT_ENT); Exec(sb, tim, WDown, WUp); VP.Wexe = W1; }

void Help(Cell argc, char *argv[], Cell *flag) {
  if (argc > On) { if (MemCmp(argv[On], "-?", 2) == Off || MemCmp(argv[On], "-h", 2) == Off || MemCmp(argv[On], "-help", 5) == Off) {
      if (*flag) { Print(Base.Border, Off, "\033[?1049l"); Print(Base.Fone, Off, ""); Print(I4, aB | aC, " Created by Alexey Pozdnyakov "); *flag = Off;
        Print(I7, aC, "in 05.02.2026 version 10.12 email: avp70ru@mail.ru https://github.com/AVPscan \n\033[?1049h"); } } } }
int main(int argc, char *argv[]) { Cell flag = SystemSwitch(); Help((Cell)argc, argv, &flag); if (flag) { Init(); while(VP.Loop) Free(); } return (int)SystemSwitch(); }
