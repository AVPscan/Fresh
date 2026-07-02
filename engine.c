/*
 * Fresh (C) 2026 A.Pozdnyakov GPLv3 - see LICENSE
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 */

#include "sys.h"

ENGINE_VARS_INIT;

As StrLen(char *s) { if (!s) return Off;
  anu *f = (anu*)s; while(*f++);
  return (--f - (anu*)s); }
void MemSet(void* buf, anu val, As len) { anu *p = (anu*)buf;
  while(len && ((As)p & (SCell - 1))) { len--; *p++ = val; }
  if (len >= SCell) {
    As vW = val * ((SCell - 1) / 255); As *pW = (As*)p;
    As i = len / SCell; len &= (SCell - 1); while(i--) *pW++ = vW;
    p = (anu*)pW; }
  while(len--) *p++ = val; }
void MemMove(void* dst, void* src, As len) {
  if (dst > src) { anu *d = (anu*)dst + len, *s = (anu*)src + len;
    while(len--) { *--d = *--s ; } }
  else if (dst != src ) { MemCpy(dst, src, len); } }
void MemCpy(void* dst, void* src, As len) { anu *d = (anu*)dst, *s = (anu*)src;
  while(len && ((As)d & (SCell - 1))) { *d++ = *s++; len--; }
  if (len >= SCell && ((As)s & (SCell - 1)) == 0) {
    As *dW = (As*)d; As *sW = (As*)s;
    As i = len / SCell; len &= (SCell - 1); while(i--) *dW++ = *sW++;
    d = (anu*)dW; s = (anu*)sW; }
  while(len--) *d++ = *s++ ; }
nanu MemCmp(void* dst, void* src, As len) { anu *d = (anu*)dst, *s = (anu*)src;
  while(len && ((As)d & (SCell- 1))) { len--; if (*d++ != *s++) return (nanu)(*--d - *--s); }
  if (len >= SCell && ((As)s & (SCell - 1)) == 0) {
    As *dW = (As*)d; As *sW = (As*)s;
    As i = len / SCell; len &= (SCell - 1); while(i-- && (*dW++ == *sW++));
    if (i + 1) { --dW; --sW; len += SCell; }
    d = (anu*)dW; s = (anu*)sW; }
  while(len--) { if (*d++ != *s++) return (nanu)(*--d - *--s); }
  return Off; }

void UTFinfoTile(anu *s, As len) { Buf.Dat = 0xC0; if (!len) return;
  if ((*s & 0xE0) == 0xC0 && len < 0x02) return;
  else if ((*s & 0xF0) == 0xE0 && len < 0x03) return;
  else if ((*s & 0xF8) == 0xF0 && len < 0x04) return;
  UTFinfo(s); }
void UTFinfo(anu *s) { var.Y = *s++; Buf.Dat = Off; if (var.Y < 0x80) var.RGB = (an)var.Y;
  else if ((var.Y & 0xE0) == 0xC0 && (*s & 0xC0) == 0x80) { Buf.Dat++; var.RGB = ((var.Y & 0x1F) << 0x06) | (*s & 0x3F); }
  else if ((var.Y & 0xF0) == 0xE0 && (*s & 0xC0) == 0x80 && (*(s + 0x01) & 0xC0) == 0x80)
    { Buf.Dat = 0x02; var.RGB = ((var.Y & 0x0F) << 0x0C) | ((*s & 0x3F) << 0x06) | (*(s + 0x01) & 0x3F); }
  else if ((var.Y & 0xF8) == 0xF0 && (*s & 0xC0) == 0x80 && (*(s + 0x01) & 0xC0) == 0x80 && (*(s + 0x02) & 0xC0) == 0x80) 
    { Buf.Dat = 0x03; var.RGB = ((var.Y & 0x07) << 0x12) | ((*s & 0x3F) << 0x0C) | ((*(s + 0x01) & 0x3F) << 0x06) | (*(s + 0x02) & 0x3F); }
  else { Buf.Dat |= b7; return; }
  if (var.RGB < 0x20 || (var.RGB >= 0x7F && var.RGB < 0xA0)) { Buf.Dat |= b5; return; }
  if (var.RGB < 0x100) { Buf.Dat |= b2; return; }
  if (var.RGB >= 0x0590 && var.RGB <= 0x08FF) Buf.Dat |= b4;
  if (((Buf.Dat & 0x03) == 0x01 && var.RGB < 0x80) || ((Buf.Dat & 0x03) == 0x02 && (var.RGB < 0x800 || (var.RGB >= 0xD800 && var.RGB <= 0xDFFF))) || 
      ((Buf.Dat & 0x03) == 0x03 && (var.RGB < 0x10000 || var.RGB > 0x10FFFF))) { Buf.Dat |= b7; return; }
  if ((var.RGB >= 0x0300 && var.RGB <= 0x036F) || (var.RGB >= 0x1DC0 && var.RGB <= 0x1DFF) || (var.RGB >= 0x20D0 && var.RGB <= 0x20FF) ||
      (var.RGB == 0x200D || (var.RGB >= 0xFE00 && var.RGB <= 0xFE0F))) { Buf.Dat &= 0xF3; return; }
  if (var.RGB == 0x200B || var.RGB == 0x200C || var.RGB == 0x200E || var.RGB == 0x200F || (var.RGB >= 0xFE20 && var.RGB <= 0xFE2F) ||
      (var.RGB >= 0xE0100 && var.RGB <= 0xE01EF)) { Buf.Dat &= 0xF3; return; }
  if ((var.RGB >= 0x1100 && var.RGB <= 0x115F) || (var.RGB == 0x2329 || var.RGB == 0x232A) || (var.RGB >= 0x2E80 && var.RGB <= 0xA4CF && var.RGB != 0x303F) || 
      (var.RGB >= 0xAC00 && var.RGB <= 0xD7A3) || (var.RGB >= 0xF900 && var.RGB <= 0xFAFF) || (var.RGB >= 0xFE10 && var.RGB <= 0xFE19) || 
      (var.RGB >= 0xFE30 && var.RGB <= 0xFE6F) || (var.RGB >= 0xFF00 && var.RGB <= 0xFF60) || (var.RGB >= 0xFFE0 && var.RGB <= 0xFFE6) || 
      (var.RGB >= 0x20000 && var.RGB <= 0x2FFFD) || (var.RGB >= 0x30000 && var.RGB <= 0x3FFFD) || (var.RGB >= 0x1F300)) { Buf.Dat |= 0x08; return; }
  Buf.Dat |= b2; }

