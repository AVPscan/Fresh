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

void UTFinfoTile(uint8_t *s, Cell len) { Buf.Data = 0xC0;
  if (!len) return;
  if ((*s & 0xE0) == 0xC0 && len < 0x02) return;
  else if ((*s & 0xF0) == 0xE0 && len < 0x03) return;
  else if ((*s & 0xF8) == 0xF0 && len < 0x04) return;
  UTFinfo(s); }
void UTFinfo(uint8_t *s) { uint32_t cp; uint8_t c = *s++; Buf.Data = Off;
  if (c < 0x80) cp = (uint32_t) c;
  else if ((c & 0xE0) == 0xC0 && (*s & 0xC0) == 0x80) { Buf.Data++; cp = ((c & 0x1F) << 0x06) | (*s & 0x3F); }
  else if ((c & 0xF0) == 0xE0 && (*s & 0xC0) == 0x80 && (*(s + 0x01) & 0xC0) == 0x80)
    { Buf.Data = 0x02; cp = ((c & 0x0F) << 0x0C) | ((*s & 0x3F) << 0x06) | (*(s + 0x01) & 0x3F); }
  else if ((c & 0xF8) == 0xF0 && (*s & 0xC0) == 0x80 && (*(s + 0x01) & 0xC0) == 0x80 && (*(s + 0x02) & 0xC0) == 0x80) 
    { Buf.Data = 0x03; cp = ((c & 0x07) << 0x12) | ((*s & 0x3F) << 0x0C) | ((*(s + 0x01) & 0x3F) << 0x06) | (*(s + 0x02) & 0x3F); }
  else { Buf.Data |= b7; return; }
  if (cp < 0x20 || (cp >= 0x7F && cp < 0xA0)) { Buf.Data |= b5; return; }
  if (cp < 0x100) { Buf.Data |= b2; return; }
  if (cp >= 0x0590 && cp <= 0x08FF) Buf.Data |= b4;
  if (((Buf.Data & 0x03) == 0x01 && cp < 0x80) || ((Buf.Data & 0x03) == 0x02 && (cp < 0x800 || (cp >= 0xD800 && cp <= 0xDFFF))) || 
      ((Buf.Data & 0x03) == 0x03 && (cp < 0x10000 || cp > 0x10FFFF))) { Buf.Data |= b7; return; }
  if ((cp >= 0x0300 && cp <= 0x036F) || (cp >= 0x1DC0 && cp <= 0x1DFF) || (cp >= 0x20D0 && cp <= 0x20FF) ||
      (cp == 0x200D || (cp >= 0xFE00 && cp <= 0xFE0F))) { Buf.Data &= 0xF3; return; }
  if (cp == 0x200B || cp == 0x200C || cp == 0x200E || cp == 0x200F || (cp >= 0xFE20 && cp <= 0xFE2F) ||
      (cp >= 0xE0100 && cp <= 0xE01EF)) { Buf.Data &= 0xF3; return; }
  if ((cp >= 0x1100 && cp <= 0x115F) || (cp == 0x2329 || cp == 0x232A) || (cp >= 0x2E80 && cp <= 0xA4CF && cp != 0x303F) || 
      (cp >= 0xAC00 && cp <= 0xD7A3) || (cp >= 0xF900 && cp <= 0xFAFF) || (cp >= 0xFE10 && cp <= 0xFE19) || 
      (cp >= 0xFE30 && cp <= 0xFE6F) || (cp >= 0xFF00 && cp <= 0xFF60) || (cp >= 0xFFE0 && cp <= 0xFFE6) || 
      (cp >= 0x20000 && cp <= 0x2FFFD) || (cp >= 0x30000 && cp <= 0x3FFFD) || (cp >= 0x1F300)) { Buf.Data |= 0x08; return; }
  Buf.Data |= b2; }
  
