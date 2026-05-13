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

void UTFinfo(uint8_t *s) {
  uint32_t cp; uint8_t c = *s++; Buf.Data = Off;
  if (c < 0x80) cp = (uint32_t) c;
  else if ((c & 0xE0) == 0xC0 && (*s & 0xC0) == 0x80)
    { Buf.Data++; cp = ((c & 0x1F) << 0x06) | (*s & 0x3F); }
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
void UTFinfoTile(uint8_t *s, Cell len) {
  Buf.Data = 0xC0; if (!len) return;
  if ((*s & 0xE0) == 0xC0 && len < 0x02) return;
  else if ((*s & 0xF0) == 0xE0 && len < 0x03) return;
  else if ((*s & 0xF8) == 0xF0 && len < 0x04) return;
  UTFinfo(s); }

void PushKey(void) {
  uint8_t *sav, *dst, d, l; if (Buf.Data & b7) { Buf.Cod = Off; return; }
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
      else while(l--) *(dst + l) = *(Buf.Key + l); } }
  if (!Buf.Cod) Buf.Cod--; }
uint8_t ShowKey(void) {
  if (Buf.pop == Buf.push) { Buf.Data = Off; Buf.Count = Off; return Off; }
  uint8_t d, *dst; dst = AKey(Buf.push); d = *dst++; if (d & b6) { dst++; d = *dst++; }
  Buf.Count = *dst; Buf.Data = (d & ~b76); dst += b10; d = On + (d & b10); while(d--) *(Buf.Key + d) = *(dst + d);
  return On; }
uint8_t PopKey(void) {
  uint8_t *dst, d; while(!((d = *(dst = AKey(Buf.pop))) & b76) && (Buf.pop != Buf.push)) Buf.pop++;
  if (!(d & b76)) { Buf.Data = Off; Buf.Count = Off; return Off; } if (Buf.pop == Buf.push) Buf.pop--;
  if (d & b7) { *dst &= ~b7; } else { *dst &= ~b6; dst += b1; d = *dst; }
  Buf.Count = *++dst; Buf.Data = (d & ~b76); dst += b10; d = On + (d & b10); while(d--) *(Buf.Key + d) = *(dst + d);
  return On; }
ugoc Keys(void) {
  ugoc s = Off; uint8_t d, c = Buf.push; while (c != Buf.pop) { d = *AKey(c--); if (d & b7) s++; if (d & b6) s++; }
  return s; }

void Print(uint8_t n, char *str) {
  if (!n || !str) return;
  char *dst = Cdbuf; n--; n &= ~b765; PalBuf* pal =  APal(n); MemCpy(dst, pal->d, pal->l); dst += pal->l;
  ugoc len = StrLen(str); MemCpy(dst, str, len); SysWrite(Cdbuf, dst + len - Cdbuf); }
void BPrint(uint8_t n, char *str) {
  if (!str) return;
  char *dst = Cdbuf; n -= 0x21; n &= b210; PalBuf* pal =  APal(n + 0x20); MemCpy(dst, pal->d, pal->l); dst += pal->l;
  ugoc len = StrLen(str); MemCpy(dst, str, len); 
  if (n + On != Convas.Inc) { dst += len; pal =  APal(Convas.Inc - On); len = pal->l; MemCpy(dst, pal->d, len); }
  SysWrite(Cdbuf, dst + len - Cdbuf); }
