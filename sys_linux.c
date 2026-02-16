/*
 * Copyright (C) 2026 Поздняков Алексей Васильевич
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 */
 
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>   
#include <unistd.h>     
#include <stdint.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "sys.h"

void* os_open_file(const char* name) { return (void*)fopen(name, "rb"); }
void* os_create_file(const char* name) { return (void*)fopen(name, "wb"); }
void  os_close_file(void* handle) { if (handle) fclose((FILE*)handle); }
int   os_read_file(void* handle, unsigned char* buf, int len) { if (!handle) return 0;
    return (int)fread(buf, 1, len, (FILE*)handle); }
int   os_read_file_at(void* handle, long offset, unsigned char* buf, int len) { if (!handle) return 0;
    FILE* f = (FILE*)handle; if (fseek(f, offset, SEEK_SET) != 0) return 0;
    return (int)fread(buf, 1, len, f); }
int   os_print_file(void* handle, const char* format, ...) { if (!handle) return 0;
    va_list args; va_start(args, format); int res = vfprintf((FILE*)handle, format, args); va_end(args); return res; }
int   os_snprintf(char* buf, size_t size, const char* format, ...) {
    va_list args; va_start(args, format); int res = vsnprintf(buf, size, format, args); va_end(args); return res; }
void   os_printf(const char* format, ...) {
    va_list args; va_start(args, format); vprintf(format, args); va_end(args); }

void MemSet(void* buf, int val, size_t len) {
    uint8_t *p = (uint8_t *)buf; uint8_t v = (uint8_t)val;
    while (len && ((Cell)p & (CELL_SIZE - 1))) { *p++ = v; len--; }
    if (len >= CELL_SIZE) {
        Cell vW = v * ((Cell)-1 / 255); Cell *pW = (Cell *)p;
        size_t i = len / CELL_SIZE; len &= (CELL_SIZE - 1); while (i--) *pW++ = vW;
        p = (uint8_t *)pW; }
    while (len--) *p++ = v; }

void MemCpy(void* dst, const void* src, size_t len) {
    uint8_t *d = (uint8_t *)dst; const uint8_t *s = (const uint8_t *)src;
    while (len && ((Cell)d & (CELL_SIZE - 1))) { *d++ = *s++; len--; }
    if (len >= CELL_SIZE && ((Cell)s & (CELL_SIZE - 1)) == 0) {
        Cell *dW = (Cell *)d; const Cell *sW = (const Cell *)s; size_t i = len / CELL_SIZE;
        len &= (CELL_SIZE - 1); while (i--) *dW++ = *sW++;
        d = (uint8_t *)dW; s = (uint8_t *)sW; }
    while (len--) *d++ = *s++ ; }

void MemMove(void* dst, const void* src, size_t len) {
    if (dst > src) { uint8_t *d = (uint8_t *)dst; const uint8_t *s = (const uint8_t *)src;
        d += len; s += len; while (len && ((Cell)d & (CELL_SIZE - 1))) { *--d = *--s; len--; }
        if (len >= CELL_SIZE && ((Cell)s & (CELL_SIZE - 1)) == 0) {
            Cell *dW = (Cell *)d; const Cell *sW = (const Cell *)s; size_t i = len / CELL_SIZE;
            len &= (CELL_SIZE - 1); while (i--) *--dW = *--sW;
            d = (uint8_t *)dW; s = (uint8_t *)sW; } }
    else if (dst < src ) MemCpy(dst, src, len); }
    
int8_t MemCmp(void* dst, const void* src, size_t len) {
    uint8_t *d = (uint8_t *)dst; const uint8_t *s = (const uint8_t *)src;
    while (len && ((Cell)d & (CELL_SIZE - 1))) { if (*d != *s) return (int8_t)(*d - *s);
                                                 d++; s++; len--; }
    if (len >= CELL_SIZE && ((Cell)s & (CELL_SIZE - 1)) == 0) {
        Cell *dW = (Cell *)d; const Cell *sW = (const Cell *)s; size_t i = len / CELL_SIZE;
        len &= (CELL_SIZE - 1); while (i-- && (*dW == *sW)) { dW++; sW++; }
        d = (uint8_t *)dW; s = (uint8_t *)sW;
        if (i != (Cell)-1) len = CELL_SIZE; }
    while (len--) { if (*d != *s) return (int8_t)(*d - *s);
                    d++; s++ ; }
    return 0; }

