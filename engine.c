/*
 * Copyright (C) 2026 Поздняков Алексей Васильевич
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 */

#include <stdio.h>  //snprintf для теста
#include "sys.h"

char      *Cdata      = NULL;
uint16_t  *Coffset    = NULL;
uint8_t   *Cattr      = NULL;
uint8_t   *Cvlen      = NULL;
uint8_t   *Clen       = NULL;
char      *Cpdat      = NULL;
char      *Ckbuf      = NULL;
char      *Cvdat      = NULL;
#define Data(r)       (Cdata + ((r) << Data_shift))
#define Offset(r, c)  (Coffset + ((r) << Offset_row_shift) + (c))
#define Attr(r, c)    (Cattr + ((r) << CellPow) + (c))
#define Visi(r, c)    (Cvlen + ((r) << CellPow) + (c))
#define Len(r, c)     (Clen + ((r) << CellPow) + (c))
#define Parse(cbi)    (Cpdat + ((cbi) << Parse_shift))
#define KeyBuf(n)     (Ckbuf + ((n) << KeyBuf_shift))

Cell StrLen(char *s) { if (!s) return 0;
    char *f = s; while (*f++);
    return (--f - s); }
void MemSet(void* buf, uint8_t val, Cell len) {
    uint8_t *p = (uint8_t *)buf; while (len && ((Cell)p & (SizeCell - 1))) { *p++ = val; len--; }
    if (len >= SizeCell) {
        Cell vW = val * ((Cell)-1 / 255); Cell *pW = (Cell *)p;
        Cell i = len / SizeCell; len &= (SizeCell - 1); while (i--) *pW++ = vW;
        p = (uint8_t *)pW; }
    while (len--) *p++ = val; }
void MemCpy(void* dst, void* src, Cell len) {
    uint8_t *d = (uint8_t *)dst; uint8_t *s = (uint8_t *)src;
    while (len && ((Cell)d & (SizeCell - 1))) { *d++ = *s++; len--; }
    if (len >= SizeCell && ((Cell)s & (SizeCell - 1)) == 0) {
        Cell *dW = (Cell *)d; Cell *sW = (Cell *)s; Cell i = len / SizeCell;
        len &= (SizeCell - 1); while (i--) *dW++ = *sW++;
        d = (uint8_t *)dW; s = (uint8_t *)sW; }
    while (len--) *d++ = *s++ ; }
void MemMove(void* dst, void* src, Cell len) {
    if (dst > src) { uint8_t *d = (uint8_t *)dst; uint8_t *s = (uint8_t *)src;
        d += len; s += len; while (len && ((Cell)d & (SizeCell - 1))) { *--d = *--s; len--; }
        if (len >= SizeCell && ((Cell)s & (SizeCell - 1)) == 0) {
            Cell *dW = (Cell *)d; Cell *sW = (Cell *)s; Cell i = len / SizeCell;
            len &= (SizeCell - 1); while (i--) *--dW = *--sW;
            d = (uint8_t *)dW; s = (uint8_t *)sW; } }
    else if (dst < src ) MemCpy(dst, src, len); }
int8_t MemCmp(void* dst, void* src, Cell len) {
    uint8_t *d = (uint8_t *)dst; uint8_t *s = (uint8_t *)src;
    while (len && ((Cell)d & (SizeCell - 1))) { len--; if (*d++ != *s++) return (int8_t)(*--d - *--s); }
    if (len >= SizeCell && ((Cell)s & (SizeCell - 1)) == 0) {
        Cell *dW = (Cell *)d; Cell *sW = (Cell *)s; Cell i = len / SizeCell;
        len %= SizeCell; while (i-- && (*dW++ == *sW++));
        if (i + 1) { --dW; --sW; len += SizeCell; }
        d = (uint8_t *)dW; s = (uint8_t *)sW; }
    while (len--) { if (*d++ != *s++) return (int8_t)(*--d - *--s); }
    return 0; }