void InitVram(Cell addr, Cell size) { if (!addr || (size < SizeVram)) return;
  uint8_t i, c = 4; uint8_t* base = (uint8_t*)addr; PalBuf *pal, *mode, *src;
  char* colors[] = { Black, Blue, Red, Grey, Green, Orange, Gold, White}; char* modes[] = { "\7;22;27m", "\6;22;7m", "\6;1;27m", "\5;1;7m" };
  Cdata = (char*)base; Cinfo = (uint8_t*)(base + SDCell); Cds = Cinfo + SInfo; Coffset = (ugoc*)(Cds + SDs); Cdpal = (char*)(Coffset + SOffset);
  Cdkey = (uint8_t*)(Cdpal + SPal); Cdcon = (ugoc*)(Cdkey + SKeys); Cdwin = Cdcon + SConvas; Cvsw = Cdwin + SWin; Ccsw = Cvsw + SVsw;
  Cevent = (char*)(Ccsw + SCsw); Cexec = Cevent + SEvent; Cdbuf = Cexec + SExec;
  while(c) { mode = (PalBuf*)modes[--c]; i = 8;
    while(i--) { pal = APal((i << 2) + c); src = APal(i + 0x20); src->l = StrLen(colors[i]); MemCpy(src->d, colors[i], src->l);
      pal->l = src->l + mode->l - 1; MemCpy(pal->d, src->d, src->l - 1); MemCpy(pal->d + src->l - 1, mode->d, mode->l); *(src->d + b1) = '4'; } }
  VP.Win = MAX_WIN; Convas.Win = VP.Win; Convas.Min = Off; Convas.Max = VP.Win; Convas.D = Off; Convas.S = VP.Win; Convas.CW = CellLine; Convas.W = Convas.CW;
  Convas.CH = CellStr; Convas.H = Convas.CH; Convas.Inc = BWhite; Convas.Border = BBlack; VP.Mode = b2; VP.Loop = On; Vector(K_Mouse) = RPEncode; }
Cell SystemSwitch(void) {
  if (VRam.SystemSwitch) { VRam.size = SizeVram; if (!(VRam.addr = GetRam(&VRam.size))) return Off;
    VRam.SystemSwitch--; SWD(VRam.addr); InitVram(VRam.addr,VRam.size); SwitchRaw(); Delay_ms(Off);
    IRnd(); SyncSize(VRam.addr); Print(Convas.Inc,AltBufOn HideCur WrapOff MouseX10on); BPrint(Convas.Border, HideCur Cls); }
  else { VRam.SystemSwitch++; if (VRam.size) { SwitchRaw(); Print(Convas.Inc,AltBufOff Reset ShowCur WrapOn MouseX10off);
    FreeRam(VRam.addr, VRam.size); } }
  return On; }

void MoveConvas(goc dx, goc dy) {
  ugoc r, c = TermCR(&r); goc x = VP.X + dx, y = VP.Y + dy; Buf.Ctrl = On;
  if (VP.Mode & b1) { return; }
  else {
    if ((VP.X < -MaxSpeed || VP.X > MaxSpeed) && ((x ^ VP.X) & GOC_MIN)) x = (x < Off) ? GOC_MAX : GOC_MIN;
    if ((VP.Y < -MaxSpeed || VP.Y > MaxSpeed) && ((y ^ VP.Y) & GOC_MIN)) y = (y < Off) ? GOC_MAX : GOC_MIN; }
  VP.X = x; VP.Y = y; dx = x / c; dy = y / r; x = (x < Off) ? (c + (x % c)) : (x % c); y = (y < Off) ? (r + (y % r)) : (y % r);
  if ((x / c) != dx || (y / r) != dy) Buf.Ctrl++;
  VP.Xs = x; VP.Ys = y; }
uint8_t MoveScreen(goc mx, goc my) {
  goc dx = VP.X - mx, dy = VP.Y - my;
  if (VP.Mode & b1) { return Off; }
  else if (((dx ^ VP.X) & GOC_MIN) || ((dy ^ VP.Y) & GOC_MIN)) return Off;
  VP.X = dx; VP.Xs -= mx; VP.Y = dy; VP.Ys -= my; return On; }
void Mouse(void) {
  uint8_t p = Off; Buf.Mkey = *(Buf.Key + 2); Buf.MX = *(Buf.Key + 3) - 0x21; Buf.MY = *(Buf.Key + 4) - 0x21;
  if (Buf.Mkey == Buf.Ru) Buf.Cod = VP.up;
  else if (Buf.Mkey == Buf.Rd) Buf.Cod = VP.ud;
  else if (Buf.Mkey == Buf.cRu) Buf.Cod = VP.ri;
  else if (Buf.Mkey == Buf.cRd) Buf.Cod = VP.le;
  else if (Buf.Mkey == Buf.Lk) { Buf.LkX = Buf.MX; Buf.LkY = Buf.MY; p++; }
  else if (Buf.Mkey == Buf.Mk) { Buf.MkX = Buf.MX; Buf.MkY = Buf.MY; p++; }
  else if (Buf.Mkey == Buf.Rk) { Buf.RkX = Buf.MX; Buf.RkY = Buf.MY; p++; }
  if (p && MoveScreen(VP.Xs - Buf.MX, VP.Ys - Buf.MY)) {  } 
  return; }
