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
#define SYSTEM_SECTOR     65536// атрибуты 76543210 бит из них 7 обновить 210 цвет 
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
static const char ESC_HOME[] = "\033[H";
static const char ESC_INV[] = "\033[7m";
static const char ESC_NOINV[] = "\033[27m";
static const char ESC_BOLD[] = "\033[1m";
static const char ESC_NOBOLD[] = "\033[22m";
static const char ESC_INV53[] = "\033[7;53m";
static const char ESC_NOINV55[] = "\033[27;55m";
static const char ESC_CURSOR_ON[] = "\033[?25h";
static struct {
    uint8_t len;
    char seq[32];
} ESC_CACHE[32];
static char* fast_itoa(int val, char *p) { if (val < 0) { *p++ = '-'; val = -val; }
    char *start = p; do { *p++ = '0' + (val % 10); val /= 10; } while (val);
    char *end = p - 1; while (start < end) { char t = *start; *start++ = *end; *end-- = t; }
    return p; }

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
    if ((cp >= 0x0300 && cp <= 0x036F) || (cp >= 0x1DC0 && cp <= 0x1DFF) || (cp >= 0x20D0 && cp <= 0x20FF) || 
        (cp >= 0xFE00 && cp <= 0xFE0F) || (cp >= 0xFE20 && cp <= 0xFE2F) || (cp >= 0xE0100 && cp <= 0xE01EF)) return 0;
    if (cp >= 0x1100 && ((cp >= 0x1100 && cp <= 0x115F) || (cp == 0x2329 || cp == 0x232A) ||
        (cp >= 0x2E80 && cp <= 0xA4CF && cp != 0x303F) || (cp >= 0xAC00 && cp <= 0xD7A3) || (cp >= 0xF900 && cp <= 0xFAFF) || 
        (cp >= 0xFE10 && cp <= 0xFE19) || (cp >= 0xFE30 && cp <= 0xFE6F) || (cp >= 0xFF00 && cp <= 0xFF60) ||
        (cp >= 0xFFE0 && cp <= 0xFFE6) || (cp >= 0x20000 && cp <= 0x2FFFD) || (cp >= 0x30000 && cp <= 0x3FFFD) ||
        (cp >= 0x1F300))) return 2;
    return 1; }
    
int UTFadd(int row, int col, unsigned char *key) {
    if (row >= GLOBAL_STR || row < 0) return 0;
    if (row < 0 || col < 0 || key[0] < 32) return 0;
    uint16_t *l_vis = GET_LIN_VIS(row);
    uint16_t *l_cnt = GET_LIN_CNT(row);
    uint16_t *l_len = GET_LIN_LEN(row);
    char *data_ptr = GET_BLINE(row);
    if (*l_vis < col) { int missing = col - *l_vis;
    if (missing > GLOBAL_CELL - *l_vis) missing = GLOBAL_CELL - *l_vis;
    if (missing > 0) { MemSet(data_ptr + *l_len, ' ', missing);
        for (int i = 0; i < missing; i++) {
            *GET_TOK_LEN(row, *l_cnt + i) = 1;
            *GET_TOK_VIS(row, *l_cnt + i) = 1; }
        *l_len += missing; *l_vis += missing; *l_cnt += missing; } }
    int len; uint32_t cp = UTFlen(key, &len); int vw = UTFvw(cp);  int is_comb = UTFcom(cp);
    int t_idx = 0, cur_v = 0, b_off = 0;
    while (t_idx < *l_cnt && cur_v < col) {
        cur_v += *GET_TOK_VIS(row, t_idx); b_off += *GET_TOK_LEN(row, t_idx); t_idx++; }
    if (is_comb && t_idx > 0) { int target = t_idx - 1;
        if (*l_len + len < (GLOBAL_CELL * 4)) {
            char *insert_ptr = GET_BLINE(row) + b_off;
            if (b_off < *l_len) memmove(insert_ptr + len, insert_ptr, *l_len - b_off);
            memcpy(insert_ptr, key, len); *(GET_TOK_LEN(row, target)) += len;
            *l_len += len; *GET_ATTR(row, cur_v - *GET_TOK_VIS(row, target)) |= Fresh; }
        return 0; }
    if (vw > 0) {
        if (*l_cnt < GLOBAL_CELL && *l_len + len < (GLOBAL_CELL * 4)) { char *data_ptr = GET_BLINE(row);
            if (b_off < *l_len) memmove(data_ptr + b_off + len, data_ptr + b_off, *l_len - b_off);
            memcpy(data_ptr + b_off, key, len); int tokens_to_move = *l_cnt - t_idx;
            if (tokens_to_move > 0) {
                memmove(GET_TOK_LEN(row, t_idx + 1), GET_TOK_LEN(row, t_idx), tokens_to_move * 2);
                memmove(GET_TOK_VIS(row, t_idx + 1), GET_TOK_VIS(row, t_idx), tokens_to_move * 2); }
            *GET_TOK_LEN(row, t_idx) = len; *GET_TOK_VIS(row, t_idx) = vw;
            *l_len += len; *l_vis += vw; (*l_cnt)++; *GET_ATTR(row, col) |= Fresh; } }
    G_LINE_DIRTY[row] = 1; return vw; }