uint8_t UTFinfo(char *s, uint8_t *len, uint8_t *Mrtl) {
    unsigned char c = *s++; uint32_t cp = 0xFFFD; *len = 1; *Mrtl = 0;
    if (c < 0x80) cp = (uint32_t) c;
    else if ((c & 0xE0) == 0xC0 && (*s & 0xC0) == 0x80)
            { *len = 2; cp = ((c & 0x1F) << 6) | (*s & 0x3F); }
    else if ((c & 0xF0) == 0xE0 && (*s & 0xC0) == 0x80 && (*(s + 1) & 0xC0) == 0x80)
            { *len = 3; cp = ((c & 0x0F) << 12) | ((*s & 0x3F) << 6) | (*(s + 1) & 0x3F); }
    else if ((c & 0xF8) == 0xF0 && (*s & 0xC0) == 0x80 && (*(s + 1) & 0xC0) == 0x80 && (*(s + 2) & 0xC0) == 0x80) 
            { *len = 4; cp = ((c & 0x07) << 18) | ((*s & 0x3F) << 12) | ((*(s + 1) & 0x3F) << 6) | (*(s + 2) & 0x3F); }
    else return 4;
    if (cp == 0 || cp < 32 || (cp >= 0x7F && cp < 0xA0)) return 3;                                              // управляющие
    if (cp < 0x100) return 1;                                                                                   // обычный
    if (cp >= 0x0590 && cp <= 0x08FF) *Mrtl = 1;
    if ((*len == 2 && cp < 0x80) || (*len == 3 && (cp < 0x800 || (cp >= 0xD800 && cp <= 0xDFFF))) || 
        (*len == 4 && (cp < 0x10000 || cp > 0x10FFFF))) return 4;                                               // битый
    if ((cp >= 0x0300 && cp <= 0x036F) || (cp >= 0x1DC0 && cp <= 0x1DFF) || (cp >= 0x20D0 && cp <= 0x20FF) ||
        (cp == 0x200D || (cp >= 0xFE00 && cp <= 0xFE0F))) return 0;                                             // прилепало
    if (cp == 0x200B || cp == 0x200C || cp == 0x200E || cp == 0x200F || (cp >= 0xFE20 && cp <= 0xFE2F) ||
        (cp >= 0xE0100 && cp <= 0xE01EF)) return 0;
    if ((cp >= 0x1100 && cp <= 0x115F) || (cp == 0x2329 || cp == 0x232A) || (cp >= 0x2E80 && cp <= 0xA4CF && cp != 0x303F) || 
        (cp >= 0xAC00 && cp <= 0xD7A3) || (cp >= 0xF900 && cp <= 0xFAFF) || (cp >= 0xFE10 && cp <= 0xFE19) || 
        (cp >= 0xFE30 && cp <= 0xFE6F) || (cp >= 0xFF00 && cp <= 0xFF60) || (cp >= 0xFFE0 && cp <= 0xFFE6) || 
        (cp >= 0x20000 && cp <= 0x2FFFD) || (cp >= 0x30000 && cp <= 0x3FFFD) || (cp >= 0x1F300)) return 2;      // двойной
    return 1; }
uint8_t UTFinfoTile(char *s, uint8_t *len, uint8_t *Mrtl, Cell rem) {
    *len = 0; *Mrtl = 0; if (rem == 0) return 5;                                                                // нет влезет и не проверяем
    *len = 1;
    if ((*s & 0xE0) == 0xC0 && rem < 2) return 5;
    else if ((*s & 0xF0) == 0xE0 && rem < 3) return 5;
    else if ((*s & 0xF8) == 0xF0 && rem < 4) return 5;
    return UTFinfo(s, len, Mrtl); }

typedef struct { int16_t X, Y, viewX, viewY; uint8_t Mode, dXY, Tic, Cod, oCod, Key, up, ud, le, ri, cup, cdo, cle, cri, F2, F3, F4, es; } Cur_;
typedef struct { int16_t LkX, LkY, MkX, MkY, RkX, RkY; char key[6]; uint8_t pop, push, mode, tic, Mkey, MX, MY, Lk, Mk, Rk, Ru, Rd, cRu, cRd; } Buf_;
typedef struct { Cell addr, size; } Vrm_;
Cur_ Cur = {0,0,0,0,0,1,0,0,0,12,K_UP,K_DOW,K_LEF,K_RIG,K_Ctrl_UP,K_Ctrl_DOW,K_Ctrl_LEF,K_Ctrl_RIG,K_F2,K_F3,K_F4,K_ESC};
Buf_ Buf = {0,0,0,0,0,0,{0,0,0,0,0,0},0,0,0,0,0,0,0,0x20,0x21,0x22,0x60,0x61,0x64,0x65};
Vrm_ VRam = {0};

void Print(uint8_t n, char *str) { n &= Mcbi; if (!str) return;
  char *dst = Cvdat + 1024, *sav; uint16_t len;
  sav = Parse(n); len = *sav++; MemCpy(dst, sav, len); dst += len;
  len = StrLen(str); MemCpy(dst, str, len); dst += len;
  sav = Parse(Cdefault); len = *sav++; MemCpy(dst, sav, len); dst += len; SysWrite(Cvdat + 1024, (dst - Cvdat - 1024)); }
