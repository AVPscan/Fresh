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
  char *p = Cdbuf + 512, *b = p; uint8_t l, v, j = 48, w = Sys.Colours, i = On + w; Print(Sys.Inc, Off, "\033[H"); while(i--) Print((w - i + Fdark), cA, " ");
  Sys.Fone = Firis; Print(Sys.Fone, cA, "\n"); ugoc r, c = TermCR(&r), s = (ugoc)((VRam.size + 1048575) / 1048576); *p++ = 'v'; i = 3;
  while(i--) { *p++ = (VP.Mode & (1 << i)) ? '1' : '0'; } snprintf(p, 91, " %dMb %d c%d r%d ", s, K_Mouse, c, r); i = (uint8_t)StrLen(b); Print(moss, aI, b);
  snprintf(b, 100, " Time %s             ", Sys.T);
  if ((uint8_t)StrLen(b) > (j - 1 - i)) { *(b + j - 1 - i) = Off; } if ((uint8_t)StrLen(b) >= (c - i)) { *(b + c - On - i) = Off; } Print(aColours, Off, b);
  if (r < 3) { return; } snprintf(b, 100, "\nH%d F%d D%d C%d x%d y%d %d %d b%d x%d y%d                         ", Sys.Hz/10, Sys.Fps, Sys.Deep, Sys.Colours,
  VP.X, VP.Y, VP.Xs, VP.Ys, Buf.Mkey, Buf.MX, Buf.MY); if (StrLen(b) > j) { *(b + j) = Off; } if (StrLen(b) >= c) { *(b + c) = Off; } Print(moss, aC | aB, b);
  if (r < 4) { return; } if (Buf.pop > Buf.push) { i = PopKey(); } else { i = ShowKey(); } w = Buf.Data; if (i || Buf.Count) { l = On + (w & b10);
    v = ((w>>2) & b10); w = (w & b5) ? On : Off; p = b; snprintf(p, 100, "\nKeys %d {%d:%d} Repeat %d lvm %d%d%d ", Keys(), Buf.pop, Buf.push, Buf.Count, l, v, w);
    p += StrLen(p); if (!(w)) { *(p + l) = 0; while (l--) { *(p + l) = *(Buf.Key + l); } } else { w = *Buf.Key; snprintf(p, 10, "{%d}", w); } p += StrLen(p);
    snprintf(p, 100, "                  "); if (StrLen(b) > j) { *(b + j) = Off; } if (StrLen(b) >= c) { *(b + c) = Off; } if (r > 3) { Print(coral, aF , b); } } }
//void body(void) { WinData(VP.Wec, "%1b%2d%3d", VP.Mode, VP.X, VP.Y); }
void sb(void) { if (!++Sys.Border || Sys.Border > (Sys.Colours + Fdark)) { Sys.Border = Fdark; } SetBorder(); }
void tim(void) { static int16_t c = 511; if (--c < 0) { c = 511; } Grgb(Off, c, 511); GenFonCol(aColours, Sys.Deep); } 

void Init(void) {
  ugoc control = Window(On,coral, -2, -2), W1 = Window(Off,berry, Rand(10), Rand(10), Rand(40), Rand(10)), W2 = Window(Off,Rand(Sys.Colours), Off, Off, 80, 24);
  WinData(control, " %+5dMb %+3 %06c:%06c ", ((VRam.size + 1048575)/1048576), moss, moss); WinExecs(control, K_Ctrl_K, WSwitch); WinExecs(control, K_NO, body); 
  WinSet(W1, On, On); WinSet(W2, Off, Off); WinView(W2, 10, 3); Events(' ', Timer, K_ALT_TAB, K_ALT_ENT); Execs(sb, tim, WinDown, WinUp); SysSet(500, 1000, 24, 15, 10);
  SKeys(K_F1,K_Ctrl_DOW,K_Ctrl_LEF,K_Ctrl_UP,K_Ctrl_RIG,K_UP,K_LEF,K_DOW,K_RIG); }

Cell Help(Cell argc, char *argv[], Cell flag) {
  if (argc > On) { if (MemCmp(argv[On], "-?", 2) == Off || MemCmp(argv[On], "-h", 2) == Off || MemCmp(argv[On], "-help", 5) == Off) {
      if (flag) { Print(Sys.Fone, cA, "\033[?1049l"); Print(berry, cA, "Created by Alexey Pozdnyakov"); flag = Off;
        Print(coral, cA, " in 07.02.2026 version 9.72 email: avp70ru@mail.ru https://github.com/AVPscan"); } } } return flag; }
int main(int argc, char *argv[]) { Cell flag = SystemSwitch(); flag = Help((Cell)argc, argv, flag); if (flag) { Init(); while(VP.Loop) Free(); } return (int)SystemSwitch(); }
