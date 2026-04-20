/*
 * Copyright (C) 2026 Поздняков Алексей Васильевич
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 */

#include "sys.h"

ENGINE_VARS_INIT;

Cell StrLen(char *s) { if (!s) return 0;
  char *f = s; while (*f++);
  return (--f - s); }
void MemSet(void* buf, uint8_t val, Cell len) {
  uint8_t *p = (uint8_t *)buf;
  while (len && ((Cell)p & (SCell - 1))) { len--; *p++ = val; }
  if (len >= SCell) {
    Cell vW = val * (((Cell) - 1) / 255); Cell *pW = (Cell *)p;
    Cell i = len / SCell; len &= (SCell - 1); while (i--) *pW++ = vW;
    p = (uint8_t *)pW; }
  while (len--) *p++ = val; }
void MemCpy(void* dst, void* src, Cell len) {
  uint8_t *d = (uint8_t *)dst; uint8_t *s = (uint8_t *)src;
  while (len && ((Cell)d & (SCell - 1))) { *d++ = *s++; len--; }
  if (len >= SCell && ((Cell)s & (SCell - 1)) == 0) {
    Cell *dW = (Cell *)d; Cell *sW = (Cell *)s;
    Cell i = len / SCell; len &= (SCell - 1); while (i--) *dW++ = *sW++;
    d = (uint8_t *)dW; s = (uint8_t *)sW; }
  while (len--) *d++ = *s++ ; }
void MemMove(void* dst, void* src, Cell len) {
  if (dst > src) { 
    uint8_t *d = (uint8_t *)dst; uint8_t *s = (uint8_t *)src; d += len; s += len;
    while (len && ((Cell)d & (SCell - 1))) { len--; *--d = *--s; }
    if (len >= SCell && ((Cell)s & (SCell - 1)) == 0) {
      Cell *dW = (Cell *)d; Cell *sW = (Cell *)s;
      Cell i = len / SCell; len &= (SCell - 1); while (i--) *--dW = *--sW;
      d = (uint8_t *)dW; s = (uint8_t *)sW; }
    while (len--) *--d = *--s ; }
  else if (dst < src ) MemCpy(dst, src, len); }
int8_t MemCmp(void* dst, void* src, Cell len) {
  uint8_t *d = (uint8_t *)dst; uint8_t *s = (uint8_t *)src;
  while (len && ((Cell)d & (SCell - 1))) { len--; if (*d++ != *s++) return (int8_t)(*--d - *--s); }
  if (len >= SCell && ((Cell)s & (SCell - 1)) == 0) {
    Cell *dW = (Cell *)d; Cell *sW = (Cell *)s;
    Cell i = len / SCell; len %= (SCell - 1); while (i-- && (*dW++ == *sW++));
    if (i + 1) { --dW; --sW; len += SCell; }
    d = (uint8_t *)dW; s = (uint8_t *)sW; }
  while (len--) { if (*d++ != *s++) return (int8_t)(*--d - *--s); }
  return 0; }