void InitVram(Cell addr, Cell size) { if (!addr || (size < SizeVram)) return;
  char* colors[] = { Reset, Grey, Green, Red, Blue, Orange, Gold, Reset };
  char* modes[] = { "\007;22;27m", "\006;22;7m", "\006;1;27m", "\005;1;7m" };
  uint8_t lm, cbi, ca, c = StrLen(Reset), i = 8; char *ac, *dst;
  Cdata = (char*)(addr); Coffset = (uint16_t*)(Cdata + SizeData); Cattr = (uint8_t*)((uint8_t*)Coffset + SizeOffset);
  Cvlen = (uint8_t*)(Cattr + SizeAttr); Clen = (uint8_t*)(Cvlen + SizeVizLen);
  Cpdat = (char*)(Clen + SizeLen); Ckbuf = (char*)(Cpdat + SizePalBuff); Cvdat = (char*)(Ckbuf + SizeKeyBuf);
  while (i--) { ac = (Cvdat + ((i) << 5)); dst = ac; *dst++ = c; MemCpy(dst, Reset, c);
      ca = StrLen(colors[i]); if (ca) { *ac++ = ca; MemCpy(ac, colors[i], ca); } }
  i = 4; while(i) { char* mode = modes[--i]; lm = *mode++, c = 8; 
            while(c) { ac = (Cvdat + ((--c) << 5)); cbi = (c << 2) + i; ca = (*ac++ - 1);
                dst = Parse(cbi); *dst++ = (lm + ca); MemCpy(dst, ac, ca); MemCpy(dst + ca, mode, lm); } } }
Cell SystemSwitch(void) { static uint8_t flag = 1;
  if (flag) { VRam.size = SizeVram; if (!(VRam.addr = GetRam(&VRam.size))) return 0;
              flag--; SWD(VRam.addr); InitVram(VRam.addr,VRam.size); SwitchRaw(); Delay_ms(0);
              SyncSize(VRam.addr,Off); Print(Cdefault,AltBufOn Reset HideCur WrapOn Cls MouseX10on); }
  else { flag++; if (VRam.size) { SwitchRaw(); Print(Cdefault,AltBufOff Reset ShowCur WrapOn MouseX10off); FreeRam(VRam.addr, VRam.size); } }
  return 1; }