void PushKey(void) { uint8_t *sav, *dst, d, l;
  if (Buf.Data & b7) { Buf.Cod = Off; return; }
  if (Buf.pop == Buf.push) { dst = AKey(++Buf.push);
    *dst++ = Buf.Data | b7; *dst = On; dst += b10; l = On + (Buf.Data & b10);
    if (Buf.Cod) *dst = Buf.Cod;
    else while(l--) *(dst + l) = *(Buf.Key + l); }
  else {
    dst = AKey(Buf.push); d = *dst++; sav = dst; dst += b10; l = On + (Buf.Data & b10);
    if (d & b6) { d = *++sav; sav++; dst += b1; }
    if ((d & ~b76) == Buf.Data) {
      if (Buf.Cod) { if (*dst == Buf.Cod) l = ~Off; }
      else while(l--) { if (*(dst + l) != *(Buf.Key + l)) break; } }
    if (l == (uint8_t)~Off) { if (!(*sav += On)) *sav = ~Off; }
    else { dst = AKey(Buf.push); d = *dst;
      if (!(d & b6) && (d & b10) < b1 && (Buf.Data & b10) < b1) { *dst |= b6; dst += 2; }
      else { dst = AKey(++Buf.push); if (Buf.pop == Buf.push) ++Buf.pop; }
      *dst++ = Buf.Data | b7; *dst = On; dst += b10; l = On + (Buf.Data & b10);
      if (Buf.Cod) *dst = Buf.Cod;
      else while(l--) *(dst + l) = *(Buf.Key + l); } } if (!Buf.Cod) Buf.Cod--; }
uint8_t ShowKey(void) { uint8_t d, *dst; dst = AKey(Buf.push);
  if (Buf.pop == Buf.push) { Buf.Data = Off; Buf.Count = Off; return Off; }
  d = *dst++; if (d & b6) { dst++; d = *dst++; } Buf.Count = *dst; Buf.Data = (d & ~b76);
  dst += b10; d = On + (d & b10); while(d--) { *(Buf.Key + d) = *(dst + d); } return On; }
uint8_t PopKey(void) { uint8_t *dst, d;
  while(!((d = *(dst = AKey(Buf.pop))) & b76) && (Buf.pop != Buf.push)) Buf.pop++;
  if (!(d & b76)) { Buf.Data = Off; Buf.Count = Off; return Off; } if (Buf.pop == Buf.push) Buf.pop--;
  if (d & b7) { *dst &= ~b7; } else { *dst &= ~b6; dst += b1; d = *dst; } Buf.Count = *++dst; Buf.Data = (d & ~b76);
  dst += b10; d = On + (d & b10); while(d--) { *(Buf.Key + d) = *(dst + d); } return On; }
ugoc Keys(void) { ugoc s = Off; uint8_t d, c = Buf.push;
  while(c != Buf.pop) { d = *AKey(c--); if (d & b7) s++; if (d & b6) s++; } return s; }

void IRnd(void) { VP.Rnd = GetDelay() | On; }
ugoc Rand(ugoc n) { return (ugoc)(((Cell)(VP.Rnd = (ugoc)(Sys.A * VP.Rnd + Sys.B)) * n) >> (sizeof(ugoc) * 8)); }
int8_t Fcos(int16_t u) { return Fsin(u + 128); }
int8_t Fsin(int16_t u) { static int8_t s[64] = { 0,1,2,3,4,6,7,8,9,11,12,13,14,15,17,18,19,20,21,23,24,25,26,27,28,30,31,
  32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,51,52,53,54,55,55,56,57,58,58,59,60,60,61,62,62,63 }; int8_t r = u & 63;
  r = (u & b6) ? 64 + s[r] : s[r]; r = (u & b7) ? 127 - r : r; return ((u & b8) ? -r : r); }
int8_t Ftg(int16_t u) { return (Fcos(u) ? (Fsin(u) / Fcos(u)) : -128); }
int8_t Fctg(int16_t u) { return (Fsin(u) ? (Fcos(u) / Fsin(u)) : -128); }

void Print(uint8_t n, char *str) { if (!str) return;
  char *dst = Cdbuf; ugoc len = StrLen(str); PalBuf* pal = APal(n); MemCpy(dst, pal->d, pal->l); dst += pal->l; MemCpy(dst, str, len);
  SysWrite(Cdbuf, dst + len - Cdbuf); }