uint8_t UTFinfo(char *s) {
  uint32_t cp; uint8_t d = 0x00; unsigned char c = *s++;
  if (c < 0x80) cp = (uint32_t) c;
  else if ((c & 0xE0) == 0xC0 && (*s & 0xC0) == 0x80)
    { d++; cp = ((c & 0x1F) << 0x06) | (*s & 0x3F); }
  else if ((c & 0xF0) == 0xE0 && (*s & 0xC0) == 0x80 && (*(s + 0x01) & 0xC0) == 0x80)
    { d = 0x02; cp = ((c & 0x0F) << 0x0C) | ((*s & 0x3F) << 0x06) | (*(s + 0x01) & 0x3F); }
  else if ((c & 0xF8) == 0xF0 && (*s & 0xC0) == 0x80 && (*(s + 0x01) & 0xC0) == 0x80 && (*(s + 0x02) & 0xC0) == 0x80) 
    { d = 0x03; cp = ((c & 0x07) << 0x12) | ((*s & 0x3F) << 0x0C) | ((*(s + 0x01) & 0x3F) << 0x06) | (*(s + 0x02) & 0x3F); }
  else return (d |= b7);
  if (cp < 0x20 || (cp >= 0x7F && cp < 0xA0)) return (d = b5);
  if (cp < 0x100) return (d |= b2);
  if (cp >= 0x0590 && cp <= 0x08FF) d |= b4;
  if (((d & 0x03) == 0x01 && cp < 0x80) || ((d & 0x03) == 0x02 && (cp < 0x800 || (cp >= 0xD800 && cp <= 0xDFFF))) || 
      ((d & 0x03) == 0x03 && (cp < 0x10000 || cp > 0x10FFFF))) return (d |= b7);
  if ((cp >= 0x0300 && cp <= 0x036F) || (cp >= 0x1DC0 && cp <= 0x1DFF) || (cp >= 0x20D0 && cp <= 0x20FF) ||
      (cp == 0x200D || (cp >= 0xFE00 && cp <= 0xFE0F))) return (d &= 0xF3);
  if (cp == 0x200B || cp == 0x200C || cp == 0x200E || cp == 0x200F || (cp >= 0xFE20 && cp <= 0xFE2F) ||
      (cp >= 0xE0100 && cp <= 0xE01EF)) return (d &= 0xF3);
  if ((cp >= 0x1100 && cp <= 0x115F) || (cp == 0x2329 || cp == 0x232A) || (cp >= 0x2E80 && cp <= 0xA4CF && cp != 0x303F) || 
      (cp >= 0xAC00 && cp <= 0xD7A3) || (cp >= 0xF900 && cp <= 0xFAFF) || (cp >= 0xFE10 && cp <= 0xFE19) || 
      (cp >= 0xFE30 && cp <= 0xFE6F) || (cp >= 0xFF00 && cp <= 0xFF60) || (cp >= 0xFFE0 && cp <= 0xFFE6) || 
      (cp >= 0x20000 && cp <= 0x2FFFD) || (cp >= 0x30000 && cp <= 0x3FFFD) || (cp >= 0x1F300)) return (d |= 0x08);
  return (d |= b2); }
uint8_t UTFinfoTile(char *s, Cell len) {
  if (!len) return 0xC0;
  if ((*s & 0xE0) == 0xC0 && len < 0x02) return 0xC0;
  else if ((*s & 0xF0) == 0xE0 && len < 0x03) return 0xC0;
  else if ((*s & 0xF8) == 0xF0 && len < 0x04) return 0xC0;
  return UTFinfo(s); }

void Print(uint8_t n, char *str) {
  char *dst = Cdbuf + 512, *sav; uint16_t len; n &= Mcbi; if (!str) return;
  sav = Palette(n); len = *sav++; MemCpy(dst, sav, len); dst += len; len = StrLen(str); MemCpy(dst, str, len); dst += len;
  if (n != Cconvas) { sav = Palette(Cconvas); len = *sav++; MemCpy(dst, sav, len); dst += len; }
  SysWrite(Cdbuf + 512, (dst - Cdbuf - 512)); }
void InitVram(Cell addr, Cell size) { if (!addr || (size < SizeVram)) return;
  uint8_t lm, cbi, ca, c = StrLen(Reset), i = 8; char *ac, *dst; uint8_t* base = (uint8_t*)addr;
  Cdata = (char*)base; Cattr = (uint16_t*)(base + SizeCell); Cvlswin = Cattr + SizeADOCell;
  Cdwin = Cvlswin + SizeVlsWin; Cdlay = Cdwin + SizeDataWin; Cdcon = Cdlay + SizeDataLayer;
  Cdpal = (char*)(Cdcon + SizeDataConvas); Cdkey = Cdpal + SizeBufPal; Cdbuf = Cdkey + SizeBufKey;
  char* colors[] = { Reset, Grey, Green, Red, Blue, Orange, Gold, Reset };
  char* modes[] = { "\007;22;27m", "\006;22;7m", "\006;1;27m", "\005;1;7m" };
  while (i--) { ac = (Cdbuf + ((i) << 5)); dst = ac; *dst++ = c; MemCpy(dst, Reset, c);
    ca = StrLen(colors[i]); if (ca) { *ac++ = ca; MemCpy(ac, colors[i], ca); } }
  i = 4; while(i) { char* mode = modes[--i]; lm = *mode++, c = 8; 
    while(c) { ac = (Cdbuf + ((--c) << 5)); cbi = (c << 2) + i; ca = (*ac++ - 1);
      dst = Palette(cbi); *dst++ = (lm + ca); MemCpy(dst, ac, ca); MemCpy(dst + ca, mode, lm); } } 
  Convas.No = MaxWin; Convas.N = MaxWin; Convas.W = CellLine; Convas.H = CellStr; }
