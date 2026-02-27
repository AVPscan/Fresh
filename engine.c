/*
 * Copyright (C) 2026 Поздняков Алексей Васильевич
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 */

#include <stdio.h>
#include <unistd.h>
#include "sys.h"

const uint8_t Fresh = 0x80;
const uint8_t  Mcol = 0x1C;
const uint8_t  Mbol = 0x02;
const uint8_t  Minv = 0x01;
const uint8_t  Mcbi = 0x1F;
char      *Cdata      = NULL;
uint16_t  *Coffset    = NULL;
uint8_t   *Cattr      = NULL;
uint8_t   *Cvlen      = NULL;
uint8_t   *Clen       = NULL;
char      *Cpdat      = NULL;
char      *Ckbuf      = NULL;
char      *Cvdat      = NULL;
#define CellLine      8192
#define String        5062
#define SKey          256
#define SizeCOL       CellLine * 2
#define SizeCL        CellLine * 4
#define SizeBKey      SKey * 4
#define SizeData      ((Cell)String * SizeCL)
#define SizeOffset    ((Cell)String * SizeCOL)
#define SizeAttr      ((Cell)String * CellLine)
#define SizeVizLen    ((Cell)String * CellLine)
#define SizeLen       ((Cell)String * CellLine)
#define SizePalBuff   32 * 32
#define SizeKeyBuf    SKey * 12
#define SizeVBuff     (34 * 1024 * 1024)
#define SizeVram      (SizeData + SizeOffset + SizeAttr + SizeVizLen + SizeLen + SizePalBuff + SizeKeyBuf + SizeVBuff)
#define Data(r)       (Cdata + ((r) << 15))
#define Offset(r, c)  (Coffset + ((r) << 14) + (c))
#define Attr(r, c)    (Cattr + ((r) << 13) + (c))   // 7 Dirty 65 Reserve 432 Colour 1 Bold 0 Inverse
#define Visi(r, c)    (Cvlen + ((r) << 13) + (c))
#define Len(r, c)     (Clen + ((r) << 13) + (c))
#define Parse(cbi)    (Cpdat + ((cbi) << 5))        // 0-31 All
#define KeyBuf(n)     (Ckbuf + ((n) << 3))          // ( tic mrtl vlen len UTF8[4 byte])[8 bytes]

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
    if ((*len == 2 && cp < 0x80) || (*len == 3 && (cp < 0x800 || (cp >= 0xD800 && cp <= 0xDFFF))) || 
        (*len == 4 && (cp < 0x10000 || cp > 0x10FFFF))) return 4;                                               // битый
    if (cp >= 0x0590 && cp <= 0x08FF) *Mrtl = 1;
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
    
typedef struct { Cell addr, size; } Vram_;
Vram_ VRam = {0};
void SetColour(uint8_t col) { if (!(col &= Mcol)) col = 3;
  col <<= 2; MemCpy(Parse(Ccurrent), Parse(col), 128); }
void InitVram(Cell addr, Cell size) { if (!addr || (size < SizeVram)) return;
  char* colors[] = { Green, ColorOff, Grey, Green, Red, Blue, Orange, Gold };
  char* modes[] = { "\007;22;27m", "\006;22;7m", "\006;1;27m", "\005;1;7m" };
  uint8_t lm, cbi, ca, c = StrLen(ColorOff), i = 8; char *ac, *dst;
  Cdata = (char*)(addr); Coffset = (uint16_t*)(Cdata + SizeData); Cattr = (uint8_t*)((uint8_t*)Coffset + SizeOffset);
  Cvlen = (uint8_t*)(Cattr + SizeAttr); Clen = (uint8_t*)(Cvlen + SizeVizLen);
  Cpdat = (char*)(Clen + SizeLen); Ckbuf = (char*)(Cpdat + SizePalBuff); Cvdat = (char*)(Ckbuf + SizeKeyBuf);
  while (i--) { ac = (Cvdat + ((i) << 5)); dst = ac; *dst++ = c; MemCpy(dst, ColorOff, c);
      ca = StrLen(colors[i]); if (ca) { *ac++ = ca; MemCpy(ac, colors[i], ca); } }
  i = 4; while(i) { char* mode = modes[--i]; lm = *mode++, c = 8; 
            while(c) { ac = (Cvdat + ((--c) << 5)); cbi = (c << 2) + i; ca = (*ac++ - 1);
                dst = Parse(cbi); *dst++ = (lm + ca); MemCpy(dst, ac, ca); MemCpy(dst + ca, mode, lm); } } }