uint8_t Key(uint8_t *num, uint8_t *tic, uint8_t *control) {
  uint8_t vlen, len, mrtl, t = 0, c = 0; uint16_t d; *control = 0;
  GetKey(Buf.key); vlen = UTFinfo(Buf.key, &len, &mrtl); if (vlen == 3) c = Buf.key[1];
  if ((vlen == 3 && c == K_NO) || vlen == 4) { *tic = Buf.tic; return 0; }
  if (c == K_Mouse) { int16_t dx = 0, dy = 0; uint16_t y, x = TermCR(&y);
    Buf.Mkey = (uint8_t)Buf.key[2]; Buf.MX = (uint8_t)Buf.key[3] - 33; Buf.MY = (uint8_t)Buf.key[4] - 33;
    if (Buf.Mkey == Buf.Lk) { Cur.X = Buf.MX - Cur.viewX; Cur.Y = Buf.MY - Cur.viewY; Buf.LkX = Cur.X; Buf.LkY = Cur.Y; t++; }
    else if (Buf.Mkey == Buf.Mk) { Cur.X = Buf.MX - Cur.viewX; Cur.Y = Buf.MY - Cur.viewY; Buf.MkX = Cur.X; Buf.MkY = Cur.Y; t++; }
    else if (Buf.Mkey == Buf.Rk) { Cur.X = Buf.MX - Cur.viewX; Cur.Y = Buf.MY - Cur.viewY; Buf.RkX = Cur.X; Buf.RkY = Cur.Y; t++; }
    if (Buf.Mkey == Buf.Ru) dy--;
    else if (Buf.Mkey == Buf.Rd) dy++;
    else if (Buf.Mkey == Buf.cRu) dx++;
    else if (Buf.Mkey == Buf.cRd) dx--;
    if (dy) { Cur.Y += dy * Cur.dXY; t++;
      if (!(Cur.Mode & 6)) { Cur.viewY += dy * Cur.dXY;
        if ((Cur.Y + Cur.viewY) < 0) { Cur.viewY = - Cur.Y; t++; }
        else if ((Cur.Y + Cur.viewY) >= y) { Cur.viewY = y - 1 - Cur.Y; t++; } }
      else {
        if (Cur.Y + Cur.viewY < 0) Cur.Y = -Cur.viewY;
        else if (Cur.Y + Cur.viewY >= y) Cur.Y = y - 1 - Cur.viewY; } } 
    else if (dx) { Cur.X += dx * Cur.dXY; t++;
      if (!(Cur.Mode & 6)) { Cur.viewX += dx * Cur.dXY;
        if ((Cur.X + Cur.viewX) < 0) { Cur.viewX = - Cur.X; t++; }
        else if ((Cur.X + Cur.viewX) >= x) { Cur.viewX = x - 1 - Cur.X; t++; } }
      else {
        if (Cur.X + Cur.viewX < 0) Cur.X = -Cur.viewX;
        else if (Cur.X + Cur.viewX >= x) Cur.X = x - 1 - Cur.viewX; } }
    *tic = Buf.tic; *control = t; return c; }
  if (c && *num < K_Max) { d = *num++; while (d--) if (*num++ == c) { *control = 1; break; } }
  if (!(*control && (Buf.mode & 1))) { char *sav, *dst; t = len;
    if (Buf.push == Buf.pop) { Buf.push++; dst = KeyBuf(Buf.push); *dst++ = t; vlen |= 0x10;
      if (len < 3 && mrtl) { vlen |= 0x02; mrtl--; }
      *dst++ = vlen; *dst++ = 0; *dst++ = mrtl;
      if (c) *dst = c;
      else { while(t--) *(dst + t) = Buf.key[t]; } }
    else { sav = KeyBuf(Buf.push); dst = sav + 4; d = 2; if (*sav < 3 && (*(sav + 1) & 0x20)) { dst += 2; d++; } 
      if (*sav == t) {
        if (c) { if (*dst == c) t = 0xFF; }
        else { while (t--) { if (*(dst + t) != Buf.key[t]) break; } } }
      sav += d;
      if (t != 0xFF) { d = ((*sav << 8) + Buf.tic) / AutoR;
        *sav = (d > 0xFF) ? 0xFF:(uint8_t)d; dst = KeyBuf(Buf.push);
        if (len < 3 && *dst < 3 && !(*(dst + 1) & 0x20)) { d = *(dst + 1) | 0x20; if (vlen) d |= 0x04;
          if (mrtl) { d |= 0x08; mrtl--; }
          vlen = (uint8_t)d; d = *(dst + 2); t = 2; }
        else  { d = 0; if (++Buf.push == Buf.pop) Buf.pop++;
                dst = KeyBuf(Buf.push); t = 0; vlen |= 0x10; if (len < 3 && mrtl) { vlen |= 0x02; mrtl--; } } 
        *dst++ = len; *dst++ = vlen; *dst++ = (uint8_t)d; *dst++ = mrtl; dst += t;
        if (c) *dst = c;
        else { while(len--) *(dst + len) = Buf.key[len]; } }
      else { if (!Buf.tic) *sav += 1; } }
    if (!c) c = 0xFF; }
  *tic = ++Buf.tic; return c; }
uint16_t KeysBuf(void) { uint16_t s = 0; uint8_t c = Buf.push;
  while (c != Buf.pop) { s++; if (*(KeyBuf(c--) + 1) & 0x20) s++; }
  return s; }

void ShowC(void) {
  if (!(Cur.Mode & 1) && (uint16_t)Cur.X < CellLine && (uint16_t)Cur.Y < String) {
    int16_t x = Cur.X; uint8_t *a = Attr(Cur.Y,Cur.X), len = *Visi(Cur.Y, Cur.X); if (len > 4) len = 4;
    while (len-- && x++ < CellLine) { *a ^= Minv; *a++ |= Fresh; } } }
