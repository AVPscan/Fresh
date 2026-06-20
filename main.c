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

void body(void) {  // пока не дописано WinData,Render
  char *p = var.dbuf + 512, *b = p; uint8_t l, v, w, j = 51, i = 3; Print(Base.Fone, var.A, "\033[H");
  Cell s = ((VRam.size + 1023) / 1024); *p++ = 'v'; while(i--) { *p++ = (VP.Mode & (1 << i)) ? '1' : '0'; } 
  snprintf(p, 91, " %ld Kbyte %d %d c%d r%d ", s, Base.CellP, Base.Win, TS.c, TS.r); i = (uint8_t)StrLen(b); Print(I4, aB, b);
  snprintf(b, 100, " Time %s             ", Base.T); if ((uint8_t)StrLen(b) > (j - 1 - i)) { *(b + j - 1 - i) = Off; }
  if ((uint8_t)StrLen(b) >= (TS.c - i)) { *(b + TS.c - On - i) = Off; } Print((Base.On) ? last : I2, Off, b); if (TS.r < 3) return;
  snprintf(b, 100, "\nH%d A%d D%d C%d x%d y%d %d %d b%d x%d y%d                         ", Base.Hz/10, Base.Apm, Base.Deep, Base.Colours, VP.X, VP.Y, VP.Xs, VP.Ys,
  Buf.Mkey, Buf.MX, Buf.MY); if (StrLen(b) > j) { *(b + j) = Off; } if (StrLen(b) >= TS.c) { *(b + TS.c) = Off; } Print(I3, aC | aD, b);
  if (TS.r < 4) { return; } if (Buf.pop > Buf.push) { i = PopKey(); } else { i = ShowKey(); } w = Buf.Dat; if (i || Buf.Count) { l = On + (w & b10); v = ((w>>2) & b10);
    w = (w & b5) ? On : Off; p = b; snprintf(p, 100, "\nKeys %d {%d:%d} Repeat %d lvm %d%d%d ", Key(), Buf.pop, Buf.push, Buf.Count, l, v, w); p += StrLen(p);
    if (!(w)) { *(p + l) = 0; while (l--) { *(p + l) = *(Buf.Key + l); } } else { w = *Buf.Key; snprintf(p, 10, "{%d}", w); } p += StrLen(p);
    snprintf(p, 100, "                                 "); if (StrLen(b) > j) { *(b + j) = Off; } if (StrLen(b) >= TS.c) *(b + TS.c) = Off;
    if (TS.r > 3) Print((Base.On) ? last : I2, aF | aI, b); } }
//void body(void) { WinData(VP.Wec, "%1fb%d%d%s", VP.Mode, VP.X, VP.Y, Base.T); }
void sb(void) { if (!Base.On) SetBorder(Base.On, ++Base.Border); }
void tim(void) { static int16_t c = Off; GenLast(++c); SetBorder(Base.On, Base.Border); }

void Init(void) { Keys(K_F1,K_Ctrl_DOW,K_Ctrl_LEF,K_Ctrl_UP,K_Ctrl_RIG,K_UP,K_LEF,K_DOW,K_RIG); Fresh(10, 500, 10, 500, 250); Colour(7, 24);
  ugoc control = Window(On,dark, -2, -2), W1 = Window(Off,Rand(Base.Colours + On), Off, Off, 80, 24); WExec(control, K_NO, body);
  WData(control, " %+5dMb %+3 %06c:%06c %5c ", ((VRam.size + 1048575)/1048576), dark, dark, last); WExec(control, K_Ctrl_K, WSwitch);
  WSet(W1, On, On); WView(W1, On, On); Even(' ', Timer, K_ALT_TAB, K_ALT_ENT); Exec(sb, tim, WDown, WUp); Convas.Win = W1; }

void Help(Cell argc, char *argv[], Cell *flag) {
  if (argc > On) { if (MemCmp(argv[On], "-?", 2) == Off || MemCmp(argv[On], "-h", 2) == Off || MemCmp(argv[On], "-help", 5) == Off) {
      if (*flag) { Print(Dark, Off, "\033[?1049l"); Print(Snow, var.A, ""); Print(I4, aB | aC, "Created by Alexey Pozdnyakov "); *flag = Off;
        Print(I3, aC, "in 05.02.2026 version 10.10 email: avp70ru@mail.ru https://github.com/AVPscan \n\033[?1049h"); } } } }
int main(int argc, char *argv[]) { Cell flag = SystemSwitch(); Help((Cell)argc, argv, &flag);
  if (flag) { Init(); while(VP.Loop) Free(); } return (int)SystemSwitch(); }
