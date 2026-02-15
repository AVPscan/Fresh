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
char      *Adata      = NULL;
uint8_t   *Aattr      = NULL;
uint8_t   *Avlen      = NULL;
uint16_t  *Ablen      = NULL;
char      *Asbuf      = NULL;
char      *Avbuf      = NULL;
#define CellLine      8192
#define SizeSL        CellLine * 4
#define String        5062
#define SizeData      ((size_t)String * SizeSL)
#define SizeAttr      ((size_t)String * CellLine)
#define SizeVizLen    ((size_t)String * CellLine)
#define SizeBlen      ((size_t)String * CellLine * 2)
#define SizeSysBuff   4096
#define SizeVBuff     65536 
#define SizeVram      (SizeData + SizeAttr + SizeVizLen + SizeBlen + SizeSysBuff + SizeVBuff)

#define DATA(r)       (Adata + ((r) << 15))
#define ATTR(r, c)    (Aattr + ((r) << 13) + (c))
#define VIS(r, c)     (Avlen + ((r) << 13) + (c))
#define LEN(r, c)     (Ablen + ((r) << 13) + (c))

void InitPalette(void) {
    for (int i = 0; i < 8; i++) { char *slot = Asbuf + (i << 5);
        uint8_t len = strlen(ColorOff); *slot = len; memcpy(slot + 1, ColorOff, len); }
    const char* colors[] = { Green, Grey, Green, Red, Blue, Orange, Gold, ColorOff };  
    for (int i = 0; i < 7; i++) { char *slot = Asbuf + (i << 5);
        uint8_t len = strlen(colors[i]); *slot = len; memcpy(slot + 1, colors[i], len); } }
    
void help() {
    printf(Grey "Created by " Green "Alexey Pozdnyakov" Grey " in " Green "02.2026" Grey 
           " version " Green "2.11" Grey ", email " Green "avp70ru@mail.ru" Grey 
           " github " Green "https://github.com" Grey "\n"); }

int main(int argc, char *argv[]) {
  if (argc > 1) { if (strcmp(argv[1], "-?") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0) help();
                  return 0; }
  int w = 0, h = 0, cur_x = 0, cur_y = 0;
  size_t size = SizeVram, ram, sc; if (!(ram = GetRam(&size))) return 0;
  Adata = (char*)(ram);
  Aattr = (uint8_t*)((char*)Adata + SizeData);
  Avlen = (uint8_t*)((char*)Aattr + SizeAttr);
  Ablen = (uint16_t*)((char*)Avlen + SizeVizLen);
  Asbuf = (char*)((char*)Ablen + SizeBlen);
  Avbuf = (char*)((char*)Asbuf + SizeSysBuff);
  SWD(ram); InitPalette(); Delay_ms(0); SetInputMode(1); sc = ((size*10)/1048576);
  printf(Reset HideCur WrapOff "%zu", sc); fflush(stdout); snprintf((char*)Adata, 128, "%zu", sc);
  while (1) {
    sc = SyncSize(ram); Delay_ms(20);
    const char* k = GetKey();
    if (k[0] == 27 && k[1] == K_NO) continue;
    if (k[0] == 27 && k[1] == K_ESC) break;
    if (k[0] == 27) {
        if (k[1] == K_UP)  cur_y--;
        if (k[1] == K_DOW) cur_y++;
        if (k[1] == K_RIG) cur_x++;
        if (k[1] == K_LEF) cur_x--;
        if (k[1] == K_TAB) cur_x = (cur_x + 8) & ~7;
        if (k[1] == K_HOM) cur_x = 0;
        if (k[1] == K_END) { if (cur_y >= 0 && cur_y < String) cur_x = w;
                             else cur_x = 0; }
        if (k[1] == K_PUP) cur_y -= h;
        if (k[1] == K_PDN) cur_y += h; }
    }
  SetInputMode(0); printf(ShowCur WrapOn Reset); fflush(stdout); FreeRam(ram, size); return 0; }
