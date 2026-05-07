/* 
 * Fresh (C) 2026 A.Pozdnyakov GPLv3 - see LICENSE
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 */
 
#include <stdio.h> // временно snprintf
#include "sys.h"
void body(void) {  // пока не дописано WinData,Render
  char *p = Cdbuf + 512, *b = p; uint8_t l, v, q, w, i = 8; ugoc s, r, c = TermCR(&r); Cell m = VRam.size;
  s = (ugoc)((m + 1048575) / 1048576); *p++ = 'v'; while (i--) *p++ = (VP.Mode & (1 << i)) ? '1' : '0';
  snprintf(p, 91, " %dMb c%d r%d b%d x%d y%d %d          ", s, c, r, Buf.Mkey, Buf.MX, Buf.MY, Rand(100)); if (StrLen(b) >= c) *(b + c + On) = Off;
  Print(Cdefault,Home); Print(Corange,b); if (r < 2) return;
  snprintf(b, 100, "\nx%d y%d %d %d                      ", VP.X, VP.Y, VP.Xs, VP.Ys); if (StrLen(b) >= c) *(b + c + On) = Off;
  Print(CredB,b); if (r < 3) return;
  if (Buf.pop > Buf.push) { i = PopKey(&w,&q,Buf.key); if (i || q) { l = On + (w & b10); v = ((w>>2) & b10); w = (w & b5) ? On : Off; p = b;
    snprintf(p, 100, "\nKeys %d {%d:%d} Repeat %d lvm %d%d%d ", Keys(), Buf.pop, Buf.push, q, l, v, w); p += StrLen(p);
    if (v != 3) { i = l; while (i--) { *(p + i) = *(Buf.key + i); } p += l; *p = Off; }
    else { v = *Buf.key; snprintf(p, 10, "{%d}", v); }
    snprintf(p + StrLen(b), 10, "    "); if (StrLen(b) >= c) *(b + c + On) = Off;
    if (r > 2) Print(Cgreen,b); } }
  else { i = ShowKey(&w,&q,Buf.key); if (q) { l = On + (w & b10); v = ((w>>2) & b10); w = (w & b5) ? On : Off; p = b;
    snprintf(p, 100, "\nKeys %d {%d:%d} Repeat %d lvm %d%d%d ", Keys(), Buf.pop, Buf.push, q, l, v, w); p += StrLen(p);
    if (!(w)) { i = l; while (i--) { *(p + i) = *(Buf.key + i); } p += l; *p = Off; }
    else { w = *Buf.key; snprintf(p, 10, "{%d}", w); p += StrLen(p); }
    snprintf(p, 10, "         "); if (StrLen(b) >= c) *(b + c + On) = Off;
    if (r > 2) Print(Cgreen,b); } } } 
//void body(void) { WinData(VP.Win, "&2bdd", VP.Mode, VP.X, VP.Y); }
void show(void) { if ((Win(VP.Win)->WF ^= b7) & b7) WinView(VP.Win, -2, -2); }
void Init(void) {
  ugoc size = ((SizeVram + 1048575)/1048576), control = Window(-Cgold, -2, -2), menu = Window(-CgoldIB), c = Window(-CgreenI), test = Window(Cgreen, Off, Off, 120, 40);
  Exec(K_F11) = Off; Exec(K_F12) = Off; VP.Anchor = K_PUP; VP.Exit = K_PDN; Exec(VP.Anchor) = Anchor; Exec(VP.Exit) = Bye; Window(Cconvas);
  WinData(control, " %5dMb %3 %06c:%06c ", size, CblueB, CblueB); WinEvent(control, K_Ctrl_K, show); WinEvent(control, K_NO, body);
  WinData(menu, "%0mCreate\nLoad\nSave\nExit", 'c', 'l', 's', 'e'); WinEvent(menu, K_ESC, Adaptive, Nop, Nop, Nop, Bye);
  WinData(c, "%12mName:", 'n'); WinEvent(c, 'c', Adaptive, Nop); WinView(test, 10, 2); WinSet(test, Off, On); }

Cell Help(Cell argc, char *argv[], Cell flag) {
  if (argc > On) {
    if (MemCmp(argv[On], "-?", 2) == Off || MemCmp(argv[On], "-h", 2) == Off || MemCmp(argv[On], "-help", 5) == Off) {
      if (flag) { Print(Cconvas,AltBufOff); Print(CorangeB,"Created by Alexey Pozdnyakov "); flag = Off;
        Print(Corange,"in 07.02.2026 version 8.56 email: avp70ru@mail.ru https://github.com/AVPscan\n"); } } } return flag; }
int main(int argc, char *argv[]) {
  Cell c_argc = (Cell)argc, flag = SystemSwitch(); flag = Help(c_argc, argv, flag);
  if (flag) { Init(); while (ViewPort()) Delay_ms(Fps); }
  return (int)SystemSwitch(); }
