/*
 * Copyright (C) 2026 Поздняков Алексей Васильевич
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "sys.h"

const uint8_t Fresh = 0x80;
const uint8_t  Mcol = 0x1C;
const uint8_t  Mbol = 0x02;
const uint8_t  Minv = 0x01;
const uint8_t  Mcbi = 0x1F;
const Cell UNIT = (Cell)-1 / 255;
const Cell DIRTY_MASK = UNIT * Fresh;
const Cell CLEAN_MASK = ~DIRTY_MASK;
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

void SetColour(uint8_t col) { if (!(col &= Mcol)) col = 3;
  col <<= 2; MemCpy(Parse(0), Parse(col), 128); }

void InitVram(size_t addr, size_t size) { if (!addr || (size < SizeVram)) return;
  const char* colors[] = { Green, ColorOff, Grey, Green, Red, Blue, Orange, Gold };
  const char* modes[] = { "\007;22;27m", "\006;22;7m", "\006;1;27m", "\005;1;7m" };
  uint8_t lm, cbi, ca, c = strlen(ColorOff), i = 8; char *ac, *dst;
  Cdata = (char*)(addr); Coffset = (uint16_t*)(Cdata + SizeData); Cattr = (uint8_t*)((uint8_t*)Coffset + SizeOffset);
  Cvlen = (uint8_t*)(Cattr + SizeAttr); Clen = (uint8_t*)(Cvlen + SizeVizLen);
  Apdat = (char*)(Clen + SizeLen); Awdat = (uint16_t*)(Apdat + SizePalBuff); Avdat = (char*)((uint8_t*)Awdat + SizeWinDat);
  while (i--) { ac = (Avdat + ((i) << 5)); dst = ac; *dst++ = c; MemCpy(dst, ColorOff, c);
      ca = strlen(colors[i]); if (ca) { *ac++ = ca; MemCpy(ac, colors[i], ca); } }
  i = 4; while(i) { const char* mode = modes[--i]; lm = *mode++, c = 8; 
            while(c) { ac = (Avdat + ((--c) << 5)); cbi = (c << 2) + i; ca = (*ac++ - 1);
                dst = Parse(cbi); *dst++ = (lm + ca); MemCpy(dst, ac, ca); MemCpy(dst + ca, mode, lm); } } }

void ShowC(int16_t x, int16_t y, uint8_t on, char *str) {
  char *src, *dst = Avdat, *sav; uint8_t i,c;
  *dst++ = 27; *dst++ = '['; src = dst; do { *src++ = '0' + (y % 10); y /= 10; } while (y);
  sav = src; i = (uint8_t)(src - dst) / 2; while(i--) { c = *dst; *dst++ = *--src; *src = c; }
  *sav++ = ';'; dst = sav; do { *dst++ = '0' + (x % 10); x /= 10; } while (x);
  src = dst; i = (uint8_t)(dst - sav) / 2; while(i--) { c = *sav; *sav++ = *--dst; *dst = c; }
  *src++ = 'H'; if (on) { dst = Parse(1); MemCpy(src, (dst + 1), *dst); src += *dst; }
  i = strlen(str); MemCpy(src, str, i); src += i; if (on) { sav = Parse(0); MemCpy(src, (sav + 1), (*sav)); src += *sav; }
  write (1, Avdat, (src - Avdat)); }
  
typedef struct {int16_t X, Y, viewX, viewY, dX, dY; uint8_t on, flag, key, Lkey; uint16_t rk; } Cur_;
Cur_ Cur = {30,0,0,0,1,1,0,0,0,0,0};
void Cursor(void) {
  uint16_t r, c =TermCR(&r); Cur.rk++; Cur.flag = 0;
  if (Cur.key != Cur.Lkey) { Cur.Lkey = Cur.key; Cur.rk = 0; Cur.dX = 1; Cur.dY = 1; }
  if (Cur.key == K_UP || Cur.key == K_DOW || Cur.key == K_LEF || Cur.key == K_RIG) {
      if (Cur.rk > 5) {
          if (!(Cur.rk % 3)) {
              if (Cur.dX < 100) Cur.dX <<= 2;
              if (Cur.dY < 100) Cur.dY <<= 2; } } }
  if (Cur.on) { ShowC(Cur.X + Cur.viewX + 1, Cur.Y + Cur.viewY + 1, 0, " "); Cur.on = 0; }
  if (Cur.key == K_LEF) Cur.X -= Cur.dX;
  else if (Cur.key == K_RIG) Cur.X += Cur.dX;
  else if (Cur.key == K_UP) Cur.Y -= Cur.dY;
  else if (Cur.key == K_DOW) Cur.Y += Cur.dY;
  if ((Cur.X + Cur.viewX) < 0) { Cur.viewX = - Cur.X; Cur.flag++; }
  else if ((Cur.X + Cur.viewX) >= c) { Cur.viewX = Cur.X - c; Cur.flag++; }
  if ((Cur.Y + Cur.viewY) < 0) { Cur.viewY = - Cur.Y; Cur.flag++; }
  else if ((Cur.Y + Cur.viewY) >= r) { Cur.viewY = Cur.Y - 1 - r; Cur.flag++; } 
  ShowC(Cur.X + Cur.viewX + 1, Cur.Y + Cur.viewY + 1, 1, " "); Cur.on = 1; }

void Print(uint8_t n, char *str) { n &= Mcbi; if (!str) return;
  char *dst = Avdat + 256, *sav; uint16_t len;
  sav = Parse(n); len = *sav++; MemCpy(dst, sav, len); dst += len;
  len = strlen(str); MemCpy(dst, str, len); dst += len;
  sav = Parse(0); len = *sav++; MemCpy(dst, sav, len); dst += len; write(1, Avdat + 256, (dst - Avdat - 256)); }
 
void help() {
    printf(Grey "Created by " Green "Alexey Pozdnyakov" Grey " in " Green "02.2026" Grey 
           " version " Green "2.13" Grey ", email " Green "avp70ru@mail.ru" Grey 
           " github " Green "https://github.com" Grey "\n"); }

int main(int argc, char *argv[]) {
  if (argc > 1) { if (strcmp(argv[1], "-?") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0) help();
                  return 0; }
  uint16_t ff; size_t size = SizeVram, ram, sc; if (!(ram = GetRam(&size))) return 0;
  SWD(ram); InitVram(ram,size); SwitchRaw(); Delay_ms(0); ff = SyncSize(ram,0); sc = ((size*10)/1048576);
  Print(0,Reset HideCur Cls WrapOn); snprintf(Avdat, 128, "%zu %zu\n", sc, size); Print(42,Avdat);
  char *aa;
  for (int i = 0; i < 32; i++) { aa = Parse(i); uint8_t c = *aa++;
      for (int j = 0; j < c; j++) { uint8_t c = *aa++;
          if (c == 27) Print(58,"Esc");
          else { snprintf(Avdat, 128, "%c", c); Print(56,Avdat); } }
      Print(0,"\n"); }
  Print(0,SaveCur);
  while (1) {
    if ((ff = SyncSize(ram,1))) { ff = 1; }
    Print(0,LoadCur ClearLine LoadCur); snprintf(Avdat, 128, "%d %d ", Cur.X, Cur.Y); Print(4,Avdat);
    Delay_ms(20); const char* k = GetKey();
    if (k[0] == 27) Cur.key = k[1];
    else Cur.key = 0;
    Cursor();
    if (k[0] == 27) {
        if (k[1] == K_NO) continue;
        if (k[1] == K_ESC) break;
         } }
  SwitchRaw(); Print(0,ShowCur WrapOn Reset); FreeRam(ram, size); return 0; }
