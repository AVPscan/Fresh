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

Cell StrLen(char *s) { if (!s) return 0;
  uint8_t *f = (uint8_t*)s; while (*f++);
  return (--f - (uint8_t*)s); }
void MemSet(void* buf, uint8_t val, Cell len) {
  uint8_t *p = (uint8_t*)buf;
  while (len && ((Cell)p & (SCell - 1))) { len--; *p++ = val; }
  if (len >= SCell) {
    Cell vW = val * (((Cell) - 1) / 255); Cell *pW = (Cell*)p;
    Cell i = len / SCell; len &= (SCell - 1); while (i--) *pW++ = vW;
    p = (uint8_t*)pW; }
  while (len--) *p++ = val; }
void MemCpy(void* dst, void* src, Cell len) {
  uint8_t *d = (uint8_t*)dst, *s = (uint8_t*)src;
  while (len && ((Cell)d & (SCell - 1))) { *d++ = *s++; len--; }
  if (len >= SCell && ((Cell)s & (SCell - 1)) == 0) {
    Cell *dW = (Cell*)d; Cell *sW = (Cell*)s;
    Cell i = len / SCell; len &= (SCell - 1); while (i--) *dW++ = *sW++;
    d = (uint8_t*)dW; s = (uint8_t*)sW; }
  while (len--) *d++ = *s++ ; }
void MemMove(void* dst, void* src, Cell len) {
  if (dst > src) {
    uint8_t *d = (uint8_t*)dst, *s = (uint8_t*)src; d += len; s += len;
    while (len && ((Cell)d & (SCell - 1))) { len--; *--d = *--s; }
    if (len >= SCell && ((Cell)s & (SCell - 1)) == 0) {
      Cell *dW = (Cell*)d; Cell *sW = (Cell*)s;
      Cell i = len / SCell; len &= (SCell - 1); while (i--) *--dW = *--sW;
      d = (uint8_t*)dW; s = (uint8_t*)sW; }
    while (len--) *--d = *--s ; }
  else if (dst < src ) MemCpy(dst, src, len); }
int8_t MemCmp(void* dst, void* src, Cell len) {
  uint8_t *d = (uint8_t*)dst, *s = (uint8_t*)src;
  while (len && ((Cell)d & (SCell - 1))) { len--; if (*d++ != *s++) return (int8_t)(*--d - *--s); }
  if (len >= SCell && ((Cell)s & (SCell - 1)) == 0) {
    Cell *dW = (Cell*)d; Cell *sW = (Cell*)s;
    Cell i = len / SCell; len &= (SCell - 1); while (i-- && (*dW++ == *sW++));
    if (i + 1) { --dW; --sW; len += SCell; }
    d = (uint8_t*)dW; s = (uint8_t*)sW; }
  while (len--) { if (*d++ != *s++) return (int8_t)(*--d - *--s); }
  return 0; }

