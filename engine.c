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

Cell StrLen(char *s) { if (!s) return Off;
  uint8_t *f = (uint8_t*)s; while(*f++);
  return (--f - (uint8_t*)s); }
void MemSet(void* buf, uint8_t val, Cell len) { uint8_t *p = (uint8_t*)buf;
  while(len && ((Cell)p & (SCell - 1))) { len--; *p++ = val; }
  if (len >= SCell) {
    Cell vW = val * (((Cell) - 1) / 255); Cell *pW = (Cell*)p;
    Cell i = len / SCell; len &= (SCell - 1); while(i--) *pW++ = vW;
    p = (uint8_t*)pW; }
  while(len--) *p++ = val; }
void MemMove(void* dst, void* src, Cell len) {
  if (dst > src) { uint8_t *d = (uint8_t*)dst + len, *s = (uint8_t*)src + len;
    while(len--) { *--d = *--s ; } }
  else if (dst != src ) { MemCpy(dst, src, len); } }
void MemCpy(void* dst, void* src, Cell len) { uint8_t *d = (uint8_t*)dst, *s = (uint8_t*)src;
  while(len && ((Cell)d & (SCell - 1))) { *d++ = *s++; len--; }
  if (len >= SCell && ((Cell)s & (SCell - 1)) == 0) {
    Cell *dW = (Cell*)d; Cell *sW = (Cell*)s;
    Cell i = len / SCell; len &= (SCell - 1); while(i--) *dW++ = *sW++;
    d = (uint8_t*)dW; s = (uint8_t*)sW; }
  while(len--) *d++ = *s++ ; }
int8_t MemCmp(void* dst, void* src, Cell len) { uint8_t *d = (uint8_t*)dst, *s = (uint8_t*)src;
  while(len && ((Cell)d & (SCell - 1))) { len--; if (*d++ != *s++) return (int8_t)(*--d - *--s); }
  if (len >= SCell && ((Cell)s & (SCell - 1)) == 0) {
    Cell *dW = (Cell*)d; Cell *sW = (Cell*)s;
    Cell i = len / SCell; len &= (SCell - 1); while(i-- && (*dW++ == *sW++));
    if (i + 1) { --dW; --sW; len += SCell; }
    d = (uint8_t*)dW; s = (uint8_t*)sW; }
  while(len--) { if (*d++ != *s++) return (int8_t)(*--d - *--s); }
  return Off; }

void UTFinfoTile(uint8_t *s, Cell len) { Buf.Dat = 0xC0; if (!len) return;
  if ((*s & 0xE0) == 0xC0 && len < 0x02) return;
  else if ((*s & 0xF0) == 0xE0 && len < 0x03) return;
  else if ((*s & 0xF8) == 0xF0 && len < 0x04) return;
  UTFinfo(s); }
