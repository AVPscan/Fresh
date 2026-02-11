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

const Cell DIRTY_MASK = (Cell)0x8181818181818181ULL & (Cell)-1;
inline int is_cell_dirty(const void *ptr) { return (*(const Cell *)ptr & DIRTY_MASK) != 0; }
typedef struct {
    uint16_t len;                                               // длина в байтах токена
    uint16_t visual;                                            // визуальная ширина в ячейках терминала токена
} Token;
typedef struct {
    uint16_t len;                                               // Текущая длина в байтах строки
    uint16_t count;                                             // Текущее кол-во токенов в стороке
    uint16_t visual;                                            // Общая визуальная ширина строки
} LineData;

void help() {
    printf(Cnn "Created by " Cna "Alexey Pozdnyakov" Cnn " in " Cna "02.2026" Cnn 
           " version " Cna "1.10" Cnn ", email " Cna "avp70ru@mail.ru" Cnn 
           " github " Cna "https://github.com" Cnn "\n"); }

int main(int argc, char *argv[]) {
  if (argc > 1) { if (strcmp(argv[1], "-?") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0) help();
                  return 0; }
  int w = 0, h = 0, s = 0, mode = 0; size_t sz, Vram; double real_ms; struct timespec real_start, real_end;
  if (!(Vram = GetVram(&sz))) return 0;
  char *G_DATA = (char*)(Vram + SYSTEM_SECTOR_SIZE);            // Данные (байты)
  char *G_ATTRIBUTE = (char*)(G_DATA + GLOBAL_DATA_SIZE);       // цвет, жирность.. аттрибут изменения токена x081
  Token *G_TOKENS = (Token*)(G_ATTRIBUTE + GLOBAL_ATTR_SIZE);   // Метаданные (атомы) в строке
  LineData *G_LINE = (LineData*)(G_TOKENS + GLOBAL_TOKEN_SIZE); // Строки

  SWD(); delay_ms(0); SetInputMode(1); printf(HCur SCur); fflush(stdout);
  while (1) {
    if (os_sync_size()) { w = GetWH(&h); s = GetC(); 
    printf (Cls Cam "%ld" Cnu " %d %d %d" SCur "                                     ", sz, w, h, s); fflush(stdout); }
    clock_gettime(CLOCK_MONOTONIC, &real_start); delay_ms(60); clock_gettime(CLOCK_MONOTONIC, &real_end);
    real_ms = (real_end.tv_sec - real_start.tv_sec) * 1000.0 + (real_end.tv_nsec - real_start.tv_nsec) / 1000000.0;
    const char* k = GetKey(); if (k[0] == 27 && k[1] == K_NO) continue;
    if (k[0] == 'p') mode = (mode + 1) & 1;
    if (k[0] != 27) printf (LCur Cna " тоси%sбоси " Cap " %s ", Button("Погнали", mode), k);
    else printf (LCur Cna " тоси%sбоси " Cap "(%d)", Button("Погнали", mode), k[1]);
    printf(Cna " [Ms: " Cnu "%f" Cna "]       ", real_ms); fflush(stdout); 
    if (k[0] == 27 && k[1] == K_ESC) break; }
  SetInputMode(0); printf(ShCur Crs); fflush(stdout); FreeVram(); return 0; }
