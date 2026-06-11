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
  char *p = var.dbuf + 512, *b = p; uint8_t l, v, j = 50, w = Base.Colours, i = On + w; Print(Sys.Inc, Off, "\033[H");
  while(i--) { Print((w - i + Fdark), var.A, " "); } Sys.Fone = Firis; Print(Sys.Fone, var.A, "\n"); Cell s = ((VRam.size + 1023) / 1024); *p++ = 'v'; i = 3;
  while(i--) { *p++ = (VP.Mode & (1 << i)) ? '1' : '0'; } snprintf(p, 91, " %ld Kbyte %d %d c%d r%d ", s, Base.CellP, Base.Win, TS.c, TS.r); i = (uint8_t)StrLen(b);
  Print(moss, aI, b); snprintf(b, 100, " Time %s             ", Base.T); if ((uint8_t)StrLen(b) > (j - 1 - i)) { *(b + j - 1 - i) = Off; }
  if ((uint8_t)StrLen(b) >= (TS.c - i)) { *(b + TS.c - On - i) = Off; } Print(127, Off, b); if (TS.r < 3) return;
  snprintf(b, 100, "\nH%d A%d D%d C%d x%d y%d %d %d b%d x%d y%d                         ", Base.Hz/10, Base.Apm, Base.Deep, Base.Colours, VP.X, VP.Y, VP.Xs, VP.Ys,
  Buf.Mkey, Buf.MX, Buf.MY); if (StrLen(b) > j) { *(b + j) = Off; } if (StrLen(b) >= TS.c) { *(b + TS.c) = Off; } Print(moss, aC | aB, b); if (TS.r < 4) return;
  if (Buf.pop > Buf.push) { i = PopKey(); } else { i = ShowKey(); } w = Buf.Data; if (i || Buf.Count) { l = On + (w & b10); v = ((w>>2) & b10);
    w = (w & b5) ? On : Off; p = b; snprintf(p, 100, "\nKeys %d {%d:%d} Repeat %d lvm %d%d%d ", Keys(), Buf.pop, Buf.push, Buf.Count, l, v, w); p += StrLen(p);
    if (!(w)) { *(p + l) = 0; while (l--) { *(p + l) = *(Buf.Key + l); } } else { w = *Buf.Key; snprintf(p, 10, "{%d}", w); } p += StrLen(p);
    snprintf(p, 100, "                  "); if (StrLen(b) > j) { *(b + j) = Off; } if (StrLen(b) >= TS.c) { *(b + TS.c) = Off; }
    if (TS.r > 3) Print(coral, aF , b); } }
//void body(void) { WinData(VP.Wec, "%1b%2d%3d%4s", VP.Mode, VP.X, VP.Y, Base.T); }
void sb(void) { if (!++Sys.Border || Sys.Border > (Base.Colours + Fdark)) { Sys.Border = Fdark; } SetBorder(); }
void tim(void) { static int16_t c = 511; if (--c < 0) { c = 511; } Grgb(Off, c, 511); GenFonCol(last, Base.Deep); }

void Init(void) { GlobalSet(11, Off, 2, 500, 200); SysSet(7, 24, 5); SKeys(K_F1,K_Ctrl_DOW,K_Ctrl_LEF,K_Ctrl_UP,K_Ctrl_RIG,K_UP,K_LEF,K_DOW,K_RIG);
  ugoc control = Window(On,coral, -2, -2), W1 = Window(Off,Rand(Base.Colours), Off, Off, 80, 24); WinExecs(control, K_NO, body);
  WinData(control, " %+5dMb %+3 %06c:%06c %5c ", ((VRam.size + 1048575)/1048576), moss, moss, last); WinExecs(control, K_Ctrl_K, WSwitch);
  WinSet(W1, On, On); WinView(W1, 10, 3); Events(' ', Timer, K_ALT_TAB, K_ALT_ENT); Execs(sb, tim, WinDown, WinUp); Sys.Win = W1; }

Cell Help(Cell argc, char *argv[], Cell flag) {
  if (argc > On) { if (MemCmp(argv[On], "-?", 2) == Off || MemCmp(argv[On], "-h", 2) == Off || MemCmp(argv[On], "-help", 5) == Off) {
      if (flag) { Print(Sys.Fone, Off, "\033[?1049l"); Print(coral, aB | aC, " Created by Alexey Pozdnyakov"); flag = Off;
        Print(berry, aC, " in 07.02.2026 version 9.93 email: avp70ru@mail.ru https://github.com/AVPscan \033[0m\n"); } } } return flag; }
int main(int argc, char *argv[]) { Cell flag = SystemSwitch(); flag = Help((Cell)argc, argv, flag);
  if (flag) { Init(); while(VP.Loop) Free(); } SystemSwitch(); return (int)var.off; }