Cell SystemSwitch(void) {
  if (VRam.SystemSwitch) { VRam.size = SizeVram; if (!(VRam.addr = GetRam(&VRam.size))) return Off;
    VRam.SystemSwitch--; SWD(VRam.addr); InitVram(VRam.addr,VRam.size); SwitchRaw(); Delay_ms(0);
    SyncSize(VRam.addr); Print(Cdefault,AltBufOn Reset HideCur WrapOff Cls MouseX10on); }
  else { VRam.SystemSwitch++; if (VRam.size) { SwitchRaw(); Print(Cdefault,AltBufOff Reset ShowCur WrapOn MouseX10off);
    FreeRam(VRam.addr, VRam.size); } }
  return On; }

uint8_t PushKey(char *key) {
  char *sav, *dst; uint8_t d, l, c, data = UTFinfo(key); c = (data == b5) ? *(key + 1) : 0; if (data & b7) return 0;
  if (Buf.pop == Buf.push) {
    dst = KeyBuf(++Buf.push); *dst++ = data | b6; dst++; *dst++ = 1; dst++;
    if (Buf.pop == Buf.push) ++Buf.pop;
    l = 1 + (data & 0x03);
    if (c) *dst = c;
    else while(l--) *(dst + l) = *(key + l); }
  else {
    dst = KeyBuf(Buf.push); d = *dst; sav = dst + 2; dst += 4;
    if (d & b7) { sav++; dst += 2; }
    l = 1 + (data & 0x03);
    if ((d & 0x3F) == data) {
      if (c) { if (*dst == c) l = 0xFF; }
      else while(l--) { if (*(dst + l) != *(key + l)) break; } }
    if (l == 0xFF) { if (!(*sav += 1)) *sav = 0xFF; }
    else { l = 1 + (data & 0x03);
      if ((d & 0x03) < 2 && ((data & 0x03) < 2 && !(d & b7))) {
        dst = KeyBuf(Buf.push); *dst |= b7; *(dst + 1) = data; *(dst + 3) = 1; dst += 6; }
      else {
        dst = KeyBuf(++Buf.push); *dst = data | b6; *(dst + 2) = 1; dst += 4;
        if (Buf.pop == Buf.push) ++Buf.pop; }
      if (c) *dst = c;
      else while(l--) *(dst + l) = *(key + l); } }
  if (!c) c = 0xFF;
  return c; }
uint8_t ShowKey(uint8_t *data, uint8_t *count, char *key) {
  uint8_t d; char *dst; if (Buf.pop == Buf.push) { *data = 0; *count = 0; return 0; }
  dst = KeyBuf(Buf.push); d = *dst; *count = *(dst + 2); dst += 4;
  if (d & b7) { d = *(dst - 3); *count = *(dst - 1); dst += 2; }
  *data = (d & 0x3F); d = 1 + (d & 0x03); while(d--) *(key + d) = *(dst + d);
  return 1; }
uint8_t PopKey(uint8_t *data, uint8_t *count, char *key) {
  char *dst; uint8_t d, n = 1;
  while(!((d = *(dst = KeyBuf(Buf.pop))) & 0xC0) && (Buf.pop != Buf.push)) Buf.pop++;
  if (!(d & 0xC0)) { *data = 0; *count = 0; return 0; }
  if (Buf.pop == Buf.push) { n--; Buf.pop--; if ((d & 0xC0) == 0xC0) n++; }
  if (d & b6) { *count = *(dst + 2); if (n) *dst &= 0xBF; }
  else { dst += 2; d = *(dst - 1); *count = *(dst + 1); if (n) *(dst - 2) &= 0x3F; }
  dst += 4; *data = (d & 0x3F); d = 1 + (d & 0x03); while(d--) *(key + d) = *(dst + d);
  return n; }
void ForgetKey(void) {
  if (Buf.pop == Buf.push) return;
  char *src = KeyBuf(Buf.push--); Buf.tic--;
  if (*src & b7) { Buf.push++; *src &= ~b7; } }
uint16_t Keys(void) {
  uint16_t s = 0; uint8_t c = Buf.push; while (c != Buf.pop) { s++; if (*KeyBuf(c--) & b7) s++; }
  return s; }
