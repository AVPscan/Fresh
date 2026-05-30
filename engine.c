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

void UTFinfoTile(uint8_t *s, Cell len) { Buf.Data = 0xC0; if (!len) return;
  if ((*s & 0xE0) == 0xC0 && len < 0x02) return;
  else if ((*s & 0xF0) == 0xE0 && len < 0x03) return;
  else if ((*s & 0xF8) == 0xF0 && len < 0x04) return;
  UTFinfo(s); }
void UTFinfo(uint8_t *s) { cY = *s++; Buf.Data = Off; if (cY < 0x80) cRGB = (uint32_t)cY;
  else if ((cY & 0xE0) == 0xC0 && (*s & 0xC0) == 0x80) { Buf.Data++; cRGB = ((cY & 0x1F) << 0x06) | (*s & 0x3F); }
  else if ((cY & 0xF0) == 0xE0 && (*s & 0xC0) == 0x80 && (*(s + 0x01) & 0xC0) == 0x80)
    { Buf.Data = 0x02; cRGB = ((cY & 0x0F) << 0x0C) | ((*s & 0x3F) << 0x06) | (*(s + 0x01) & 0x3F); }
  else if ((cY & 0xF8) == 0xF0 && (*s & 0xC0) == 0x80 && (*(s + 0x01) & 0xC0) == 0x80 && (*(s + 0x02) & 0xC0) == 0x80) 
    { Buf.Data = 0x03; cRGB = ((cY & 0x07) << 0x12) | ((*s & 0x3F) << 0x0C) | ((*(s + 0x01) & 0x3F) << 0x06) | (*(s + 0x02) & 0x3F); }
  else { Buf.Data |= b7; return; }
  if (cRGB < 0x20 || (cRGB >= 0x7F && cRGB < 0xA0)) { Buf.Data |= b5; return; }
  if (cRGB < 0x100) { Buf.Data |= b2; return; }
  if (cRGB >= 0x0590 && cRGB <= 0x08FF) Buf.Data |= b4;
  if (((Buf.Data & 0x03) == 0x01 && cRGB < 0x80) || ((Buf.Data & 0x03) == 0x02 && (cRGB < 0x800 || (cRGB >= 0xD800 && cRGB <= 0xDFFF))) || 
      ((Buf.Data & 0x03) == 0x03 && (cRGB < 0x10000 || cRGB > 0x10FFFF))) { Buf.Data |= b7; return; }
  if ((cRGB >= 0x0300 && cRGB <= 0x036F) || (cRGB >= 0x1DC0 && cRGB <= 0x1DFF) || (cRGB >= 0x20D0 && cRGB <= 0x20FF) ||
      (cRGB == 0x200D || (cRGB >= 0xFE00 && cRGB <= 0xFE0F))) { Buf.Data &= 0xF3; return; }
  if (cRGB == 0x200B || cRGB == 0x200C || cRGB == 0x200E || cRGB == 0x200F || (cRGB >= 0xFE20 && cRGB <= 0xFE2F) ||
      (cRGB >= 0xE0100 && cRGB <= 0xE01EF)) { Buf.Data &= 0xF3; return; }
  if ((cRGB >= 0x1100 && cRGB <= 0x115F) || (cRGB == 0x2329 || cRGB == 0x232A) || (cRGB >= 0x2E80 && cRGB <= 0xA4CF && cRGB != 0x303F) || 
      (cRGB >= 0xAC00 && cRGB <= 0xD7A3) || (cRGB >= 0xF900 && cRGB <= 0xFAFF) || (cRGB >= 0xFE10 && cRGB <= 0xFE19) || 
      (cRGB >= 0xFE30 && cRGB <= 0xFE6F) || (cRGB >= 0xFF00 && cRGB <= 0xFF60) || (cRGB >= 0xFFE0 && cRGB <= 0xFFE6) || 
      (cRGB >= 0x20000 && cRGB <= 0x2FFFD) || (cRGB >= 0x30000 && cRGB <= 0x3FFFD) || (cRGB >= 0x1F300)) { Buf.Data |= 0x08; return; }
  Buf.Data |= b2; }
  