typedef struct {int16_t X, Y, viewX, viewY, LkX, LkY, MkX, MkY, RkX, RkY; uint16_t  oldC, oldR;
                uint8_t Vision, Key, Tic, CodeKey, PenCK, Mkey, MX, MY, dXY;} Cur_;
typedef struct { uint8_t pop, push; char key[6]; } Buf_;
Cur_ Cur = {0};
Buf_ Buf = {0};

uint8_t Key(uint8_t *num) { uint8_t vlen, len, mrtl, t = 1, c = 1; t += Buf.push; *num = Buf.pop; char *dst;
  if (t != Buf.pop) { GetKey(Buf.key); vlen = UTFinfo(Buf.key, &len, &mrtl);
    if ((Buf.key[1] == K_NO && vlen == 3) || vlen == 4) c--;
    else {
      if (Buf.pop == Buf.push) { t = Buf.push; dst = KeyBuf(Buf.push++);
        *dst++ = 0; *dst++ = mrtl; *dst++ = vlen; *dst++ = len;
        if (vlen == 3) *dst = Buf.key[1];
        else while(len--) *(dst + len) = Buf.key[len]; }
      else { dst = KeyBuf(--Buf.push) + 4; c = len; t = 0; if (vlen == 3) t++;
        while (c--) if (*(dst + c) != Buf.key[t + c]) break;
        t = Buf.push; Buf.push++;
        if (c == 0xFF) *(dst - 4) += 1;
        else { t = Buf.push; dst = KeyBuf(Buf.push++);
          *dst++ = 0; *dst++ = mrtl; *dst++ = vlen; *dst++ = len;
          if (vlen == 3) *dst = Buf.key[1];
          else while(len--) *(dst + len) = Buf.key[len]; } }
      c = 0xFF; if (vlen == 3) c = *dst;
      if (c == K_Mouse) { *++dst = Buf.key[2]; *++dst = Buf.key[3] - 33; *++dst = Buf.key[4] - 33; } }
    *num = t; }
  return c; }

void ShowC(uint8_t on) {
  char *src, *dst = Cvdat, *sav; uint8_t i, c, p = CcurrentI; Cur.Vision &= 0xFE;
  if (!(Cur.Vision & 8)) { Cur.Vision += (on & 1); if (Cur.Vision & 4) p = CredI;
    int16_t x = Cur.X + Cur.viewX + 1, y = Cur.Y + Cur.viewY + 1; *dst++ =  27;
    *dst++ = '['; src = dst; do { *src++ = '0' + (y % 10); y /= 10; } while (y);
    sav = src; i = (uint8_t)(src - dst) / 2; while(i--) { c = *dst; *dst++ = *--src; *src = c; }
    *sav++ = ';'; dst = sav; do { *dst++ = '0' + (x % 10); x /= 10; } while (x);
    src = dst; i = (uint8_t)(dst - sav) / 2; while(i--) { c = *sav; *sav++ = *--dst; *dst = c; }
    *src++ = 'H'; if (on) { sav = Parse(p); MemCpy(src, (sav + 1), *sav); src += *sav; }
    *src++ = ' '; if (on) { sav = Parse(Ccurrent); MemCpy(src, (sav + 1), *sav); src += *sav; }
    SysWrite (Cvdat, (src - Cvdat)); } }