void PushKey(void) { anu l, d, i = Off; KeyBuf* k;
  if (Buf.Dat & b7) { Buf.Cod = Off; return; }
  if (Buf.pop == Buf.push) {
    k = AKey(++Buf.push); k->d[i] = Buf.Dat | b7; k->d[i + On] = On; l = On + (Buf.Dat & b10);
    if (Buf.Cod) k->u[i] = Buf.Cod;
    else while(l--) k->u[l] = *(Buf.Key + l); }
  else {
    k = AKey(Buf.push); d = k->d[i]; l = On + (Buf.Dat & b10);
    if (d & b6) { i = b1; d = k->d[i]; }
    if ((d & ~b76) == Buf.Dat) {
      if (Buf.Cod) { if (k->u[i] == Buf.Cod) l = -On; }
      else while(l--) { if (k->u[i + l] != *(Buf.Key + l)) break; } }
    if (!++l) { if (!++k->d[i + On]) --k->d[i + On]; }
    else {
      if (!i && ((Buf.Dat & b10) < b1)) { k->d[i] |= b6; i = b1; }
      else { k = AKey(++Buf.push); i = Off; if (Buf.pop == Buf.push) ++Buf.pop; }
      k->d[i] = Buf.Dat | b7; k->d[i + On] = On; l = On + (Buf.Dat & b10);
      if (Buf.Cod) k->u[i] = Buf.Cod;
      else while(l--) k->u[i + l] = *(Buf.Key + l); } }
  if (!Buf.Cod) Buf.Cod--; }
anu ShowKey(void) { anu d, i = Off; KeyBuf* k = AKey(Buf.push);
  if (Buf.pop == Buf.push) { Buf.Dat = Off; Buf.Count = Off; return Off; }
  if (k->d[Off] & b6) i = b1;
  Buf.Count = k->d[i + On]; Buf.Dat = k->d[i] & ~b76;
  d = On + (k->d[i] & b10); while(d--) *(Buf.Key + d) = k->u[i + d];
  return On; }
anu PopKey(void) { anu d, i = Off;
  while(!(AKey(Buf.pop)->d[Off] & b76) && (Buf.pop != Buf.push)) Buf.pop++;
  if (Buf.pop == Buf.push) { Buf.Dat = Off; Buf.Count = Off; return Off; }
  KeyBuf* k = AKey(Buf.pop); if (k->d[Off] & b7) k->d[Off] &= ~b7;
  else { k->d[Off] &= ~b6; i = b1; } Buf.Count = k->d[i + On]; Buf.Dat = k->d[i] & ~b76;
  d = On + (k->d[i] & b10); while(d--) *(Buf.Key + d) = k->u[i + d];
  return On; }
vanu Key(void) { vanu s = Off; anu d, c = Buf.push;
  while(c != Buf.pop) { d = AKey(c--)->d[Off]; if (d & b7) s++; if (d & b6) s++; }
  return s; }