void PushKey(void) { uint8_t l, d, i = Off; KeyBuf* k;
  if (Buf.Data & b7) { Buf.Cod = Off; return; }
  if (Buf.pop == Buf.push) {
    k = AKey(++Buf.push); k->d[i] = Buf.Data | b7; k->d[i + On] = On; l = On + (Buf.Data & b10);
    if (Buf.Cod) k->u[i] = Buf.Cod;
    else while(l--) k->u[l] = *(Buf.Key + l); }
  else {
    k = AKey(Buf.push); d = k->d[i]; l = On + (Buf.Data & b10);
    if (d & b6) { i = b1; d = k->d[i]; }
    if ((d & ~b76) == Buf.Data) {
      if (Buf.Cod) { if (k->u[i] == Buf.Cod) l = -On; }
      else while(l--) { if (k->u[i + l] != *(Buf.Key + l)) break; } }
    if (!++l) { if (!++k->d[i + On]) --k->d[i + On]; }
    else {
      if (!i && ((Buf.Data & b10) < b1)) { k->d[i] |= b6; i = b1; }
      else { k = AKey(++Buf.push); i = Off; if (Buf.pop == Buf.push) ++Buf.pop; }
      k->d[i] = Buf.Data | b7; k->d[i + On] = On; l = On + (Buf.Data & b10);
      if (Buf.Cod) k->u[i] = Buf.Cod;
      else while(l--) k->u[i + l] = *(Buf.Key + l); } }
  if (!Buf.Cod) Buf.Cod--; }
uint8_t ShowKey(void) { uint8_t d, i = Off; KeyBuf* k = AKey(Buf.push);
  if (Buf.pop == Buf.push) { Buf.Data = Off; Buf.Count = Off; return Off; }
  if (k->d[Off] & b6) { i = b1; } Buf.Count = k->d[i + On]; Buf.Data = k->d[i] & ~b76;
  d = On + (k->d[i] & b10); while(d--) { *(Buf.Key + d) = k->u[i + d]; } return On; }
uint8_t PopKey(void) { uint8_t d, i = Off;
  while(!(AKey(Buf.pop)->d[Off] & b76) && (Buf.pop != Buf.push)) Buf.pop++;
  if (Buf.pop == Buf.push) { Buf.Data = Off; Buf.Count = Off; return Off; } KeyBuf* k = AKey(Buf.pop);
  if (k->d[Off] & b7) { k->d[Off] &= ~b7; } else { k->d[Off] &= ~b6; i = b1; } Buf.Count = k->d[i + On]; Buf.Data = k->d[i] & ~b76;
  d = On + (k->d[i] & b10); while(d--) { *(Buf.Key + d) = k->u[i + d]; } return On; }
ugoc Keys(void) { ugoc s = Off; uint8_t d, c = Buf.push; while(c != Buf.pop) { d = AKey(c--)->d[Off]; if (d & b7) s++; if (d & b6) s++; } return s; }

void IRnd(void) { Sys.Rnd = GetDelay() | On; }
ugoc Rand(ugoc n) { return (ugoc)(((Cell)(Sys.Rnd = (ugoc)(Sys.A * Sys.Rnd + Sys.B)) * n) >> (sizeof(ugoc) * 8)); }
int8_t Fcos(int16_t u) { return Fsin(u + 128); }
int8_t Fsin(int16_t u) { static int8_t s[64] = { 0,1,2,3,4,6,7,8,9,11,12,13,14,15,17,18,19,20,21,23,24,25,26,27,28,30,31,
  32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,51,52,53,54,55,55,56,57,58,58,59,60,60,61,62,62,63 }; int8_t r = u & 63;
  r = (u & b6) ? 64 + s[r] : s[r]; r = (u & b7) ? 127 - r : r; return ((u & b8) ? -r : r); }
int8_t Ftg(int16_t u) { return (Fcos(u) ? (Fsin(u) / Fcos(u)) : -128); }
int8_t Fctg(int16_t u) { return (Fsin(u) ? (Fcos(u) / Fsin(u)) : -128); }