void SetInputMode(int raw) {
    static struct termios oldt;
    if (raw) {
        tcgetattr(0, &oldt); struct termios newt = oldt; newt.c_lflag &= ~(ICANON | ECHO | ISIG);
        tcsetattr(0, TCSANOW, &newt); fcntl(0, F_SETFL, O_NONBLOCK); } 
    else { tcsetattr(0, TCSANOW, &oldt); fcntl(0, F_SETFL, 0); } }
typedef struct { const char *name; unsigned char id; } KeyIDMap;
KeyIDMap nameid[] = {
    {"[A", K_UP}, {"[B", K_DOW}, {"[C", K_RIG}, {"[D", K_LEF},
    {"[H", K_HOM}, {"[F", K_END}, {"[1~", K_HOM}, {"[4~", K_END},
    {"[2~", K_INS}, {"[3~", K_DEL}, {"[5~", K_PUP}, {"[6~", K_PDN},
    {"OP", K_F1}, {"OQ", K_F2}, {"OR", K_F3}, {"OS", K_F4},
    {"[15~", K_F5}, {"[17~", K_F6}, {"[18~", K_F7}, {"[19~", K_F8},
    {"[20~", K_F9}, {"[21~", K_F10}, {"[23~", K_F11}, {"[24~", K_F12} };
const char* GetKey(void) {
    static unsigned char b[6]; unsigned char *p = b; int len = 6; while (len) b[--len] = 0;
    if (read(0, p, 1) <= 0) { *p = 27; return (char*)b; }
    unsigned char c = *p; if (c > 127) {
        len = (c >= 0xF0) ? 4 : (c >= 0xE0) ? 3 : (c >= 0xC0) ? 2 : 1;
        while (--len) read(0, ++p, 1);
        return (char*)b; }
    if (c > 31 && c < 127) return (char*)b; 
    *p++ = 27; *p = c; if (c != 27) return (char*)b; 
    unsigned char *s1; const unsigned char *s2; if (read(0, p, 1) > 0) {
        s1 = p; len = 3; while (len-- && read(0, ++s1, 1) > 0);
        s1++; while (read(0, s1, 1) > 0) if (*s1 >= 64) break; 
        *s1 = 0; if (*p < 32 || (*p != '[' && *p != 'O')) { *p = 0; return (char*)b; }
        int j = (int)(sizeof(nameid)/sizeof(KeyIDMap));
        while(j--) { s1 = p; s2 = (const unsigned char*)nameid[j].name;
            while (*s1 && *s1 == *s2) { s1++; s2++; }
            if (*s1 == '\0' && *s2 == '\0') { *p++ = nameid[j].id; *p = 0; break; } }
        if (j < 0) b[1] = 0; }
    return (char*)b; }

uint64_t GetCycles(void) {
    union { uint64_t total; struct { uint32_t lo, hi; } part; } t;
    __asm__ __volatile__ ("rdtsc" : "=a" (t.part.lo), "=d" (t.part.hi));
    return t.total; }

void Delay_ms(int ms) {
    static uint64_t cpu_hz = 0;
    if (cpu_hz == 0) { struct timespec ts = {0, 10000000L}; uint64_t start = GetCycles();
        nanosleep(&ts, NULL); cpu_hz = (GetCycles() - start) * 100; if (cpu_hz == 0) cpu_hz = 1; }
    uint64_t total_cycles = (uint64_t)ms * (cpu_hz / 1000); uint64_t start_time = GetCycles();
    if (ms > 2) { struct timespec sleep_ts = {0, (ms - 1) * 1000000L}; nanosleep(&sleep_ts, NULL); }
    struct timespec check_start; clock_gettime(CLOCK_MONOTONIC_COARSE, &check_start); uint32_t safety = 0;
    while ((GetCycles() - start_time) < total_cycles) { __asm__ volatile("pause");
        if (++safety > 2000) { struct timespec now; clock_gettime(CLOCK_MONOTONIC_COARSE, &now);
                               if (now.tv_sec > check_start.tv_sec) { cpu_hz = 0; break; }
                               safety = 0; } } }

typedef struct { int16_t col, row; } TermState;
static TermState TS = {0};
int16_t TermCR(int16_t *r) { *r = TS.row; return TS.col; }
    
