/*
 * Copyright (C) 2026 Поздняков Алексей Васильевич
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 */
 
#define _POSIX_C_SOURCE 200809L
#include <time.h>      // nanosleep, clock_gettime
#include <termios.h>   // tcgetattr, tcsetattr
#include <fcntl.h>     // fcntl, O_NONBLOCK
#include <unistd.h>    // read, write, chdir, readlink
#include <stdint.h>    // uint8_t, uint64_t
#include <sys/mman.h>  // mmap, munmap
#include <sys/ioctl.h> // ioctl, TIOCGWINSZ
#include "sys.h"

void SwitchRaw(void) {
    static struct termios oldt; static uint8_t flag = 1;
    if (flag) {
        tcgetattr(0, &oldt); struct termios newt = oldt; newt.c_lflag &= ~(ICANON | ECHO | ISIG);
        tcsetattr(0, TCSANOW, &newt); fcntl(0, F_SETFL, O_NONBLOCK); flag = 0; } 
    else { tcsetattr(0, TCSANOW, &oldt); fcntl(0, F_SETFL, 0); flag = 1; } }
typedef struct { const char *name; unsigned char id; } KeyIdMap;
KeyIdMap NameId[] = { {"[A", K_UP}, {"[B", K_DOW}, {"[C", K_RIG}, {"[D", K_LEF},
    {"[1;5A", K_Ctrl_UP}, {"[1;5B", K_Ctrl_DOW}, {"[1;5C", K_Ctrl_RIG}, {"[1;5D", K_Ctrl_LEF},
    {"[M", K_Mouse}, {"[1;2P", K_F13}, {"[1;2Q", K_F14}, {"[1;2R", K_F15}, {"[15~", K_F5}, {"[17~", K_F6},
    {"[18~", K_F7}, {"[19~", K_F8}, {"[1~", K_HOM}, {"[2~", K_INS}, {"[20~", K_F9}, {"[21~", K_F10},
    {"[23~", K_F11}, {"[24~", K_F12},  {"[3~", K_DEL}, {"[4~", K_END}, {"[5~", K_PUP}, {"[6~", K_PDN},
    {"[F", K_END}, {"[H", K_HOM}, {"OP", K_F1}, {"OQ", K_F2}, {"OR", K_F3}, {"OS", K_F4} };
void GetKey(char *b) {
    unsigned char *p = (unsigned char *)b; uint8_t len = 6; while (len) b[--len] = 0;
    if (read(0, p, 1) <= 0) { *p = 27; return; }
    unsigned char c = *p; if (c > 127) {
        len = (c >= 0xF0) ? 4 : (c >= 0xE0) ? 3 : (c >= 0xC0) ? 2 : 1;
        while (--len) read(0, ++p, 1);
        return; }
    if (c > 31 && c < 127) return;
    *p++ = 27; *p = c; if (c != 27) return; 
    unsigned char *s1; const unsigned char *s2; int8_t j = (int)(sizeof(NameId)/sizeof(KeyIdMap));
    if (read(0, p, 1) > 0) { s1 = p; while (((s1 - p) < 5) && (read(0, ++s1, 1) > 0)) if (*s1 > 63) break;
        if (*s1 < 64) while((read(0,&c,1) > 0) && (c < 64));
        while(j--) { s2 = (const unsigned char*)NameId[j].name;
            if (*p != *s2) continue;
            s1 = p; while (*++s1 == *++s2 && *s2);
            if (!*s2) { *p++ = NameId[j].id; *p = 0; break; } }
        if (j < 0) b[1] = 0; 
        if (b[1] == K_Mouse) { len = 4; while(--len) read(0, p++, 1); } } }

size_t GetRam(size_t *size) { if (!*size) return 0;
    size_t l = (*size + 0xFFF) & ~0xFFF; void *r = mmap(0, l, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (r == MAP_FAILED) { r = 0; l = 0; }
    *size = l; return (size_t)r; }
void FreeRam(size_t addr, size_t size) { if (addr) munmap((void*)addr, size); }

extern char **environ;
void SWD(size_t addr) { if (!addr) return;
    char *path = (char *)(addr); ssize_t len = readlink("/proc/self/exe", path, 1024); if (len <= 0) return;
    path[len] = '\0';
    if (MemCmp(path, "/nix/store", 10) == 0) {
        for (char **env = environ; *env != NULL; env++) { char *e = *env;
            if (e[0] == 'H' && e[1] == 'O' && e[2] == 'M' && e[3] == 'E' && e[4] == '=') { chdir(e + 5); return; } }
        return; }
    for (char *p = path + len; p > path; p--) if (*p == '/') { *p = '\0'; chdir(path); break; } }

typedef struct { uint16_t col , row; } TermState;
TermState TS = {0};
uint16_t TermCR(uint16_t *r) { *r = TS.row; return TS.col; }
int16_t SyncSize(size_t addr, uint8_t flag) { if (!addr) return 0;
    struct winsize ws, cur; if (ioctl(0, TIOCGWINSZ, &ws) < 0) return 0;
    if (ws.ws_col == TS.col && ws.ws_row == TS.row) return 0;
    if (flag) { uint8_t stable = 100;
        while (stable) {
            Delay_ms(10); stable -= 10;
            if (ioctl(0, TIOCGWINSZ, &cur) >= 0) if (cur.ws_col != ws.ws_col || cur.ws_row != ws.ws_row) { ws = cur; stable = 100; } } }
    TS.col = ws.ws_col; TS.row = ws.ws_row; return 1; }
  
uint64_t GetCycles(void) {
    union { uint64_t total; struct { uint32_t lo, hi; } part; } t;
    __asm__ __volatile__ ("rdtsc" : "=a" (t.part.lo), "=d" (t.part.hi));
    return t.total; }
void Delay_ms(uint8_t ms) {
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
int GetSC(size_t addr) { if (!addr || !TS.col) return 1;
    struct timespec cs, ce; char *p = (char *)(addr); MemSet(p, ' ', TS.col - 1); p[TS.col - 1] = '\r';
    clock_gettime(CLOCK_MONOTONIC, &cs);
    for(int i = 0; i < 100; i++) write(1, p, TS.col);
    clock_gettime(CLOCK_MONOTONIC, &ce); 
    long long ns = (ce.tv_sec - cs.tv_sec) * 1000000000LL + (ce.tv_nsec - cs.tv_nsec); return (int)((ns * 1000) / (TS.col * 100)); }