void CSTime(an add) { if (Base.SJDN < 2461079) { Base.SJDN = 2461079; } if (Base.SJDN == 2461079 && Base.STime < 43200) Base.STime = 43200;
  if ((Base.TTime += add) > 86399) { Base.TTime -= 86400; Base.TJDN++; } Base.JDN = Base.TJDN + Base.SJDN; Base.Din = (Base.JDN % b210) + On;
  Base.Sam = (Base.JDN * 400) / 146097; var.XYz = Base.TTime + Base.STime;
  var.X = var.XYz % 60; var.Y = (((((((((var.X << 1) + var.X) << 3) + var.X) << 1) + var.X) << 2) + var.X) >> 11);
  Base.T[7] = 0x30 + var.X - (((var.Y << 2) + var.Y) << 1); Base.T[6] = 0x30 + var.Y;
  var.XYz /= 60; var.X = var.XYz % 60; var.Y = (((((((((var.X << 1) + var.X) << 3) + var.X) << 1) + var.X) << 2) + var.X) >> 11);
  Base.T[4] = 0x30 + var.X - (((var.Y << 2) + var.Y) << 1); Base.T[3] = 0x30 + var.Y;
  var.XYz /= 60; var.X = var.XYz % 24; var.Y = (((((((((var.X << 1) + var.X) << 3) + var.X) << 1) + var.X) << 2) + var.X) >> 11);
  Base.T[1] = 0x30 + var.X - (((var.Y << 2) + var.Y) << 1); Base.T[0] = 0x30 + var.Y; }

void IRnd(void) { Base.Rnd = (vanu)(Flag.ns | On); }
vnanu Rand(vnanu n) { Base.Rnd = (vanu)(var.XYz = 0x3A7B + (0x4F2D * Base.Rnd)); return ((var.XYz * n) >> 16); }
nanu Fcos(vnanu u) { return Fsin(u + 128); }
nanu Fsin(vnanu u) { static nanu s[64] = { 0,1,2,3,4,6,7,8,9,11,12,13,14,15,17,18,19,20,21,23,24,25,26,27,28,30,31,
  32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,51,52,53,54,55,55,56,57,58,58,59,60,60,61,62,62,63 };
  nanu r = u & 63; r = (u & b6) ? 64 + s[r] : s[r]; r = (u & b7) ? 127 - r : r; return ((u & b8) ? -r : r); }
nanu Ftg(vnanu u) { return (Fcos(u) ? (Fsin(u) / Fcos(u)) : -128); }
nanu Fctg(vnanu u) { return (Fsin(u) ? (Fcos(u) / Fsin(u)) : -128); }

vanu CreateCA(vanu c, anu a, char *dst) { char *src, *b = dst; static char *s = "24;21;22;1;\00022;2;\00023;3;\00024;4;\00027;7;\00029;9;\000";
  if ((var.A ^= a)) { var.X = 64; var.Y = 36; *dst++ = '\33'; *dst++ = '['; while(var.X) { if ((var.A & var.X)) {
    src = s + var.Y + ((a & var.X) ? 3 : 0); *dst++ = *src++; *dst++ = *src++; if (*src) *dst++ = *src++; } var.X >>= 1; var.Y -= 6; } var.A = 2; }
  if (var.C == c) { if (var.A) *(dst - 1) = 'm'; } else { var.C = c; PalBuf* pal = APal(c); MemCpy(dst, pal->d + var.A, pal->l - var.A); dst += pal->l - var.A; }
  var.A = a; return (vanu)(dst - b); }
void Print(vanu c, anu a, char *str) { char *dst = var.dbuf + CreateCA(c, a, var.dbuf);
  MemCpy(dst, str, (var.Z = (vanu)StrLen(str))); SysWrite(var.dbuf, dst + var.Z - var.dbuf); }