uint8_t ViewPort(void) {
  Buf.Ctrl = Off; if (Vector(Off)) { VP.Wec = Event(Off)->W; Vector(Off)(); }
  Vector(K_Mouse)(); if (*Buf.Key == K_ESC && *(Buf.Key + On) == K_NO) Buf.Cod = Off;
  else { Buf.Cod = (*Buf.Key == K_ESC) ? *(Buf.Key + On) : (*Buf.Key & b7) ? Off : *Buf.Key;
    if (Buf.Cod == K_Mouse) Mouse();
    if (Buf.Cod < K_Mouse) {
      if (Buf.Cod) {
        if (Vector(Buf.Cod)) { VP.Wec = Event(Buf.Cod)->W; Vector(Buf.Cod)(); }
        uint8_t *p = &VP.Key, t = *p++; while (t--) if (*p++ == Buf.Cod) { Buf.Ctrl++; break; } }
      if (!Buf.Ctrl) PushKey(); }
    if (Buf.Cod) ++Buf.tic; }
  if (Buf.Ctrl) { goc dx = Off, dy = Off;
    if (Buf.Cod == VP.scs) { if (!(VP.Mode ^= b2)) VP.dXY = On; }
    else if (Buf.Cod == VP.Anchor) { if (VP.Mode ^= b1) { } else { Convas.W = Convas.CW; Convas.H = Convas.CH; } }
    else if (Buf.Cod == VP.bcu) { Convas.Border -= 0x21; Convas.Border++; Convas.Border &= b210; Convas.Border += 0x21; BPrint(Convas.Border,Cls); }
    else if (Buf.Cod == VP.ssc) VP.Mode ^= b0;
    else if (Buf.Cod == VP.Exit) VP.Loop = Off;
    else {
      if (Buf.Cod != VP.Cod) { VP.dXY = On; VP.Cod = Buf.Cod; }
      if ((VP.Mode & b2) && ((Buf.tic > 7) && !(Buf.tic & b10) && (VP.dXY < MaxSpeed))) VP.dXY <<= On;
      if (Buf.Cod == VP.le) dx = -VP.dXY;
      else if (Buf.Cod == VP.ri) dx = VP.dXY;
      else if (Buf.Cod == VP.up) dy = -VP.dXY;
      else if (Buf.Cod == VP.ud) dy = VP.dXY;
      MoveConvas(dx, dy); } }
  if (SyncSize(VRam.addr)) { MoveConvas(Off, Off); BPrint(Convas.Border,Cls); }
  if (Buf.Ctrl > On) { BPrint(Convas.Border,Cls); }
  else {  }
  return VP.Loop; }

void IRnd(void) { VP.Rnd = GetDelay() | On; }
ugoc Rand(ugoc n) { return (ugoc)(((Cell)(VP.Rnd = (ugoc)(RNG_A * VP.Rnd + RNG_B)) * n) >> (sizeof(ugoc) * 8)); }

void RPEncode(void) { GetKey(Buf.Key); UTFinfo(Buf.Key); }
void Nop(void) { }
void Anchor(void) { if (VP.Mode ^= b1) { } else { Convas.W = Convas.CW; Convas.H = Convas.CH; } }
void Bye(void) { VP.Loop = Off; }
void Adaptive(void) { if (Win(VP.Wec)->WF ^= b7) WinView(VP.Wec); }
void WinDown(void) { if (Convas.D) { uint16_t l = Convas.D; Win(l)->Layer = Off; while(l) ++Win(--l)->Layer; } }
void WinUp(void) { if (Convas.D) { uint16_t l = Convas.D; Win(Off)->Layer = --l; while(l) --Win(--l)->Layer; } }

void WinTop(uint16_t n) { uint16_t l = Convas.D;
  if ((n >= Convas.D && n < Convas.S) || n >= Convas.Win) return;
  if (n > l) { l = Convas.Max; } Win(n)->Layer = l; l += On - n; while(--l) --Win(n + l)->Layer; }
