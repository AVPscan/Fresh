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

const Cell UNIT = (Cell)-1 / 255; 
const Cell DIRTY_MASK = UNIT * 0x80;
const Cell CLEAN_MASK = ~DIRTY_MASK;

size_t     G_RAM_BASE   = 0;
char      *G_DATA       = NULL;
char      *G_PALETTE    = NULL;
uint8_t   *G_ATTRIBUTE  = NULL;
uint16_t  *G_TOK_LEN    = NULL;
uint16_t  *G_TOK_VIS    = NULL;
uint16_t  *G_LIN_LEN    = NULL;
uint16_t  *G_LIN_VIS    = NULL;
uint16_t  *G_LIN_CNT    = NULL;
uint8_t   *G_LINE_DIRTY = NULL;
#define GLOBAL_CELL       4096 // 4096 utf8 на 4 байта  X=10922 всё это масштабируется изменением размера шрифта терминала.
#define GLOBAL_STR        2048 // 2048 реальных строк   Y=8192 это не перебор, в играх можно использовать как теневые экраны, причём много экранов.
#define SYSTEM_SECTOR     65536// атрибуты 76543210 бит из них 70 обновить 321 цвет 654 фон
#define SIZE_DATA         ((size_t)GLOBAL_STR * GLOBAL_CELL * 4) // 32 MB
#define SIZE_ATTR         ((size_t)GLOBAL_STR * GLOBAL_CELL * 1) // 8 MB (uint8)
#define SIZE_TOK_LEN      ((size_t)GLOBAL_STR * GLOBAL_CELL * 2) // 16 MB (uint16)
#define SIZE_TOK_VIS      ((size_t)GLOBAL_STR * GLOBAL_CELL * 2) // 16 MB (uint16)
#define SIZE_LINE_INF     ((size_t)GLOBAL_STR * 2)               // 4 KB (uint16)
#define SIZE_LINE_DIRTY   ((size_t)GLOBAL_STR)                   // 8 MB (uint8)
#define GLOBAL_VRAM       (SYSTEM_SECTOR + SIZE_DATA + SIZE_ATTR + (SIZE_TOK_LEN + SIZE_TOK_VIS) + (SIZE_LINE_INF * 3) + SIZE_LINE_DIRTY)

#define GET_BLINE(r)      (G_DATA + ((r) << 14)) 
#define GET_ATTR(r, c)    (G_ATTRIBUTE + ((r) << 12) + (c))
#define GET_TOK_LEN(r, c) (G_TOK_LEN + ((r) << 12) + (c))
#define GET_TOK_VIS(r, c) (G_TOK_VIS + ((r) << 12) + (c))
#define GET_LIN_LEN(r)    (G_LIN_LEN + (r))
#define GET_LIN_VIS(r)    (G_LIN_VIS + (r))
#define GET_LIN_CNT(r)    (G_LIN_CNT + (r))

