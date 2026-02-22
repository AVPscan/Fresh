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
char      *Apdat      = NULL;
uint16_t  *Awdat      = NULL;
char      *Avdat      = NULL;
#define CellLine      8192
#define String        5062
#define SizeCOL       CellLine * 2
#define SizeCL        CellLine * 4
#define SizeData      ((size_t)String * SizeCL)
#define SizeOffset    ((size_t)String * SizeCOL)
#define SizeAttr      ((size_t)String * CellLine)
#define SizeVizLen    ((size_t)String * CellLine)
#define SizeLen       ((size_t)String * CellLine)
#define SizePalBuff   1024
#define SizeWinDat    1024
#define SizeVBuff     (34 * 1024 * 1024)
#define SizeVram      (SizeData + SizeOffset + SizeAttr + SizeVizLen + SizeLen + SizePalBuff + SizeWinDat + SizeVBuff)
#define Data(r)       (Cdata + ((r) << 15))
#define Offset(r, c)  (Coffset + ((r) << 14) + (c))
#define Attr(r, c)    (Cattr + ((r) << 13) + (c))   // 7 Dirty 65 Reserve 432 Colour 1 Bold 0 Inverse
#define Visi(r, c)    (Cvlen + ((r) << 13) + (c))
#define Len(r, c)     (Clen + ((r) << 13) + (c))
#define Parse(cbi)    (Apdat + ((cbi) << 5))        // 0-31 All
#define Window(n)     (Awdat + ((n) << 3))

int StrLen(char *s) { if (!s) return 0;
    int count = 0; while (*s) { if ((*s++ & 0xC0) != 0x80) count++; }
    return count; }
    
void MemSet(void* buf, uint8_t val, size_t len) {
    uint8_t *p = (uint8_t *)buf; while (len && ((Cell)p & (SizeCell - 1))) { *p++ = val; len--; }
    if (len >= SizeCell) {
        Cell vW = val * ((Cell)-1 / 255); Cell *pW = (Cell *)p;
        size_t i = len / SizeCell; len &= (SizeCell - 1); while (i--) *pW++ = vW;
        p = (uint8_t *)pW; }
    while (len--) *p++ = val; }
void MemCpy(void* dst, const void* src, size_t len) {
    uint8_t *d = (uint8_t *)dst; const uint8_t *s = (const uint8_t *)src;
    while (len && ((Cell)d & (SizeCell - 1))) { *d++ = *s++; len--; }
    if (len >= SizeCell && ((Cell)s & (SizeCell - 1)) == 0) {
        Cell *dW = (Cell *)d; const Cell *sW = (const Cell *)s; size_t i = len / SizeCell;
        len &= (SizeCell - 1); while (i--) *dW++ = *sW++;
        d = (uint8_t *)dW; s = (uint8_t *)sW; }
    while (len--) *d++ = *s++ ; }
void MemMove(void* dst, const void* src, size_t len) {
    if (dst > src) { uint8_t *d = (uint8_t *)dst; const uint8_t *s = (const uint8_t *)src;
        d += len; s += len; while (len && ((Cell)d & (SizeCell - 1))) { *--d = *--s; len--; }
        if (len >= SizeCell && ((Cell)s & (SizeCell - 1)) == 0) {
            Cell *dW = (Cell *)d; const Cell *sW = (const Cell *)s; size_t i = len / SizeCell;
            len &= (SizeCell - 1); while (i--) *--dW = *--sW;
            d = (uint8_t *)dW; s = (uint8_t *)sW; } }
    else if (dst < src ) MemCpy(dst, src, len); }
int8_t MemCmp(void* dst, const void* src, size_t len) {
    uint8_t *d = (uint8_t *)dst; const uint8_t *s = (const uint8_t *)src;
    while (len && ((Cell)d & (SizeCell - 1))) { len--; if (*d++ != *s++) return (int8_t)(*--d - *--s); }
    if (len >= SizeCell && ((Cell)s & (SizeCell - 1)) == 0) {
        Cell *dW = (Cell *)d; const Cell *sW = (const Cell *)s; size_t i = len / SizeCell;
        len %= SizeCell; while (i-- && (*dW++ == *sW++));
        if (i + 1) { --dW; --sW; len += SizeCell; }
        d = (uint8_t *)dW; s = (uint8_t *)sW; }
    while (len--) { if (*d++ != *s++) return (int8_t)(*--d - *--s); }
    return 0; }
    
