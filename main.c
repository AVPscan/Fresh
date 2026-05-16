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
  char *p = Cdbuf + 512, *b = p; uint8_t l, v, w, i = 8; ugoc r, c = TermCR(&r), s = (ugoc)((VRam.size + 1048575) / 1048576); *p++ = 'v';
  Print(Convas.Fone,Home);Print(Convas.Fone," "); while(i--) { BPrint(i + b6,"     "); } Print(Convas.Fone,"  \n"); i = 8;
  while (i--) { *p++ = (VP.Mode & (1 << i)) ? '1' : '0'; } snprintf(p, 91, " %dMb %d c%d r%d                    ", s, K_Mouse, c, r);
  if (StrLen(b) > 43) { *(b + 43) = Off; } if (StrLen(b) >= c) { *(b + c + On) = Off; } Print(oliveB,b); if (r < 3) return;
  snprintf(b, 100, "\nx%d y%d %d %d b%d x%d y%d                           ", VP.X, VP.Y, VP.Xs, VP.Ys, Buf.Mkey, Buf.MX, Buf.MY);
  if (StrLen(b) > 44) { *(b + 44) = Off; } if (StrLen(b) >= c) { *(b + c + On) = Off; } Print(marsala,b); if (r < 4) return;
  if (Buf.pop > Buf.push) { i = PopKey(); } else { i = ShowKey(); } w = Buf.Data; 
  if (i || Buf.Count) { l = On + (w & b10); v = ((w>>2) & b10); w = (w & b5) ? On : Off; p = b;
  snprintf(p, 100, "\nKeys %d {%d:%d} Repeat %d lvm %d%d%d ", Keys(), Buf.pop, Buf.push, Buf.Count, l, v, w); p += StrLen(p);
  if (!(w)) { while (l--) { *(p + l) = *(Buf.Key + l); } } else { w = *Buf.Key; snprintf(p, 10, "{%d}", w); } p += StrLen(p);
  snprintf(p, 30, "                   "); if (StrLen(b) > 44) { *(b + 44) = Off; } if (StrLen(b) >= c) { *(b + c + On) = Off; } if (r > 3) Print(navyCB,b); } }

//void body(void) { WinData(VP.Wec, "%1b%2d%3d", VP.Mode, VP.X, VP.Y); }
void Init(void) {
  ugoc control = Window(On,olive, -2, -2), W1 = Window(Off,oliveBI, Rand(10), Rand(10), Rand(40), Rand(10)), W2 = Window(Off,Rand(64) + On, Rand(10), Rand(10), 80, 24);
  VKeys(K_ALT_ESC,K_Ctrl_DOW,K_Ctrl_LEF,K_Ctrl_UP,K_Ctrl_RIG,K_UP,K_LEF,K_DOW,K_RIG); Events(K_ALT_TAB, K_ALT_ENT); Execs(WinDown, WinUp);
  WinData(control, " %+5dMb %+3 %06c:%06c ", ((VRam.size + 1048575)/1048576), cyanB, cyanB); WinEvent(control, K_Ctrl_K, WSwitch);
  WinEvent(control, K_NO, body); WinView(W2, 10, 3); WinSet(W1, On, On); WinSet(W2, Off, Off); }
Cell Help(Cell argc, char *argv[], Cell flag) {
  if (argc > On) {
    if (MemCmp(argv[On], "-?", 2) == Off || MemCmp(argv[On], "-h", 2) == Off || MemCmp(argv[On], "-help", 5) == Off) {
      if (flag) { Print(black,AltBufOff); Print(navyB,"Created by Alexey Pozdnyakov "); flag = Off;
        Print(navyC,"in 07.02.2026 version 8.94 email: avp70ru@mail.ru https://github.com/AVPscan\n"); } } } return flag; }
int main(int argc, char *argv[]) {
  Cell c_argc = (Cell)argc, flag = SystemSwitch(); flag = Help(c_argc, argv, flag);
  if (flag) { Init(); while (ViewPort()) Delay_ms(Fps); }
  return (int)SystemSwitch(); }

