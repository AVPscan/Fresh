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
  return Off; }

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

uint8_t PushKey(uint8_t c, uint8_t *key) {
  uint8_t *sav, *dst, d, l, data = UTFinfo(key); if (data & b7) return Off;
  if (Buf.pop == Buf.push) { dst = AKey(++Buf.push);
    *dst++ = data | b7; *dst = On; dst += b10; l = On + (data & b10);
    if (c) *dst = c;
    else while(l--) *(dst + l) = *(key + l); }
  else {
    dst = AKey(Buf.push); d = *dst++; sav = dst; dst += b10; l = On + (data & b10);
    if (d & b6) { d = *++sav; sav++; dst += b1; }
    if ((d & ~b76) == data) {
      if (c) { if (*dst == c) l = ~Off; }
      else while(l--) { if (*(dst + l) != *(key + l)) break; } }
    if (l == (uint8_t)~Off) { if (!(*sav += On)) *sav = ~Off; }
    else { dst = AKey(Buf.push); d = *dst;
      if (!(d & b6) && (d & b10) < b1 && (data & b10) < b1) { *dst |= b6; dst += 2; }
      else { dst = AKey(++Buf.push); if (Buf.pop == Buf.push) ++Buf.pop; }
      *dst++ = data | b7; *dst = On; dst += b10; l = On + (data & b10);
      if (c) *dst = c;
      else while(l--) *(dst + l) = *(key + l); } }
  if (!c) c = ~Off;
  return c; }
uint8_t ShowKey(uint8_t *data, uint8_t *count, uint8_t *key) {
  if (Buf.pop == Buf.push) { *data = Off; *count = Off; return Off; }
  uint8_t d, *dst; dst = AKey(Buf.push); d = *dst++; 
  if (d & b6) { dst++; d = *dst++; }
  *count = *dst; *data = (d & ~b76); dst += b10; d = On + (d & b10); while(d--) *(key + d) = *(dst + d);
  return On; }
uint8_t PopKey(uint8_t *data, uint8_t *count, uint8_t *key) {
  uint8_t *dst, d;
  while(!((d = *(dst = AKey(Buf.pop))) & b76) && (Buf.pop != Buf.push)) Buf.pop++;
  if (!(d & b76)) { *data = Off; *count = Off; return Off; }
  if (Buf.pop == Buf.push) Buf.pop--;
  if (d & b7) *dst &= ~b7;
  else { *dst &= ~b6; dst += b1; d = *dst; }
  *count = *++dst; *data = (d & ~b76); dst += b10; d = On + (d & b10); while(d--) *(key + d) = *(dst + d);
  return On; }
ugoc Keys(void) {
  ugoc s = Off; uint8_t d, c = Buf.push; while (c != Buf.pop) { d = *AKey(c--); if (d & b7) s++; if (d & b6) s++; }
  return s; }

void Print(uint8_t n, char *str) {
  char *dst = Cdbuf; n &= ~b765; if (!str) return;
  PalBuf* pal =  APal(n); MemCpy(dst, pal->d, pal->l); dst += pal->l;
  ugoc len = StrLen(str); MemCpy(dst, str, len); dst += len;
  if (n != Cconvas) { pal =  APal(Cconvas); MemCpy(dst, pal->d, pal->l); dst += pal->l; }
  SysWrite(Cdbuf, dst - Cdbuf); }