int8_t UTFinfo(unsigned char *s, uint8_t *len, uint8_t *Mrtl) {
    unsigned char c = s[0]; uint32_t cp = 0xFFFD; *len = 1; *Mrtl = 0;
    if (c < 0x80) cp = (uint32_t) c;
    else if ((c & 0xE0) == 0xC0 && (s[1] & 0xC0) == 0x80)
            { *len = 2; cp = ((c & 0x1F) << 6) | (s[1] & 0x3F); }
    else if ((c & 0xF0) == 0xE0 && (s[1] & 0xC0) == 0x80 && (s[2] & 0xC0) == 0x80)
            { *len = 3; cp = ((c & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F); }
    else if ((c & 0xF8) == 0xF0 && (s[1] & 0xC0) == 0x80 && (s[2] & 0xC0) == 0x80 && (s[3] & 0xC0) == 0x80) 
            { *len = 4; cp = ((c & 0x07) << 18) | ((s[1] & 0x3F) << 12) | ((s[2] & 0x3F) << 6) | (s[3] & 0x3F); }
    else return -2;
    if ((*len == 2 && cp < 0x80) || (*len == 3 && (cp < 0x800 || (cp >= 0xD800 && cp <= 0xDFFF))) || 
        (*len == 4 && (cp < 0x10000 || cp > 0x10FFFF))) return -2;  // битый
    if (cp >= 0x0590 && cp <= 0x08FF) *Mrtl = 1;
    if ((cp >= 0x0300 && cp <= 0x036F) || (cp >= 0x1DC0 && cp <= 0x1DFF) || (cp >= 0x20D0 && cp <= 0x20FF) ||
        (cp == 0x200D || (cp >= 0xFE00 && cp <= 0xFE0F))) return 0; // прилепало
    if (cp == 0 || cp < 32 || (cp >= 0x7F && cp < 0xA0)) return -1; // управляющие
    if (cp < 0x100) return 1;
    if (cp == 0x200B || cp == 0x200C || cp == 0x200E || cp == 0x200F || (cp >= 0xFE20 && cp <= 0xFE2F) || (cp >= 0xE0100 && cp <= 0xE01EF)) return 0;
    if ((cp >= 0x1100 && cp <= 0x115F) || (cp == 0x2329 || cp == 0x232A) || (cp >= 0x2E80 && cp <= 0xA4CF && cp != 0x303F) || 
        (cp >= 0xAC00 && cp <= 0xD7A3) || (cp >= 0xF900 && cp <= 0xFAFF) || (cp >= 0xFE10 && cp <= 0xFE19) || 
        (cp >= 0xFE30 && cp <= 0xFE6F) || (cp >= 0xFF00 && cp <= 0xFF60) || (cp >= 0xFFE0 && cp <= 0xFFE6) || 
        (cp >= 0x20000 && cp <= 0x2FFFD) || (cp >= 0x30000 && cp <= 0x3FFFD) || (cp >= 0x1F300)) return 2;
    return 1; }
int8_t UTFinfoTile(unsigned char *s, uint8_t *len, uint8_t *Mrtl, size_t rem) {
    *len = 0; *Mrtl = 0; if (rem == 0) return -3;
    *len = 1;
    if ((*s & 0xE0) == 0xC0 && rem < 2) return -3;
    else if ((*s & 0xF0) == 0xE0 && rem < 3) return -3; // не влез
    else if ((*s & 0xF8) == 0xF0 && rem < 4) return -3;
    return UTFinfo(s, len, Mrtl); }
    
typedef struct { size_t addr, size; } Vram_;
Vram_ VRam = {0};

void Print(uint8_t n, char *str) { n &= Mcbi; if (!str) return;
  char *dst = Avdat + 1024, *sav; uint16_t len;
  sav = Parse(n); len = *sav++; MemCpy(dst, sav, len); dst += len;
  len = StrLen(str); MemCpy(dst, str, len); dst += len;
  sav = Parse(0); len = *sav++; MemCpy(dst, sav, len); dst += len; write(1, Avdat + 1024, (dst - Avdat - 1024)); }

void SetColour(uint8_t col) { if (!(col &= Mcol)) col = 3;
  col <<= 2; MemCpy(Parse(0), Parse(col), 128); }
void InitVram(size_t addr, size_t size) { if (!addr || (size < SizeVram)) return;
  char* colors[] = { Green, ColorOff, Grey, Green, Red, Blue, Orange, Gold };
  char* modes[] = { "\007;22;27m", "\006;22;7m", "\006;1;27m", "\005;1;7m" };
  uint8_t lm, cbi, ca, c = StrLen(ColorOff), i = 8; char *ac, *dst;
  Cdata = (char*)(addr); Coffset = (uint16_t*)(Cdata + SizeData); Cattr = (uint8_t*)((uint8_t*)Coffset + SizeOffset);
  Cvlen = (uint8_t*)(Cattr + SizeAttr); Clen = (uint8_t*)(Cvlen + SizeVizLen);
  Apdat = (char*)(Clen + SizeLen); Awdat = (uint16_t*)(Apdat + SizePalBuff); Avdat = (char*)((uint8_t*)Awdat + SizeWinDat);
  while (i--) { ac = (Avdat + ((i) << 5)); dst = ac; *dst++ = c; MemCpy(dst, ColorOff, c);
      ca = StrLen(colors[i]); if (ca) { *ac++ = ca; MemCpy(ac, colors[i], ca); } }
  i = 4; while(i) { char* mode = modes[--i]; lm = *mode++, c = 8; 
            while(c) { ac = (Avdat + ((--c) << 5)); cbi = (c << 2) + i; ca = (*ac++ - 1);
                dst = Parse(cbi); *dst++ = (lm + ca); MemCpy(dst, ac, ca); MemCpy(dst + ca, mode, lm); } } }

typedef struct {int16_t X, Y, viewX, viewY, dX, dY, LkX, LkY, RkX, RkY;
                uint16_t oldRows, oldCols; uint8_t Vision, CodeKey, PenCK, Tic, Mkey, MX, MY; } Cur_;
Cur_ Cur = {30,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0};
void VPSwitch(uint8_t off) {
  off &= 1; Cur.Vision &= 0xFB; if (off) Cur.Vision += 4; }
void ShowC(uint8_t on) {
  char *src, *dst = Avdat, *sav; uint8_t i, c, p = CcurrentI; on &= 1; Cur.Vision &= 0xFE; Cur.Vision += on; if (Cur.Vision & 4) p = CredI;
  int16_t x = Cur.X + Cur.viewX + 1, y = Cur.Y + Cur.viewY + 1;
  *dst++ =  27;
  *dst++ = '['; src = dst; do { *src++ = '0' + (y % 10); y /= 10; } while (y);
  sav = src; i = (uint8_t)(src - dst) / 2; while(i--) { c = *dst; *dst++ = *--src; *src = c; }
  *sav++ = ';'; dst = sav; do { *dst++ = '0' + (x % 10); x /= 10; } while (x);
  src = dst; i = (uint8_t)(dst - sav) / 2; while(i--) { c = *sav; *sav++ = *--dst; *dst = c; }
  *src++ = 'H'; if (on) { sav = Parse(p); MemCpy(src, (sav + 1), *sav); src += *sav; }
  *src++ = ' '; if (on) { sav = Parse(Ccurrent); MemCpy(src, (sav + 1), *sav); src += *sav; }
  write (1, Avdat, (src - Avdat)); }
void ViewPort(const char* key) {
  uint16_t control = 0, r, c; if (Cur.Vision & 1) ShowC(0);
  if (key[0] != 27) Cur.CodeKey = 0;
  else { Cur.CodeKey = (uint8_t)key[1];
      if (key[1] == K_Mouse) { Cur.Mkey = (uint8_t)key[2]; Cur.MX = (uint8_t)key[3] - 33; Cur.MY = (uint8_t)key[4] - 33;
                                if (Cur.Mkey == 32) { Cur.X = Cur.MX - Cur.viewX; Cur.Y = Cur.MY - Cur.viewY; Cur.LkX = Cur.X; Cur.LkY = Cur.Y; }
                                else if (Cur.Mkey == 33) Cur.Vision ^= 2;
                                else if (Cur.Mkey == 34) { Cur.X = Cur.MX - Cur.viewX; Cur.Y = Cur.MY - Cur.viewY; Cur.RkX = Cur.X; Cur.RkY = Cur.Y; }
                                else if (Cur.Mkey == 96) control--;
                                else if (Cur.Mkey == 97) control++; 
                                if (control) { if (Cur.Vision & 4) Cur.Y += control;
                                               else Cur.viewY += control; } } }
  control = SyncSize(VRam.addr,1); c = TermCR(&r);
  if (control) {
      if (Cur.X + Cur.viewX >= c || Cur.Y + Cur.viewY >= r || Cur.X + Cur.viewX < 0 || Cur.Y + Cur.viewY < 0) Cur.Vision &= 0xFE;
      if (Cur.X + Cur.viewX >= c) Cur.viewX = c - 1 - Cur.X;
      else if (Cur.X + Cur.viewX < 0)  Cur.viewX = -Cur.X;
      if (Cur.Y + Cur.viewY >= r) Cur.viewY = r - 1 - Cur.Y;
      else if (Cur.Y + Cur.viewY < 0)  Cur.viewY = -Cur.Y;
      Cur.oldCols = c; Cur.oldRows = r; control =0;
      Print(0, Cls); }
  if (Cur.CodeKey == K_Ctrl_W) Cur.Vision ^= 4;
  if (Cur.CodeKey != Cur.PenCK) { Cur.PenCK = Cur.CodeKey; Cur.Tic = 0; Cur.dX = 1; Cur.dY = 1; }
  if ((Cur.CodeKey & 0xF8) == 0x20) { Cur.Tic++; control++;
      if ((Cur.Tic > 7) && !(Cur.Tic & 3) && (Cur.dX < 64)) { Cur.dX <<= 1; Cur.dY <<= 1; } }
  if (control) {
      if (Cur.CodeKey == K_LEF) Cur.X -= Cur.dX;
      else if (Cur.CodeKey == K_RIG) Cur.X += Cur.dX;
      else if (Cur.CodeKey == K_UP) Cur.Y -= Cur.dY;
      else if (Cur.CodeKey == K_DOW) Cur.Y += Cur.dY;
      if (!(Cur.Vision & 4)) {
          if (Cur.CodeKey == K_Ctrl_LEF) Cur.viewX -= Cur.dX;
          else if (Cur.CodeKey == K_Ctrl_RIG) Cur.viewX += Cur.dX;
          else if (Cur.CodeKey == K_Ctrl_UP) Cur.viewY -= Cur.dY;
          else if (Cur.CodeKey == K_Ctrl_DOW) Cur.viewY += Cur.dY; } }
  if (Cur.Vision & 2 && !(Cur.Vision & 4)) {
      if ((Cur.X + Cur.viewX) < 0) { Cur.viewX = - Cur.X; }
      else if ((Cur.X + Cur.viewX) >= c) { Cur.viewX = c - 1 - Cur.X; }
      if ((Cur.Y + Cur.viewY) < 0) { Cur.viewY = - Cur.Y; }
      else if ((Cur.Y + Cur.viewY) >= r) { Cur.viewY = r - 1 - Cur.Y; } }
  else {
      if (Cur.X + Cur.viewX < 0) Cur.X = -Cur.viewX;
      else if (Cur.X + Cur.viewX >= c) Cur.X = c - 1 - Cur.viewX;
      if (Cur.Y + Cur.viewY < 0) Cur.Y = -Cur.viewY;
      else if (Cur.Y + Cur.viewY >= r) Cur.Y = r - 1 - Cur.viewY; }
  ShowC(1); }
  
int SystemSwitch(void) {
  static uint8_t flag = 1;
  if (flag) { VRam.size = SizeVram; if (!(VRam.addr = GetRam(&VRam.size))) return 0;
              SWD(VRam.addr); InitVram(VRam.addr,VRam.size); SwitchRaw(); Delay_ms(0); flag--;
              SyncSize(VRam.addr,0); VPSwitch(1); Print(0,AltBufOn Reset HideCur WrapOn Cls MouseX10on); }
  else { SwitchRaw(); Print(0,MouseX10off AltBufOff WrapOn ShowCur Reset); FreeRam(VRam.addr, VRam.size); flag++; }
  return 1; }

char* TakeTb(size_t *size) { 
  if (!(VRam.addr)) { *size = 0; return NULL; }
  if (!*size) return NULL;
  if (*size > 1024) *size = 1024;
  return Avdat; }

void Show(void) { uint16_t r = 0;  size_t size = 256; char *buf = TakeTb(&size);
    Print(Ccurrent,Home); snprintf(buf, size, "%d %d %d %d %d %d         \n", TermCR(&r), r, Cur.X + Cur.viewX, Cur.Y + Cur.viewY, Cur.MX, Cur.MY);
    Print(Cblue,buf); snprintf(buf, size, "%d %d %d %d %d        ", Cur.X, Cur.viewX, Cur.Y, Cur.viewY, Cur.Mkey); Print(Cgreen,buf); }

int Help(int argc, char *argv[], int flag) {
  if (argc > 1 && flag) { 
    if (MemCmp(argv[1], "-?",2) == 0 || MemCmp(argv[1], "-h",2) == 0 || MemCmp(argv[1], "-help",5) == 0) {
      Print(CorangeB,"Created by Alexey Pozdnyakov in 02.2026 version 2.30\n");
      Print(Cgold,"email: avp70ru@mail.ru https://github.com/AVPscan/Fresh"); }
    flag = 0; }
  return flag; }