void BPrint(uint8_t n, char *str) { if (!str) return;
  char *dst = Cdbuf; ugoc len = StrLen(str); PalBuf* pal = AFon(n); MemCpy(dst, pal->d, pal->l); dst += pal->l;
  MemCpy(dst, str, len); if (n != Convas.Fone) { dst += len; pal = AFon(Convas.Fone); len = pal->l; MemCpy(dst, pal->d, len); }
  SysWrite(Cdbuf, dst + len - Cdbuf); }
void SetColour(uint8_t c) { Sys.Deep &= b10; if (c >= aColours) c = aColours - On;
  char* modes[] = { "\12;22;23;27m", "\11;22;23;7m", "\11;23;27;1m", "\10;23;1;7m", "\11;22;27;3m", "\10;22;3;7m", "\10;27;1;3m", "\7;1;3;7m" };
  char *deep[] = { "\6\33[38;2", "\2\33[", "\6\33[38;5", "\6\33[38;2" }; uint8_t i, j = On; PalBuf *pal, *mode = (PalBuf*)deep[Sys.Deep], *src = AFon(c);
  if (Sys.Deep == b1 ) { cR = 16 + 36 * ((cR * 5 + 128) / 255) + 6 * ((cG * 5 + 128)/ 255) + ((cB * 5 + 128)/ 255); }
  else if (Sys.Deep == On ) { cR = (((cR * 299 + cG * 587 + cB * 114) > 127000) ? 90 : 30) + (((cR > 127) << 2) | ((cG > 127) << 1) | (cB > 127)); }
  if (!Sys.Deep || Sys.Deep == b10) { j += b1; } src->l = mode->l; MemCpy(src->d, mode->d, mode->l); cRGB = cR | (cG << 8) | (cB << 16);
  while(j--) { i = (uint8_t)(cRGB); cRGB >>= b3; if (Sys.Deep != On) { src->d[src->l++] = ';'; } if (i / 100) { src->d[src->l++] = 0x30 + (i / 100); i %= 100; }
    if (i / 10) { src->d[src->l++] = 0x30 + (i / 10); i %= 10; } src->d[src->l++] = 0x30 + i; } src->d[src->l++] = 'm';
  while(++j < b3) { pal = APal((j << aShift) + c); pal->l = src->l; MemCpy(pal->d, src->d, src->l); if (!Sys.Deep) break;
    mode = (PalBuf*)modes[j]; MemCpy(pal->d + src->l - On, mode->d, mode->l); pal->l += mode->l - On; } src->d[2] = '4';
  if (Sys.Deep == On) { if (APal(c)->d[2] == '9') { src->l++; src->d[2] = '1'; src->d[5] = src->d[4]; src->d[4] = src->d[3]; src->d[3] = '0'; } } } //!!!
void SwitchPal(void) { char* a = (char*)(Coffset + SOffset); Cdpal = (a + SFon);
  if (a == Cdfon) { a += SFon / 2; Cdpal += SPal / 2; } Cdfon = a; }
void SetPalette(uint8_t set, uint8_t deep) {
  Sys.Colours = Fcolour; if ((Sys.Deep = deep)) { Sys.Deep = b1; if (deep > b3) { Sys.Deep++; } else if (deep < b3) { Sys.Deep--; } }
  if (Sys.Colours > aColours) { Sys.Colours = aColours; } else { if (Sys.Colours < b1) Sys.Colours = b1; }
  if ((char*)(Coffset + SOffset) == Cdfon) { if (set) SwitchPal(); } else { if (!set) SwitchPal(); }
  Convas.Fone = Sys.Colours - On; Convas.Border = Off; uint8_t c = Sys.Colours;
  while(c--) { if (set) { uint16_t a = (c * 511) / Convas.Fone; cB = 128 + Fsin(a); cG = 128 + Fsin(a + 171); cR = 128 + Fsin(a + 342);
      if (!c) { cR = 0; cG = 0; cB = 0; } else if (c == Convas.Fone) { cR = 255; cG = 255; cB = 255; } }
    else { cRGB = (c) ? (((1 << 24) * (c)) / (Convas.Fone)) - On : Off; cG = (uint8_t)cRGB; cRGB >>= 8; cB = (uint8_t)cRGB;
      cRGB >>= 8; cR = (uint8_t)cRGB; } SetColour(c); } }
