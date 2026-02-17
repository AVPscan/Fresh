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
  col <<= 2; MemCpy( Parse(0), Parse(col), 128); }

void InitVram(size_t addr, size_t size) { if (!addr || (size < SizeVram)) return;
  const char* colors[] = { Green, ColorOff, Grey, Green, Red, Blue, Orange, Gold };
  const char* modes[] = { "\011\033[1;7;53;", "\012\033[1;27;55;", "\012\033[22;7;53;", "\013\033[22;27;55;" };
  uint8_t lm, cbi, ca, c = strlen(ColorOff), i = 8; char *ac, *dst;
  Cdata = (char*)(addr); Coffset = (uint16_t*)(Cdata + SizeData); Cattr = (uint8_t*)((uint8_t*)Coffset + SizeOffset);
  Cvlen = (uint8_t*)(Cattr + SizeAttr); Clen = (uint8_t*)(Cvlen + SizeVizLen);
  Apdat = (char*)(Clen + SizeLen); Awdat = (uint16_t*)(Apdat + SizePalBuff); Avdat = (char*)((uint8_t*)Awdat + SizeWinDat);
  while (i--) { ac = (Avdat + ((i) << 5)); dst = ac; *dst++ = c; MemCpy(dst, ColorOff, c);
      ca = strlen(colors[i]); if (ca) { *ac++ = ca; MemCpy(ac, colors[i], ca); } }
  i = 4; while(i) { const char* mode = modes[--i]; lm = *mode++, c = 8; 
            while(c) { ac = (Avdat + ((--c) << 5)); cbi = (c << 2) + i; ca = *ac - 2; ac += 2;
                dst = Parse(cbi); *dst++ = lm + ca; MemCpy(dst, mode, lm); MemCpy(dst + lm, ac, ca); } } }

typedef struct { int16_t X, Y, luX, luY, dX, dY, oX, oY; uint8_t on, flag, key, Lkey; uint16_t rk, rLk; } Cur_;
Cur_ Cur = {0,0,0,0,1,1,0,0,0,0,0,0,0,0};
void Cursor(void) {
  Cur.rk++;
  if (Cur.key != Cur.Lkey) { Cur.Lkey = Cur.key; Cur.rLk = Cur.rk; Cur.rk = 0; Cur.dX = 1; Cur.dY = 1; }
  else if (Cur.key == K_UP || Cur.key == K_DOW || Cur.key == K_LEF || Cur.key == K_RIG) {
          if (!(Cur.rk % 30)) {
              if (Cur.dX < 64) Cur.dX <<= 1;
              if (Cur.dY < 64) Cur.dY <<= 1; }
          uint16_t r, c =TermCR(&r); Cur.flag = 0;
          if (Cur.on) { Cur.oX = Cur.X; Cur.oY = Cur.Y; }
          if (Cur.key == K_LEF) Cur.X -= Cur.dX;
          else if (Cur.key == K_RIG) Cur.X += Cur.dX;
          else if (Cur.key == K_UP) Cur.Y -= Cur.dY;
          else Cur.Y += Cur.dY; 
          if ((Cur.X + Cur.luX) < 0) { Cur.luX = 10 - Cur.X; Cur.flag++; }
          if ((Cur.X + Cur.luX) > c) { Cur.luX = c - 10 - Cur.X; Cur.flag++; }
          if ((Cur.Y + Cur.luY) < 0) { Cur.luY = 4 - Cur.Y; Cur.flag++; }
          if ((Cur.Y + Cur.luY) > r) { Cur.luY = r - 4 - Cur.Y; Cur.flag++; } } }
 
void help() {
    printf(Grey "Created by " Green "Alexey Pozdnyakov" Grey " in " Green "02.2026" Grey 
           " version " Green "2.13" Grey ", email " Green "avp70ru@mail.ru" Grey 
           " github " Green "https://github.com" Grey "\n"); }

int main(int argc, char *argv[]) {
  if (argc > 1) { if (strcmp(argv[1], "-?") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0) help();
                  return 0; }
  uint16_t ff; size_t size = SizeVram, ram, sc; if (!(ram = GetRam(&size))) return 0;
  SWD(ram); InitVram(ram,size); SwitchRaw(); Delay_ms(0); ff = SyncSize(ram,0); sc = ((size*10)/1048576);
  printf(Reset HideCur WrapOn "%zu\n" SaveCur, sc); fflush(stdout); snprintf((char*)Cdata, 128, "%zu", size);
  while (1) {
    if ((ff = SyncSize(ram,1))) { ff = 1; }
    printf( LoadCur ClearLine LoadCur "%d %d %d %d", Cur.X, Cur.Y, Cur.on, Cur.flag); fflush(stdout);
    Delay_ms(20); const char* k = GetKey();
    if (k[0] == 27) Cur.key = k[1];
    else Cur.key = 0;
    Cursor();
    if (k[0] == 27) {
        if (k[1] == K_NO) continue;
        if (k[1] == K_ESC) break;
         } }
  SwitchRaw(); printf(ShowCur WrapOn Reset); fflush(stdout); FreeRam(ram, size); return 0; }