uint32_t UTFlen(unsigned char *s, int *len) {
    unsigned char c = s[0];
    if (c < 0x80) { *len = 1; return c; }
    if ((c & 0xE0) == 0xC0) { *len = 2; return ((c & 0x1F) << 6) | (s[1] & 0x3F); }
    if ((c & 0xE0) == 0xE0) { *len = 3; return ((c & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F); }
    if ((c & 0xF0) == 0xF0) { *len = 4; return ((c & 0x07) << 18) | ((s[1] & 0x3F) << 12) | ((s[2] & 0x3F) << 6) | (s[3] & 0x3F); }
    *len = 1; return 0xFFFD; }
int UTFcom(uint32_t cp) {
    if ((cp >= 0x0300 && cp <= 0x036F) || (cp >= 0x1DC0 && cp <= 0x1DFF) || (cp >= 0x20D0 && cp <= 0x20FF) ||
        (cp == 0x200D || (cp >= 0xFE00 && cp <= 0xFE0F))) return 1;
    return 0; }
int UTFvw(uint32_t cp) {
    if (cp == 0 || cp < 32 || (cp >= 0x7F && cp < 0xA0)) return 0;
    if (cp < 256) return 1;
    if (cp == 0x200B || cp == 0x200C || cp == 0x200D || cp == 0x200E || cp == 0x200F) return 0;
    if ((cp >= 0x0300 && cp <= 0x036F) || (cp >= 0x1DC0 && cp <= 0x1DFF) || 
        (cp >= 0x20D0 && cp <= 0x20FF) || (cp >= 0xFE00 && cp <= 0xFE0F) || 
        (cp >= 0xFE20 && cp <= 0xFE2F) || (cp >= 0xE0100 && cp <= 0xE01EF)) return 0;
    if (cp >= 0x1100 && (
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

int UTFadd(int row, int col, unsigned char *key) {
    if (row >= GLOBAL_STR || row < 0) return 0;
    if (row < 0 || col < 0 || key[0] < 32) return 0;
    uint16_t *l_vis = GET_LIN_VIS(row);
    while (*l_vis < col) {
        if (*l_vis >= GLOBAL_CELL) break;      
        unsigned char space = ' ';
        UTFadd(row, *l_vis, &space); }
    int len; 
    uint32_t cp = UTFlen(key, &len); 
    int vw = UTFvw(cp);
    int is_comb = UTFcom(cp);
    uint16_t *l_cnt = GET_LIN_CNT(row);
    uint16_t *l_len = GET_LIN_LEN(row);
    int t_idx = 0, cur_v = 0, b_off = 0;
    while (t_idx < *l_cnt && cur_v < col) {
        cur_v += *GET_TOK_VIS(row, t_idx);
        b_off += *GET_TOK_LEN(row, t_idx);
        t_idx++; }
    if (is_comb && t_idx > 0) {
        int target = t_idx - 1;
        if (*l_len + len < (GLOBAL_CELL * 4)) {
            char *insert_ptr = GET_BLINE(row) + b_off;
            if (b_off < *l_len) memmove(insert_ptr + len, insert_ptr, *l_len - b_off);
            memcpy(insert_ptr, key, len);
            *(GET_TOK_LEN(row, target)) += len;
            *l_len += len;
            *GET_ATTR(row, cur_v - *GET_TOK_VIS(row, target)) |= 0x80; }
        return 0; }
    if (vw > 0) {
        if (*l_cnt < GLOBAL_CELL && *l_len + len < (GLOBAL_CELL * 4)) {
            char *data_ptr = GET_BLINE(row);
            if (b_off < *l_len) memmove(data_ptr + b_off + len, data_ptr + b_off, *l_len - b_off);
            memcpy(data_ptr + b_off, key, len);
            int tokens_to_move = *l_cnt - t_idx;
            if (tokens_to_move > 0) {
                memmove(GET_TOK_LEN(row, t_idx + 1), GET_TOK_LEN(row, t_idx), tokens_to_move * 2);
                memmove(GET_TOK_VIS(row, t_idx + 1), GET_TOK_VIS(row, t_idx), tokens_to_move * 2); }
            *GET_TOK_LEN(row, t_idx) = len;
            *GET_TOK_VIS(row, t_idx) = vw;
            *l_len += len;
            *l_vis += vw;
            (*l_cnt)++;
            *GET_ATTR(row, col) |= 0x80; } }
    G_LINE_DIRTY[row] = 1; return vw; }
void UTFdel(int row, int col) {
    if (row < 0 || col < 0) return;
    uint16_t *l_cnt = GET_LIN_CNT(row);
    uint16_t *l_len = GET_LIN_LEN(row);
    uint16_t *l_vis = GET_LIN_VIS(row);
    if (*l_cnt == 0) return;
    int t_idx = 0, cur_v = 0, b_off = 0;
    while (t_idx < *l_cnt && cur_v < col) {
        cur_v += *GET_TOK_VIS(row, t_idx);
        b_off += *GET_TOK_LEN(row, t_idx);
        t_idx++; }
    if (t_idx >= *l_cnt) return;
    int del_len = *GET_TOK_LEN(row, t_idx);
    int del_vis = *GET_TOK_VIS(row, t_idx);
    char *data_ptr = GET_BLINE(row);
    int bytes_to_move = *l_len - (b_off + del_len);
    if (bytes_to_move > 0) memmove(data_ptr + b_off, data_ptr + b_off + del_len, bytes_to_move);
    int tokens_to_move = *l_cnt - (t_idx + 1);
    if (tokens_to_move > 0) {
        memmove(GET_TOK_LEN(row, t_idx), GET_TOK_LEN(row, t_idx + 1), tokens_to_move * 2);
        memmove(GET_TOK_VIS(row, t_idx), GET_TOK_VIS(row, t_idx + 1), tokens_to_move * 2); }
    *l_len -= del_len;
    *l_vis -= del_vis;
    (*l_cnt)--;
    *GET_ATTR(row, col) |= 0x80; G_LINE_DIRTY[row] = 1; }

int view_x = 0, view_y = 0;
void ReFresh(int cur_x, int cur_y) {
    int w, h; w = GetCR(&h);
    if (cur_x < view_x) view_x = cur_x; if (cur_y < view_y) view_y = cur_y;
    if (cur_x >= view_x + w) view_x = cur_x - w + 1;
    if (cur_y >= view_y + h) view_y = cur_y - h + 1;
    char *buf_start = (char*)G_RAM_BASE + 128; char *p = buf_start; uint8_t last_clr = 0xFF;
    memcpy(p, "\033[H", 3); p += 3;
    for (int i = 0; i < h; i++) {
        int world_row = view_y + i; int screen_col = 0; uint8_t inv_now = 0; 
        if (world_row < 0 || world_row >= GLOBAL_STR) {
            if (!inv_now) { memcpy(p, "\033[7m", 4); p += 4; inv_now = 1; }
            if (last_clr != 0) { char *c=G_PALETTE+(0<<5); memcpy(p,c+1,*c); p+=*c; last_clr=0; }
            MemSet(p, ' ', w); p += w; screen_col = w; } 
        else {
            int left_pad = (view_x < 0) ? -view_x : 0;
            if (left_pad > w) left_pad = w;
            if (left_pad > 0) {
                if (!inv_now) { memcpy(p, "\033[7m", 4); p += 4; inv_now = 1; }
                if (last_clr != 0) { char *c=G_PALETTE+(0<<5); memcpy(p,c+1,*c); p+=*c; last_clr=0; }
                MemSet(p, ' ', left_pad); p += left_pad; screen_col += left_pad; }
            if (inv_now) { memcpy(p, "\033[27m", 5); p += 5; inv_now = 0; }
            uint16_t l_cnt = *GET_LIN_CNT(world_row);
            uint16_t l_vis = *GET_LIN_VIS(world_row);
            int start_x = (view_x < 0) ? 0 : view_x;
            if (l_cnt > 0 && start_x < l_vis) {
                int cur_v_x = 0, byte_off = 0, t_idx = 0;
                while (t_idx < l_cnt && cur_v_x < start_x) {
                    byte_off += *GET_TOK_LEN(world_row, t_idx);
                    cur_v_x  += *GET_TOK_VIS(world_row, t_idx);
                    t_idx++; }
                char *data_ptr = GET_BLINE(world_row);
                while (t_idx < l_cnt && screen_col < w) {
                    uint8_t *a_ptr = GET_ATTR(world_row, cur_v_x);
                    uint8_t clr = *a_ptr & 0x1F;
                    if (clr != last_clr) {
                        if (clr & 0x08) { memcpy(p, "\033[1m", 4); p += 4; } // Жирный
                        else { memcpy(p, "\033[22m", 5); p += 5; }
                        if (clr & 0x10) { memcpy(p, "\033[7;53m", 7); p += 7; } // Инв + Над
                        else { memcpy(p, "\033[27;55m", 8); p += 8; }
                        char *s = G_PALETTE + ((clr & 0x07) << 5); memcpy(p, s+1, *s); p += *s;
                        last_clr = clr; }
                    int bv = 0, bb = 0;
                    while (t_idx < l_cnt && screen_col + bv < w) {
                        uint8_t *cur_a = GET_ATTR(world_row, cur_v_x + bv);
                        if ((*cur_a & 0x1F) != clr) break;
                        bb += *GET_TOK_LEN(world_row, t_idx);
                        bv += *GET_TOK_VIS(world_row, t_idx);
                        *cur_a &= CLEAN_MASK;
                        t_idx++; }
                    memcpy(p, data_ptr + byte_off, bb);
                    p += bb; byte_off += bb; screen_col += bv; cur_v_x += bv; } }
            if (screen_col < w) {
                int canvas_end = (GLOBAL_CELL - view_x);
                if (canvas_end > w) canvas_end = w;
                if (screen_col < canvas_end) {
                    if (last_clr != 7) { char *c=G_PALETTE+(7<<5); memcpy(p,c+1,*c); p+=*c; last_clr=7; }
                    MemSet(p, ' ', canvas_end - screen_col);
                    p += (canvas_end - screen_col);
                    screen_col = canvas_end; }
                if (screen_col < w) {
                    if (!inv_now) { memcpy(p, "\033[7m", 4); p += 4; inv_now = 1; }
                    if (last_clr != 0) { char *c=G_PALETTE+(0<<5); memcpy(p,c+1,*c); p+=*c; last_clr=0; }
                    MemSet(p, ' ', w - screen_col); p += (w - screen_col); } } }
        if (inv_now) { memcpy(p, "\033[27m", 5); p += 5; }
        if (i < h - 1) { *p++ = '\r'; *p++ = '\n'; }
        last_clr = 0xFF; 
        if (world_row >= 0 && world_row < GLOBAL_STR) G_LINE_DIRTY[world_row] = 0; }
    int sx = (cur_x - view_x) + 1; int sy = (cur_y - view_y) + 1;
    write(1, buf_start, p - buf_start); p = buf_start;
    if (cur_y < 0 || cur_x < 0 || cur_y >= GLOBAL_STR || cur_x >= GLOBAL_CELL) {
        p += sprintf(p, "\033[%d;%dH X:%d Y:%d | RAM:%s ", h, w-38, cur_x, cur_y, (char*)G_RAM_BASE); }
    if (cur_y >= 0 && cur_y < GLOBAL_STR && cur_x >= 0 && cur_x < GLOBAL_CELL) { memcpy(p, "\033[?25h", 6); p += 6; }
    p += sprintf(p, "\033[%d;%dH", sy, sx); write(1, buf_start, p - buf_start); }

void LineAdd(int row, int col, char *str) { 
    while (*str) { int shift = UTFadd(row, col, (unsigned char*)str);
        int bytes; UTFlen((unsigned char*)str, &bytes); str += bytes;
        col += shift; } }

void InitPalette() {
    for (int i = 0; i < 8; i++) {
        char *slot = G_PALETTE + (i << 5); uint8_t len = strlen(Crs);
        *slot = len; memcpy(slot + 1, Crs, len); }
    const char* colors[] = { Cna, Cnn, Cna, Cpr, Cnu, Cap, Cam, Crs };  
    for (int i = 0; i < 7; i++) {
        char *slot = G_PALETTE + (i << 5); uint8_t len = strlen(colors[i]);
        *slot = len; memcpy(slot + 1, colors[i], len); } 
    memset(G_LINE_DIRTY, 1, GLOBAL_STR); }
    
void help() {
    printf(Cnn "Created by " Cna "Alexey Pozdnyakov" Cnn " in " Cna "02.2026" Cnn 
           " version " Cna "1.90" Cnn ", email " Cna "avp70ru@mail.ru" Cnn 
           " github " Cna "https://github.com" Cnn "\n"); }
           
int main(int argc, char *argv[]) {
  if (argc > 1) { if (strcmp(argv[1], "-?") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0) help();
                  return 0; }
  int w, h, cur_x = 0, cur_y = 0; size_t size = GLOBAL_VRAM, ram; if (!(ram = GetRam(&size))) return 0;
  G_RAM_BASE = ram;
  G_DATA      = (char*)(ram + SYSTEM_SECTOR);
  G_PALETTE   = (char*)G_DATA - 256;
  G_ATTRIBUTE = (uint8_t*)((char*)G_DATA + SIZE_DATA);
  G_TOK_LEN   = (uint16_t*)((char*)G_ATTRIBUTE + SIZE_ATTR);
  G_TOK_VIS   = (uint16_t*)((char*)G_TOK_LEN + SIZE_TOK_LEN);
  G_LIN_LEN   = (uint16_t*)((char*)G_TOK_VIS + SIZE_TOK_VIS);
  G_LIN_VIS   = (uint16_t*)((char*)G_LIN_LEN + SIZE_LINE_INF);
  G_LIN_CNT   = (uint16_t*)((char*)G_LIN_VIS + SIZE_LINE_INF);
  G_LINE_DIRTY = (uint8_t*)(G_LIN_CNT + GLOBAL_STR);
  SWD(ram); InitPalette(); Delay_ms(0); SetInputMode(1); printf(Crs LWOff);  fflush(stdout);
  snprintf((char*)ram, 128, "%zu", size); LineAdd(0,0,(char*)ram);
  while (1) {
    if (SyncSize(ram)) { write(1, "\033[2J\033[H\033[0m", 11); memset(G_LINE_DIRTY, 1, GLOBAL_STR);
          ReFresh(cur_x, cur_y); w = GetCR(&h); (void)w; }
    Delay_ms(20);
    const char* k = GetKey();
    if (k[0] == 27 && k[1] == K_NO) continue;
    if (k[0] == 27 && k[1] == K_ESC) break;
    if (k[0] == 27) {
        if (k[1] == K_UP)  cur_y--;
        if (k[1] == K_DOW) cur_y++;
        if (k[1] == K_RIG) cur_x++;
        if (k[1] == K_LEF) cur_x--;
        if (k[1] == K_HOM) cur_x = 0;
        if (k[1] == K_END) { if (cur_y >= 0 && cur_y < GLOBAL_STR) cur_x = *GET_LIN_VIS(cur_y);
                             else cur_x = 0; }
        if (k[1] == K_PUP) cur_y -= h;
        if (k[1] == K_PDN) cur_y += h;
        if ((k[1] == K_LF || k[1] == K_ENT)) { 
              if (cur_y >= 0 && cur_x >= 0) { G_LINE_DIRTY[cur_y] = 1; cur_x = 0; if (cur_y < GLOBAL_STR-1) cur_y++; }
              else { cur_y = 0; cur_x = 0; G_LINE_DIRTY[cur_y] = 1; } }
        if (k[1] == K_DEL) if (cur_x >= 0) { UTFdel(cur_y, cur_x); }
        if (k[1] == K_BAC) if (cur_x > 0) { cur_x--; UTFdel(cur_y, cur_x); } }
    else cur_x += UTFadd(cur_y, cur_x, (unsigned char*)k);
    ReFresh(cur_x, cur_y); }
  SetInputMode(0); printf(ShCur LWOn Crs); fflush(stdout); FreeRam(ram, size); return 0; }