uint8_t ViewPort(void) {
  uint16_t r, c = TermCR(&r), control = 0; char *dst; int16_t mdxy = 0;
  if (Cur.Vision & 1) ShowC(Off);
  if ((Cur.CodeKey = Key(&Cur.Key))) { dst = KeyBuf(Cur.Key); Cur.Tic = *dst;
    if (Cur.CodeKey != 0xFF) {
      if (Cur.CodeKey == K_ESC) return 0;
      else  if (Cur.CodeKey != K_NO) {
            if (Cur.CodeKey == K_F4) { Cur.Vision ^= 2; control++; }
            else if (Cur.CodeKey == K_F3) { Cur.Vision ^= 4; control++; }
            else if (Cur.CodeKey == K_F2) { Cur.Vision ^= 8; control++; }
            else if (Cur.CodeKey == K_F1) { Cur.Vision ^= 16; control++; }
            else if ((Cur.CodeKey & 0xF8) == 0x20) control++;
            else if (Cur.CodeKey == K_Mouse) { Cur.CodeKey = 0xFF; Cur.Mkey = (uint8_t)*(dst + 5);
                    if ((Cur.Mkey & 0xFC) == 32) {
                      Cur.MX = (uint8_t)*(dst + 6); Cur.MY = (uint8_t)*(dst + 7);
                      Cur.X = Cur.MX - Cur.viewX; Cur.Y = Cur.MY - Cur.viewY;
                      if (Cur.Mkey == 32) { Cur.LkX = Cur.X; Cur.LkY = Cur.Y; control++; }
                      else if (Cur.Mkey == 33) { Cur.MkX = Cur.X; Cur.MkY = Cur.Y; control++; }
                      else if (Cur.Mkey == 34) { Cur.RkX = Cur.X; Cur.RkY = Cur.Y; control++; } }
                      else if (Cur.Mkey == 96) { mdxy--; control++; }
                      else if (Cur.Mkey == 97) { mdxy++; control++; }
                    if (mdxy) { if (!(Cur.Vision & 6)) Cur.viewY += mdxy; 
                                Cur.Y += mdxy; } } } }
    if (Cur.CodeKey != Cur.PenCK) { if (Cur.PenCK) Buf.pop++;
      Cur.PenCK = Cur.CodeKey; Cur.dXY = 1; }
    mdxy = 0;
    if ((Cur.CodeKey & 0xF8) == 0x20) {
      if ((Cur.Tic > 7) && !(Cur.Tic & 3) && (Cur.dXY < 64)) Cur.dXY <<= 1;
      if (!(Cur.Vision & 4)) {
        if (Cur.CodeKey == K_Ctrl_LEF) { Cur.viewX -= Cur.dXY; mdxy++; }
        else if (Cur.CodeKey == K_Ctrl_RIG) { Cur.viewX += Cur.dXY; mdxy++; }
        else if (Cur.CodeKey == K_Ctrl_UP) { Cur.viewY -= Cur.dXY; mdxy++; }
        else if (Cur.CodeKey == K_Ctrl_DOW) { Cur.viewY += Cur.dXY; mdxy++; } }
      Cur.CodeKey &= 0xFE;
      if (Cur.CodeKey == K_LEF) Cur.X -= Cur.dXY;
      else if (Cur.CodeKey == K_RIG) Cur.X += Cur.dXY;
      else if (Cur.CodeKey == K_UP) Cur.Y -= Cur.dXY;
      else if (Cur.CodeKey == K_DOW) Cur.Y += Cur.dXY; }
    if (Cur.Vision & 6) {
      if (Cur.X + Cur.viewX < 0) Cur.X = -Cur.viewX;
      else if (Cur.X + Cur.viewX >= c) Cur.X = c - 1 - Cur.viewX;
      if (Cur.Y + Cur.viewY < 0) Cur.Y = -Cur.viewY;
      else if (Cur.Y + Cur.viewY >= r) Cur.Y = r - 1 - Cur.viewY; }
    else {
      if ((Cur.X + Cur.viewX) < 0) { Cur.viewX = - Cur.X; mdxy++; }
      else if ((Cur.X + Cur.viewX) >= c) { Cur.viewX = c - 1 - Cur.X; mdxy++; }
      if ((Cur.Y + Cur.viewY) < 0) { Cur.viewY = - Cur.Y; mdxy++; }
      else if ((Cur.Y + Cur.viewY) >= r) { Cur.viewY = r - 1 - Cur.Y; mdxy++; } } }
  Cur.oldC = c; Cur.oldR = r; control = SyncSize(VRam.addr,On);
  if (control) { c = TermCR(&r);
      if (Cur.X + Cur.viewX >= c) Cur.X = c - 1 - Cur.viewX;
      else if (Cur.X + Cur.viewX < 0)  Cur.X = -Cur.viewX;
      if (Cur.Y + Cur.viewY >= r) Cur.Y = r - 1 - Cur.viewY;
      else if (Cur.Y + Cur.viewY < 0)  Cur.Y = -Cur.viewY; }
  
  ShowC(On); return 1; }