void InitVram(Cell addr, Cell size) { if (!addr || (size < SizeVram)) return;
  Cdata = (char*)addr; Cinfo = (uint8_t*)(Cdata + SDCell); Cds = Cinfo + SInfo; Coffset = (ugoc*)(Cds + SDs); Cdfon = (char*)(Coffset + SOffset);
  Cdpal = Cdfon + SFon; Cdkey = (uint8_t*)(Cdpal + SPal); Cdsys = (ugoc*)(Cdkey + SKeys); Cdcon = Cdsys + SSys; Cdwin = Cdcon + SConvas;
  Cvsw = Cdwin + SWin; Ccsw = Cvsw + SVsw; Cevent = (char*)(Ccsw + SCsw); Cexec = Cevent + SEvent; Cdbuf = Cexec + SExec; VP.Win = MAX_WIN;
  Sys.MWin = VP.Win; Convas.Win = Sys.MWin; Sys.CellP = CellPow; Sys.Spd1 = MaxSpeed; Sys.Spd0 = b3; Sys.Speed = Sys.Spd1; Sys.Ctb = 5;
  Sys.Time = Off; Sys.Fps = ((FFps / 50) < 2) ? 500 : ((FFps / 50) > 4) ? 2500 : ((FFps / 50) == 3) ? 1000 : ((FFps / 5) * 5);
  Sys.Delay = (1000 / Sys.Fps); Sys.Ginf = GOC_INF; Sys.Gmax = GOC_MAX; Sys.Gmin = GOC_MIN; Sys.A = RNG_A; Sys.B = RNG_B;
  Convas.Min = Off; Convas.Max = VP.Win; Convas.D = Off; Convas.S = VP.Win; Convas.CW = CellLine; Convas.W = Convas.CW; Convas.CH = CellStr; 
  Convas.H = Convas.CH; VP.Mode = b2; VP.Loop = On; Vector(K_Mouse) = RPEncode; SetPalette(On,CFDeep); SetPalette(Off,CFDeep); uint8_t i = Sys.Ctb;
  while(i--) { Sys.T[i] = Off; Sys.S[i] = Off; } }
Cell SystemSwitch(void) {
  if (VRam.SystemSwitch) { VRam.size = SizeVram; if (!(VRam.addr = GetRam(&VRam.size))) return Off;
    VRam.SystemSwitch--; SWD(VRam.addr); InitVram(VRam.addr,VRam.size); SwitchRaw(); Delay_ms(Off); IRnd();
    SyncSize(VRam.addr); Print(Convas.Fone,AltBufOn HideCur WrapOn MouseX10on); BPrint(Convas.Border, HideCur Cls); }
  else { VRam.SystemSwitch++; if (VRam.size) { SwitchRaw(); Print(Convas.Fone,AltBufOff Reset ShowCur WrapOn MouseX10off);
    FreeRam(VRam.addr, VRam.size); } }
  return On; }

void MoveConvas(goc dx, goc dy) { Buf.Ctrl = On; ugoc r, c = TermCR(&r); goc x = VP.X + dx, y = VP.Y + dy;
  if (VP.Mode & b1) { return; }
  else {
    x = ((VP.X < -MaxSpeed || VP.X > MaxSpeed) && ((x ^ VP.X) & Sys.Ginf)) ? ((x < Off) ? Sys.Gmax : Sys.Gmin) : ((x == Sys.Ginf) ? Sys.Gmin : x);
    y = ((VP.Y < -MaxSpeed || VP.Y > MaxSpeed) && ((y ^ VP.Y) & Sys.Ginf)) ? ((y < Off) ? Sys.Gmax : Sys.Gmin) : ((y == Sys.Ginf) ? Sys.Gmin : y); }
  dx = VP.X / c; dy = VP.Y / r; VP.X = x; VP.Y = y; VP.Xs = (VP.X < Off) ? (c + (VP.X % c)) : (VP.X % c);
  VP.Ys = (VP.Y < Off) ? (r + (VP.Y % r)) : (VP.Y % r); if ((x / c) != dx || (y / r) != dy) Buf.Ctrl++; }
