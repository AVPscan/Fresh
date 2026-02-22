/*
 * Copyright (C) 2026 Поздняков Алексей Васильевич
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 */
 
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <direct.h>
#include <io.h>

#include <stdio.h>    / vfprintf, vsnprintf и setvbuf
#include <stdarg.h>   / va_list

#include <stdlib.h> / malloc/free))
#include <string.h>   / strlen)

#include "sys.h"

#define read win_read
ssize_t win_read(int fd, void* buf, size_t count) {
    if (fd != 0) return _read(fd, buf, (unsigned int)count);
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE); DWORD events = 0;
    if (!GetNumberOfConsoleInputEvents(hIn, &events) || events == 0) return -1;
    return _read(fd, buf, (unsigned int)count); }
    
void* os_open_file(const char* name) { return (void*)fopen(name, "rb"); }
void* os_create_file(const char* name) { return (void*)fopen(name, "wb"); }
void  os_close_file(void* handle) { if (handle) fclose((FILE*)handle); }
int os_read_file(void* handle, unsigned char* buf, int len) { if (!handle) return 0;
    return (int)fread(buf, 1, len, (FILE*)handle); }
int os_read_file_at(void* handle, long offset, unsigned char* buf, int len) { if (!handle) return 0;
    FILE* f = (FILE*)handle; if (fseek(f, offset, SEEK_SET) != 0) return 0;
    return (int)fread(buf, 1, len, f); }
int os_print_file(void* handle, const char* format, ...) { if (!handle) return 0;
    va_list args; va_start(args, format); int res = vfprintf((FILE*)handle, format, args);
    va_end(args); return res; }
int os_snprintf(char* buf, size_t size, const char* format, ...) {
    va_list args; va_start(args, format); int res = vsnprintf(buf, size, format, args);
    va_end(args); return res; }
void os_printf(const char* format, ...) {
    va_list args; va_start(args, format); vprintf(format, args);
    va_end(args); }

void SwitchRaw(void) {
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE); HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    static DWORD oldModeIn, oldModeOut; static uint8_t flag = 1;
    if (flag) {
        setvbuf(stdout, NULL, _IONBF, 0); SetConsoleCP(CP_UTF8); SetConsoleOutputCP(CP_UTF8); GetConsoleMode(hIn, &oldModeIn);
        GetConsoleMode(hOut, &oldModeOut); DWORD newModeIn = oldModeIn;
        newModeIn &= ~(ENABLE_QUICK_EDIT_MODE | ENABLE_WINDOW_INPUT | ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
        newModeIn |= ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS | ENABLE_VIRTUAL_TERMINAL_INPUT;
        SetConsoleMode(hIn, newModeIn); SetConsoleMode(hOut, oldModeOut | ENABLE_VIRTUAL_TERMINAL_PROCESSING); flag = 0; }
    else {
        FlushConsoleInputBuffer(hIn); SetConsoleMode(hIn, oldModeIn); SetConsoleMode(hOut, oldModeOut);
        setvbuf(stdout, NULL, _IOLBF, BUFSIZ); flag = 1; } }

typedef struct { const char *name; unsigned char id; } KeyIdMap;
KeyIdMap NameId[] = { {"[A", K_UP}, {"[B", K_DOW}, {"[C", K_RIG}, {"[D", K_LEF},
    {"[1;5A", K_Ctrl_UP}, {"[1;5B", K_Ctrl_DOW}, {"[1;5C", K_Ctrl_RIG}, {"[1;5D", K_Ctrl_LEF},
    {"[M", K_Mouse}, {"[1;2P", K_F13}, {"[1;2Q", K_F14}, {"[1;2R", K_F15}, {"[15~", K_F5}, {"[17~", K_F6},
    {"[18~", K_F7}, {"[19~", K_F8}, {"[1~", K_HOM}, {"[2~", K_INS}, {"[20~", K_F9}, {"[21~", K_F10},
    {"[23~", K_F11}, {"[24~", K_F12},  {"[3~", K_DEL}, {"[4~", K_END}, {"[5~", K_PUP}, {"[6~", K_PDN},
    {"[F", K_END}, {"[H", K_HOM}, {"OP", K_F1}, {"OQ", K_F2}, {"OR", K_F3}, {"OS", K_F4} };