void Print(uint8_t n, uint8_t m, char *str) { char *src, *dst = Cdbuf; PalBuf* pal = APal(n);
  static char *s = "24;21;22;1;\00022;2;\00023;3;\00024;4;\00027;7;\00029;9;\000"; if (!str) return;
  if ((cA ^= m)) { cX = 64; cY = 36; *dst++ = '\33'; *dst++ = '['; while(cX) { if ((cA & cX)) { src = s + cY + ((m & cX) ? 3 : 0);
      *dst++ = *src++; *dst++ = *src++; if (*src) *dst++ = *src++; } cX >>= 1; cY -= 6; } cA = 2; }
  if (n & b7) { if (n == cF) { if (cA) *(dst - 1) = 'm'; } else { cF = n; MemCpy(dst, pal->d + cA, pal->l - cA); dst += pal->l - cA; } }
  else { if (n == cI) { if (cA) *(dst - 1) = 'm'; } else { cI = n; MemCpy(dst, pal->d + cA, pal->l - cA); dst += pal->l - cA; } }
  ugoc len = StrLen(str); MemCpy(dst, str, len); cA = m; SysWrite(Cdbuf, dst + len - Cdbuf); }
void GenFonCol(uint8_t c, uint8_t deep) {
  uint8_t i, j = 1, k = (deep > b3) ? 2 : (deep < b3) ? 0 : 1; c = (c > aColours) ? aColours : c;
  char *d[3] = { "\2\33[", "\6\33[38;5", "\6\33[38;2" }; PalBuf *mode = (PalBuf*)d[k], *pal = APal(c), *src = APal(c + aColours + 1);
  if (k == 1 ) { cR = (16 + 36 * ((cR * 5 + 128) / 255) + 6 * ((cG * 5 + 128)/ 255) + ((cB * 5 + 128)/ 255)); }
  else  { if (!k) { cR = (((cR * 299 + cG * 587 + cB * 114) > 127999) ? 90 : 30) + (((cR > 127) << 2) | ((cG > 127) << 1) | (cB > 127)); }
          else { j = 3; } } src->l = mode->l; MemCpy(src->d, mode->d, mode->l); cRGB = cR | (cG << 8) | (cB << 16);
  while(j--) { i = (uint8_t)cRGB; cRGB >>= b3; if (k) { src->d[src->l++] = ';'; } if ((cY = (i * 41) >> 12)) { src->d[src->l++] = 0x30 + cY; i -= cY * 100; }
    if (cY || ((i * 205) >> 11)) { src->d[src->l++] = 0x30 + (cY = (i * 205) >> 11); i -= cY * 10; } src->d[src->l++] = 0x30 + i; }
  src->d[src->l++] = 'm'; pal->l = src->l; MemCpy(pal->d, src->d, src->l); src->d[2] = '4'; if (pal->d[2] == '9') {
    src->l++; src->d[2] = '1'; src->d[5] = src->d[4]; src->d[4] = src->d[3]; src->d[3] = '0'; } }
void SetBorder(void) { cY = cF; Print(Sys.Border, cA, Cls); cF = cY; }
void SetPalette(uint8_t set) { Cdpal = (char*)(Coffset + SOffset); if (set) { Cdpal += SPal / 2; } }
void SwitchPalette(void) { char* a = (char*)(Coffset + SOffset); if (a == Cdpal) { a += SPal / 2; } Cdpal = a; }
void GenPalette(uint8_t set) { SetPalette(set); cX = On + Sys.Colours; cXYz = 1 << 24;
  while(cX--) { if (cX == Sys.Colours) { cR = 255; cG = cR; cB = cR; } else if (!cX) { cR = 0; cG = 0; cB = 0; } 
    else { if (set) { cRGB = ((cXYz * cX) / Sys.Colours) - On; cG = (uint8_t)cRGB; cRGB >>= 8; cB = (uint8_t)cRGB; cRGB >>= 8; cR = (uint8_t)cRGB; } 
           else { cZ = cU + (cX * 512) / Sys.Colours; cB = 128 + Fsin(cZ); cG = 128 + Fsin(cZ + 171); cR = 128 + Fsin(cZ + 342); } }
    GenFonCol(cX, Sys.Deep); } }
