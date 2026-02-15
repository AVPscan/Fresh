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

size_t     G_RAM_BASE   = 0;
char      *G_PALETTE    = NULL;
char      *G_DATA       = NULL;
uint8_t   *G_ATTRIBUTE  = NULL;
uint8_t   *G_VIS        = NULL;
uint16_t  *G_LENB       = NULL;

#define GLOBAL_CELL       8192
#define GLOBAL_STR        5062
#define SYSTEM_SECTOR     65536 
#define SIZE_DATA         ((size_t)GLOBAL_STR * GLOBAL_CELL * 4)
#define SIZE_ATTR         ((size_t)GLOBAL_STR * GLOBAL_CELL * 1)
#define SIZE_VIS          ((size_t)GLOBAL_STR * GLOBAL_CELL * 1)
#define SIZE_LENB         ((size_t)GLOBAL_STR * GLOBAL_CELL * 2)
#define GLOBAL_VRAM       (SYSTEM_SECTOR + SIZE_DATA + SIZE_ATTR + SIZE_VIS + SIZE_LENB)

#define GET_DATA(r)       (G_DATA + ((r) << 15)) 
#define GET_ATTR(r, c)    (G_ATTRIBUTE + ((r) << 13) + (c))
#define GET_VIS(r, c)     (G_VIS + ((r) << 13) + (c))
#define GET_LEN(r, c)     (G_LENB + ((r) << 13) + (c))

void InitPalette(void) {
    for (int i = 0; i < 8; i++) { char *slot = G_PALETTE + (i << 5);
        uint8_t len = strlen(ColorOff); *slot = len; memcpy(slot + 1, ColorOff, len); }
    const char* colors[] = { Green, Grey, Green, Red, Blue, Orange, Gold, ColorOff };  
    for (int i = 0; i < 7; i++) { char *slot = G_PALETTE + (i << 5);
        uint8_t len = strlen(colors[i]); *slot = len; memcpy(slot + 1, colors[i], len); } }
    
void help() {
    printf(Grey "Created by " Green "Alexey Pozdnyakov" Grey " in " Green "02.2026" Grey 
           " version " Green "1.90" Grey ", email " Green "avp70ru@mail.ru" Grey 
           " github " Green "https://github.com" Grey "\n"); }
           
int main(int argc, char *argv[]) {
  if (argc > 1) { if (strcmp(argv[1], "-?") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0) help();
                  return 0; }
  int w = 0, h = 0, cur_x = 0, cur_y = 0; size_t size = GLOBAL_VRAM, ram, sc; if (!(ram = GetRam(&size))) return 0;
  G_RAM_BASE   = ram;
  G_PALETTE    = (char*)(ram + SYSTEM_SECTOR) - 256;
  G_DATA       = (char*)(G_PALETTE + 256);
  G_ATTRIBUTE  = (uint8_t*)((char*)G_DATA + SIZE_DATA);
  G_VIS        = (uint8_t*)((char*)G_ATTRIBUTE + SIZE_ATTR);
  G_LENB       = (uint16_t*)((char*)G_VIS + SIZE_VIS);
  
  SWD(ram); InitPalette(); Delay_ms(0); SetInputMode(1); printf(Reset HideCur WrapOff);
  sc = ((size*10)/1048576); printf("%zu", sc); fflush(stdout);
  snprintf((char*)G_DATA, 128, "%zu", sc);
  while (1) { sc = SyncSize(ram);
    Delay_ms(20);
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
        if (k[1] == K_END) { if (cur_y >= 0 && cur_y < GLOBAL_STR) cur_x = w;
                             else cur_x = 0; }
        if (k[1] == K_PUP) cur_y -= h;
        if (k[1] == K_PDN) cur_y += h; }
    }
  SetInputMode(0); printf(ShowCur WrapOn Reset); fflush(stdout); FreeRam(ram, size); return 0; }