void GenFC(vanu c, anu deep) { anu i, j = 1, k = (deep > b3) ? 2 : (deep < b3) ? 0 : 1; c = (c < 255) ? c : 255;
  char *d[3] = { "\2\33[", "\6\33[38;5", "\6\33[38;2" }; PalBuf *mode = (PalBuf*)d[k], *dst = APal(c), *src = APal(Base.AF + c);
  if (k == 1 ) { var.R  = (16 + 36 * ((var.R  * 5 + 128) / 255) + 6 * ((var.G * 5 + 128)/ 255) + ((var.B * 5 + 128)/ 255)); }
  else { if (!k) { var.R  = (((var.R  * 299 + var.G * 587 + var.B * 114) > 127999) ? 90 : 30) + (((var.R  > 127) << 2) | ((var.G > 127) << 1) | (var.B > 127)); }
         else j = 3; } src->l = mode->l; MemCpy(src->d, mode->d, mode->l); var.RGB = var.R  | (var.G << 8) | (var.B << 16);
  while(j--) { i = (anu)var.RGB; var.RGB >>= b3; if (k) { src->d[src->l++] = ';'; }
    if ((var.Y = ((((i << 2) + i) << 3) + i) >> 12)) { src->d[src->l++] = 0x30 + var.Y; i -= (((((var.Y << 1) + var.Y) << 3) + var.Y) << 2); }
    if ((var.R = (((((((((i << 1) + i) << 3) + i) << 1) + i) << 2) + i) >> 11)) || var.Y) { src->d[src->l++] = 0x30 + var.R; i -= (((var.R << 2) + var.R) << 1); }
    src->d[src->l++] = 0x30 + i; }
  src->d[src->l++] = 'm'; dst->l = src->l; MemCpy(dst->d, src->d, src->l); src->d[2] = '4';
  if (dst->d[2] == '9') { src->l++; src->d[2] = '1'; src->d[5] = src->d[4]; src->d[4] = src->d[3]; src->d[3] = '0'; } }
void GenRGB(anu mode, vanu c, vanu n) { n = (n) ? n : 1; if (mode) { var.RGB = (((an)c << 24) / n) - On; var.G = (anu)var.RGB;
    var.RGB >>= 8; var.B = (anu)var.RGB; var.RGB >>= 8; var.R  = (anu)var.RGB; } 
  else { var.Z = var.U + ((an)c << 9) / n; var.B = 128 + Fsin(var.Z); var.G = 128 + Fsin(var.Z + 171); var.R  = 128 + Fsin(var.Z + 342); } }
void SetSeparator(char s) { Base.T[2] = s; Base.T[5] = s; }
void SetBorder(anu on, vanu b) {
  Print((Base.Border = (on) ? (Base.AF + Base.Last) : (b < Base.AF || b > (Base.AF + Base.Colours)) ? Base.AF : b), var.A, "\033[2J"); }
void SetPalette(anu set) { var.dpal = (var.dcon + sizeof(Canalysis)); if (set) var.dpal += var.Spal; }
void SwitchPalette(void) { anu* a = (var.dcon + sizeof(Canalysis)); if (a == var.dpal) { a += var.Spal; } var.dpal = a; }
void GenLast(vnanu c) { GenRGB(Off, c, 511); GenFC(Base.Last, Base.Deep); var.XY = var.Spal; anu *off = (anu*)APal(Base.Last);
  if (var.dpal != (var.dcon + sizeof(Canalysis))) { var.XY = -var.XY; } MemCpy(off + var.XY, off, 20);
  MemCpy(off + var.XY + (var.Spal >> 1), off + (var.Spal >> 1), 20); }
void GenPalettes(void) { Base.Colours = (Base.Colours < 1) ? 1 : (Base.Colours < 255) ? Base.Colours : 254; Base.Last = Base.Colours + 1; GenLast(Off); var.X = 2;
  while(var.X--) { SetPalette(var.X); var.R = 0; var.G = 0; var.B = 0; GenFC(Off, Base.Deep); var.R = 255; var.G = var.R; var.B = var.R;
    var.XZ = Base.Colours; while(var.XZ) { GenFC((Base.Last - var.XZ), Base.Deep); GenRGB(var.X, --var.XZ, Base.Colours); } } }
void ColourInit(vanu c, vanu d) { Base.Deep = (d < b3) ? 3 : (d > b3) ? 24 : 8; Base.Colours = (c < 1) ? 1 : (c < 255) ? c : 254; Base.Last = Base.Colours + 1;
  Base.AF = Base.Colours + 2; var.Spal = (((Base.AF << 2) + Base.AF) << 3); var.dbuf = (char*)var.dcon + sizeof(Canalysis) + (var.Spal << 1);
  Base.I[0] = Off; Base.I[1] = On; Base.I[2] = (c == 1) ? 1 : 2; Base.I[3] = (c == 1) ? 1 : ((((c << 3) / 5) + 14) >> 3); Base.I[4] = ((((c << 4) / 5) + 12) >> 3);
  Base.I[5] = (c == 1) ? 1 : ((((((c << 1) + c) << 3) / 5) + 12) >> 3); Base.I[6] = (c == 1) ? 1 : ((((c << 5) / 5) + 10) >> 3); Base.I[7] = Base.Colours;
  Base.Border = Fon(0); Base.Fone = Fon(1); Base.Ink = Ink(0); GenPalettes(); }
