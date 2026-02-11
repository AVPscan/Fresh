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
const Cell CLEAN_MASK = ~((Cell)0x8181818181818181ULL & (Cell)-1);

typedef struct {
    uint16_t len;                                               // длина в байтах токена
    uint16_t visual;                                            // визуальная ширина в ячейках терминала токена
} Token;
typedef struct {
    uint16_t len;                                               // Текущая длина в байтах строки
    uint16_t count;                                             // Текущее кол-во токенов в стороке
    uint16_t visual;                                            // Общая визуальная длина строки
} LineData;
char          *G_DATA      = NULL;
unsigned char *G_ATTRIBUTE = NULL;
Token         *G_TOKENS    = NULL;
LineData      *G_LINE      = NULL;
#define GET_BLINE(row)      (char*)(G_DATA + ((size_t)(row) << 13))
#define GET_ATTR(row,col)   (unsigned char*)(G_ATTRIBUTE + ((size_t)(row) << 13) + (size_t)(col))
#define GET_TOKEN(row)      (Token*)((char*)G_TOKENS + ((size_t)(row) << 13))
#define GET_DLINE(row)      (LineData*)((char*)G_LINE + (((size_t)(row) << 2) + ((size_t)(row) << 1)))
uint32_t decode_utf8_fast(unsigned char *s, int *len) {         // Декодер
    unsigned char c = s[0];
    if (c < 0x80) { *len = 1; return c; }
    if ((c & 0xE0) == 0xC0) { *len = 2; return ((c & 0x1F) << 6) | (s[1] & 0x3F); }
    if ((c & 0xE0) == 0xE0) { *len = 3; return ((c & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F); }
    if ((c & 0xF0) == 0xF0) { *len = 4; return ((c & 0x07) << 18) | ((s[1] & 0x3F) << 12) | ((s[2] & 0x3F) << 6) | (s[3] & 0x3F); }
    *len = 1; return 0xFFFD; }
int is_combining(uint32_t cp) {                                 // Проверка на "прилипалу" (модификатор)
    if ((cp >= 0x0300 && cp <= 0x036F) || (cp >= 0x1DC0 && cp <= 0x1DFF) || (cp >= 0x20D0 && cp <= 0x20FF) ||
        (cp == 0x200D || (cp >= 0xFE00 && cp <= 0xFE0F))) return 1;
    return 0; }
int get_vis_width(uint32_t cp) {
    if (cp == 0 || cp < 32 || (cp >= 0x7F && cp < 0xA0)) return 0;
    if (cp < 256) return 1;                                     // Специфические "нулевые" (ZWJ, модификаторы, диакритика)
    if (cp == 0x200B || cp == 0x200C || cp == 0x200D || cp == 0x200E || cp == 0x200F) return 0;
    if ((cp >= 0x0300 && cp <= 0x036F) || (cp >= 0x1DC0 && cp <= 0x1DFF) || 
        (cp >= 0x20D0 && cp <= 0x20FF) || (cp >= 0xFE00 && cp <= 0xFE0F) || 
        (cp >= 0xFE20 && cp <= 0xFE2F) || (cp >= 0xE0100 && cp <= 0xE01EF)) return 0;
    if (cp >= 0x1100 && (                                       // Широкие (CJK, Emoji, Fullwidth) — 2
        (cp >= 0x1100 && cp <= 0x115F) || 
        (cp == 0x2329 || cp == 0x232A) ||
        (cp >= 0x2E80 && cp <= 0xA4CF && cp != 0x303F) ||
        (cp >= 0xAC00 && cp <= 0xD7A3) ||
        (cp >= 0xF900 && cp <= 0xFAFF) ||
        (cp >= 0xFE10 && cp <= 0xFE19) ||
        (cp >= 0xFE30 && cp <= 0xFE6F) ||
        (cp >= 0xFF00 && cp <= 0xFF60) ||
        (cp >= 0xFFE0 && cp <= 0xFFE6) ||
        (cp >= 0x20000 && cp <= 0x2FFFD) ||
        (cp >= 0x30000 && cp <= 0x3FFFD) ||
        (cp >= 0x1F300) // Открытая граница для будущего!
    )) return 2;
    return 1; }
void smart_append(int row, int col, unsigned char *key) {
    if (row < 0 || col < 0 || key[0] < 32) return;
    int len; uint32_t cp = decode_utf8_fast(key, &len); int vw = get_vis_width(cp);
    LineData *ld = GET_DLINE(row);                              // "Мозг" строки
    char *d_ptr = GET_BLINE(row) + ld->len;                     // Текущий хвост в G_DATA
    Token *t_ptr = GET_TOKEN(row);                              // Начало массива токенов для строки
    if (is_combining(cp) && ld->count > 0) {                    // ПРИЛИПАЛА: расширяем последний токен
        Token *last = &t_ptr[ld->count - 1];
        if (ld->len + len < GLOBAL_SIZE_STR) { memcpy(d_ptr, key, len); last->len += len; ld->len += len; } }
    else if (vw > 0) {                                          // НОВЫЙ СИМВОЛ
        if (ld->count < (GLOBAL_SIZE_STR / 4) && ld->len + len < GLOBAL_SIZE_STR) {
            memcpy(d_ptr, key, len); t_ptr[ld->count].len = len; t_ptr[ld->count].visual = vw;
            ld->len += len; ld->count++; ld->visual += vw; } }
    unsigned char *a_ptr = GET_ATTR(row, ld->visual - (vw ? vw : 0));
    *a_ptr |= 0x81; }                                           // Помечаем ячейку атрибутов грязной (dirty бит 0x81)
int view_x = 0, view_y = 0;
void CRD(int row) {
    Cell *attr_ptr = (Cell*)GET_ATTR(row, 0); for (size_t i = 0; i < (GLOBAL_SIZE_STR / CELL_SIZE); i++) attr_ptr[i] &= CLEAN_MASK; }
void render_line_auto(int world_row, int screen_row) { int w, h; w = GetWH(&h); (void)w;
    printf("\033[%d;1H%s", screen_row + 1, LWOff);
    if (world_row < 0 || world_row >= GLOBAL_STRING) { printf("\033[%d;1H%s%s", screen_row + 1, LWOff, ELin); return; }
    LineData *ld = GET_DLINE(world_row);
    Token *t_ptr = GET_TOKEN(world_row);
    char *data_ptr = GET_BLINE(world_row);
    if (view_x < 0) { int padding = -view_x; for (int p = 0; p < padding; p++) printf(" "); }
    int cur_v_x = 0, byte_off = 0, i = 0; int start_x = (view_x < 0) ? 0 : view_x;
    while (i < ld->count && cur_v_x < view_x) { byte_off += t_ptr[i].len; cur_v_x += t_ptr[i].visual; i++; }
    if (ld->len > byte_off) { if (cur_v_x > start_x) printf(" ");
        fwrite(data_ptr + byte_off, 1, ld->len - byte_off, stdout); }
    printf(ELin); CRD(world_row); }
void refresh_world(int cur_x, int cur_y) {
    int w, h; w = GetWH(&h); if (cur_x < view_x) view_x = cur_x; 
    if (cur_x >= view_x + w) view_x = cur_x - w + 1;
    if (cur_y < view_y) view_y = cur_y;
    if (cur_y >= view_y + h) view_y = cur_y - h + 1;
    printf(HCur); for (int i = 0; i < h; i++) render_line_auto(view_y + i, i);
    int screen_x = (cur_x - view_x) + 1; int screen_y = (cur_y - view_y) + 1;
    if (screen_x >= 1 && screen_x <= w && screen_y >= 1 && screen_y <= h) printf("\033[%d;%dH%s", screen_y, screen_x, ShCur);
    else printf("\033[1;1H%s", HCur);
    fflush(stdout); }
    
void help() {
    printf(Cnn "Created by " Cna "Alexey Pozdnyakov" Cnn " in " Cna "02.2026" Cnn 
           " version " Cna "1.10" Cnn ", email " Cna "avp70ru@mail.ru" Cnn 
           " github " Cna "https://github.com" Cnn "\n"); }
           
int main(int argc, char *argv[]) {
  if (argc > 1) { if (strcmp(argv[1], "-?") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0) help();
                  return 0; }
  int len, cur_x = 0, cur_y = 0; size_t sz, Vram; if (!(Vram = GetVram(&sz))) return 0;
  G_DATA = (char*)(Vram + SYSTEM_SECTOR_SIZE);                  // Данные (байты)
  G_ATTRIBUTE = (unsigned char*)(G_DATA + GLOBAL_DATA_SIZE);    // цвет, фон, обновление токена x081
  G_TOKENS = (Token*)(G_ATTRIBUTE + GLOBAL_ATTR_SIZE);          // Метаданные (атомы) в строке
  G_LINE = (LineData*)((char*)G_TOKENS + GLOBAL_TOKEN_SIZE);    // Строки
  SWD(); Delay_ms(0); SetInputMode(1); printf(Cls); fflush(stdout);
  while (1) {
    if (SyncSize()) refresh_world(cur_x, cur_y); 
    Delay_ms(30);
    const char* k = GetKey();
    if (k[0] == 27 && k[1] == K_NO) continue;
    if (k[0] == 27 && k[1] == K_ESC) break;
    if (k[0] == 27) {
        if (k[1] == K_UP)  cur_y--;
        if (k[1] == K_DOW) cur_y++;
        if (k[1] == K_RIG) cur_x++;
        if (k[1] == K_LEF) cur_x--; }
    else { smart_append(cur_y, cur_x, (unsigned char*)k); cur_x += get_vis_width(decode_utf8_fast((unsigned char*)k, &len)); }
    refresh_world(cur_x, cur_y); }
  SetInputMode(0); printf(ShCur Crs); fflush(stdout); FreeVram(); return 0; }