void InitVram(Cell addr, Cell size) { if (!addr || (size < SizeVram)) return;
  uint8_t cbi, c = StrLen(Reset), i = 8; uint8_t* base = (uint8_t*)addr; PalBuf *pal, *mode, *src;
  char* colors[] = { Reset, Grey, Green, Red, Blue, Orange, Gold, Reset }; char* modes[] = { "\7;22;27m", "\6;22;7m", "\6;1;27m", "\5;1;7m" };
  Cdata = (char*)base; Cinfo = (uint8_t*)(base + SDCell); Cds = Cinfo + SInfo; Coffset = (ugoc*)(Cds + SDs);
  Cdpal = (char*)(Coffset + SOffset); Cdkey = (uint8_t*)(Cdpal + SPal); Cevent = (char*)(Cdkey + SKeys); Cexec = Cevent + SEvent;
  Cdcon = (ugoc*)(Cexec + SExec); Cdwin = Cdcon + SConvas; Cvsw = Cdwin + SWin; Ccsw = Cvsw + SVsw; Cdbuf = (char*)(Ccsw + SCsw);
  while (i--) { pal = (PalBuf*)(Cdbuf + (i << 5)); pal->l = c; MemCpy(pal->d, Reset, pal->l);
    if (StrLen(colors[i])) { pal->l = StrLen(colors[i]); MemCpy(pal->d, colors[i], pal->l); } }
  i = 4; while(i) { mode = (PalBuf*)modes[--i]; c = 8; while(c) { cbi = (--c << 2) + i; src = (PalBuf*)(Cdbuf + ((c) << 5)); pal = APal(cbi);
      pal->l = src->l + mode->l - 1; MemCpy(pal->d, src->d, src->l - 1); MemCpy(pal->d + src->l - 1, mode->d, mode->l); } }
  VP.Win = MAX_WIN; Convas.Win = VP.Win; Convas.Min = Off; Convas.Max = VP.Win; Convas.D = Off; Convas.S = VP.Win; VP.Mode = On; VP.Loop = On;
  VP.Anchor = K_F11; VP.Exit = K_F12; Exec(VP.Anchor) = Anchor; Exec(VP.Exit) = Bye; Exec(K_ALT_TAB) = WinShow; Exec(K_ALT_ENT) = WinRev; }
Cell SystemSwitch(void) {
  if (VRam.SystemSwitch) { VRam.size = SizeVram; if (!(VRam.addr = GetRam(&VRam.size))) return Off;
    VRam.SystemSwitch--; SWD(VRam.addr); InitVram(VRam.addr,VRam.size); SwitchRaw(); Delay_ms(Off);
    IRnd(); SyncSize(VRam.addr); Print(Cdefault,AltBufOn Reset HideCur WrapOff Cls MouseX10on); }
  else { VRam.SystemSwitch++; if (VRam.size) { SwitchRaw(); Print(Cdefault,AltBufOff Reset ShowCur WrapOn MouseX10off);
    FreeRam(VRam.addr, VRam.size); } }
  return On; }

uint8_t MoveConvas(ugoc *sx, ugoc *sy, goc *cx, goc *cy, goc dx, goc dy) {
  uint8_t t = Off; ugoc r, c = TermCR(&r); goc x = *cx, y = *cy; x += dx; y += dy;
  if ((*cx < -MaxSpeed || *cx > MaxSpeed) && ((x ^ *cx) & GOC_MIN)) x = (x < Off) ? GOC_MAX : GOC_MIN;
  if ((*cy < -MaxSpeed || *cy > MaxSpeed) && ((y ^ *cy) & GOC_MIN)) y = (y < Off) ? GOC_MAX : GOC_MIN;
  *cx = x; *cy = y; dx = x / c; dy = y / r; x = (x < Off) ? (c + (x % c)) : (x % c); y = (y < Off) ? (r + (y % r)) : (y % r);
  if ((x / c) != dx || (y / r) != dy) t++;
  *sx = x; *sy = y; return t; }
uint8_t MoveScreen(ugoc *sx, ugoc *sy, goc *cx, goc *cy, goc mx, goc my) {
  goc dx = *cx - mx, dy = *cy - my;
  if ((dx ^ *cx) & GOC_MIN) return Off;
  if ((dy ^ *cy) & GOC_MIN) return Off;
  *cx = dx; *sx -= mx; *cy = dy; *sy -= my; return On; }