void UTFdel(int row, int col) {
    if (row < 0 || col < 0) return;
    uint16_t *l_cnt = GET_LIN_CNT(row); uint16_t *l_len = GET_LIN_LEN(row); uint16_t *l_vis = GET_LIN_VIS(row);
    if (*l_cnt == 0) return;
    int t_idx = 0, cur_v = 0, b_off = 0;
    while (t_idx < *l_cnt && cur_v < col) {
        cur_v += *GET_TOK_VIS(row, t_idx); b_off += *GET_TOK_LEN(row, t_idx); t_idx++; }
    if (t_idx >= *l_cnt) return;
    int del_len = *GET_TOK_LEN(row, t_idx); int del_vis = *GET_TOK_VIS(row, t_idx);
    char *data_ptr = GET_BLINE(row); int bytes_to_move = *l_len - (b_off + del_len);
    if (bytes_to_move > 0) memmove(data_ptr + b_off, data_ptr + b_off + del_len, bytes_to_move);
    int tokens_to_move = *l_cnt - (t_idx + 1);
    if (tokens_to_move > 0) {
        memmove(GET_TOK_LEN(row, t_idx), GET_TOK_LEN(row, t_idx + 1), tokens_to_move * 2);
        memmove(GET_TOK_VIS(row, t_idx), GET_TOK_VIS(row, t_idx + 1), tokens_to_move * 2); }
    *l_len -= del_len; *l_vis -= del_vis; (*l_cnt)--;
    *GET_ATTR(row, col) |= Fresh; G_LINE_DIRTY[row] = 1; }