As HowSize(anu c, anu d, vanu w, As a) { Base.PCell = sizeof(AFunction); Base.Goc = sizeof(goc); w = (w < 2) ? 2 : w;
  var.R = (Base.Goc < 8) ? (Base.Goc << 3) : 60; c = (c < 3) ? 3 : (c > var.R) ? var.R : c; var.off = 1 << c; Base.Mcol = var.off - 1;
  Base.W = ((var.off + (var.off << 3)) >> 5); Base.Mstr = Base.W - 1; var.off = (1 << ((c << 1) - 2)) | (1 << ((c << 1) - 5));
  Base.Colours = (Base.Colours < 1) ? 1 : (Base.Colours < 255) ? Base.Colours : 254; Base.Last = Base.Colours + 1; Base.AF = Base.Colours + 2;
  var.Spal = (((Base.AF << 2) + Base.AF) << 3); Base.Dynamic = (d < w) ? d : 1; Base.CellP = c; Base.Win = w; var.dkey = (anu*)a;
  if (var.dkey < (var.data = var.dkey + (256 << 3))) {
    if (var.data < (var.ds = var.data + (var.off << 2))) {
      if (var.ds < (var.pal = var.ds + var.off)) {
        if (var.pal < (anu*)(var.offset = (rgoc*)(var.pal + var.off))) {
          if (var.offset < (var.dlwin = (var.offset + var.off))) {
            if ((anu*)var.dlwin <= (var.dwin = (anu*)(var.dlwin + (Base.W * Base.Dynamic)))) {
              if (var.dwin < (var.event = (var.dwin + Base.Win * sizeof(Windows)))) {
                if (var.event < (var.exec = var.event + (sizeof(Events) << 8))) {
                  if (var.exec < (var.dcon = var.exec + (Base.PCell << 8))) {
                    if (var.dcon < (var.dpal = (var.dcon + sizeof(Canalysis)))) {
                      if ((char*)var.dpal < (var.dbuf = (char*)var.dpal + (var.Spal << 1))) {
                        if (var.dbuf < (var.end = (char*)var.dpal + ((((256 << 2) + 256) << 4) + 4096))) return (As)(var.end - a); }
  } } } } } } } } } } return Off; }
void InitVram(anu c, anu d, vanu w, vanu how, vanu hz, vanu apm) { Base.Goc = sizeof(goc); w = (w < 2) ? 2 : w;
  var.R = (Base.Goc < 8) ? (Base.Goc << 3) : 60; c = (c < 3) ? 3 : (c > var.R) ? var.R : c; var.addr = VRam.addr; var.size = VRam.size;
  MemCpy(&var.Save, &var.dpal, sizeof(var.Save)); Base.Loop = On; Base.Error = 4; if (!(VRam.size = HowSize(c,d,w,Off))) --Base.Error;
  if (Base.Error == 4 && !(VRam.addr = GetRam(&VRam.size))) { Base.Error  = 2; } if (Base.Error  == 4 && !(VRam.size = HowSize(c,d,w,VRam.addr))) Base.Error = 1;
  if (Base.Error < 4) { MemCpy(&var.dpal, &var.Save, sizeof(var.Save)); VRam.addr = var.addr; VRam.size = var.size; Base.Loop--; return; }
  if (var.size) { FreeRam(var.addr,var.size); } Base.UGmax = (rgoc)(-1); Base.Gmax = Base.UGmax >> 1; Base.Ginf = ~Base.Gmax; Base.Gmin = Base.Ginf + 1;
  Base.Count = 6; Base.Error = Off; Base.D = Base.CellP + 2; Base.DS = Base.CellP; Base.O = Base.CellP; Base.V = 2; Base.Spd1 = Base.Mcol >> 4;
  Base.Spd0 = Base.Spd1 >> 2; Base.Speed = Base.Spd1; Convas.D = Off; Convas.S = Base.Win; Convas.CW = Base.Mcol; Convas.W = Off; Convas.CH = Base.Mstr;
  Convas.H = Off; Convas.Win = Base.Win; VP.Mode = b2; VP.Key = 9; var.Syn = Base.Hz - var.Syn; Base.Hz = (hz < 25) ? 25 : (hz > 10000) ? 10000 : hz;
  var.Syn = Base.Hz - var.Syn; Base.Apm = (apm < 50) ? 50 : ((apm > 1000) ? 1000 : apm); Base.On = (how > Base.Hz) ? Base.Hz : how;
  Base.FTime = (Base.On) ? Base.On : 25; var.Loop = (Base.Apm * Base.Hz) / Base.FTime; Vector(ECD) = Encode; Vector(RPE) = RPEncode; }