uint8_t MoveScreen(goc mx, goc my) { goc dx = VP.X - mx, dy = VP.Y - my;
  if (VP.Mode & b1) { return Off; }
  else if (((dx ^ VP.X) & Sys.Ginf) || ((dy ^ VP.Y) & Sys.Ginf)) return Off;
  VP.X = dx; VP.Y = dy; VP.Xs -= mx; VP.Ys -= my; return On; }
  
uint8_t ViewPort(void) { uint8_t p = Off; Buf.Ctrl = Off;
  if (Vector(Off)) { VP.Wec = Event(Off)->W; Vector(Off)(); }
  Vector(K_Mouse)(); if (*Buf.Key == K_ESC && *(Buf.Key + On) == K_NO) Buf.Cod = Off;
  else { Buf.Cod = (*Buf.Key == K_ESC) ? *(Buf.Key + On) : (*Buf.Key & b7) ? Off : *Buf.Key;
    if (Buf.Cod == K_Mouse) {
      Buf.Mkey = *(Buf.Key + 2); Buf.MX = *(Buf.Key + 3) - 0x21; Buf.MY = *(Buf.Key + 4) - 0x21;
      if (Buf.Mkey == Buf.Ru) Buf.Cod = VP.up;
      else if (Buf.Mkey == Buf.Rd) Buf.Cod = VP.ud;
      else if (Buf.Mkey == Buf.cRu) Buf.Cod = VP.ri;
      else if (Buf.Mkey == Buf.cRd) Buf.Cod = VP.le;
      else if (Buf.Mkey == Buf.Lk) { Buf.LkX = Buf.MX; Buf.LkY = Buf.MY; p++; }
      else if (Buf.Mkey == Buf.Mk) { Buf.MkX = Buf.MX; Buf.MkY = Buf.MY; p = b10; }
      else if (Buf.Mkey == Buf.Rk) { Buf.RkX = Buf.MX; Buf.RkY = Buf.MY; p = b1; }
      if (p && MoveScreen(VP.Xs - Buf.MX, VP.Ys - Buf.MY)) {  } }
    else {
      if (Buf.Cod) {
        if (Vector(Buf.Cod)) { VP.Wec = Event(Buf.Cod)->W; Vector(Buf.Cod)(); Buf.Ctrl++; }
        uint8_t *p = &VP.Key, t = *p++; while (t--) if (*p++ == Buf.Cod) { Buf.Ctrl++; break; } }
      if (!Buf.Ctrl) PushKey(); }
    if (Buf.Cod) { ++Buf.tic; Buf.Ctrl = On; } }
  if (Buf.Ctrl) {
    if (Buf.Cod == VP.scs) { VP.dXY = On; Sys.Speed = (VP.Mode ^= b2) ? Sys.Spd1 : Sys.Spd0; }
    else if (Buf.Cod == VP.Anchor) { if (VP.Mode ^= b1) { } else { Convas.W = Convas.CW; Convas.H = Convas.CH; } }
    else if (Buf.Cod == VP.bcu) { SwitchPal(); BPrint(Convas.Border,Cls); }
    else if (Buf.Cod == VP.ssc) VP.Mode ^= b0;
    else if (Buf.Cod == VP.Exit) VP.Loop = Off;
    else { goc dx = Off, dy = Off;
      if (Buf.Cod != VP.Cod) { VP.dXY = On; VP.Cod = Buf.Cod; }
      if ((Buf.tic > 7) && !(Buf.tic & b10) && (VP.dXY < Sys.Speed)) VP.dXY <<= On;
      if (Buf.Cod == VP.le) dx = -VP.dXY;
      else if (Buf.Cod == VP.ri) dx = VP.dXY;
      else if (Buf.Cod == VP.up) dy = -VP.dXY;
      else if (Buf.Cod == VP.ud) dy = VP.dXY;
      MoveConvas(dx, dy); } }
  if (SyncSize(VRam.addr) || Buf.Ctrl > On) { BPrint(Convas.Border,Cls); }
  else {  }
  if (++Sys.Time > Sys.Fps) { Sys.Time = Off; p = Sys.Ctb; while(p-- && (!++Sys.T[p])); }
  return VP.Loop; }
  
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
      if ((ugoc)VP.X >= Convas.W || (ugoc)VP.Y >= Convas.H) return; } }
  if (count == Off) { x = VP.Xs + On; y = VP.Ys + On;
    if (w->EF & b0) { ugoc r, c = TermCR(&r); if (c < (x + w->W)) { x = -On; } if (r < (y + w->H)) y = -On; } 
    else if ((ugoc)VP.X >= Convas.W || (ugoc)VP.Y >= Convas.H) return; }
  w->Xr = x; w->Yr = y; if (w->Xr) { w->EF |= b1; } else { w->EF &= ~b1; } }