int SyncSize(size_t addr, uint8_t flag) { if (!addr) return 0;
    struct winsize ws, cur; if (ioctl(0, TIOCGWINSZ, &ws) < 0) return 0;
    if (ws.ws_col == TS.col && ws.ws_row == TS.row) return 0;
    if (flag) { uint8_t stable = 100;
        while (stable) {
            Delay_ms(10); stable -= 10;
            if (ioctl(0, TIOCGWINSZ, &cur) >= 0) if (cur.ws_col != ws.ws_col || cur.ws_row != ws.ws_row) { ws = cur; stable = 100; } } }
    TS.col = ws.ws_col; TS.row = ws.ws_row; return 1; }
    
int GetSC(size_t addr) { if (!addr || !TS.col) return 1;
    struct timespec cs, ce; char *p = (char *)(addr); MemSet(p, ' ', TS.col - 1); p[TS.col - 1] = '\r';
    clock_gettime(CLOCK_MONOTONIC, &cs);
    for(int i = 0; i < 100; i++) write(1, p, TS.col);
    clock_gettime(CLOCK_MONOTONIC, &ce); 
    long long ns = (ce.tv_sec - cs.tv_sec) * 1000000000LL + (ce.tv_nsec - cs.tv_nsec); return (int)((ns * 1000) / (TS.col * 100)); }

size_t GetRam(size_t *size) {
    size_t l = (*size + 0xFFF) & ~0xFFF; void *r = mmap(0, l, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (r == MAP_FAILED) { r = 0; l = 0; }
    *size = l; return (size_t)r; }

void FreeRam(size_t addr, size_t size) { if (addr) munmap((void*)addr, size); }
    
void SWD(size_t addr) { if (!addr) return;
    char *path = (char *)(addr); ssize_t len = readlink("/proc/self/exe", path, 1024); if (len <= 0) return;
    path[len] = '\0'; if (strncmp(path, "/nix/store", 10) == 0) {
                          const char *home = getenv("HOME"); if (home != NULL) chdir(home);
                          return; }
    for (char *p = path + len; p > path; p--) if (*p == '/') { *p = '\0'; chdir(path); break; } }
    
int8_t UTFinfo(unsigned char *s, uint8_t *len) {
    unsigned char c = s[0]; uint32_t cp = 0xFFFD; *len = 1;
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
    if ((cp >= 0x0300 && cp <= 0x036F) || (cp >= 0x1DC0 && cp <= 0x1DFF) || (cp >= 0x20D0 && cp <= 0x20FF) ||
        (cp == 0x200D || (cp >= 0xFE00 && cp <= 0xFE0F))) return 0; // прилепало
    if (cp == 0 || cp < 32 || (cp >= 0x7F && cp < 0xA0)) return -1; // управляющие
    if (cp < 256) return 1;
    if (cp == 0x200B || cp == 0x200C || cp == 0x200E || cp == 0x200F || (cp >= 0xFE20 && cp <= 0xFE2F) || (cp >= 0xE0100 && cp <= 0xE01EF)) return 0;
    if ((cp >= 0x1100 && cp <= 0x115F) || (cp == 0x2329 || cp == 0x232A) ||
        (cp >= 0x2E80 && cp <= 0xA4CF && cp != 0x303F) || (cp >= 0xAC00 && cp <= 0xD7A3) || (cp >= 0xF900 && cp <= 0xFAFF) ||
        (cp >= 0xFE10 && cp <= 0xFE19) || (cp >= 0xFE30 && cp <= 0xFE6F) || (cp >= 0xFF00 && cp <= 0xFF60) ||
        (cp >= 0xFFE0 && cp <= 0xFFE6) || (cp >= 0x20000 && cp <= 0x2FFFD) || (cp >= 0x30000 && cp <= 0x3FFFD) ||
        (cp >= 0x1F300)) return 2;
    return 1; }

int8_t UTFinfoTile(unsigned char *s, uint8_t *len, size_t rem) {
    *len = 0; if (rem == 0) return -3;
    *len = 1;
    if ((*s & 0xE0) == 0xC0 && rem < 2) return -3;
    else if ((*s & 0xF0) == 0xE0 && rem < 3) return -3; // не влез
    else if ((*s & 0xF8) == 0xF0 && rem < 4) return -3;
    return UTFinfo(s, len); }