uint8_t Mouse(uint8_t key, uint8_t x, uint8_t y) {
  uint8_t t = 0; int16_t dx = 0, dy = 0; uint16_t r, c = TermCR(&r); Buf.Mkey = key; Buf.MX = x - 32; Buf.MY = y - 32;
  if (Buf.Mkey == Buf.Lk) {
    VP.X = Buf.MX - VP.viewX; VP.Y = Buf.MY - VP.viewY; Buf.LkX = VP.X; Buf.LkY = VP.Y; t++; }
  else if (Buf.Mkey == Buf.Mk) {
    VP.X = Buf.MX - VP.viewX; VP.Y = Buf.MY - VP.viewY; Buf.MkX = VP.X; Buf.MkY = VP.Y; t++; }
  else if (Buf.Mkey == Buf.Rk) {
    VP.X = Buf.MX - VP.viewX; VP.Y = Buf.MY - VP.viewY; Buf.RkX = VP.X; Buf.RkY = VP.Y; t++; }
  if (Buf.Mkey == Buf.Ru) { dy--; t++; }
  else if (Buf.Mkey == Buf.Rd) { dy++; t++; }
  else if (Buf.Mkey == Buf.cRu) { dx++; t++; }
  else if (Buf.Mkey == Buf.cRd) { dx--; t++; }
  if (dx || dy) {
    VP.Y += dy * VP.dXY; VP.X += dx * VP.dXY;
    if (VP.Mode & 6) {
      VP.X = ((VP.X + VP.viewX < 1) ? 1 : (VP.X + VP.viewX > c) ? c : VP.X + VP.viewX) - VP.viewX;
      VP.Y = ((VP.Y + VP.viewY < 1) ? 1 : (VP.Y + VP.viewY > r) ? r : VP.Y + VP.viewY) - VP.viewY; }
    else {
      dx = (VP.X > 0) ? (1 - VP.X)/c : (c - VP.X)/c; dy = (VP.Y > 0) ? (1 - VP.Y)/r : (r - VP.Y)/r;
      if (VP.viewX != dx*c) { VP.viewX = dx*c; t++; }
      if (VP.viewY != dy*r) { VP.viewY = dy*r; t++; } } }
  return t; }
uint8_t GetEventKM(uint8_t *num, uint8_t *tic, uint8_t *control) {
  uint8_t t, c = 0; *control = 0; *tic = Buf.tic; GetKey(Buf.key);
  if (*Buf.key == 27) { c = *(Buf.key + 1); if (c == K_NO) return c; }
  if (c == K_Mouse) { *control = Mouse(*(Buf.key + 2),*(Buf.key + 3),*(Buf.key + 4)); return c; }
  if (c && *num < K_Max) { t = *num++; while (t--) if (*num++ == c) { *control = 1; break; } }
  if (!(*control && (Buf.mode & 1))) c = PushKey(Buf.key);
  if (c) *tic = ++Buf.tic;
  return c; }