void SysInit(uint16_t hz, uint16_t fps, uint8_t deep, uint8_t col) {
  Cinfo = (uint8_t*)(Cdata + SDCell); Cds = Cinfo + SInfo; Coffset = (ugoc*)(Cds + SDs); Cdpal = (char*)(Coffset + SOffset);
  Cdkey = (uint8_t*)(Cdpal + SPal); Cdsys = (ugoc*)(Cdkey + SKeys); Cdcon = Cdsys + SSys; Cdwin = Cdcon + SConvas;
  Cvsw = Cdwin + SWin; Ccsw = Cvsw + SVsw; Cevent = (char*)(Ccsw + SCsw); Cexec = Cevent + SEvent; Cdbuf = Cexec + SExec;
  Sys.Spd1 = MaxSpeed; Sys.Spd0 = b3; Sys.Speed = Sys.Spd1; Sys.CellP = CellPow; Sys.MWin = MAX_WIN; Sys.Ginf = GOC_INF;
  Sys.MT = 6; Sys.Gmax = GOC_MAX; Sys.Gmin = GOC_MIN; IRnd(); Sys.A = RNG_A; Sys.B = RNG_B; VP.Win = Sys.MWin; VP.Mode = b2; VP.Loop = On; 
  Vector(K_Mouse) = RPEncode; Sys.Fps = (fps < 51) ? 50 : ((fps > 1000) ? 1000 : fps); Sys.Delay = 1000 / Sys.Fps;
  Sys.Hz = hz ? ((hz > 10000) ? 10000 : hz) : 500; deep = (deep < b3) ? 3 : (deep > b3) ? 24 : 8;
  col = (col < b0) ? b0 : (col > aColours) ? aColours : col; hz = Sys.MT; while(hz--) { Sys.Time[hz] = Off; }
  if (col != Sys.Colours || deep != Sys.Deep) { Sys.Colours = col; Sys.Deep = deep; cU = Off; GenPalette(On); GenPalette(Off); }
  Sys.Fone = Sys.Colours + aColours + 1; Sys.Border = Fdark; Sys.Inc = dark; cI = Sys.Border; cA = Off; }
void InitVram(Cell addr, Cell size) { if (!addr || (size < SizeVram)) return;
  Cdata = (char*)addr; SysInit(FHz, FFps, CFDeep, Fcolour); Sys.Syn = Off; Convas.D = Off; Convas.S = Sys.MWin; Convas.CW = CellLine;
  Convas.W = Convas.CW - On; Convas.CH = CellStr; Convas.H = Convas.CH - On; Convas.Min = Off; Convas.Max = Sys.MWin; Convas.Win = Sys.MWin; }
Cell SystemSwitch(void) {
  if (VRam.SystemSwitch) { VRam.size = SizeVram; if (!(VRam.addr = GetRam(&VRam.size))) return Off;
    VRam.SystemSwitch--; SWD(VRam.addr); Delay(Off); InitVram(VRam.addr,VRam.size); SwitchRaw(); SyncSize(VRam.addr);
    Print(Sys.Fone, cA, AltBufOn HideCur WrapOn MouseX10on); Print(Sys.Border, cA, HideCur Cls); }
  else { VRam.SystemSwitch++; if (VRam.size) { SwitchRaw(); Print(Sys.Fone, cA, AltBufOff Reset ShowCur WrapOn MouseX10off);
    FreeRam(VRam.addr, VRam.size); } } return On; }

void MoveConvas(goc dx, goc dy) { Buf.Ctrl = On; ugoc r, c = TermCR(&r); goc x = VP.X + dx, y = VP.Y + dy;
  if (VP.Mode & b1) { return; }
  else {
    x = ((VP.X < -Sys.Speed || VP.X > Sys.Speed) && ((x ^ VP.X) & Sys.Ginf)) ? ((x < Off) ? Sys.Gmax : Sys.Gmin) : ((x == Sys.Ginf) ? Sys.Gmin : x);
    y = ((VP.Y < -Sys.Speed || VP.Y > Sys.Speed) && ((y ^ VP.Y) & Sys.Ginf)) ? ((y < Off) ? Sys.Gmax : Sys.Gmin) : ((y == Sys.Ginf) ? Sys.Gmin : y); }
  dx = VP.X / c; dy = VP.Y / r; VP.X = x; VP.Y = y; VP.Xs = (VP.X < Off) ? (c + (VP.X % c)) : (VP.X % c);
  VP.Ys = (VP.Y < Off) ? (r + (VP.Y % r)) : (VP.Y % r); if ((x / c) != dx || (y / r) != dy) Buf.Ctrl++; }