void Print(uint8_t n, char *str) { n &= Mcbi; if (!str) return;
  char *dst = Cvdat + 1024, *sav; uint16_t len;
  sav = Parse(n); len = *sav++; MemCpy(dst, sav, len); dst += len;
  len = StrLen(str); MemCpy(dst, str, len); dst += len;
  sav = Parse(Ccurrent); len = *sav++; MemCpy(dst, sav, len); dst += len; SysWrite(Cvdat + 1024, (dst - Cvdat - 1024)); }
Cell SystemSwitch(void) { static uint8_t flag = 1;
  if (flag) { VRam.size = SizeVram; if (!(VRam.addr = GetRam(&VRam.size))) return 0;
              flag--; SWD(VRam.addr); InitVram(VRam.addr,VRam.size); SwitchRaw(); Delay_ms(0);
              SyncSize(VRam.addr,Off); Print(Ccurrent,AltBufOn Reset HideCur WrapOn Cls MouseX10on); }
  else { flag++; if (VRam.size) { SwitchRaw(); Print(Ccurrent,MouseX10off AltBufOff WrapOn ShowCur Reset); FreeRam(VRam.addr, VRam.size); } }
  return 1; }
Cell Help(Cell argc, char *argv[], Cell flag) {
  if (argc > 1) { 
    if (MemCmp(argv[1], "-?",2) == 0 || MemCmp(argv[1], "-h",2) == 0 || MemCmp(argv[1], "-help",5) == 0) {
      if (flag) { Print(Ccurrent,AltBufOff); Print(CorangeIB," Created by Alexey Pozdnyakov ");
                  Print(Corange," in 07.02.2026 version 2.53 email: avp70ru@mail.ru https://github.com/AVPscan\n"); }
      else printf("The processor did not allocate memory\n"); }
    flag = 0; }
  return flag; }


uint32_t Bin( uint8_t x) {
  uint32_t c = 0, i = 8; while(i--){ c *= 10; if (x & 0x80) c++;
                                    x <<= 1; }
  return c + 100000000; }
void Show(void) { uint16_t s, r, c = TermCR(&r); Cell o, m = VRam.size;
  Print(Ccurrent,Home); if (18 > c) return;
  Print(Cgrey," esc 842  F2 F3 F4\n");
  o = m % (1024 * 1024); m /= (1024 * 1024); if (o) m++;
  snprintf(Cvdat, 100, "%d %luMb c%d r%d b%d x%d y%d         \n", Bin(Cur.Vision), m, c, r, Cur.Mkey, Cur.MX, Cur.MY);
  *Cvdat = 'v'; if (StrLen(Cvdat) > c) return;
  if (Buf.pop < Buf.push) s = Buf.push - Buf.pop;
  else s = Buf.push + 256 - Buf.pop;
  snprintf(Cvdat + 100, 100, "x%d y%d wx%d wy%d xy%d KeyBuf %d    ", Cur.X, Cur.Y, Cur.viewX, Cur.viewY, Cur.dXY, s);
  if (StrLen(Cvdat + 100) > c) return;
  Print(Cblue,Cvdat); Print(Cgreen,Cvdat + 100); }