uint8_t UTFinfo(uint8_t *s) {
  uint32_t cp; uint8_t d = 0x00, c = *s++;
  if (c < 0x80) cp = (uint32_t) c;
  else if ((c & 0xE0) == 0xC0 && (*s & 0xC0) == 0x80)
    { d++; cp = ((c & 0x1F) << 0x06) | (*s & 0x3F); }
  else if ((c & 0xF0) == 0xE0 && (*s & 0xC0) == 0x80 && (*(s + 0x01) & 0xC0) == 0x80)
    { d = 0x02; cp = ((c & 0x0F) << 0x0C) | ((*s & 0x3F) << 0x06) | (*(s + 0x01) & 0x3F); }
  else if ((c & 0xF8) == 0xF0 && (*s & 0xC0) == 0x80 && (*(s + 0x01) & 0xC0) == 0x80 && (*(s + 0x02) & 0xC0) == 0x80) 
    { d = 0x03; cp = ((c & 0x07) << 0x12) | ((*s & 0x3F) << 0x0C) | ((*(s + 0x01) & 0x3F) << 0x06) | (*(s + 0x02) & 0x3F); }
  else return (d |= b7);
  if (cp < 0x20 || (cp >= 0x7F && cp < 0xA0)) return (d |= b5);
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
uint8_t UTFinfoTile(uint8_t *s, Cell len) {
  if (!len) return 0xC0;
  if ((*s & 0xE0) == 0xC0 && len < 0x02) return 0xC0;
  else if ((*s & 0xF0) == 0xE0 && len < 0x03) return 0xC0;
  else if ((*s & 0xF8) == 0xF0 && len < 0x04) return 0xC0;
  return UTFinfo(s); }

void Print(uint8_t n, char *str) {
  char *dst = Cdbuf + 512; n &= Mcbi; if (!str) return;
  PalData* pal =  Palette(n); MemCpy(dst, pal->data, pal->len); dst += pal->len;
  ugoc len = StrLen(str); MemCpy(dst, str, len); dst += len;
  if (n != Cconvas) { pal =  Palette(Cconvas); MemCpy(dst, pal->data, pal->len); dst += pal->len; }
  SysWrite(Cdbuf + 512, dst - Cdbuf - 512); }
void ext(void) { VP.Loop = Off; }
void InitVram(Cell addr, Cell size) { if (!addr || (size < SizeVram)) return;
  uint8_t cbi, c = StrLen(Reset), i = 8; uint8_t* base = (uint8_t*)addr; PalData *pal, *mode, *src;
  char* colors[] = { Reset, Grey, Green, Red, Blue, Orange, Gold, Reset }; char* modes[] = { "\7;22;27m", "\6;22;7m", "\6;1;27m", "\5;1;7m" };
  Cdata = (char*)base; Cattr = (ugoc*)(base + SizeCell); Cvlswin = Cattr + SizeADOCell;
  Cdwin = Cvlswin + SizeVlsWin; Cdcon = Cdwin + SizeDataWin; Cdpal = (char*)(Cdcon + SizeDataConvas);
  Cdkey = (uint8_t*)(Cdpal + SizeBufPal); Cdmenu = (char*)(Cdkey + SizeBufKey); Cdbuf = Cdmenu + SizeBufMenu; Cvector = Cdbuf + SizeBuf; Vector(K_ESC) = ext;
  while (i--) { pal = (PalData*)(Cdbuf + (i << 5)); pal->len = c; MemCpy(pal->data, Reset, pal->len);
    if (StrLen(colors[i])) { pal->len = StrLen(colors[i]); MemCpy(pal->data, colors[i], pal->len); } }
  i = 4; while(i) { mode = (PalData*)modes[--i]; c = 8; while(c) { cbi = (--c << 2) + i; src = (PalData*)(Cdbuf + ((c) << 5)); pal = Palette(cbi);
      pal->len = src->len + mode->len - 1; MemCpy(pal->data, src->data, src->len - 1); MemCpy(pal->data + src->len - 1, mode->data, mode->len); } }
  VP.Mode = 1; VP.Loop = 1; Convas.W = MaxWin; Convas.Flag = Convas.W; Convas.WinMax = Convas.W; Convas.Dwin = Convas.W; Convas.Swin = Convas.W;
  Convas.Wmax = CellLine; Convas.Hmax = CellStr; Convas.Xdwin = 0; Convas.Ydwin = 0; Convas.Xswin = Convas.Wmax; Convas.Yswin = Convas.Hmax; }
Cell SystemSwitch(void) {
  if (VRam.SystemSwitch) { VRam.size = SizeVram; if (!(VRam.addr = GetRam(&VRam.size))) return Off;
    VRam.SystemSwitch--; SWD(VRam.addr); InitVram(VRam.addr,VRam.size); SwitchRaw(); Delay_ms(Off);
    SyncSize(VRam.addr); Print(Cdefault,AltBufOn Reset HideCur WrapOff Cls MouseX10on); }
  else { VRam.SystemSwitch++; if (VRam.size) { SwitchRaw(); Print(Cdefault,AltBufOff Reset ShowCur WrapOn MouseX10off);
    FreeRam(VRam.addr, VRam.size); } }
  return On; }

uint8_t PushKey(uint8_t *key) {
  uint8_t *sav, *dst, d, l, c, data = UTFinfo(key); c = (data & b5) ? *(key + 1) : 0; if (data & b7) return 0;
  if (Buf.pop == Buf.push) { dst = KeyBuf(++Buf.push);
    *dst++ = data | b7; *dst = On; dst += 3; l = 1 + (data & b10);
    if (c) *dst = c;
    else while(l--) *(dst + l) = *(key + l); }
  else {
    dst = KeyBuf(Buf.push); d = *dst++; sav = dst; dst += 3; l = 1 + (data & b10);
    if (d & b6) { d = *++sav; sav++; dst += 2; }
    if ((d & ~b76) == data) {
      if (c) { if (*dst == c) l = ~Off; }
      else while(l--) { if (*(dst + l) != *(key + l)) break; } }
    if (l == (uint8_t)~Off) { if (!(*sav += 1)) *sav = ~Off; }
    else { dst = KeyBuf(Buf.push); d = *dst;
      if (!(d & b6) && (d & b10) < 2 && (data & b10) < 2) { *dst |= b6; dst += 2; }
      else { dst = KeyBuf(++Buf.push); if (Buf.pop == Buf.push) ++Buf.pop; }
      *dst++ = data | b7; *dst = On; dst += 3; l = 1 + (data & b10);
      if (c) *dst = c;
      else while(l--) *(dst + l) = *(key + l); } }
  if (!c && !(*key & b7)) c = *key;
  return c; }
uint8_t ShowKey(uint8_t *data, uint8_t *count, uint8_t *key) {
  if (Buf.pop == Buf.push) { *data = Off; *count = Off; return Off; }
  uint8_t d, *dst; dst = KeyBuf(Buf.push); d = *dst++; 
  if (d & b6) { dst++; d = *dst++; }
  *count = *dst; *data = (d & ~b76); dst += 3; d = 1 + (d & b10); while(d--) *(key + d) = *(dst + d);
  return On; }
uint8_t PopKey(uint8_t *data, uint8_t *count, uint8_t *key) {
  uint8_t *dst, d;
  while(!((d = *(dst = KeyBuf(Buf.pop))) & b76) && (Buf.pop != Buf.push)) Buf.pop++;
  if (!(d & b76)) { *data = Off; *count = Off; return Off; }
  if (Buf.pop == Buf.push) Buf.pop--;
  if (d & b7) *dst &= ~b7;
  else { *dst &= ~b6; dst += 2; d = *dst; }
  *count = *++dst; *data = (d & ~b76); dst += 3; d = 1 + (d & b10); while(d--) *(key + d) = *(dst + d);
  return On; }
void ForgetKey(void) {
  if (Buf.pop == Buf.push) return;
  uint8_t *dst = KeyBuf(Buf.push--); Buf.tic--; if (*dst & b6) { Buf.push++; *dst &= ~b6; } }
ugoc Keys(void) {
  ugoc s = 0; uint8_t d, c = Buf.push; while (c != Buf.pop) { d = *KeyBuf(c--); if (d & b7) s++; if (d & b6) s++; }
  return s; }

uint8_t Move(goc dx, goc dy) {
  uint8_t t = 0; ugoc r, c = TermCR(&r); goc x = VP.X, y = VP.Y; VP.X += dx * VP.dXY; VP.Y += dy * VP.dXY;
  if (VP.Mode & b1) {  }
  else {
    if ((x < -MaxSpeed || x > MaxSpeed) && ((x ^ VP.X) & GOC_MIN)) VP.X = (VP.X < Off) ? GOC_MAX : GOC_MIN;
    if ((y < -MaxSpeed || y > MaxSpeed) && ((y ^ VP.Y) & GOC_MIN)) VP.Y = (VP.Y < Off) ? GOC_MAX : GOC_MIN;
    if ((ugoc)VP.X % c != VP.Xs || (ugoc)VP.Y % r != VP.Ys) t++;
    VP.Xs = (ugoc)VP.X % c; VP.Ys = (ugoc)VP.Y % r; }
  return t; }
uint8_t Mouse(uint8_t key, uint8_t x, uint8_t y) {
  uint8_t t = 0, p = 0; goc dx = 0, dy = 0; Buf.Mkey = key; Buf.MX = x - 32; Buf.MY = y - 32;
  if (Buf.Mkey == Buf.Ru) dy--;
  else if (Buf.Mkey == Buf.Rd) dy++;
  else if (Buf.Mkey == Buf.cRu) dx++;
  else if (Buf.Mkey == Buf.cRd) dx--;
  if (dx || dy) t = Move(dx,dy);
  if (Buf.Mkey == Buf.Lk) { Buf.LkX = Buf.MX; Buf.LkY = Buf.MY; p++; }
  else if (Buf.Mkey == Buf.Mk) { Buf.MkX = Buf.MX; Buf.MkY = Buf.MY; p++; }
  else if (Buf.Mkey == Buf.Rk) { Buf.RkX = Buf.MX; Buf.RkY = Buf.MY; p++; }
  if (p) { VP.X += (goc)Buf.MX - On - (goc)VP.Xs; VP.Y += (goc)Buf.MY - On - (goc)VP.Ys;
    VP.Xs = Buf.MX - On; VP.Ys = Buf.MY - On; t++; }
  return t; }
uint8_t GetEventKM(uint8_t *num, uint8_t *tic, uint8_t *control) {
  uint8_t t, c = 0; *control = 0; *tic = Buf.tic; GetKey(Buf.key);
  if (!(*Buf.key & b7)) c = *Buf.key;
  if (*Buf.key == K_ESC) { c = *(Buf.key + 1); if (c == K_NO) return c; }
  if (c == K_Mouse) *control = Mouse(*(Buf.key + 2),*(Buf.key + 3),*(Buf.key + 4));
  if (c && *num < K_Max) { t = *num++; while (t--) if (*num++ == c) { *control = On; break; } }
  if (!(*control && (Buf.mode & b0))) c = PushKey(Buf.key);
  *tic = ++Buf.tic; return c; }

uint8_t ViewPort(void) {
  uint8_t control, s = Buf.mode; goc dx = 0, dy = 0; Buf.mode |= b0; if (VP.Mode & b1) Buf.mode--;
  VP.Cod = GetEventKM(&VP.Key, &VP.Tic, &control); Buf.mode = s;
  if (control && VP.Cod != K_Mouse) {
    if (VP.Cod == VP.F11) { VP.Mode ^= b1;
      if (!(VP.Mode & b1)) { VP.Win = Off; ForgetKey(); } }
    if (VP.Cod != VP.oCod) { VP.dXY = On; VP.oCod = VP.Cod; }
    if (VP.Cod == VP.le || VP.Cod == VP.ri || VP.Cod == VP.up || VP.Cod == VP.ud || VP.Cod == VP.cle || VP.Cod == VP.cri || VP.Cod == VP.cup || VP.Cod == VP.cdo) {
      if ((VP.Tic > 7) && !(VP.Tic & b10) && (VP.dXY < MaxSpeed)) VP.dXY <<= On;
      if (VP.Cod == VP.le || VP.Cod == VP.cle) dx--;
      else if (VP.Cod == VP.ri || VP.Cod == VP.cri) dx++;
      else if (VP.Cod == VP.up || VP.Cod == VP.cup) dy--;
      else dy++;
      control += Move(dx,dy); } }
  if (SyncSize(VRam.addr)) control += Move(Off,Off);
  if (Vector(Off)) { Convas.W = Menu(Off)->Win; Vector(Off)(); }
  if (Vector(VP.Cod)) { Convas.W = Menu(VP.Cod)->Win; Vector(VP.Cod)(); }
  if (control > 1) { control--; }
  else { control--; }
  return VP.Loop; }

void WinView(uint16_t n, goc x, goc y) {
  if (Convas.Flag || (n > Convas.Dwin && n < Convas.Swin)) return;
  WindowData* w = Win(n); w->Xrender = x; w->Yrender = y; }
void WinTop(uint16_t n) {
  if (Convas.Flag || (n > Convas.Dwin && n < Convas.Swin)) return;
  ugoc l = Convas.Dwin; if (n > Convas.Dwin) l = Convas.WinMax - 1;
  WindowData* w = Win(n); w->Layer = l; l += 1 - n;
  while(--l) { w = Win(n + l); --w->Layer; } }
uint16_t _Window(int8_t col, uint8_t count, ugoc *args) {
  ugoc l, c = 0, h = 0; uint16_t n = ++Convas.Dwin; Convas.Flag = 0; WindowData* w = Win(n); if (count) { h = args[0]; if (--count) c = args[1]; }
  if (col < 0) { n = --Convas.Swin; --Convas.Dwin;
    if (n < 1) { Convas.Swin = Convas.WinMax - 1; n = Convas.Swin; Convas.Xswin = Convas.Wmax; Convas.Yswin = Convas.Hmax; }
    w = Win(n); w->Flags = (((-col) & Mcbi) | b7); w->Layer = Convas.WinMax - 1; l = Convas.WinMax - n;
    while(--l) { WindowData* d = Win(n + l); --d->Layer; } }
  else { if (n >= Convas.Swin) { n = 0; Convas.Dwin = 0; Convas.Xdwin = 0; Convas.Ydwin = 0; } w = Win(n); w->Flags = ((col & Mcbi) | b65); w->Layer = n; }
  w->Key = 0; w->W = c; w->H = h; w->parent = n; w->child = n; w->MaxCs = 0; w->MaxVs = 0; w->MaxH = 0;
  w->XCur = 0; w->YCur = 0; w->WFirstSR = Convas.Hmax; w->Xconvas = Convas.Wmax; w->Yconvas = Convas.Hmax; w->Xrender = 0; w->Yrender = 0; return n; }
void _WSet(uint16_t n, uint8_t cur, uint8_t count, AFunction *args) {
  if (Convas.Flag || (n > Convas.Dwin && n < Convas.Swin)) return;
  WindowData* w = Win(n); if (w->Flags & b7) { if (cur) w->Key = cur;
    if (count--) { Vector(cur) = args[0]; Menu(cur)->Win = n;
      if (Menu(cur)->CountMenu && count) {  } } return; }
  w->Flags &= ~b5; if (cur) w->Flags |= b5;
  if (count) { w->Flags &= ~b6; if (args[0]) w->Flags |= b6; } }
void _WData(uint16_t n, char    *str, uint8_t count, goc *args) {
  if (Convas.Flag || (n > Convas.Dwin && n < Convas.Swin)) return;
  WindowData* w = Win(n); if (w->Xconvas == Convas.Wmax) { ugoc c = w->W, r = w->H; (void)r;
    if (!c) { } } 
  (void)*str; (void)count; (void)*args; }