const char* GetKey(void) {
    static unsigned char b[6]; unsigned char *p = b; uint8_t len = 6; while (len) b[--len] = 0;
    if (read(0, p, 1) <= 0) { *p = 27; return (char*)b; }
    unsigned char c = *p; if (c > 127) {
        len = (c >= 0xF0) ? 4 : (c >= 0xE0) ? 3 : (c >= 0xC0) ? 2 : 1;
        while (--len) read(0, ++p, 1);
        return (char*)b; }
    if (c > 31 && c < 127) return (char*)b;
    *p++ = 27; *p = c; if (c != 27) return (char*)b; 
    unsigned char *s1; const unsigned char *s2; int8_t j = (int)(sizeof(NameId)/sizeof(KeyIdMap));
    if (read(0, p, 1) > 0) { s1 = p; while (((s1 - p) < 5) && (read(0, ++s1, 1) > 0)) if (*s1 > 63) break;
        if (*s1 < 64) while((read(0,&c,1) > 0) && (c < 64));
        while(j--) { s2 = (const unsigned char*)NameId[j].name;
            if (*p != *s2) continue;
            s1 = p; while (*++s1 == *++s2 && *s2);
            if (!*s2) { *p++ = NameId[j].id; *p = 0; break; } }
        if (j < 0) b[1] = 0; 
        if (b[1] == K_Mouse) { len = 4; while(--len) read(0, p++, 1); } }
    return (char*)b; }

size_t GetRam(size_t *size) { if (!*size) return 0;
    size_t l = (*size + 0xFFF) & ~0xFFF; void *r = VirtualAlloc(NULL, l, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!r) l = 0;
    *size = l; return (size_t)r; }
void FreeRam(size_t addr, size_t size) { if (addr) VirtualFree((void*)addr, 0, MEM_RELEASE); }

void SWD(size_t addr) { if (!addr) return;
    char *path = (char *)(addr); DWORD len = GetModuleFileNameA(NULL, path, 1024); if (len == 0) return;
    for (char *p = path + len; p > path; p--) if (*p == '\\' || *p == '/') { *p = '\0'; _chdir(path); break; } }

typedef struct { uint16_t col , row; } TermState;
TermState TS = {0};
uint16_t TermCR(uint16_t *r) { *r = TS.row; return TS.col; }
int16_t SyncSize(size_t addr, uint8_t flag) { 
    if (!addr) return 0;
    static HANDLE hOut = NULL; 
    if (!hOut) hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hOut, &csbi)) return 0;
    uint16_t w = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    uint16_t h = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    if (w == TS.col && h == TS.row) return 0;
    if (flag) { 
        uint8_t stable = 100;
        while (stable) {
            Delay_ms(10); stable -= 10;
            if (GetConsoleScreenBufferInfo(hOut, &csbi)) {
                uint16_t cur_w = csbi.srWindow.Right - csbi.srWindow.Left + 1;
                uint16_t cur_h = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
                if (cur_w != w || cur_h != h) { w = cur_w; h = cur_h; stable = 100; } } } }
    TS.col = w; TS.row = h; return 1; }

uint64_t GetCycles(void) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo; }
void Delay_ms(uint8_t ms) {
    static LARGE_INTEGER freq; static int init = 0;
    if (!init) { QueryPerformanceFrequency(&freq); init = 1; }
    LARGE_INTEGER start, end; QueryPerformanceCounter(&start);
    int64_t target = start.QuadPart + (ms * freq.QuadPart / 1000); if (ms > 1) Sleep(ms - 1);
    do { __asm__ volatile("pause"); QueryPerformanceCounter(&end); } while (end.QuadPart < target); }

int GetSC(size_t addr) { 
    if (!addr || !TS.col) return 1;
    LARGE_INTEGER freq, cs, ce;
    QueryPerformanceFrequency(&freq);
    char *p = (char *)(addr); 
    MemSet(p, ' ', TS.col - 1); 
    p[TS.col - 1] = '\r';
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written;
    QueryPerformanceCounter(&cs);
    for(int i = 0; i < 100; i++) WriteFile(hOut, p, TS.col, &written, NULL);
    QueryPerformanceCounter(&ce); 
    long long ns = (ce.QuadPart - cs.QuadPart) * 1000000000LL / freq.QuadPart;
    return (int)((ns * 1000) / (TS.col * 100)); }