void UTFinfo(uint8_t *s) { var.Y = *s++; Buf.Dat = Off; if (var.Y < 0x80) var.RGB = (uint32_t)var.Y;
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

void PushKey(void) { uint8_t l, d, i = Off; KeyBuf* k;
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
uint8_t ShowKey(void) { uint8_t d, i = Off; KeyBuf* k = AKey(Buf.push);
  if (Buf.pop == Buf.push) { Buf.Dat = Off; Buf.Count = Off; return Off; }
  if (k->d[Off] & b6) i = b1;
  Buf.Count = k->d[i + On]; Buf.Dat = k->d[i] & ~b76;
  d = On + (k->d[i] & b10); while(d--) *(Buf.Key + d) = k->u[i + d];
  return On; }
uint8_t PopKey(void) { uint8_t d, i = Off;
  while(!(AKey(Buf.pop)->d[Off] & b76) && (Buf.pop != Buf.push)) Buf.pop++;
  if (Buf.pop == Buf.push) { Buf.Dat = Off; Buf.Count = Off; return Off; }
  KeyBuf* k = AKey(Buf.pop); if (k->d[Off] & b7) k->d[Off] &= ~b7;
  else { k->d[Off] &= ~b6; i = b1; } Buf.Count = k->d[i + On]; Buf.Dat = k->d[i] & ~b76;
  d = On + (k->d[i] & b10); while(d--) *(Buf.Key + d) = k->u[i + d];
  return On; }
ugoc Key(void) { ugoc s = Off; uint8_t d, c = Buf.push;
  while(c != Buf.pop) { d = AKey(c--)->d[Off]; if (d & b7) s++; if (d & b6) s++; }
  return s; }

void ASu(uint32_t add) { var.RGB = 0; var.Y = Base.Count;
  while(var.Y--) { Base.Time[var.Y] = (uint16_t)(var.RGB += Base.Time[var.Y] + add); add = 0; if (!(var.RGB >>= 16)) break; } } 
void CSu(void) { var.RGB = 0; var.Y = Base.Count;
  while(var.Y--) { Base.Su[var.Y] = (uint16_t)(var.RGB += Base.Time[var.Y] + Base.Timer[var.Y]); var.RGB >>= 16; } } 
uint16_t DSu(uint16_t d) { var.XYz = 0; var.Y = -1;
  while(++var.Y < Base.Count) { Base.Su[var.Y] = (uint16_t)((var.XYz = (var.XYz << 16) | Base.Su[var.Y]) / d); var.XYz %= d; }
  return (uint16_t)var.XYz; }
void Time(void) { var.RGB = 0; var.Y = Base.Count;
  while(var.Y--) { Base.Su[var.Y] = (uint16_t)(var.RGB += Base.Time[var.Y] + Base.Timer[var.Y]); var.RGB >>= 16; }
  var.X = DSu(60); var.Y = ((var.X * 205) >> 11); Base.T[7] = 0x30 + var.X - (var.Y * 10); Base.T[6] = 0x30 + var.Y;
  var.X = DSu(60); var.Y = ((var.X * 205) >> 11); Base.T[4] = 0x30 + var.X - (var.Y * 10); Base.T[3] = 0x30 + var.Y;
  var.X = DSu(24); var.Y = ((var.X * 205) >> 11); Base.T[1] = 0x30 + var.X - (var.Y * 10); Base.T[0] = 0x30 + var.Y; }

void IRnd(void) { Base.Rnd = (uint16_t)(Flag.ns | On); }
int16_t Rand(int16_t n) { Base.Rnd = (uint16_t)(var.XYz = 0x3A7B + (0x4F2D * Base.Rnd)); return ((var.XYz * n) >> 16); }
int8_t Fcos(int16_t u) { return Fsin(u + 128); }
int8_t Fsin(int16_t u) { static int8_t s[64] = { 0,1,2,3,4,6,7,8,9,11,12,13,14,15,17,18,19,20,21,23,24,25,26,27,28,30,31,
  32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,51,52,53,54,55,55,56,57,58,58,59,60,60,61,62,62,63 };
  int8_t r = u & 63; r = (u & b6) ? 64 + s[r] : s[r]; r = (u & b7) ? 127 - r : r; return ((u & b8) ? -r : r); }
int8_t Ftg(int16_t u) { return (Fcos(u) ? (Fsin(u) / Fcos(u)) : -128); }
int8_t Fctg(int16_t u) { return (Fsin(u) ? (Fcos(u) / Fsin(u)) : -128); }

uint8_t CreateCA(uint8_t n, uint8_t m, char *dst) { char *src, *a = dst; static char *s = "24;21;22;1;\00022;2;\00023;3;\00024;4;\00027;7;\00029;9;\000";
  PalBuf* pal = APal(n); if ((var.A ^= m)) { var.X = 64; var.Y = 36; *dst++ = '\33'; *dst++ = '['; while(var.X) { if ((var.A & var.X)) {
    src = s + var.Y + ((m & var.X) ? 3 : 0); *dst++ = *src++; *dst++ = *src++; if (*src) *dst++ = *src++; } var.X >>= 1; var.Y -= 6; } var.A = 2; }
  var.X = pal->l - var.A; if (n & b7) { if (n == var.F) { if (var.A) *(dst - 1) = 'm'; } else { var.F = n; MemCpy(dst, pal->d + var.A, var.X); dst += var.X; } }
  else { if (n == var.I) { if (var.A) *(dst - 1) = 'm'; } else { var.I = n; MemCpy(dst, pal->d + var.A, var.X); dst += var.X; } }
  var.A = m; return (uint8_t)(dst - a); }
void Print(uint8_t n, uint8_t m, char *str) { if (!str) { return; } char *dst = var.dbuf; uint8_t len = CreateCA(n, m, dst); dst += len;
  len = (uint8_t)StrLen(str); MemCpy(dst, str, len); SysWrite(var.dbuf, dst - var.dbuf + len); }
void GenFC(uint8_t c, uint8_t deep) { uint8_t i, j = 1, k = (deep > b3) ? 2 : (deep < b3) ? 0 : 1; c = (c > last) ? last : c;
  char *d[3] = { "\2\33[", "\6\33[38;5", "\6\33[38;2" }; PalBuf *mode = (PalBuf*)d[k], *dst = APal(c), *src = APal(c + 128);
  if (k == 1 ) { var.R  = (16 + 36 * ((var.R  * 5 + 128) / 255) + 6 * ((var.G * 5 + 128)/ 255) + ((var.B * 5 + 128)/ 255)); }
  else {
    if (!k) { var.R  = (((var.R  * 299 + var.G * 587 + var.B * 114) > 127999) ? 90 : 30) + (((var.R  > 127) << 2) | ((var.G > 127) << 1) | (var.B > 127)); }
    else { j = 3; } } src->l = mode->l; MemCpy(src->d, mode->d, mode->l); var.RGB = var.R  | (var.G << 8) | (var.B << 16);
  while(j--) { i = (uint8_t)var.RGB; var.RGB >>= b3; if (k) { src->d[src->l++] = ';'; }
    if ((var.Y = (i * 41) >> 12)) { src->d[src->l++] = 0x30 + var.Y; i -= var.Y * 100; }
    if (var.Y || ((i * 205) >> 11)) { src->d[src->l++] = 0x30 + (var.Y = (i * 205) >> 11); i -= var.Y * 10; } src->d[src->l++] = 0x30 + i; }
  src->d[src->l++] = 'm'; dst->l = src->l; MemCpy(dst->d, src->d, src->l); src->d[2] = '4'; if (dst->d[2] == '9') {
    src->l++; src->d[2] = '1'; src->d[5] = src->d[4]; src->d[4] = src->d[3]; src->d[3] = '0'; } }
void SetSeparator(char s) { Base.T[2] = s; Base.T[5] = s; }
void SetBorder(uint8_t on, uint8_t b) { if (on) { b = Last; } else  { b = (b < Dark || b > (Base.Colours + Dark)) ? Dark : b; }
  Print((Base.Border = b), var.A, "\033[2J"); }
void SetPalette(uint8_t set) { var.dpal = (uint8_t*)VRam.addr; if (set) { var.dpal += 8192; } }
void SwitchPalette(void) { uint8_t* a = (uint8_t*)VRam.addr; if (a == var.dpal) { a += 8192; } var.dpal = a; }
void GenRGB(uint8_t mode, uint16_t c, uint16_t n) { n = (n) ? n : 1; if (mode) { var.RGB = (((1 << 24) * c) / n) - On; var.G = (uint8_t)var.RGB;
    var.RGB >>= 8; var.B = (uint8_t)var.RGB; var.RGB >>= 8; var.R  = (uint8_t)var.RGB; } 
  else { var.Z = var.U + (c << 9) / n; var.B = 128 + Fsin(var.Z); var.G = 128 + Fsin(var.Z + 171); var.R  = 128 + Fsin(var.Z + 342); } }
void GenLast(int16_t c) { GenRGB(Off, c, 511); GenFC(last, Base.Deep); c = 8192; uint8_t* off = (uint8_t*)VRam.addr + 4064;
  if (var.dpal != (uint8_t*)VRam.addr) { off += c; c = -c; } MemCpy(off + c, off, 32); MemCpy(off + 4096 + c, off + 4096, 32); }
void GenPalette(uint8_t s) { Base.Colours = (Base.Colours < 1) ? 1 : (Base.Colours > Maxcol) ? Maxcol : Base.Colours; SetPalette(s); GenLast(Off);
  var.R = 0; var.G = 0; var.B = 0; GenFC(Off, Base.Deep); var.R = 255; var.G = var.R ; var.B = var.R ; var.X = Base.Colours;
  while(var.X) { GenFC((Base.Colours - var.X + On), Base.Deep); GenRGB(s, --var.X, Base.Colours); } }
void ColourInit(uint8_t c, uint8_t d) { c = (c < 1) ? 1 : (c > Maxcol) ? Maxcol : c; d = (d < b3) ? 3 : (d > b3) ? 24 : 8;
  if (c != Base.Colours || d != Base.Deep) { Base.Colours = c; Base.Deep = d; var.U = Off; GenPalette(On); GenPalette(Off); } }
Cell HowSize(uint8_t c, uint16_t w, Cell a) { var.off = (1 << ((c << 1) - 2)) | (1 << ((c << 1) - 5)); var.dpal = (uint8_t*)a;
  if (var.dpal < (var.dkey = var.dpal + 16384)) {
    if (var.dkey < (var.event = var.dkey + 2048)) {
      if (var.event < (var.exec = var.event + (sizeof(Events) << 8))) {
        if (var.exec < (var.dcon = var.exec + (sizeof(AFunction) << 8))) {
          if (var.dcon < (uint8_t*)(var.dbuf = (char*)var.dcon + sizeof(Canalysis))) {
            if (var.dbuf < (char*)(var.data = (uint8_t*)(var.dbuf + 14336 - (((sizeof(Events) + sizeof(AFunction)) << 8) + sizeof(Canalysis))))) {
              if (var.data < (var.info = var.data + (var.off << 2))) {
                if (var.info < (var.ds = var.info + var.off)) {
                  if (var.ds < (uint8_t*)(var.offset = (ugoc*)(var.ds + var.off))) {
                    if ((uint8_t*)var.offset < (var.dwin = (uint8_t*)(var.offset + var.off))) {
                      if (var.dwin < (uint8_t*)(var.end = (char*)(var.dwin + w * sizeof(Windows)))) return (Cell)(var.end - a); } } } } } } } } } } return Off; }
Cell InitVram(uint8_t c, uint16_t w, uint16_t how, uint16_t hz, uint16_t apm) { Base.PCell = sizeof(AFunction); Base.Goc = sizeof(goc);
  var.R = (Base.Goc < 8) ? (Base.Goc << 3) : 60; c = (c < 3) ? 3 : (c > var.R) ? var.R : c; w = (w > On) ? w : 2; if ((var.R = Base.Goc >> 1) > 2) var.R--;
  if (!VRam.size || c != Base.CellP || w != Base.Win) { var.addr = VRam.addr; var.size = VRam.size; MemCpy(var.Save, &var.dpal, sizeof(var.Save));
    var.G = 4; if (!(VRam.size = HowSize(c,w,Off))) { var.G--; } if (var.G == 4 && !(VRam.addr = GetRam(&VRam.size))) { var.G = 2; } 
    if (var.G == 4 && !(VRam.size = HowSize(c,w,VRam.addr))) { var.G = 1; } if (var.G < 4) { MemCpy(&var.dpal, var.Save, sizeof(var.Save)); VRam.addr = var.addr;
    VRam.size = var.size; return var.G; } if (var.size) { FreeRam(var.addr,var.size); } SetSeparator(Base.Sep); var.off = 1 << c;
    Base.CellP = c; Base.Win = w; Base.UGmax = (ugoc)(-1); Base.Gmax = Base.UGmax >> 1; Base.Ginf = ~Base.Gmax; Base.Gmin = Base.Ginf + 1; Base.Mcol = var.off - 1;
    Base.Mstr = ((var.off + (var.off << 3)) >> 5) - 1; Base.D = c + 2; Base.DS = c; Base.O = c + var.R; Base.P = 5; Base.K = 3; Base.V = 2; Base.Count = 6;
    var.Syn = Base.Hz - var.Syn; Base.Hz = (hz < 1) ? 1 : (hz > 10000) ? 10000 : hz; var.Syn = Base.Hz - var.Syn; Base.Apm = (apm < 51) ? 50 : ((apm > 1000) ? 1000 : apm);
    Base.Spd1 = Base.Mcol >> 4; Base.Spd0 = Base.Spd1 >> 2; Base.Speed = Base.Spd1; Base.On = (how > Base.Hz) ? Base.Hz : how; Base.FTime = (Base.On) ? Base.On : 1;
    var.Loop = (Base.Apm * Base.Hz) / Base.FTime; Convas.D = Off; Convas.S = Base.Win; Convas.CW = Base.Mcol; Convas.W = Off;
    Convas.CH = Base.Mstr; Convas.H = Off; Convas.Min = Off; Convas.Max = Base.Win; Convas.Win = Base.Win + 1; VP.Win = Base.Win - 1; VP.Mode = b2;
    VP.Loop = On; Vector(ECD) = Encode; Vector(RPE) = RPEncode; } ColourInit(Base.Colours, Base.Deep); return Off; }
Cell SystemSwitch(void) {
  if (VRam.SystemSwitch) { VRam.SystemSwitch--; SwitchRaw(); if (InitVram(Base.CellP, Base.Win, Base.On, Base.Hz, Base.Apm)) { return Off; }
    var.I = Base.Inc + 1; var.F = Base.Fone + 1; var.A = Base.Attr; var.off = Real(Off); (void)var.off; IRnd(); SWD(); SyncSize();
    Print(Base.Inc , var.A, "\033[?1049;7;1000h"); Print(Base.Fone , var.A, "\033[?25l"); SetBorder(Off, Base.Border); }
  else { VRam.SystemSwitch++; SwitchRaw(); Print(var.I, var.A, "\033[?1049;1000l\033[0m\033[?25h"); if (VRam.size) FreeRam(VRam.addr,VRam.size); } return On; }

void MoveNorm(dgoc x, dgoc y) { static uint8_t Wait = 7; Wait = (Wait) ? Wait : 7;
  if (VP.Mode & b1 && Convas.Win < Base.Win) {
    if (Convas.Win < Convas.D || Convas.Win >= Convas.S) { Windows* w = Win(Convas.Win);
      dgoc c = (w->W > w->MaxCs) ? w->W : w->MaxCs, s = (w->H > w->MaxH) ? w->H : w->MaxH;
      if (w->Xr && w->Yr && c && s) {
        var.Xr = (x < w->Xr) ? ((--Wait) ? w->Xr : (w->Xr + c - 1)) : (x >= (w->Xr + c)) ? ((--Wait) ? (w->Xr + c - 1) : w->Xr) : x;
        var.Yr = (y < w->Yr) ? ((--Wait) ? w->Yr : (w->Yr + s - 1)) : (y >= (w->Yr + s)) ? ((--Wait) ? (w->Yr + s - 1) : w->Yr) : y; return; } } }
  var.Xr = Base.Mcol; var.Yr = Base.Mstr;
  var.Xr = (x < -var.Xr) ? ((--Wait) ? -var.Xr : ((var.Xr << 1) + 1)) : (x > (var.Xr << 1)) ? ((--Wait) ? ((var.Xr << 1) + 1) : -var.Xr) : x;
  var.Yr = (y < -var.Yr) ? ((--Wait) ? -var.Yr : ((var.Yr << 1) + 1)) : (y > (var.Yr << 1)) ? ((--Wait) ? ((var.Yr << 1) + 1) : -var.Yr) : y; }
void MoveConvas(dgoc dx, dgoc dy) { Buf.Ctrl = On; MoveNorm(VP.X + dx, VP.Y + dy);
  dx = VP.X / (goc)TS.c; dy = VP.Y / (goc)TS.r; VP.X = var.Xr; VP.Y = var.Yr; VP.Xs = (VP.X < Off) ? (TS.c + (VP.X % (goc)TS.c)) : (VP.X % (goc)TS.c);
  VP.Ys = (VP.Y < Off) ? (TS.r + (VP.Y % (goc)TS.r)) : (VP.Y % (goc)TS.r); if ((VP.X / (goc)TS.c) != dx || (VP.Y / (goc)TS.r) != dy) Buf.Ctrl++; }
uint8_t MoveScreen(dgoc mx, dgoc my) {
  if ((((VP.X - mx) ^ VP.X) & Base.Ginf) || (((VP.Y - my) ^ VP.Y) & Base.Ginf)) return Off;
  MoveNorm(VP.X - mx, VP.Y - my); VP.X = var.Xr; VP.Y = var.Yr; VP.Xs -= mx; VP.Ys -= my; return On; }

void Free(void) { dgoc dx = Off, dy = Off; uint8_t i, *n; Buf.Ctrl = Off; Vector(RPE)();
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
        if (Vector(Buf.Cod)) { Buf.Ctrl++; VP.Wec = Event(Buf.Cod)->W; Vector(Buf.Cod)(); }
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
    if ((var.Dis += (var.Syn / var.Loop)) >= Base.FTime) { ASu(var.Dis / Base.FTime); Time(); var.Dis %= Base.FTime;  }
    if (Base.On && Vector(Timer)) { VP.Wec = Event(Timer)->W; Vector(Timer)(); }
    var.Syn %= var.Loop; }
  if (Vector(Off)) { VP.Wec = Event(Off)->W; Vector(Off)(); }
  if (SyncSize() || Buf.Ctrl > On) { SetBorder(Base.On, Base.Border); } else {  } }

void Nop(void) { }
void Encode(void) { UTFinfo(Buf.Key); }
void RPEncode(void) { GetKey(Buf.Key); Vector(ECD)(); }
void Anchor(void) { if (VP.Mode ^= b1) { } else { } }
void SwitchCur(void) { VP.Mode ^= b0; }
void Bye(void) { VP.Loop = Off; }

void WSwitch(void) { if (Win(VP.Wec)->Xr) Win(VP.Wec)->F ^= b1; }
void WASwitch(void) { if (Win(VP.Wec)->F ^= b1) WView(VP.Wec); }
void WDown(void) { if (Convas.D) { uint16_t l = Convas.D; Win(--l)->Layer = Off; while(l) ++Win(--l)->Layer; } }
void WUp(void) { if (Convas.D) { uint16_t l = Convas.D; Win(Off)->Layer = --l; while(l) --Win(--l)->Layer; } }
void WTop(uint16_t n) { uint16_t l = Convas.D; if ((n >= Convas.D && n < Convas.S) || n >= Base.Win) return;
  if (n > l) { l = Convas.Max; } Win(n)->Layer = l; l -= n; while(l--) --Win(n + l)->Layer; }

void _WView(uint16_t n, uint8_t c, dgoc *a) { dgoc x = Off, y = Off;
  if ((n >= Convas.D && n < Convas.S) || n >= Base.Win) { return; } Windows* w = Win(n);
  if (c > On) { x = a[Off]; if (!(y = a[On])) x = Off;
    if (x && !(w->F & b0)) { x = (x < Off) ? -x : x; y = (y < Off) ? -y : y;
      if ((udgoc)VP.X > (udgoc)Base.Mcol || (udgoc)VP.Y > (udgoc)Base.Mstr) return; } }
  if (c == Off) { x = VP.Xs + On; y = VP.Ys + On;
    if (w->F & b0) { if (TS.c < (x + w->W)) { x = -On; } if (TS.r < (y + w->H)) y = -On; } 
    else if ((udgoc)VP.X > (udgoc)Base.Mcol || (udgoc)VP.Y > (udgoc)Base.Mstr) return; }
  w->Xr = x; w->Yr = y; if (w->Xr) { w->F |= b1; } else { w->F &= ~b1; } }
uint16_t _Window(uint8_t t, int8_t col, uint8_t c, udgoc *a) { uint16_t l, n; Windows* w;
  if (t) { if ((n = --Convas.S) < On) { Convas.S = Convas.Max; n = --Convas.S; }
    w = Win(n); w->F = 9; w->Layer = Convas.Max; l = Convas.Max - n; while(l--) --Win(n + l)->Layer; }
  else { if ((n = Convas.D++) > Convas.S) { Convas.D = Convas.Min; n = Convas.D++; } w = Win(n); w->F = 8; w->Layer = n; }
  w->parent = n; w->child = n; w->MaxVs = Off; w->XCur = Off; w->YCur = Off; w->WFirstSR = Base.Mstr; w->Xr = Off; w->Yr = Off; w->W = Off; w->H = Off;
  if (c > On) { if ((w->Xr = a[0])) { if (!(w->Yr = a[1])) w->Xr = Off; } } if (c > 2) { w->W = a[2]; if (c > 3) w->H = a[3]; }
  w->col = col; if (w->F == 9) { if (w->W < b1) { w->W = b1; } if (!w->H) { w->H++; } } if (w->Xr) { w->F |= b1; } return n; }
void _WExecs(uint16_t n, uint8_t cur, uint8_t c, AFunction *a) { if ((n >= Convas.D && n < Convas.S) || n >= Base.Win) return;
  if ((Win(n)->F & b0) && c--) { Event(cur)->W = n; Exe(cur, a[Off]);
    if (Event(cur)->C && c) { uint8_t j, k = Event(cur)->C, i = On; while(k-- && c--) { j = K_Mouse;
      while(--j) { if (Event(j)->W == n && Event(j)->N == i) { Exe(j, a[i]); i++; break; } } } } } }
void _WSet(uint16_t n, uint8_t c, uint8_t *a) { if ((n >= Convas.D && n < Convas.S) || n >= Base.Win) return;
  if (c--) { Windows* w = Win(n); w->F &= ~b2; if (a[Off]) { w->F |= b2; } if (c) { w->F &= ~b3; if (a[On]) { w->F |= b3; } } } }
void _SEvents(uint8_t c, uint8_t *a) { uint8_t m, k = c; while(k--) { m = a[k]; Event(m)->W = Base.Win; Event(m)->C = c; Event(m)->N = k; } }
void _SExec(uint8_t c, AFunction *a) { uint8_t k = K_Mouse; while(k--) { if (Event(k)->W == Base.Win) {
  if (Event(k)->C && c) { uint8_t j, x = Event(k)->C, i = Off;
    while(x-- && c--) { j = ECD; while(--j) { if (Event(j)->W == Base.Win && Event(j)->N == i) { Exe(j, a[i]); i++; break; } } } } break; } } }
void _SKeys(uint8_t c, uint8_t *a) { uint8_t *p = &VP.Key, i = Off; p += *p; c = (c > VP.Key) ? VP.Key : c; while(c--) *p-- = a[i++]; }
void _FSet(uint8_t cp, uint8_t c, uint16_t *a) { uint16_t w = Base.Win, o = Base.On, h = Base.Hz, f = Base.Apm;
  uint8_t b = (var.dpal == (uint8_t*)VRam.addr) ? 0 : 1; if (c--) { w = a[Off]; if (c--) { o = a[On]; if (c--) { h = a[2]; if (c) { f = a[3]; } } } }
  if ((var.off = InitVram(cp, w, o, h, f))) { Bye(); } SetPalette(b); }
void _CSet(uint8_t c, uint8_t *a) { uint8_t l = Base.Colours, d = Base.Deep, b = (var.dpal == (uint8_t*)VRam.addr) ? 0 : 1;
  if (c--) { l = a[Off]; if (c) { d = a[On]; } } ColourInit(l, d); SetPalette(b); }
void _WData(uint16_t n, char *str, uint8_t count, udgoc *args) { if ((n >= Convas.D && n < Convas.S) || n >= Base.Win) return;
  Windows* w = Win(n); if (!(w->MaxVs)) {  
     }
  (void)*str; (void)count; (void)*args; }