uint8_t MoveScreen(goc mx, goc my) { goc dx = VP.X - mx, dy = VP.Y - my;
  if (VP.Mode & b1) { return Off; }
  else if (((dx ^ VP.X) & Sys.Ginf) || ((dy ^ VP.Y) & Sys.Ginf)) return Off;
  VP.X = dx; VP.Y = dy; VP.Xs -= mx; VP.Ys -= my; return On; }
  
void Fresh(void) { goc dx, dy; uint8_t i, *n;
  while(VP.Loop) { Buf.Ctrl = Off; Vector(K_Mouse)();
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
      if (Buf.Cod == VP.scs) { VP.dXY = On; Sys.Speed = (VP.Mode ^= b2) ? Sys.Spd1 : Sys.Spd0; }
      else if (Buf.Cod == VP.Anchor) { if (VP.Mode ^= b1) { } else { Convas.W = Convas.CW; Convas.H = Convas.CH; } }
      else if (Buf.Cod == VP.bcu) { SwitchPalette(); SetBorder(); }
      else if (Buf.Cod == VP.ssc) VP.Mode ^= b0;
      else if (Buf.Cod == VP.Exit) VP.Loop = Off;
      else { dx = Off, dy = Off;
        if (Buf.Cod != VP.Cod) { VP.dXY = On; VP.Cod = Buf.Cod; }
        if ((Buf.tic > 7) && !(Buf.tic & b10) && (VP.dXY < Sys.Speed)) VP.dXY <<= On;
        if (Buf.Cod == VP.le) dx = -VP.dXY;
        else if (Buf.Cod == VP.ri) dx = VP.dXY;
        else if (Buf.Cod == VP.up) dy = -VP.dXY;
        else if (Buf.Cod == VP.ud) dy = VP.dXY;
        MoveConvas(dx, dy); } }
    if (SyncSize(VRam.addr) || Buf.Ctrl > On) { SetBorder(); } else {  }
    if (Vector(Off)) { VP.Wec = Event(Off)->W; Vector(Off)(); }
    Delay(Sys.Delay); Sys.Syn += Sys.Hz; while(Sys.Syn >= Sys.Fps) { Sys.Syn -= Sys.Fps; i = Sys.MT; while(--i && (!++Sys.Time[i])) { } } } }

void RPEncode(void) { GetKey(Buf.Key); UTFinfo(Buf.Key); }
void Nop(void) { }
void Anchor(void) { if (VP.Mode ^= b1) { } else { Convas.W = Convas.CW; Convas.H = Convas.CH; } }
void Bye(void) { VP.Loop = Off; }
void WSwitch(void) { if (Win(VP.Wec)->Xr) Win(VP.Wec)->EF ^= b1; }
void WASwitch(void) { if (Win(VP.Wec)->EF ^= b1) WinView(VP.Wec); }
void WinDown(void) { if (Convas.D) { uint16_t l = Convas.D; Win(--l)->Layer = Off; while(l) ++Win(--l)->Layer; } }
void WinUp(void) { if (Convas.D) { uint16_t l = Convas.D; Win(Off)->Layer = --l; while(l) --Win(--l)->Layer; } }

void WinTop(uint16_t n) { uint16_t l = Convas.D; if ((n >= Convas.D && n < Convas.S) || n >= Convas.Win) return;
  if (n > l) { l = Convas.Max; } Win(n)->Layer = l; l += On - n; while(--l) --Win(n + l)->Layer; }
void _WView(uint16_t n, uint8_t count, goc *args) { goc x = Off, y = Off; Windows* w = Win(n);
  if ((n >= Convas.D && n < Convas.S) || n >= Convas.Win) return;
  if (count > On) { x = args[Off]; if (!(y = args[On])) x = Off;
    if (x && !(w->EF & b0)) { x = (x < Off) ? -x : x; y = (y < Off) ? -y : y;
      if ((ugoc)VP.X > Convas.W || (ugoc)VP.Y > Convas.H) return; } }
  if (count == Off) { x = VP.Xs + On; y = VP.Ys + On;
    if (w->EF & b0) { ugoc r, c = TermCR(&r); if (c < (x + w->W)) { x = -On; } if (r < (y + w->H)) y = -On; } 
    else if ((ugoc)VP.X > Convas.W || (ugoc)VP.Y > Convas.H) return; }
  w->Xr = x; w->Yr = y; if (w->Xr) { w->EF |= b1; } else { w->EF &= ~b1; } }