uint8_t ViewPort(void) {
  uint16_t r, c = TermCR(&r); int16_t x, y; uint8_t control, s = Buf.mode; Buf.mode |= 1; if (VP.Mode & 4) Buf.mode--;
  VP.Cod = GetEventKM(&VP.Key, &VP.Tic, &control); Buf.mode = s;
  if (control && VP.Cod != K_Mouse) {
    if ((uint16_t)(VP.X - 1) < Convas.W && (uint16_t)(VP.Y - 1) < Convas.H) { 
      if (VP.Cod == VP.F2) { VP.Mode ^= b3; if (!(VP.Mode & b3)) ForgetKey(); }
      else if (VP.Cod == VP.F3) { VP.Mode ^= b2; }
      else if (VP.Cod == VP.F4) { VP.Mode ^= b1; } }
    if (VP.Cod == VP.es) return 0;
    if (VP.Cod != VP.oCod) { VP.dXY = 1; VP.oCod = VP.Cod; }
    if (VP.Cod & b5) {
      if ((VP.Tic > 7) && !(VP.Tic & 3) && (VP.dXY < 128)) VP.dXY <<= 1;
      if (VP.Cod == VP.le || VP.Cod == VP.cle) VP.X -= VP.dXY;
      else if (VP.Cod == VP.ri || VP.Cod == VP.cri) VP.X += VP.dXY;
      else if (VP.Cod == VP.up || VP.Cod == VP.cup) VP.Y -= VP.dXY;
      else if (VP.Cod == VP.ud || VP.Cod == VP.cdo) VP.Y += VP.dXY;
      if (VP.Mode & b21) {
        VP.X = ((VP.X + VP.viewX < 1) ? 1 : (VP.X + VP.viewX > c) ? c : VP.X + VP.viewX) - VP.viewX;
        VP.Y = ((VP.Y + VP.viewY < 1) ? 1 : (VP.Y + VP.viewY > r) ? r : VP.Y + VP.viewY) - VP.viewY; }
      else {
        x = (VP.X > 0) ? (1 - VP.X)/c : (c - VP.X)/c; y = (VP.Y > 0) ? (1 - VP.Y)/r : (r - VP.Y)/r;
        if (VP.viewX != x*c) { VP.viewX = x*c; control++; }
        if (VP.viewY != y*r) { VP.viewY = y*r; control++; } } } }
  if (SyncSize(VRam.addr)) {
    c = TermCR(&r); control++;
    VP.X = ((VP.X + VP.viewX < 1) ? 1 : (VP.X + VP.viewX > c) ? c : VP.X + VP.viewX) - VP.viewX;
    VP.Y = ((VP.Y + VP.viewY < 1) ? 1 : (VP.Y + VP.viewY > r) ? r : VP.Y + VP.viewY) - VP.viewY; }
  if (control > 1) { control--; }
  else { control--; }
  return 1; }

void WinView(uint16_t n, int16_t x, int16_t y) {
  if (Convas.No || n > Convas.N) return;
  WindowData* w = Win(n); w->Xrender = x; w->Yrender = y; }
void WinTop(uint16_t n) { uint16_t l = Convas.N, d = Layer(n), m = Convas.win - 1;
  if (Convas.No || n > Convas.N || Layer(n) >= m) return;
  while(l) { if (Layer(l) > d && Layer(l) <= m) --Layer(l); --l; }
  WindowData* w = Win(n); w->Layer = m; Layer(n) = m; }
uint16_t _Window(int8_t col, uint8_t count, int16_t *args) {
  uint16_t c = 0, h = 0; uint16_t n = ++Convas.N; Convas.No = 0;
  WindowData* w = Win(n); if (count) { h = args[0]; if (--count) c = args[1]; }
  if (n >= MaxWin) { n = 0; Convas.N = 0; Convas.win = MaxWin; Convas.stat = MaxWin;
    Convas.Xwin = 0; Convas.Ywin = 0; Convas.Xstat = Convas.W; Convas.Ystat = Convas.H; }
  if (col < 0) { w->WinFlags = (((-col) & Mcbi) | b7); uint16_t i = n, l = ++Convas.stat, d = MaxWin - 1; Layer(n) = d;
    if (l > MaxWin) { l = 1;  Convas.stat = 1; }
    while(--l) { while(--i || Layer(i) != d) { } if (Layer(i) == d) Layer(i) = --d; } }
  else { w->WinFlags = ((col & Mcbi) | b65); Layer(n) = n; if (++Convas.win > MaxWin) Convas.win = 1; }
  w->Xrender = 0; w->Yrender = 0; w->W = c; w->H = h; w->Layer = Layer(n); w->parent = n; w->child = n; w->MaxCs = 0; w->MaxVs = 0;
  w->MaxH = 0; w->XCur = 0; w->YCur = 0; w->WFirstSR = Convas.H; w->Xconvas = Convas.W; w->Yconvas = Convas.H; return n; }
void _WSet(uint16_t n, uint8_t cur, uint8_t count, int16_t *args) {
  if (Convas.No || n > Convas.N) return;
  WindowData* w = Win(n); if (w->WinFlags & b7) return;
  w->WinFlags &= ~b5; if (cur) w->WinFlags &= b5;
  if (count) { w->WinFlags &= ~b6; if (args[0]) w->WinFlags &= b6; } }
void _WData(uint16_t n, char *str, uint8_t count, int16_t *args) {
  if (Convas.No || n > Convas.N) return;
  WindowData* w = Win(n); if (w->Xconvas == Convas.W) { uint16_t c = w->W, r = w->H; (void)r;
    if (!c) { } } 
  (void)*str; (void)count; (void)*args; }