uint8_t Mouse(uint8_t c, uint8_t key, uint8_t x, uint8_t y) {
  uint8_t p = Off; Buf.Mkey = key; Buf.MX = x - 0x21; Buf.MY = y - 0x21;
  if (Buf.Mkey == Buf.Ru) c = VP.up;
  else if (Buf.Mkey == Buf.Rd) c = VP.ud;
  else if (Buf.Mkey == Buf.cRu) c = VP.ri;
  else if (Buf.Mkey == Buf.cRd) c = VP.le;
  else if (Buf.Mkey == Buf.Lk) { Buf.LkX = Buf.MX; Buf.LkY = Buf.MY; p++; }
  else if (Buf.Mkey == Buf.Mk) { Buf.MkX = Buf.MX; Buf.MkY = Buf.MY; p++; }
  else if (Buf.Mkey == Buf.Rk) { Buf.RkX = Buf.MX; Buf.RkY = Buf.MY; p++; }
  if (p && MoveScreen(&VP.Xs, &VP.Ys, &VP.X, &VP.Y, VP.Xs - Buf.MX, VP.Ys - Buf.MY)) c = K_Mouse; // Извлечение кода пункта меню если над ним
  return c; }
uint8_t GetEventKM(uint8_t *num, uint8_t *tic, uint8_t *control) {
  uint8_t t, c; *control = Off; *tic = Buf.tic; GetKey(Buf.key); if (*Buf.key == K_ESC && *(Buf.key + On) == K_NO) return Off;
  if ((c = (*Buf.key == K_ESC) ? *(Buf.key + On) : (*Buf.key & b7) ? Off : *Buf.key) != Off) {
    if (c == K_Mouse) c = Mouse(c, *(Buf.key + 2), *(Buf.key + 3), *(Buf.key + 4));
    if (*num < K_Mouse) { t = *num++; while (t--) if (*num++ == c) { *control = On; break; } }
    if (Exec(c)) { VP.Win = Event(c)->W; Exec(c)(); } }
  if (c < K_Mouse && !(*control && (Buf.mode & b0))) c = PushKey(c, Buf.key);
  *tic = ++Buf.tic; return c; }
void Nop(void) { }
void Anchor(void) { VP.Mode ^= b1; }
void Bye(void) { VP.Loop = Off; }
uint8_t ViewPort(void) { if (Exec(Off)) { VP.Win = Event(Off)->W; Exec(Off)(); }
  goc dx = Off, dy = Off; uint8_t control, s = Buf.mode; Buf.mode |= b0; if (VP.Mode & b1) Buf.mode--;
  VP.Cod = GetEventKM(&VP.Key, &VP.Tic, &control); Buf.mode = s;
  if (control) {
    if (VP.Cod != VP.oCod) { VP.dXY = On; VP.oCod = VP.Cod; }
    if ((VP.Tic > 7) && !(VP.Tic & b10) && (VP.dXY < MaxSpeed)) VP.dXY <<= On;
    if (VP.Cod == VP.le) dx--;
    else if (VP.Cod == VP.ri) dx++;
    else if (VP.Cod == VP.up) dy--;
    else if (VP.Cod == VP.ud) dy++;
    if (VP.Mode & b1) {  }
    else control += MoveConvas(&VP.Xs, &VP.Ys, &VP.X, &VP.Y, dx * VP.dXY, dy * VP.dXY); }
  if (SyncSize(VRam.addr)) control = On + MoveConvas(&VP.Xs, &VP.Ys, &VP.X, &VP.Y, Off, Off);
  if (control > On) { control--; }
  else { control--; }
  return VP.Loop; }

void Adaptive(void) { if ((Win(VP.Win)->WF ^= b7) & b7) WinView(VP.Win); }
void WinShow(void) { if (Convas.D) { uint16_t l = Convas.D; Win(l)->Layer = Off; while(l) ++Win(--l)->Layer; } }
void WinRev(void) { if (Convas.D) { uint16_t l = Convas.D; Win(Off)->Layer = --l; while(l) --Win(--l)->Layer; } }
void WinTop(uint16_t n) {
  if ((n >= Convas.D && n < Convas.S) || n >= Convas.Win) return;
  uint16_t l = Convas.D; if (n > l) { l = Convas.Max; } Win(n)->Layer = l; l += On - n; while(--l) --Win(n + l)->Layer; }
