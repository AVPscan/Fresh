/*
 * Copyright (C) 2026 Поздняков Алексей Васильевич
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 */
 
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>      // Для fopen, fread, vfprintf, FILE
#include <stdlib.h>     // Для malloc, realloc, free
#include <string.h>     // Для strlen, memset, memcpy
#include <stdarg.h>     // Для va_list, va_start
#include <time.h>       // Для nanosleep
#include <sys/time.h>   // Для gettimeofday
#include <termios.h>    // Для tcgetattr
#include <fcntl.h>      // Для fcntl
#include <unistd.h>     // Для read, close
#include <stdint.h>
#include <sys/mman.h>
#include <stddef.h>
#include <sys/ioctl.h>
#include "sys.h"

#define RING_BUF_SLOTS 16
#define RING_BUF_SLOT_SIZE 128

void* os_open_file(const char* name) { return (void*)fopen(name, "rb"); }
void* os_create_file(const char* name) { return (void*)fopen(name, "wb"); }
void  os_close_file(void* handle) { if (handle) fclose((FILE*)handle); }
int   os_read_file(void* handle, unsigned char* buf, int len) { if (!handle) return 0;
    return (int)fread(buf, 1, len, (FILE*)handle); }
int   os_read_file_at(void* handle, long offset, unsigned char* buf, int len) { if (!handle) return 0;
    FILE* f = (FILE*)handle; if (fseek(f, offset, SEEK_SET) != 0) return 0;
    return (int)fread(buf, 1, len, f); }
void* os_malloc(size_t size) { return malloc(size); }
void* os_realloc(void* ptr, size_t size) { return realloc(ptr, size); }
void  os_free(void* ptr) { free(ptr); }
void  os_memset(void* ptr, int val, size_t size) { memset(ptr, val, size); }
char* os_strdup(const char* s) { if (!s) return NULL;
    size_t len = strlen(s) + 1; char* d = (char*)os_malloc(len); if (d) memcpy(d, s, len);
    return d; }
int   os_print_file(void* handle, const char* format, ...) { if (!handle) return 0;
    va_list args; va_start(args, format); int res = vfprintf((FILE*)handle, format, args); va_end(args); return res; }
int   os_snprintf(char* buf, size_t size, const char* format, ...) {
    va_list args; va_start(args, format); int res = vsnprintf(buf, size, format, args); va_end(args); return res; }
void   os_printf(const char* format, ...) {
    va_list args; va_start(args, format); vprintf(format, args); va_end(args); }

void SetInputMode(int raw) {
    static struct termios oldt;
    if (raw) {
        tcgetattr(0, &oldt);
        struct termios newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO | ISIG);
        tcsetattr(0, TCSANOW, &newt);
        fcntl(0, F_SETFL, O_NONBLOCK); } 
    else { tcsetattr(0, TCSANOW, &oldt);
        fcntl(0, F_SETFL, 0); } } 
typedef struct { const char *name; unsigned char id; } KeyIDMap;
KeyIDMap nameid[] = {
    {"[A", K_UP}, {"[B", K_DOW}, {"[C", K_RIG}, {"[D", K_LEF},
    {"[H", K_HOM}, {"[F", K_END}, {"[1~", K_HOM}, {"[4~", K_END},
    {"[2~", K_INS}, {"[3~", K_DEL}, {"[5~", K_PUP}, {"[6~", K_PDN},
    {"OP", K_F1}, {"OQ", K_F2}, {"OR", K_F3}, {"OS", K_F4},
    {"[15~", K_F5}, {"[17~", K_F6}, {"[18~", K_F7}, {"[19~", K_F8},
    {"[20~", K_F9}, {"[21~", K_F10}, {"[23~", K_F11}, {"[24~", K_F12} };
const char* GetKey(void) {
    static unsigned char b[6]; unsigned char *p = b;
    os_memset(b, 0, sizeof(b));
    if (read(0, p, 1) <= 0) { *p =27; return (char*)b; }
    unsigned char c = *p; int len;
    if (c > 127) {
        len = (c >= 0xF0) ? 4 : (c >= 0xE0) ? 3 : (c >= 0xC0) ? 2 : 1;
        while (--len > 0) read(0, ++p, 1);
        return (char*)b; }
    if (c > 32 && c < 127) return (char*)b; 
    *p++ = 27; *p = c;
    if (c != 27) return (char*)b; 
    const unsigned char *s1,*s2;
    if (read(0, p, 1) > 0) { 
        if (*p < 32 || (*p != '[' && *p != 'O')) { *p = 0; return (char*)b; }
        s1 = p; len = 4; while (--len > 0 && read(0, (unsigned char*)++s1, 1) > 0);
        for (int j = 0; j < (int)(sizeof(nameid)/sizeof(KeyIDMap)); j++) { s1 = p; s2 = (const unsigned char*)nameid[j].name;
            while (*s1 && *s1 == *s2) { s1++; s2++; }
            if (*s1 == '\0' && *s2 == '\0') { *p++ = nameid[j].id; *p = 0; break; } }
        if (*p != 0) b[1] = 0; }
    return (char*)b; }