As SystemSwitch(void) { if (VRam.SystemSwitch) { VRam.SystemSwitch--; SwitchRaw(); InitVram(CellPow, Dynam, Wind, FHow, FHz, FApm); if (!Base.Loop) return Off;
    Real(Off); ColourInit(Fcolour, CFDeep); CSTime(Off); SetSeparator(':'); IRnd(); SWD(); SyncSize();
    Keys(K_F1,K_Ctrl_DOW,K_Ctrl_LEF,K_Ctrl_UP,K_Ctrl_RIG,K_UP,K_LEF,K_DOW,K_RIG); Mouse(M_Lkey,M_Mkey,M_Rkey,M_Rollup,M_Rolldown,M_ShRollup,M_ShRolldown);
    Print(Off, Off, "\033[?1049;7;1000h\033[?25l"); return On; }
  else { VRam.SystemSwitch++; SwitchRaw();
    Print(Off, Off, "\033[?1049;1000l\033[0m\033[?25h"); if (VRam.size) { FreeRam(VRam.addr,VRam.size); } return Base.Error; } }

void MoveNorm(vgoc x, vgoc y) { static anu Wait = 7; Wait = (Wait) ? Wait : 7;
  if (VP.Mode & b1 && Convas.Win < Base.Win) {
    if (Convas.Win < Convas.D || Convas.Win >= Convas.S) { Windows* w = Win(Convas.Win);
      vgoc c = (w->W > w->MaxCs) ? w->W : w->MaxCs, s = (w->H > w->MaxH) ? w->H : w->MaxH;
      if (w->Xr && w->Yr && c && s) {
        var.Xr = (x < w->Xr) ? ((--Wait) ? w->Xr : (w->Xr + c - 1)) : (x >= (w->Xr + c)) ? ((--Wait) ? (w->Xr + c - 1) : w->Xr) : x;
        var.Yr = (y < w->Yr) ? ((--Wait) ? w->Yr : (w->Yr + s - 1)) : (y >= (w->Yr + s)) ? ((--Wait) ? (w->Yr + s - 1) : w->Yr) : y; return; } } }
  var.Xr = Base.Mcol; var.Yr = Base.Mstr;
  var.Xr = (x < -var.Xr) ? ((--Wait) ? -var.Xr : ((var.Xr << 1) + 1)) : (x > (var.Xr << 1)) ? ((--Wait) ? ((var.Xr << 1) + 1) : -var.Xr) : x;
  var.Yr = (y < -var.Yr) ? ((--Wait) ? -var.Yr : ((var.Yr << 1) + 1)) : (y > (var.Yr << 1)) ? ((--Wait) ? ((var.Yr << 1) + 1) : -var.Yr) : y; }
void MoveConvas(vgoc dx, vgoc dy) { Buf.Ctrl = On; MoveNorm(VP.X + dx, VP.Y + dy);
  dx = VP.X / (goc)TS.c; dy = VP.Y / (goc)TS.r; VP.X = var.Xr; VP.Y = var.Yr; VP.Xs = (VP.X < Off) ? (TS.c + (VP.X % (goc)TS.c)) : (VP.X % (goc)TS.c);
  VP.Ys = (VP.Y < Off) ? (TS.r + (VP.Y % (goc)TS.r)) : (VP.Y % (goc)TS.r); if ((VP.X / (goc)TS.c) != dx || (VP.Y / (goc)TS.r) != dy) Buf.Ctrl++; }
anu MoveScreen(vgoc mx, vgoc my) {
  if ((((VP.X - mx) ^ VP.X) & Base.Ginf) || (((VP.Y - my) ^ VP.Y) & Base.Ginf)) return Off;
  MoveNorm(VP.X - mx, VP.Y - my); VP.X = var.Xr; VP.Y = var.Yr; VP.Xs -= mx; VP.Ys -= my; return On; }