void _WView(uint16_t n, uint8_t count, goc *args) { goc x = Off, y = Off; Windows* w = Win(n);
  if ((n >= Convas.D && n < Convas.S) || n >= Convas.Win) return;
  if (count > On) { x = args[Off]; if (!(y = args[On])) x = Off;
    if (x && !(w->EF & b0)) { x = (x < Off) ? -x : x; y = (y < Off) ? -y : y;
      if ((ugoc)VP.X >= Convas.W || (ugoc)VP.Y >= Convas.H) return; } }
  if (count == Off) { x = VP.Xs + On; y = VP.Ys + On;
    if (w->EF & b0) { ugoc r, c = TermCR(&r); if (c < (x + w->W)) { x = -On; } if (r < (y + w->H)) y = -On; } 
    else if ((ugoc)VP.X >= Convas.W || (ugoc)VP.Y >= Convas.H) return; }
  w->Xr = x; w->Yr = y; if (w->Xr) { w->WF |= b7; } else { w->WF &= ~b7; } }
uint16_t _Window(int8_t col, uint8_t count, ugoc *args) { uint16_t l, n; Windows* w;
  if (col < Off) { n = --Convas.S; if (n < On) { Convas.S = Convas.Max; n = --Convas.S; }
    w = Win(n); w->WF = ((((-col) - On) & ~b765) | b6); w->EF = On; w->Layer = Convas.Max; l = On + Convas.Max - n; while(--l) --Win(n + l)->Layer; }
  else { if (!col) { Convas.Min = Convas.D; Convas.Max = Convas.S; return Convas.Win; }
    n = Convas.D++; if (n >= Convas.S) { Convas.D = Convas.Min; n = Convas.D++; } w = Win(n); w->WF = (((col - On) & ~b765) | b5); w->EF = Off; w->Layer = n; }
  w->parent = n; w->child = n; w->MaxVs = Off; w->XCur = Off; w->YCur = Off; w->WFirstSR = CellStr; w->Xr = Off; w->Yr = Off; w->W = Off; w->H = Off;
  if (count > On) { w->Xr = args[0]; if (!(w->Yr = args[1])) w->Xr = Off; } if (count > 2) { w->W = args[2]; if (count > 3) w->H = args[3]; }
  if (!w->EF) { if (w->W < b1) { w->W = b1; } if (!w->H) { w->H++; } } if (w->Xr) { w->WF |= b7; } return n; }
void _WEvent(uint16_t n, uint8_t cur, uint8_t count, AFunction *args) {
  if ((n >= Convas.D && n < Convas.S) || n >= Convas.Win) return;
  if ((Win(n)->EF & b0) && count--) { Event(cur)->W = n; Vector(cur) = ((Cell)args[Off] <= (Cell)Nop) ? Off : args[Off];
    if (Event(cur)->C && count) { uint8_t j, c = Event(cur)->C, i = On;
      while(c-- && count--) { j = K_Mouse;
        while(--j) { if (Event(j)->W == n && Event(j)->N == i) { Vector(j) = ((Cell)args[i] <= (Cell)Nop) ? Off : args[i]; i++;
          break; } } 
        } } } }
void _WSet(uint16_t n, uint8_t count, uint8_t *args) {
  if ((n >= Convas.D && n < Convas.S) || n >= Convas.Win) return;
  if (count--) { Windows* w = Win(n); w->WF &= ~b5; if (args[Off]) { w->WF |= b5; } if (count) { w->WF &= ~b6; if (args[On]) { w->WF |= b6; } } } }
void _SEvent(uint8_t count, uint8_t *args) { uint8_t a, k = count; while(k--) { a = args[k]; Event(a)->W = Convas.Win; Event(a)->C = count; Event(a)->N = k; } }
void _SExec(uint8_t count, AFunction *args) { uint16_t m = Window(Off), k = K_Mouse;
  while(k--) { if (Event(k)->W == m) {
    if (Event(k)->C && count) { uint8_t j, c = Event(k)->C, i = Off;
      while(c-- && count--) { j = K_Mouse;
        while(--j) { if (Event(j)->W == m && Event(j)->N == i) { Vector(j) = ((Cell)args[i] <= (Cell)Nop) ? Off : args[i]; i++;
          break; } } 
        } }
    break; } } }
void _WData(uint16_t n, char *str, uint8_t count, ugoc *args) {
  if ((n >= Convas.D && n < Convas.S) || n >= Convas.Win) return;
  Windows* w = Win(n); if (!(w->MaxVs)) {  } (void)*str; (void)count; (void)*args; }