void _WinView(uint16_t n, uint8_t count, goc *args) {
  goc x = Off, y = Off; Windows* w = Win(n); if ((n >= Convas.D && n < Convas.S) || n >= Convas.Win) return;
  if (count > On) { x = args[Off]; if (!(y = args[On])) x = Off;
    if (x && !(w->EF & b0)) { x = (x < Off) ? -x : x; y = (y < Off) ? -y : y;
      if ((ugoc)VP.X > CellLine && (ugoc)VP.Y > CellStr) return; } }
  if (count == Off) { x = VP.Xs + On; y = VP.Ys + On;
    if (w->EF & b0) { ugoc r, c = TermCR(&r); if (c < (x + w->W)) { x = -On; } if (r < (y + w->H)) y = -On; } 
    else if ((ugoc)VP.X > CellLine && (ugoc)VP.Y > CellStr) return; }
  w->Xr = x; w->Yr = y; if (w->Xr) { w->WF |= b7; } else { w->WF &= ~b7; } }
uint16_t _Window(int8_t col, uint8_t count, ugoc *args) {
  uint16_t l, n; Windows* w; if (col < Off) { n = --Convas.S; if (n < On) { Convas.S = Convas.Max; n = --Convas.S; }
    w = Win(n); w->WF = (((-col) & ~b765) | b6); w->EF = On; w->Layer = Convas.Max; l = On + Convas.Max - n; while(--l) --Win(n + l)->Layer; }
  else { if (!col) { Convas.Min = Convas.D; Convas.Max = Convas.S; return MAX_WIN; }
    n = Convas.D++; if (n >= Convas.S) { Convas.D = Convas.Min; n = Convas.D++; } w = Win(n); w->WF = ((col & ~b765) | b5); w->EF = Off; w->Layer = n; }
  w->parent = n; w->child = n; w->MaxVs = Off; w->XCur = Off; w->YCur = Off; w->WFirstSR = CellStr; w->Xr = Off; w->Yr = Off; w->W = Off; w->H = Off;
  if (count > On) { w->Xr = args[0]; if (!(w->Yr = args[1])) w->Xr = Off; } if (count > 2) { w->W = args[2]; if (count > 3) w->H = args[3]; }
  if (!w->EF) { if (w->W < b1) { w->W = b1; } if (!w->H) { w->H++; } } if (w->Xr) { w->WF |= b7; } return n; }
void _WEvent(uint16_t n, uint8_t cur, uint8_t count, AFunction *args) {
  if ((n >= Convas.D && n < Convas.S) || n >= Convas.Win) return;
  if ((Win(n)->EF & b0) && count--) { Event(cur)->W = n; Exec(cur) = ((Cell)args[Off] <= (Cell)Nop) ? Off : args[Off];
    if (Event(cur)->C && count) { uint8_t j, c = Event(cur)->C, i = On;
      while(c-- && count--) { j = K_Mouse;
        while(--j) { if (Event(j)->W == n && Event(j)->N == i) { Exec(j) = ((Cell)args[i] <= (Cell)Nop) ? Off : args[i]; i++; break; } } } } } }
void _WinSet(uint16_t n, uint8_t count, uint8_t *args) {
  if ((n >= Convas.D && n < Convas.S) || n >= Convas.Win) return;
  if (count--) { Windows* w = Win(n); w->WF &= ~b5; if (args[Off]) w->WF |= b5; if (count) { w->WF &= ~b6; if (args[On]) w->WF |= b6; } } }
void _WData(uint16_t n, char *str, uint8_t count, ugoc *args) {
  if ((n >= Convas.D && n < Convas.S) || n >= Convas.Win) return;
  Windows* w = Win(n); if (!(w->MaxVs)) {  } (void)*str; (void)count; (void)*args; }