uint16_t _Window(uint8_t t, int8_t col, uint8_t count, ugoc *args) { uint16_t l, n; Windows* w;
  if (t) { n = --Convas.S; if (n < On) { Convas.S = Convas.Max; n = --Convas.S; }
    w = Win(n); w->EF = 9; w->Layer = Convas.Max; l = On + Convas.Max - n; while(--l) --Win(n + l)->Layer; }
  else { n = Convas.D++; if (n >= Convas.S) { Convas.D = Convas.Min; n = Convas.D++; } w = Win(n); w->EF = 8; w->Layer = n; }
  w->parent = n; w->child = n; w->MaxVs = Off; w->XCur = Off; w->YCur = Off; w->WFirstSR = CellStr; w->Xr = Off; w->Yr = Off; w->W = Off; w->H = Off;
  if (count > On) { w->Xr = args[0]; if (!(w->Yr = args[1])) w->Xr = Off; } if (count > 2) { w->W = args[2]; if (count > 3) w->H = args[3]; }
  w->palette = col; if (w->EF == 9) { if (w->W < b1) { w->W = b1; } if (!w->H) { w->H++; } } if (w->Xr) { w->EF |= b1; } return n; }
void _WEvent(uint16_t n, uint8_t cur, uint8_t count, AFunction *args) { if ((n >= Convas.D && n < Convas.S) || n >= Convas.Win) return;
  if ((Win(n)->EF & b0) && count--) { Event(cur)->W = n; Vector(cur) = ((Cell)args[Off] <= (Cell)Nop) ? Off : args[Off];
    if (Event(cur)->C && count) { uint8_t j, c = Event(cur)->C, i = On; while(c-- && count--) { j = K_Mouse;
        while(--j) { if (Event(j)->W == n && Event(j)->N == i) { Vector(j) = ((Cell)args[i] <= (Cell)Nop) ? Off : args[i]; i++; break; } } } } } }
void _WSet(uint16_t n, uint8_t count, uint8_t *args) { if ((n >= Convas.D && n < Convas.S) || n >= Convas.Win) return;
  if (count--) { Windows* w = Win(n); w->EF &= ~b2; if (args[Off]) { w->EF |= b2; } if (count) { w->EF &= ~b3; if (args[On]) { w->EF |= b3; } } } }
void _SEvent(uint8_t count, uint8_t *args) { uint8_t a, k = count; while(k--) { a = args[k]; Event(a)->W = Convas.Win; Event(a)->C = count; Event(a)->N = k; } }
void _SExec(uint8_t count, AFunction *args) { uint8_t k = K_Mouse; Convas.Min = Convas.D; Convas.Max = Convas.S;
  while(k--) { if (Event(k)->W == Convas.Win) {
    if (Event(k)->C && count) { uint8_t j, c = Event(k)->C, i = Off; while(c-- && count--) { j = K_Mouse;
        while(--j) { if (Event(j)->W == Convas.Win && Event(j)->N == i) { Vector(j) = ((Cell)args[i] <= (Cell)Nop) ? Off : args[i]; i++; break; } } } } break; } } }
void _VKeys(uint8_t count, uint8_t *args) { uint8_t *p = &VP.Key, i = Off; p += *p; if (count > VP.Key) { count = VP.Key; } while(count--) *p-- = args[i++]; }
void _WData(uint16_t n, char *str, uint8_t count, ugoc *args) { if ((n >= Convas.D && n < Convas.S) || n >= Convas.Win) return;
  Windows* w = Win(n); if (!(w->MaxVs)) {  } (void)*str; (void)count; (void)*args; }