int view_x = 0, view_y = 0;
void ReFresh(int cur_x, int cur_y) {
    int w, h; w = GetCR(&h); int delta = cur_x - view_x;
    if (delta < 0) view_x = cur_x;
    else if (delta >= w) view_x = cur_x - w + 1; 
    delta = cur_y - view_y;
    if (delta < 0) view_y = cur_y;
    else if (delta >= h) view_y = cur_y - h + 1;
    char *buf_start = (char*)G_RAM_BASE + 128; char *p = buf_start; uint8_t last_clr = 0xFF;
    memcpy(p, ESC_HOME, sizeof(ESC_HOME)-1); p += sizeof(ESC_HOME)-1;
    for (int i = 0; i < h; i++) {
        int world_row = view_y + i; int screen_col = 0; uint8_t inv_now = 0;
        if ((unsigned)world_row >= (unsigned)GLOBAL_STR) {
            if (!inv_now) { memcpy(p, ESC_INV, sizeof(ESC_INV)-1); p += sizeof(ESC_INV)-1; inv_now = 1; }
            if (last_clr != 0) { 
                char *c = G_PALETTE + (0<<5); memcpy(p, c+1, *c); p += *c; last_clr = 0; }
            MemSet(p, ' ', w); p += w; } 
        else {
            int left_pad = (view_x < 0) ? -view_x : 0;
            if (left_pad > w) left_pad = w;
            if (left_pad > 0) {
                if (!inv_now) { memcpy(p, ESC_INV, sizeof(ESC_INV)-1); p += sizeof(ESC_INV)-1; inv_now = 1; }
                if (last_clr != 0) { 
                    char *c = G_PALETTE + (0<<5); memcpy(p, c+1, *c); p += *c; last_clr = 0; }
                MemSet(p, ' ', left_pad); p += left_pad; screen_col = left_pad; }
            if (inv_now) { memcpy(p, ESC_NOINV, sizeof(ESC_NOINV)-1); p += sizeof(ESC_NOINV)-1; inv_now = 0; }
            uint16_t l_cnt = *GET_LIN_CNT(world_row); uint16_t l_vis = *GET_LIN_VIS(world_row); int start_x = (view_x < 0) ? 0 : view_x;
            if (l_cnt && start_x < l_vis && screen_col < w) { int cur_v_x = 0, byte_off = 0, t_idx = 0;
                while (t_idx < l_cnt && cur_v_x < start_x) {
                    byte_off += *GET_TOK_LEN(world_row, t_idx); cur_v_x  += *GET_TOK_VIS(world_row, t_idx); t_idx++; }
                char *data_ptr = GET_BLINE(world_row);
                while (t_idx < l_cnt && screen_col < w) { uint8_t clr = *GET_ATTR(world_row, cur_v_x) & Mibc;
                    if (clr != last_clr) {
                        memcpy(p, ESC_CACHE[clr].seq, ESC_CACHE[clr].len); p += ESC_CACHE[clr].len; last_clr = clr; }
                    int bv = 0, bb = 0; int max_width = w - screen_col;
                    while (t_idx < l_cnt && bv < max_width) {
                        if ((*GET_ATTR(world_row, cur_v_x + bv) & Mibc) != clr) break;
                        bb += *GET_TOK_LEN(world_row, t_idx); bv += *GET_TOK_VIS(world_row, t_idx); t_idx++; }
                    memcpy(p, data_ptr + byte_off, bb); p += bb; byte_off += bb; screen_col += bv; cur_v_x += bv; } }
            if (screen_col < w) { int canvas_end = GLOBAL_CELL - view_x; if (canvas_end > w) canvas_end = w;
                if (screen_col < canvas_end) {
                    if (last_clr != 7) {
                        memcpy(p, ESC_CACHE[7].seq, ESC_CACHE[7].len); p += ESC_CACHE[7].len; last_clr = 7; }
                    MemSet(p, ' ', canvas_end - screen_col); p += canvas_end - screen_col; screen_col = canvas_end; }
                if (screen_col < w) {
                    if (!inv_now) { memcpy(p, ESC_INV, sizeof(ESC_INV)-1); p += sizeof(ESC_INV)-1; inv_now = 1; }
                    if (last_clr != 0) {
                        char *c = G_PALETTE + (0<<5); memcpy(p, c+1, *c); p += *c; last_clr = 0; }
                    MemSet(p, ' ', w - screen_col); p += w - screen_col; } } }
        if (inv_now) { memcpy(p, ESC_NOINV, sizeof(ESC_NOINV)-1); p += sizeof(ESC_NOINV)-1; }
        if (i < h - 1) { *p++ = '\r'; *p++ = '\n'; }
        last_clr = 0xFF;
        if ((unsigned)world_row < (unsigned)GLOBAL_STR) G_LINE_DIRTY[world_row] = 0; }
    int sx = cur_x - view_x + 1; int sy = cur_y - view_y + 1;
    if (sx < 1) sx = 1; else if (sx > w) sx = w;
    if (sy < 1) sy = 1; else if (sy > h) sy = h;
    if (cur_y < 0 || cur_x < 0 || cur_y >= GLOBAL_STR || cur_x >= GLOBAL_CELL) {
        memcpy(p, "\033[", 2); p += 2; p = fast_itoa(h, p); *p++ = ';'; p = fast_itoa(w-38, p);
        memcpy(p, "H X:", 4); p += 4; p = fast_itoa(cur_x, p); *p++ = ' '; *p++ = 'Y'; *p++ = ':';
        p = fast_itoa(cur_y, p); memcpy(p, " | RAM:", 7); p += 7;  
        char *ram_str = (char*)G_RAM_BASE; while (*ram_str) *p++ = *ram_str++;
        *p++ = ' '; }
    else 
    { memcpy(p, "\033[", 2); p += 2; p = fast_itoa(sy, p); *p++ = ';'; p = fast_itoa(sx, p); *p++ = 'H'; }
    if (cur_y >= 0 && cur_y < GLOBAL_STR && cur_x >= 0 && cur_x < GLOBAL_CELL) {
        memcpy(p, ESC_CURSOR_ON, sizeof(ESC_CURSOR_ON)-1); p += sizeof(ESC_CURSOR_ON)-1; }
    write(1, buf_start, p - buf_start); }

