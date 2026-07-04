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

void Body(void) {  // пока не дописано WinData,Render9
  char *b = var.dbuf + 512, *p = b; anu l, v, w, j = 60, i = 8; Print(Base.Fone, Off, "\033[H"); while(i--) Print(Fon(7-i),Off," ");
  Print(Base.Fone, Off, ""); As s = ((VRam.size + 1023) / 1024); i = 3; *p++ = '\n'; *p++ = 'v'; while(i--) { *p++ = (VP.Mode & (1 << i)) ? '1' : '0'; } 
  snprintf(p, 91, " %llu Kbyte %d %d %d %d %lld ", s, Base.CellP, Base.Dynamic, Base.Win, Base.Colours, var.end - var.dbuf); i = (anu)StrLen(b);
  Print((Base.Colours == 1) ? Ink(0) : Ink(3), aB, b); snprintf(b, 100, "%llu %llu %llu %llu:%llu:%llu                         ", Base.Din, Base.Sap, Base.Sam, Base.Hor, Base.Kal, Base.Vik);
  if ((anu)StrLen(b) > (j - i)) { *(b + j - i) = Off; } if ((anu)StrLen(b) >= (TS.c - i)) { *(b + TS.c - On - i) = Off; } Print(Ink(8), Off, b); if (TS.r < 3) return;
  snprintf(b, 100, "\nH%d A%d D%d C%d x%d y%d %d %d b%d x%d y%d                         ", Base.Hz/10, Base.Apm, Base.Deep, Base.Colours, VP.X, VP.Y, VP.Xs, VP.Ys,
  Buf.Mkey, Buf.MX, Buf.MY); if (StrLen(b) > j) { *(b + j) = Off; } if (StrLen(b) >= TS.c) { *(b + TS.c) = Off; } Print((Base.Colours == 1) ? Ink(0) : Ink(6), aC | aD, b);
  if (TS.r < 4) { return; } if (Buf.pop > Buf.push) { i = PopKey(); } else { i = ShowKey(); } w = Buf.Dat; if (i || Buf.Count) { l = On + (w & b10); v = ((w>>2) & b10);
    w = (w & b5) ? On : Off; p = b; snprintf(p, 100, "\nKeys %d {%d:%d} Repeat %d lvm %d%d%d ", Key(), Buf.pop, Buf.push, Buf.Count, l, v, w); p += StrLen(p);
    if (!(w)) { *(p + l) = 0; while (l--) { *(p + l) = *(Buf.Key + l); } } else { w = *Buf.Key; snprintf(p, 10, "{%d}", w); } p += StrLen(p);
    snprintf(p, 100, "                                 "); if (StrLen(b) > j) { *(b + j) = Off; } if (StrLen(b) >= TS.c) *(b + TS.c) = Off;
    if (TS.r > 3) Print(Ink(8), aF | aI, b); } }

//void Body(void) { WinData(VP.Wexe, "%1fb%d%d%s", VP.Mode, VP.X, VP.Y, Base.T); }
void Tim(void) { static anu c = Off; c = ((c + 1) & 7); SetBorder(Base.On, Fon(c)); }
void Space(void) { if (!Base.On) Tim(); }

void Init(void) { Fresh(11, 1, 999, 25, 500, 250); Colour(7, 24); Keys(K_ESC); Date(2026, 7, 4); Time(23, 50, 0);
  Even(' ', Timer, K_ALT_TAB, K_ALT_ENT); Exec(Space, Tim, WDown, WUp); vanu control = WStatic(Ink(7), -2, -2), W1 = WDynamic(Rand(Base.Colours + On) + On, Off, Off, 80, 24);
  WSet(W1, On, On); WView(W1, On, On); VP.Wexe = W1; WData(control, " %+5dMb %+3 %06:%06 %5c ", ((VRam.size + 1048575)/1048576), Ink(8)); WExec(control, K_NO, Body);
  WExec(control, K_Ctrl_K, WSwitch); }

int main() { if (SystemSwitch()) { Init(); while(Base.Loop) Free(); } return SystemSwitch(); }
