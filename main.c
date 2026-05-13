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
  char *p = Cdbuf + 512, *b = p; uint8_t l, v, w, i = 8; ugoc s, r, c = TermCR(&r); Cell m = VRam.size;
  s = (ugoc)((m + 1048575) / 1048576); *p++ = 'v'; while (i--) *p++ = (VP.Mode & (1 << i)) ? '1' : '0';
  snprintf(p, 91, " %dMb %d c%d r%d b%d x%d y%d        ", s, K_Mouse, c, r, Buf.Mkey, Buf.MX, Buf.MY); if (StrLen(b) > 43) *(b + 43) = Off;
  if (StrLen(b) >= c) *(b + c + On) = Off;
  Print(Cwhite,Home); Print(Cred,b); if (r < 2) return;
  snprintf(b, 100, "\n%d x%d y%d %d %d                                       ", Convas.Border - 0x21, VP.X, VP.Y, VP.Xs, VP.Ys); if (StrLen(b) > 44) *(b + 44) = Off;
  if (StrLen(b) >= c) *(b + c + On) = Off;
  Print(Corange,b); if (r < 3) return;
  if (Buf.pop > Buf.push) { i = PopKey(); w = Buf.Data; if (i || Buf.Count) { l = On + (w & b10); v = ((w>>2) & b10); w = (w & b5) ? On : Off; p = b;
    snprintf(p, 100, "\nKeys %d {%d:%d} Repeat %d lvm %d%d%d ", Keys(), Buf.pop, Buf.push, Buf.Count, l, v, w); p += StrLen(p);
    if (!(w)) { i = l; while (i--) { *(p + i) = *(Buf.Key + i); } p += l; }
    else { w = *Buf.Key; snprintf(p, 10, "{%d}", w); p += StrLen(p); }
    snprintf(p, 30, "              "); if (StrLen(b) > 44) *(b + 44) = Off;
    if (StrLen(b) >= c) *(b + c + On) = Off;
    if (r > 2) Print(Cgreen,b); } }
  else { i = ShowKey(); w = Buf.Data; if (Buf.Count) { l = On + (w & b10); v = ((w>>2) & b10); w = (w & b5) ? On : Off; p = b;
    snprintf(p, 100, "\nKeys %d {%d:%d} Repeat %d lvm %d%d%d ", Keys(), Buf.pop, Buf.push, Buf.Count, l, v, w); p += StrLen(p);
    if (!(w)) { i = l; while (i--) { *(p + i) = *(Buf.Key + i); } p += l; }
    else { w = *Buf.Key; snprintf(p, 10, "{%d}", w); p += StrLen(p); }
    snprintf(p, 30, "               "); if (StrLen(b) > 44) *(b + 44) = Off;
    if (StrLen(b) >= c) *(b + c + On) = Off;
    if (r > 2) Print(Cgreen,b); } } }
//void body(void) { WinData(VP.Wec, "%1b%2d%3d", VP.Mode, VP.X, VP.Y); }
void show(void) { if (Win(VP.Wec)->WF ^= b7) WinView(VP.Wec, -2, -2); }
void Init(void) {
  ugoc size = ((VRam.size + 1048575)/1048576), control = Window(-Cgold, -2, -2); IRnd();
  ugoc W1 = Window(CgoldIB, Rand(10), Rand(10), Rand(40), Rand(10)), W2 = Window(Rand(31) + On, Rand(10), Rand(10), 80, 24);
  Events(K_ALT_TAB, K_ALT_ENT); Execs(WinDown, WinUp);
  WinData(control, " %+5dMb %+3 %06c:%06c ", size, CblueB, CblueB); WinEvent(control, K_Ctrl_K, show); WinEvent(control, K_NO, body);
  WinSet(W1, On, On); WinView(W2, 10, 3); WinSet(W2, Off, Off); }
Cell Help(Cell argc, char *argv[], Cell flag) {
  if (argc > On) {
    if (MemCmp(argv[On], "-?", 2) == Off || MemCmp(argv[On], "-h", 2) == Off || MemCmp(argv[On], "-help", 5) == Off) {
      if (flag) { Print(Cblack,AltBufOff); Print(CorangeB,"Created by Alexey Pozdnyakov "); flag = Off;
        Print(Corange,"in 07.02.2026 version 8.81 email: avp70ru@mail.ru https://github.com/AVPscan\n"); } } } return flag; }
int main(int argc, char *argv[]) {
  Cell c_argc = (Cell)argc, flag = SystemSwitch(); flag = Help(c_argc, argv, flag);
  if (flag) { Init(); while (ViewPort()) Delay_ms(Fps); }
  return (int)SystemSwitch(); }