void LineAdd(int row, int col, char *str) { 
    while (*str) { int shift = UTFadd(row, col, (unsigned char*)str);
        int bytes; UTFlen((unsigned char*)str, &bytes); str += bytes; col += shift; } }

void InitPalette(void) {
    for (int i = 0; i < 8; i++) { char *slot = G_PALETTE + (i << 5);
        uint8_t len = strlen(Crs); *slot = len; memcpy(slot + 1, Crs, len); }
    const char* colors[] = { Cna, Cnn, Cna, Cpr, Cnu, Cap, Cam, Crs };  
    for (int i = 0; i < 7; i++) { char *slot = G_PALETTE + (i << 5);
        uint8_t len = strlen(colors[i]); *slot = len; memcpy(slot + 1, colors[i], len); } 
    memset(G_LINE_DIRTY, 1, GLOBAL_STR); 
    for (int clr = 0; clr < 32; clr++) {
        uint8_t col = clr & 7; uint8_t bold = (clr >> 3) & 1; uint8_t inv = (clr >> 4) & 1; char *p = ESC_CACHE[clr].seq;
        if (bold) { memcpy(p, ESC_BOLD, sizeof(ESC_BOLD)-1); p += sizeof(ESC_BOLD)-1; }
        else      { memcpy(p, ESC_NOBOLD, sizeof(ESC_NOBOLD)-1); p += sizeof(ESC_NOBOLD)-1; }
        if (inv)  { memcpy(p, ESC_INV53, sizeof(ESC_INV53)-1); p += sizeof(ESC_INV53)-1; }
        else      { memcpy(p, ESC_NOINV55, sizeof(ESC_NOINV55)-1); p += sizeof(ESC_NOINV55)-1; }
        char *c = G_PALETTE + (col << 5); memcpy(p, c+1, *c); p += *c; ESC_CACHE[clr].len = p - ESC_CACHE[clr].seq;} }
    
void help() {
    printf(Cnn "Created by " Cna "Alexey Pozdnyakov" Cnn " in " Cna "02.2026" Cnn 
           " version " Cna "1.90" Cnn ", email " Cna "avp70ru@mail.ru" Cnn 
           " github " Cna "https://github.com" Cnn "\n"); }
           
int main(int argc, char *argv[]) {
  if (argc > 1) { if (strcmp(argv[1], "-?") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0) help();
                  return 0; }
  int w, h = 0, cur_x = 0, cur_y = 0; size_t size = GLOBAL_VRAM, ram; if (!(ram = GetRam(&size))) return 0;
  int rp = 0, rc = 0;
  G_RAM_BASE   = ram;
  G_DATA       = (char*)(ram + SYSTEM_SECTOR);
  G_PALETTE    = (char*)G_DATA - 256;
  G_ATTRIBUTE  = (uint8_t*)((char*)G_DATA + SIZE_DATA);
  G_TOK_LEN    = (uint16_t*)((char*)G_ATTRIBUTE + SIZE_ATTR);
  G_TOK_VIS    = (uint16_t*)((char*)G_TOK_LEN + SIZE_TOK_LEN);
  G_LIN_LEN    = (uint16_t*)((char*)G_TOK_VIS + SIZE_TOK_VIS);
  G_LIN_VIS    = (uint16_t*)((char*)G_LIN_LEN + SIZE_LINE_INF);
  G_LIN_CNT    = (uint16_t*)((char*)G_LIN_VIS + SIZE_LINE_INF);
  G_LINE_DIRTY = (uint8_t*)(G_LIN_CNT + GLOBAL_STR);
  SWD(ram); InitPalette(); Delay_ms(0); SetInputMode(1); printf(Crs HCur LWOff);  fflush(stdout);
  snprintf((char*)ram, 128, "%zu", size); LineAdd(0,0,(char*)ram);
  while (1) { int sc = SyncSize(ram);
    if (sc) { rp = 1; rc = 0; }
    else { if (rp) { rc++;
                if (rc >= 3) { write(1, "\033[2J\033[H\033[0m", 11); memset(G_LINE_DIRTY, 1, GLOBAL_STR);
                    w = GetCR(&h); ReFresh(cur_x, cur_y); rp = 0; (void)w; } } }
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
    if (!rp) ReFresh(cur_x, cur_y); }
  SetInputMode(0); printf(ShCur LWOn Crs); fflush(stdout); FreeRam(ram, size); return 0; }