uint16_t _Window(uint8_t t, int8_t col, uint8_t count, ugoc *args) { uint16_t l, n; Windows* w;
  if (t) { n = --Convas.S; if (n < On) { Convas.S = Convas.Max; n = --Convas.S; }
    w = Win(n); w->EF = 9; w->Layer = Convas.Max; l = On + Convas.Max - n; while(--l) --Win(n + l)->Layer; }
  else { n = Convas.D++; if (n >= Convas.S) { Convas.D = Convas.Min; n = Convas.D++; } w = Win(n); w->EF = 8; w->Layer = n; }
  w->parent = n; w->child = n; w->MaxVs = Off; w->XCur = Off; w->YCur = Off; w->WFirstSR = CellStr; w->Xr = Off; w->Yr = Off; w->W = Off; w->H = Off;
  if (count > On) { w->Xr = args[0]; if (!(w->Yr = args[1])) w->Xr = Off; } if (count > 2) { w->W = args[2]; if (count > 3) w->H = args[3]; }
  w->palette = col; if (w->EF == 9) { if (w->W < b1) { w->W = b1; } if (!w->H) { w->H++; } } if (w->Xr) { w->EF |= b1; } return n; }
void _WExec(uint16_t n, uint8_t cur, uint8_t count, AFunction *args) { if ((n >= Convas.D && n < Convas.S) || n >= Convas.Win) return;
  if ((Win(n)->EF & b0) && count--) { Event(cur)->W = n; Vector(cur) = ((Cell)args[Off] <= (Cell)Nop) ? Off : args[Off];
    if (Event(cur)->C && count) { uint8_t j, c = Event(cur)->C, i = On; while(c-- && count--) { j = K_Mouse;
        while(--j) { if (Event(j)->W == n && Event(j)->N == i) { Vector(j) = ((Cell)args[i] <= (Cell)Nop) ? Off : args[i]; i++; break; } } } } } }
void _WSet(uint16_t n, uint8_t count, uint8_t *args) { if ((n >= Convas.D && n < Convas.S) || n >= Convas.Win) return;
  if (count--) { Windows* w = Win(n); w->EF &= ~b2; if (args[Off]) { w->EF |= b2; } if (count) { w->EF &= ~b3; if (args[On]) { w->EF |= b3; } } } }
void _SEvent(uint8_t count, uint8_t *args) { uint8_t a, k = count; while(k--) { a = args[k]; Event(a)->W = Convas.Win; Event(a)->C = count; Event(a)->N = k; } }
void _SExec(uint8_t count, AFunction *args) { uint8_t k = K_Mouse; Convas.Min = Convas.D; Convas.Max = Convas.S;
  while(k--) { if (Event(k)->W == Convas.Win) {
    if (Event(k)->C && count) { uint8_t j, c = Event(k)->C, i = Off; while(c-- && count--) { j = K_Mouse;
        while(--j) { if (Event(j)->W == Convas.Win && Event(j)->N == i) { Vector(j) = ((Cell)args[i] <= (Cell)Nop) ? Off : args[i]; i++; break; } } } }
    break; } } }
void _SKeys(uint8_t count, uint8_t *args) { uint8_t *p = &VP.Key, i = Off; p += *p; if (count > VP.Key) { count = VP.Key; } while(count--) *p-- = args[i++]; }
void _SSet(uint16_t hz, uint16_t fps, uint8_t count, uint8_t *args) { uint8_t deep = Sys.Deep, col = Sys.Colours; char *a = Cdpal;
  if (count--) { deep = args[Off]; if (count) { col = args[On]; } } SysInit(hz, fps, deep, col); Cdpal = a; }
void _WData(uint16_t n, char *str, uint8_t count, ugoc *args) { if ((n >= Convas.D && n < Convas.S) || n >= Convas.Win) return;
  Windows* w = Win(n); if (!(w->MaxVs)) {  } (void)*str; (void)count; (void)*args; }
