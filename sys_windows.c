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
#include <mmsystem.h> //
#include <direct.h>
#include <conio.h>
#include <io.h>

#include "sys.h"

void SwitchRaw(void) {
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE); HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    static DWORD oldModeIn, oldModeOut; static uint8_t flag = 1;   
    if (flag) {
        GetConsoleMode(hIn, &oldModeIn); GetConsoleMode(hOut, &oldModeOut);
        SetConsoleCP(CP_UTF8); SetConsoleOutputCP(CP_UTF8);
        CONSOLE_CURSOR_INFO cinfo = {100, FALSE}; SetConsoleCursorInfo(hOut, &cinfo);
        DWORD newModeIn = ENABLE_VIRTUAL_TERMINAL_INPUT;  // Для стрелок, F1-F12
        newModeIn |= ENABLE_EXTENDED_FLAGS;               // Чтобы отключить QuickEdit
        SetConsoleMode(hIn, newModeIn);                   // ПРИМЕНЯЕМ режимы ввода
        SetConsoleMode(hOut, oldModeOut | ENABLE_VIRTUAL_TERMINAL_PROCESSING); flag = 0; }
    else { SetConsoleMode(hIn, oldModeIn); SetConsoleMode(hOut, oldModeOut); flag = 1; } }

typedef struct { const char *name; unsigned char id; } KeyIdMap;
KeyIdMap NameId[] = { {"[A", K_UP}, {"[B", K_DOW}, {"[C", K_RIG}, {"[D", K_LEF},
    {"[1;5A", K_Ctrl_UP}, {"[1;5B", K_Ctrl_DOW}, {"[1;5C", K_Ctrl_RIG}, {"[1;5D", K_Ctrl_LEF},
    {"[M", K_Mouse}, {"[1;2P", K_F13}, {"[1;2Q", K_F14}, {"[1;2R", K_F15}, {"[15~", K_F5}, {"[17~", K_F6},
    {"[18~", K_F7}, {"[19~", K_F8}, {"[1~", K_HOM}, {"[2~", K_INS}, {"[20~", K_F9}, {"[21~", K_F10},
    {"[23~", K_F11}, {"[24~", K_F12},  {"[3~", K_DEL}, {"[4~", K_END}, {"[5~", K_PUP}, {"[6~", K_PDN},
    {"[F", K_END}, {"[H", K_HOM}, {"OP", K_F1}, {"OQ", K_F2}, {"OR", K_F3}, {"OS", K_F4} };
const char* GetKey(void) {
    static unsigned char b[6]; unsigned char *p = b; uint8_t len = 6; while (len) b[--len] = 0;
    if (!_kbhit()) { *p = 27; return (char*)b; }
    _read(0, p, 1); unsigned char c = *p; if (c > 127) {
        len = (c >= 0xF0) ? 4 : (c >= 0xE0) ? 3 : (c >= 0xC0) ? 2 : 1;
        while (--len) _read(0, ++p, 1);
        return (char*)b; }
    if (c > 31 && c < 127) return (char*)b;
    *p++ = 27; *p = c; if (c != 27) return (char*)b; 
    unsigned char *s1; const unsigned char *s2; int8_t j = (int)(sizeof(NameId)/sizeof(KeyIdMap));
    if (_read(0, p, 1) > 0) { s1 = p; while (((s1 - p) < 5) && (_read(0, ++s1, 1) > 0)) if (*s1 > 63) break;
        if (*s1 < 64) while((_read(0,&c,1) > 0) && (c < 64));
        while(j--) { s2 = (const unsigned char*)NameId[j].name;
            if (*p != *s2) continue;
            s1 = p; while (*++s1 == *++s2 && *s2);
            if (!*s2) { *p++ = NameId[j].id; *p = 0; break; } }
        if (j < 0) b[1] = 0; 
        if (b[1] == K_Mouse) { len = 4; while(--len) _read(0, p++, 1); } }
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
    static LARGE_INTEGER freq; static uint64_t cpu_hz = 0; static int init = 0;
    if (!init) { timeBeginPeriod(1); QueryPerformanceFrequency(&freq); init = 1; }
    if (cpu_hz == 0) { LARGE_INTEGER start, end, qpc_start, qpc_end; QueryPerformanceCounter(&qpc_start);
        start = qpc_start; Sleep(10); QueryPerformanceCounter(&qpc_end);
        uint64_t ns = (uint64_t)((qpc_end.QuadPart - qpc_start.QuadPart) * 1000000000ULL / freq.QuadPart);
        uint64_t cycles = GetCycles() - (uint64_t)start.QuadPart; cpu_hz = (cycles * 1000000000ULL) / ns;
        if (cpu_hz == 0) cpu_hz = 1; }
    uint64_t total_cycles = (uint64_t)ms * (cpu_hz / 1000); uint64_t start_time = GetCycles();
    if (ms > 2) Sleep(ms - 1);
    LARGE_INTEGER check_start, now; QueryPerformanceCounter(&check_start); uint32_t safety = 0;
    while ((GetCycles() - start_time) < total_cycles) {
        __asm__ volatile("pause");
        if (++safety > 2000) {
            QueryPerformanceCounter(&now);
            if (now.QuadPart - check_start.QuadPart > freq.QuadPart) { cpu_hz = 0; break; }
            safety = 0; } } }

int GetSC(size_t addr) { 
    if (!addr || !TS.col) return 1;
    LARGE_INTEGER freq, cs, ce;
    QueryPerformanceFrequency(&freq); char *p = (char *)(addr); MemSet(p, ' ', TS.col - 1); p[TS.col - 1] = '\r';
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); DWORD written; QueryPerformanceCounter(&cs);
    for(int i = 0; i < 100; i++) WriteFile(hOut, p, TS.col, &written, NULL);
    QueryPerformanceCounter(&ce); long long ns = (ce.QuadPart - cs.QuadPart) * 1000000000LL / freq.QuadPart;
    return (int)((ns * 1000) / (TS.col * 100)); }