uint8_t ViewPort(void) {
  uint16_t r, c = TermCR(&r); uint8_t control, s = Buf.mode;
  ShowC(); Buf.mode |= 1; if (Cur.Mode & 4) Buf.mode--;
  Cur.Cod = Key(&Cur.Key, &Cur.Tic, &control); Buf.mode = s;
  if (control && Cur.Cod != K_Mouse) {
    if (Cur.Cod == Cur.es) return 0;
    else if (Cur.Cod == Cur.F4) Cur.Mode ^= 1;
    else if (Cur.Cod == Cur.F3 && (uint16_t)Cur.X < CellLine && (uint16_t)Cur.Y < String) Cur.Mode ^= 2;
    else if (Cur.Cod == Cur.F2 && (uint16_t)Cur.X < CellLine && (uint16_t)Cur.Y < String) { Cur.Mode ^= 4;
      if (!(Cur.Mode & 4)) { if (Buf.push != Buf.pop) { char *tic = KeyBuf(Buf.push);
        if (*++tic & 0x10) *tic &= 3;
        else { --Buf.push; tic = KeyBuf(Buf.push); if (*++tic & 0x10) tic++; }
        uint16_t d = *++tic * AutoR; *tic = (uint8_t)(d >> 8); --Buf.tic; } } }
    if (Cur.Cod != Cur.oCod) { Cur.dXY = 1; Cur.oCod = Cur.Cod; }
    if ((Cur.Cod & 0xF8) == 0x20) {
      if ((Cur.Tic > 7) && !(Cur.Tic & 3) && (Cur.dXY < 64)) Cur.dXY <<= 1;
      if (!(Cur.Mode & 6)) {
        if (Cur.Cod == Cur.cle) { Cur.viewX -= Cur.dXY; Cur.Cod = Cur.le; }
        else if (Cur.Cod == Cur.cri) { Cur.viewX += Cur.dXY; Cur.Cod = Cur.ri; }
        else if (Cur.Cod == Cur.cup) { Cur.viewY -= Cur.dXY; Cur.Cod = Cur.up; }
        else if (Cur.Cod == Cur.cdo) { Cur.viewY += Cur.dXY; Cur.Cod = Cur.ud; } }
      if (Cur.Cod == Cur.le) Cur.X -= Cur.dXY;
      else if (Cur.Cod == Cur.ri) Cur.X += Cur.dXY;
      else if (Cur.Cod == Cur.up) Cur.Y -= Cur.dXY;
      else if (Cur.Cod == Cur.ud) Cur.Y += Cur.dXY; }
    if (Cur.Mode & 6) {
      if (Cur.X + Cur.viewX < 0) Cur.X = -Cur.viewX;
      else if (Cur.X + Cur.viewX >= c) Cur.X = c - 1 - Cur.viewX;
      if (Cur.Y + Cur.viewY < 0) Cur.Y = -Cur.viewY;
      else if (Cur.Y + Cur.viewY >= r) Cur.Y = r - 1 - Cur.viewY; }
    else { control--;
      if ((Cur.X + Cur.viewX) < 0) { Cur.viewX = - Cur.X; control++; }
      else if ((Cur.X + Cur.viewX) >= c) { Cur.viewX = c - 1 - Cur.X; control++; }
      if ((Cur.Y + Cur.viewY) < 0) { Cur.viewY = - Cur.Y; control++; }
      else if ((Cur.Y + Cur.viewY) >= r) { Cur.viewY = r - 1 - Cur.Y; control++; } } }
  if (SyncSize(VRam.addr,On)) { int16_t dr = r, dc = c; c = TermCR(&r); dr -= r; dc -= c;
    if (Cur.X + Cur.viewX >= c) Cur.X = c - 1 - Cur.viewX;
    else if (Cur.X + Cur.viewX < 0)  Cur.X = -Cur.viewX;
    if (Cur.Y + Cur.viewY >= r) Cur.Y = r - 1 - Cur.viewY;
    else if (Cur.Y + Cur.viewY < 0)  Cur.Y = -Cur.viewY;
    if (control) { 
      if (dr < 0 || dc < 0) control--; } }
  ShowC(); return 1; }

uint32_t Bin(void) { uint8_t x = Cur.Mode;
  uint32_t c = 0, i = 8; while(i--){ c *= 10; if (x & 0x80) c++;
                                    x <<= 1; }
  c += 100000000; return c; }
void Show(void) { uint16_t s, r, c = TermCR(&r); Cell o, m = VRam.size; if (18 > c) return;
  Print(Cdefault,Home); Print(Cgrey," esc 842  F2 F3 F4\n"); o = m % (1024 * 1024); m /= (1024 * 1024); if (o) m++;
  s = (uint16_t)m; snprintf(Cvdat, 100, "%d %dMb c%d r%d b%d x%d y%d       \n", Bin(), s, c, r, Buf.Mkey, Buf.MX, Buf.MY); *Cvdat = 'v'; if (StrLen(Cvdat) > c) return;
  Print(Corange,Cvdat); s = KeysBuf();
  snprintf(Cvdat + 100, 100, "x%d y%d wx%d wy%d xy%d KeyBuf %d    ", Cur.X, Cur.Y, Cur.viewX, Cur.viewY, Cur.dXY, s); if (StrLen(Cvdat + 100) > c) return;
  Print(Cdefault,Cvdat + 100); }
