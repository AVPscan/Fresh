/* 
 * Fresh (C) 2026 A.Pozdnyakov GPLv3 - see LICENSE
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 */

#include <stdio.h>
#include "sys.h"

Cell Help(Cell argc, char *argv[], Cell flag) {
  if (argc > 1) {
    if (MemCmp(argv[1], "-?", 2) == 0 || MemCmp(argv[1], "-h", 2) == 0 || MemCmp(argv[1], "-help", 5) == 0) {
      if (flag) { Print(Cconvas,AltBufOff); Print(CorangeIB," Created by Alexey Pozdnyakov "); flag = Off;
                  Print(Corange," in 07.02.2026 version 8.17 email: avp70ru@mail.ru https://github.com/AVPscan\n"); } } }
  return flag; }

void how(void) {  // костыль, пока не дописано WinData и Render!
  char *p = Cdbuf; uint8_t l, v, q = 0, w = 0, i = 8; ugoc s, r, c = TermCR(&r); Cell m = VRam.size;
  s = (ugoc)((m + 1048575) / 1048576); *p++ = 'v'; while (i--) *p++ = (VP.Mode & (1 << i)) ? '1' : '0';
  snprintf(p, 91, " %dMb c%d r%d b%d x%d y%d           ", s, c, r, Buf.Mkey, Buf.MX, Buf.MY); if (StrLen(p) >= c) *(p + c + 1) = 0;
  Print(Cdefault,Home); Print(Corange,Cdbuf); if (r < 2) return;
  snprintf(Cdbuf, 100, "\nx%d y%d wx%d wy%d                      ", VP.X, VP.Y, VP.X + VP.viewX, VP.Y + VP.viewY); if (StrLen(Cdbuf) >= c) *(Cdbuf + c + 1) = 0;
  Print(CredB,Cdbuf); if (r < 3) return;
  if (Buf.pop > Buf.push) { i = PopKey(&w,&q,Buf.key); if (i || q) { l = 1 + (w & b10); v = ((w>>2) & b10); w = (w & b5) ? 1 : 0;
    p = Cdbuf; snprintf(p, 100, "\nKeys %d {%d:%d} Repeat %d lvm %d%d%d ", Keys(), Buf.pop, Buf.push, q, l, v, w); p += StrLen(p);
    if (v != 3) { i = l; while (i--) { *(p + i) = *(Buf.key + i); } p += l; *p = 0; }
    else { v = *Buf.key; snprintf(p, 10, "{%d}", v); }
    p = Cdbuf + StrLen(Cdbuf); snprintf(p, 10, "    "); if (StrLen(Cdbuf) >= c) *(Cdbuf + c + 1) = 0;
    if (r > 2) Print(Cgreen,Cdbuf); } }
  else { i = ShowKey(&w,&q,Buf.key); if (q) { l = 1 + (w & b10); v = ((w>>2) & b10); w = (w & b5) ? 1 : 0;
    p = Cdbuf; snprintf(p, 100, "\nKeys %d {%d:%d} Repeat %d lvm %d%d%d ", Keys(), Buf.pop, Buf.push, q, l, v, w); p += StrLen(p);
    if (!(w)) { i = l; while (i--) { *(p + i) = *(Buf.key + i); } p += l; *p = 0; }
    else { w = *Buf.key; snprintf(p, 10, "{%d}", w); }
    p = Cdbuf + StrLen(Cdbuf); snprintf(p, 10,"    "); if (StrLen(Cdbuf) >= c) *(Cdbuf + c + 1) = 0;
    if (r > 2) Print(Cgreen,Cdbuf); } } }
    
void show(void) { static uint8_t flag = Off;
  if (flag) { WinView(Convas.W, -2, -2); --flag; }
  else { WinView(Convas.W, 0, 0); ++flag; } }
void body(void) { how(); } //WinData(Convas.W, "%2fbdd", VP.Mode, VP.X, VP.Y); }

int main(int argc, char *argv[]) {
  Cell c_argc = (Cell)argc, flag = SystemSwitch(); flag = Help(c_argc, argv, flag);
  if (flag) { ugoc size = (ugoc)((SizeVram + 1048575)/1048576), control = Window(-Cgold), menu = Window(-CgoldIB), test = Window(Cgreen, 40, 120);
              WinData(control, " %-5dMb %3 %06c:%06c ", size, CblueB, CblueB); WinView(control, -2, -2); WinSet(control, K_Ctrl_K, show);
              WinData(menu, "%0mCreate\nLoad\nSave\nExit", K_Ctrl_C, K_Ctrl_L, K_Ctrl_S, K_Ctrl_E); WinSet(menu, K_ESC);
              WinSet(control, Off, body); WinView(test, 10, 30);
              while (ViewPort()) Delay_ms(Fps); }
  return (int)SystemSwitch(); }
