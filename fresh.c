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
const uint8_t  Mcol = 0x07;
const uint8_t  Mbol = 0x08;
const uint8_t  Minv = 0x10;
const uint8_t  Mibc = 0x1F;
const Cell UNIT = (Cell)-1 / 255;
const Cell DIRTY_MASK = UNIT * Fresh;
const Cell CLEAN_MASK = ~DIRTY_MASK;
char      *Cdata      = NULL;
uint16_t  *Coffset    = NULL;
uint8_t   *Cattr      = NULL;
uint8_t   *Cvlen      = NULL;
uint8_t   *Clen       = NULL;
char      *Asbuf      = NULL;
char      *Avbuf      = NULL;
#define CellLine      8192
#define String        5062
#define SizeCOL       CellLine * 2
#define SizeCL        CellLine * 4
#define SizeData      ((size_t)String * SizeCL)
#define SizeOffset    ((size_t)String * SizeCOL)
#define SizeAttr      ((size_t)String * CellLine)
#define SizeVizLen    ((size_t)String * CellLine)
#define SizeLen       ((size_t)String * CellLine)
#define SizeSysBuff   4096
#define SizeVBuff     65536
#define SizeVram      (SizeData + SizeOffset + SizeAttr + SizeVizLen + SizeLen + SizeSysBuff + SizeVBuff)
#define Data(r)       (Cdata + ((r) << 15))
#define Offset(r, c)  (Coffset + ((r) << 14) + (c))
#define Attr(r, c)    (Cattr + ((r) << 13) + (c))  // 7 Dirty 65 Reserve 4 Bold 3 Inverse 210 Colour
#define Visi(r, c)    (Cvlen + ((r) << 13) + (c))
#define Len(r, c)     (Clen + ((r) << 13) + (c))
#define Colour(col)   (Asbuf + ((col) << 5))       // 0 Current 1 Bw 2-7 Palette
#define Parse(ibc)    (Asbuf + 256 + ((ibc) << 5)) // 0-31 All

void InitPD(uint8_t col) { col &= Mcol;
  const char* modes[4] = { "\012\03322;27;55;", "\011\03327;55;1;", "\011\03322;7;53;", "\010\0331;7;53;" };
  uint8_t m = 4, lm, ibc, ca, c; char *ac, *dst;
  if (col) { ac = Colour(col); dst = Colour(0); MemCpy(dst , (ac + 1), *ac); }
  while(m) { const char* mode = modes[--m]; lm = *mode++, c = 8; 
      while(c) { ac = Colour(--c); ibc = (m << 3) + c; ca = *ac - 4; ac += 4; 
        dst = Parse(ibc); *dst++ = lm + ca; MemCpy(dst, mode, lm); MemCpy(dst + lm, ac, ca); } } }
        
void InitVram(size_t addr, size_t size) { if (!addr || (size < SizeVram)) return;
  const char* colors[] = { Green, ColorOff, Grey, Green, Red, Blue, Orange, Gold };
  uint8_t len, lco = strlen(ColorOff); char *slot, *rep; uint16_t *a1, *a2, i = 0;
  Cdata   = (char*)(addr);
  Coffset = (uint16_t*)(Cdata + SizeData);
  Cattr   = (uint8_t*)((char*)Coffset + SizeOffset);
  Cvlen   = (uint8_t*)((uint8_t*)Cattr + SizeAttr);
  Clen    = (uint8_t*)((uint8_t*)Cvlen + SizeVizLen);
  Asbuf   = (char*)((uint8_t*)Clen + SizeLen);
  Avbuf   = (char*)((char*)Asbuf + SizeSysBuff);
  a1 = Coffset; while(i < CellLine) *a1++ = i++;
  a2 = a1; i = 0; while(++i < String) { MemCpy(a1,a2,SizeCOL); a1 = a2; a2 += SizeCOL; }
  i = 8; MemSet(Cdata,' ',SizeData); MemSet(Cattr,Fresh,SizeAttr); MemSet(Cvlen,1,(SizeVizLen + SizeLen));
  while (i--) { slot = Colour(i); rep = slot;
      *rep++ = lco; MemCpy(rep, ColorOff, lco); len = strlen(colors[i]);
      *slot++ = len; MemCpy(slot, colors[i], len); } 
 InitPD(0); }

void help() {
    printf(Grey "Created by " Green "Alexey Pozdnyakov" Grey " in " Green "02.2026" Grey 
           " version " Green "2.13" Grey ", email " Green "avp70ru@mail.ru" Grey 
           " github " Green "https://github.com" Grey "\n"); }

int main(int argc, char *argv[]) {
  if (argc > 1) { if (strcmp(argv[1], "-?") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0) help();
                  return 0; }
  int w, h, cur_x = 0, cur_y = 0;
  size_t size = SizeVram, ram, sc; if (!(ram = GetRam(&size))) return 0;
  SWD(ram); InitVram(ram,size); Delay_ms(0); SetInputMode(1); sc = ((size*10)/1048576);
  printf(Reset HideCur WrapOff "%zu", sc); fflush(stdout); snprintf((char*)Cdata, 128, "%zu", sc);
  while (1) {
    sc = SyncSize(ram); w = GetCR(&h); Delay_ms(20); const char* k = GetKey();
    if (k[0] == 27) {
        if (k[1] == K_NO) continue;
        if (k[1] == K_ESC) break;
        if (k[1] == K_UP)  cur_y--;
        if (k[1] == K_DOW) cur_y++;
        if (k[1] == K_RIG) cur_x++;
        if (k[1] == K_LEF) cur_x--;
        if (k[1] == K_TAB) cur_x = (cur_x + 8) & ~7;
        if (k[1] == K_HOM) cur_x = 0;
        if (k[1] == K_END) cur_x = w;
        if (k[1] == K_PUP) cur_y -= h;
        if (k[1] == K_PDN) cur_y += h; }
    }
  SetInputMode(0); printf(ShowCur WrapOn Reset); fflush(stdout); FreeRam(ram, size); return 0; }