void Free(void) { vgoc dx = Off, dy = Off; anu i, *n; Buf.Ctrl = Off; Vector(RPE)();
  if (*Buf.Key == K_ESC && *(Buf.Key + On) == K_NO) Buf.Cod = Off;
  else { Buf.Cod = (*Buf.Key & b7) ? Off : (*Buf.Key == K_ESC) ? *(Buf.Key + On) : *Buf.Key;
    if (Buf.Cod == K_Mouse) { Buf.Mkey = *(Buf.Key + 2); Buf.MX = *(Buf.Key + 3) - 0x21; Buf.MY = *(Buf.Key + 4) - 0x21; i = Off;
      if (Buf.Mkey == Buf.Ru) Buf.Cod = VP.up;
      else if (Buf.Mkey == Buf.Rd) Buf.Cod = VP.ud;
      else if (Buf.Mkey == Buf.cRu) Buf.Cod = VP.ri;
      else if (Buf.Mkey == Buf.cRd) Buf.Cod = VP.le;
      else if (Buf.Mkey == Buf.Lk) { Buf.LkX = Buf.MX; Buf.LkY = Buf.MY; i++; }
      else if (Buf.Mkey == Buf.Mk) { Buf.MkX = Buf.MX; Buf.MkY = Buf.MY; i = b10; }
      else if (Buf.Mkey == Buf.Rk) { Buf.RkX = Buf.MX; Buf.RkY = Buf.MY; i = b1; }
      if (i && MoveScreen(VP.Xs - Buf.MX, VP.Ys - Buf.MY)) {  } }
    else { if (Buf.Cod) {
        if (Vector(Buf.Cod)) { Buf.Ctrl++; VP.Wexe = Event(Buf.Cod)->W; Vector(Buf.Cod)(); }
        n = &VP.Key; i = *n++; while (i--) if (*n++ == Buf.Cod) { Buf.Ctrl++; break; } }
      if (!Buf.Ctrl) PushKey(); }
    if (Buf.Cod) { ++Buf.tic; Buf.Ctrl = On; } }
  if (Buf.Ctrl) {
    if (Buf.Cod == VP.scs) { VP.dXY = On; Base.Speed = (VP.Mode ^= b2) ? Base.Spd1 : Base.Spd0; }
    else if (Buf.Cod == VP.Anchor) Anchor();
    else if (Buf.Cod == VP.bcu) { SwitchPalette(); SetBorder(Base.On, Base.Border); }
    else if (Buf.Cod == VP.ssc) SwitchCur();
    else if (Buf.Cod == VP.Exit) Bye();
    else {
      if (Buf.Cod != VP.Cod) { VP.dXY = On; VP.Cod = Buf.Cod; }
      if ((Buf.tic > 7) && !(Buf.tic & b10) && (VP.dXY < Base.Speed)) VP.dXY <<= On;
      if (Buf.Cod == VP.le) dx = -VP.dXY;
      else if (Buf.Cod == VP.ri) dx = VP.dXY;
      else if (Buf.Cod == VP.up) dy = -VP.dXY;
      else if (Buf.Cod == VP.ud) dy = VP.dXY;
      MoveConvas(dx, dy); } }
  if ((var.Syn += Real(Base.Apm) + Base.Hz) >= var.Loop) {
    if ((var.Dis += (var.Syn / var.Loop)) >= Base.FTime) { CSTime(var.Dis / Base.FTime); var.Dis %= Base.FTime; }
    static vanu c = Off; GenLast((c = (c + 1) & 511)); if (Base.On && Vector(Timer)) { VP.Wexe = Event(Timer)->W; Vector(Timer)(); }
    var.Syn %= var.Loop; }
  if (Vector(Off)) { VP.Wexe = Event(Off)->W; Vector(Off)(); }
  if (SyncSize() || Buf.Ctrl > On) { SetBorder(Base.On, Base.Border); } else {  } }

void Nop(void) { }
void Encode(void) { UTFinfo(Buf.Key); }
void RPEncode(void) { GetKey(Buf.Key); Vector(ECD)(); }
void Anchor(void) { if (VP.Mode ^= b1) { } else { } }
void SwitchCur(void) { VP.Mode ^= b0; }
void Bye(void) { Base.Loop = Off; }

void WSwitch(void) { if (Win(VP.Wexe)->Xr) Win(VP.Wexe)->F ^= b1; }
void WASwitch(void) { if (Win(VP.Wexe)->F ^= b1) WView(VP.Wexe); }
void WDown(void) { if (Convas.D) { vanu l = Convas.D; Win(--l)->Layer = Off; while(l) ++Win(--l)->Layer; } }
void WUp(void) { if (Convas.D) { vanu l = Convas.D; Win(Off)->Layer = --l; while(l) --Win(--l)->Layer; } }
void WTop(vanu n) { vanu l = Convas.D; if ((n >= Convas.D && n < Convas.S) || n >= Base.Win) return;
  if (n > l) { l = Base.Win; } Win(n)->Layer = l; l -= n; while(l--) --Win(n + l)->Layer; }

void _WView(vanu n, anu c, vgoc *a) { vgoc x = Off, y = Off;
  if ((n >= Convas.D && n < Convas.S) || n >= Base.Win) { return; } Windows* w = Win(n);
  if (c > On) { x = a[Off]; if (!(y = a[On])) x = Off;
    if (x && !(w->F & b0)) { x = (x < Off) ? -x : x; y = (y < Off) ? -y : y;
      if ((rvgoc)VP.X > (rvgoc)Base.Mcol || (rvgoc)VP.Y > (rvgoc)Base.Mstr) return; } }
  if (c == Off) { x = VP.Xs + On; y = VP.Ys + On;
    if (w->F & b0) { if (TS.c < (x + w->W)) { x = -On; } if (TS.r < (y + w->H)) y = -On; } 
    else if ((rvgoc)VP.X > (rvgoc)Base.Mcol || (rvgoc)VP.Y > (rvgoc)Base.Mstr) return; }
  w->Xr = x; w->Yr = y; if (w->Xr) { w->F |= b1; } else { w->F &= ~b1; } }