typedef struct {
    int w, h;       // Ширина и высота в символах (cols, rows)
    int ratio;      // Целочисленный коэффициент пропорций (H * 100 / W)
    int pw, ph;     // Виртуальные пиксели (для Брайля: 2x4)
} TermState;
static TermState TS = {0};
int os_sync_size(void) {
    struct winsize ws;
    if (ioctl(0, TIOCGWINSZ, &ws) < 0) return 0;
    if (ws.ws_col != TS.w || ws.ws_row != TS.h) {
        TS.w = ws.ws_col; TS.h = ws.ws_row;
        TS.ratio = (TS.w > 0) ? (TS.h * 100) / TS.w : 0;
        TS.pw = TS.w * 2; TS.ph = TS.h * 4; return 1; }
    return 0; }
int GetWH(int *h) { *h = TS.h; return TS.w; }

static size_t GlobalBuf = 0, GlobalLen = 0;
size_t GetBuff(size_t *size) {
    GlobalLen = (*size + 0xFFF) & ~0xFFF;             // size < (GlobalLen=X*4096)
    void *ptr = mmap(0, GlobalLen, 3, 34, -1, 0);     // Прямой системный вызов: 3 = READ|WRITE, 34 = PRIVATE|ANONYMOUS
    if (ptr == (void*)-1) { GlobalBuf = 0; GlobalLen = 0; return 0; }
    GlobalBuf = (size_t)ptr; *size = GlobalLen; return GlobalBuf; }
void FreeBuff(void) {
    if (GlobalBuf) { munmap((void*)GlobalBuf, GlobalLen); GlobalBuf = 0; GlobalLen = 0; } }

int GetC(void) { if (!GlobalBuf || !TS.w) return 1;
    struct timespec cs, ce; char *p = (char *)GlobalBuf; memset(p, ' ', TS.w - 1); p[TS.w - 1] = '\r';
    clock_gettime(CLOCK_MONOTONIC, &cs);
    for(int i = 0; i < 100; i++) write(1, p, TS.w);
    clock_gettime(CLOCK_MONOTONIC, &ce); 
    long long ns = (ce.tv_sec - cs.tv_sec) * 1000000000LL + (ce.tv_nsec - cs.tv_nsec); return (int)((ns * 1000) / (TS.w * 100)); }
    
void SWD(void) { if (!GlobalBuf) return;
    char *path = (char *)GlobalBuf;
    ssize_t len = readlink("/proc/self/exe", path, 1024);
    if (len <= 0) return;
    path[len] = '\0'; if (strncmp(path, "/nix/store", 10) == 0) {
                          const char *home = getenv("HOME"); if (home != NULL) chdir(home);
                          return; }
    for (char *p = path + len; p > path; p--) if (*p == '/') { *p = '\0'; chdir(path); break; } }
    
char *GetBuf(void) {
    static int idx = 0; idx = (idx + 1) & (RING_BUF_SLOTS - 1); 
    return (char*)GlobalBuf + (idx * RING_BUF_SLOT_SIZE); }
    
const char *Button(const char *label, int active) {
    char *b = GetBuf();
    if (active) snprintf(b, 128, "\033[7;53m%s\033[27;55m", label);
    else snprintf(b, 128, "\033[1m%s\033[22m", label);
    return b; }

uint64_t get_cycles(void) {
    union { uint64_t total; struct { uint32_t lo, hi; } part; } t;
    __asm__ __volatile__ ("rdtsc" : "=a" (t.part.lo), "=d" (t.part.hi));
    return t.total; }

void delay_ms(int ms) {
    static uint64_t cpu_hz = 0;
    if (cpu_hz == 0) { struct timespec ts = {0, 10000000L}; uint64_t start = get_cycles();
        nanosleep(&ts, NULL); cpu_hz = (get_cycles() - start) * 100; if (cpu_hz == 0) cpu_hz = 1; }
    uint64_t total_cycles = (uint64_t)ms * (cpu_hz / 1000); uint64_t start_time = get_cycles();
    if (ms > 2) { struct timespec sleep_ts = {0, (ms - 1) * 1000000L}; nanosleep(&sleep_ts, NULL); }
    struct timespec check_start; clock_gettime(CLOCK_MONOTONIC_COARSE, &check_start); uint32_t safety = 0;
    while ((get_cycles() - start_time) < total_cycles) { __asm__ volatile("pause");
        if (++safety > 2000) { struct timespec now; clock_gettime(CLOCK_MONOTONIC_COARSE, &now);
                               if (now.tv_sec > check_start.tv_sec) { cpu_hz = 0; break; }
                               safety = 0; } } }