vanu _Window(anu t, anu col, anu c, rvgoc *a) { vanu l, n; Windows* w;
  if (t) { if (!Base.Dynamic) { return Base.Win; } if ((n = Convas.D++) >= Base.Dynamic) { n = --Convas.D; } w = Win(n); w->F = 8; w->Layer = n; }
  else { if ((--Convas.S) < Base.Dynamic) { ++Convas.S; } n = Convas.S; w = Win(n); w->F = 9; w->Layer = Base.Win;
    l = Base.Win - n; while(l--) --Win(n + l)->Layer; }
  w->parent = n; w->child = n; w->MaxVs = Off; w->XCur = Off; w->YCur = Off; w->WFirstSR = Base.Mstr; w->Xr = Off; w->Yr = Off; w->W = Off; w->H = Off;
  if (c > On) { if ((w->Xr = a[0])) { if (!(w->Yr = a[1])) w->Xr = Off; } } if (c > 2) { w->W = a[2]; if (c > 3) w->H = a[3]; }
  w->Colour = col; if (w->F == 9) { if (w->W < b1) { w->W = b1; } if (!w->H) { w->H++; } } if (w->Xr) { w->F |= b1; } return n; }
void _WExecs(vanu n, anu cur, anu c, AFunction *a) { if ((n >= Convas.D && n < Convas.S) || n >= Base.Win) return;
  if ((Win(n)->F & b0) && c--) { Event(cur)->W = n; Exe(cur, a[Off]);
    if (Event(cur)->C && c) { anu j, k = Event(cur)->C, i = On; while(k-- && c--) { j = K_Mouse;
      while(--j) { if (Event(j)->W == n && Event(j)->N == i) { Exe(j, a[i]); i++; break; } } } } } }
void _WSet(vanu n, anu c, anu *a) { if ((n >= Convas.D && n < Convas.S) || n >= Base.Win) return;
  if (c--) { Windows* w = Win(n); w->F &= ~b2; if (a[Off]) { w->F |= b2; } if (c) { w->F &= ~b3; if (a[On]) { w->F |= b3; } } } }
void _SEvents(anu c, anu *a) { anu m, k = c; while(k--) { m = a[k]; Event(m)->W = Base.Win; Event(m)->C = c; Event(m)->N = k; } }
void _SExec(anu c, AFunction *a) { anu k = K_Mouse; while(k--) { if (Event(k)->W == Base.Win) {
  if (Event(k)->C && c) { anu j, x = Event(k)->C, i = Off;
    while(x-- && c--) { j = ECD; while(--j) { if (Event(j)->W == Base.Win && Event(j)->N == i) { Exe(j, a[i]); i++; break; } } } } break; } } }
void _SKeys(anu c, anu *a) { anu *p = &VP.Key, i = Off; p += *p; c = (c > VP.Key) ? VP.Key : c; while(c--) *p-- = a[i++]; }
void _SMouse(anu c, anu *a) { anu *p = &Buf.Lk, i = Off; c = (c > 7) ? 7 : c; while(c--) *p++ = a[i++]; }
void _FSet(anu cp, anu c, vanu *a) { vanu d = Base.Dynamic, w = Base.Win, o = Base.On, h = Base.Hz, f = Base.Apm;
  anu b = (var.dpal == (var.dcon + sizeof(Canalysis))) ? 0 : 1;
  if (c--) { d = a[0]; if (c--) { w = a[1]; if (c--) { o = a[2]; if (c--) { h = a[3]; if (c) { f = a[4]; } } } } } if (d > 255) d = 255;
  InitVram(cp, d, w, o, h, f); ColourInit(Base.Colours, Base.Deep); SetPalette(b); }
void _CSet(anu c, vanu *a) { vanu l = Base.Colours, d = (vanu)Base.Deep, b = (var.dpal == (var.dcon + sizeof(Canalysis))) ? 0 : 1;
  if (c--) { l = a[Off]; if (c) { d = a[On]; } } ColourInit(l, d); SetPalette(b); }
void _WData(vanu n, char *str, anu c, rvgoc *a) { if ((n >= Convas.D && n < Convas.S) || n >= Base.Win) return;
  Windows* w = Win(n); if (!(w->MaxVs)) {
     }
  (void)*str; (void)c; (void)*a; }
