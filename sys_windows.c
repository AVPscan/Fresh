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
#include <stdint.h>
#include <io.h>

#include "sys.h"

Cell SysWrite(void *buf, Cell len) {
    DWORD written; HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteFile(h, buf, (DWORD)len, &written, NULL);
    return (Cell)written; }

void SwitchRaw(void) {
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE); HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); CONSOLE_CURSOR_INFO ci;
    static DWORD oldModeIn, oldModeOut; static UINT oldCP, oldOutCP; static uint8_t flag = 1;   
    if (flag) {
        oldCP = GetConsoleCP(); oldOutCP = GetConsoleOutputCP();
        GetConsoleMode(hIn, &oldModeIn); GetConsoleMode(hOut, &oldModeOut);
        SetConsoleCP(65001); SetConsoleOutputCP(65001);
        DWORD newModeIn = oldModeIn & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT | ENABLE_QUICK_EDIT_MODE);
        newModeIn |= (ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT | ENABLE_VIRTUAL_TERMINAL_INPUT);
        SetConsoleMode(hIn, newModeIn);
        SetConsoleMode(hOut, oldModeOut | ENABLE_VIRTUAL_TERMINAL_PROCESSING); flag = 0; }
    else {
        FlushConsoleInputBuffer(hIn); SetConsoleCP(oldCP); SetConsoleOutputCP(oldOutCP); SetConsoleMode(hIn, oldModeIn);
        SetConsoleMode(hOut, oldModeOut); flag = 1; } }

typedef struct { const char *name; unsigned char id; } KeyIdMap;
KeyIdMap NameId[] = { {"[A", K_UP}, {"[B", K_DOW}, {"[C", K_RIG}, {"[D", K_LEF},
    {"[1;5A", K_Ctrl_UP}, {"[1;5B", K_Ctrl_DOW}, {"[1;5C", K_Ctrl_RIG}, {"[1;5D", K_Ctrl_LEF},
    {"[M", K_Mouse}, {"[1;2P", K_F13}, {"[1;2Q", K_F14}, {"[1;2R", K_F15}, {"[15~", K_F5}, {"[17~", K_F6},
    {"[18~", K_F7}, {"[19~", K_F8}, {"[1~", K_HOM}, {"[2~", K_INS}, {"[20~", K_F9}, {"[21~", K_F10},
    {"[23~", K_F11}, {"[24~", K_F12},  {"[3~", K_DEL}, {"[4~", K_END}, {"[5~", K_PUP}, {"[6~", K_PDN},
    {"[F", K_END}, {"[H", K_HOM}, {"OP", K_F1}, {"OQ", K_F2}, {"OR", K_F3}, {"OS", K_F4} };
void GetKey(char *b) {
    unsigned char *p = (unsigned char *)b; uint8_t len = 6; while (len) b[--len] = 0;
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE); DWORD ev = 0; GetNumberOfConsoleInputEvents(hIn, &ev);
    if (ev == 0) { *p = 27; return; }
    _read(0, p, 1); unsigned char c = *p; if (c > 127) {
        len = (c >= 0xF0) ? 4 : (c >= 0xE0) ? 3 : (c >= 0xC0) ? 2 : 1;
        while (--len) _read(0, ++p, 1);
        return; }
    if (c > 31 && c < 127) return;
    *p++ = 27; *p = c; if (c != 27) return; 
    unsigned char *s1; const unsigned char *s2; int8_t j = (int)(sizeof(NameId)/sizeof(KeyIdMap));
    GetNumberOfConsoleInputEvents(hIn, &ev); if (ev == 0) return;
    _read(0, p, 1); s1 = p; while ((s1 - p) < 5) {
        GetNumberOfConsoleInputEvents(hIn, &ev); if (ev == 0) break;
        if (_read(0, ++s1, 1) <= 0) break; 
        if (*s1 > 63) break; }
    if (*s1 < 64) do { GetNumberOfConsoleInputEvents(hIn, &ev); if (ev == 0) break;
                       if (_read(0, &c, 1) <= 0) break; 
                       } while (c < 64);
        while(j--) { s2 = (const unsigned char*)NameId[j].name;
            if (*p != *s2) continue;
            s1 = p; while (*++s1 == *++s2 && *s2);
            if (!*s2) { *p = NameId[j].id; break; } }
        if (j < 0) *p = 0;
        if (*p++ == K_Mouse) { len = 4; while(--len) _read(0, p++, 1); } }

Cell GetRam(Cell *size) { if (!*size) return 0;
    Cell l = (*size + 0xFFF) & ~0xFFF; void *r = VirtualAlloc(NULL, l, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!r) l = 0;
    *size = l; return (Cell)r; }
void FreeRam(Cell addr, Cell size) { if (addr) VirtualFree((void*)addr, 0, MEM_RELEASE); }

void SWD(Cell addr) { if (!addr) return;
    char *path = (char *)(addr); DWORD len = GetModuleFileNameA(NULL, path, 1024); if (len == 0) return;
    for (char *p = path + len; p > path; p--) if (*p == '\\' || *p == '/') { *p = '\0'; SetCurrentDirectoryA(path); break; } }
    
typedef struct { uint16_t col , row; } TermState;
TermState TS = {0};
uint16_t TermCR(uint16_t *r) { *r = TS.row; return TS.col; }
int16_t SyncSize(Cell addr, uint8_t flag) { 
    if (!addr) return 0;
    static HANDLE hOut = NULL; 
    if (!hOut) hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hOut, &csbi)) return 0;
    uint16_t w = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    uint16_t h = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    if (w == TS.col && h == TS.row) return 0;
    if (flag) { 
        uint8_t stable = 70;
        while (stable) { Print(Ccurrent,HideCur);
            Delay_ms(10); stable -= 10;
            if (GetConsoleScreenBufferInfo(hOut, &csbi)) {
                uint16_t cur_w = csbi.srWindow.Right - csbi.srWindow.Left + 1;
                uint16_t cur_h = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
                if (cur_w != w || cur_h != h) { w = cur_w; h = cur_h; stable = 100; } } } }
    TS.col = w; TS.row = h; return 1; }

Cell GetCycles(void) {
    Cell lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    #if __SIZEOF_POINTER__ > 4
      return (lo + (hi << 32));
    #else
      return lo;
    #endif
    }
void Delay_ms(uint8_t ms) {
    static Cell cpu_hz = 0;
    if (cpu_hz == 0) {
        LARGE_INTEGER freq, c1, c2; QueryPerformanceFrequency(&freq); QueryPerformanceCounter(&c1);
        Cell start = GetCycles();
        do { QueryPerformanceCounter(&c2); } 
        while ((c2.QuadPart - c1.QuadPart) < freq.QuadPart / 100);
        cpu_hz = (GetCycles() - start) * 100; if (cpu_hz == 0) cpu_hz = 1; }
    Cell total = (Cell)ms * (cpu_hz / 1000);
    Cell start_time = GetCycles();
    if (ms > 2) { static HANDLE hTimer = NULL;
      if (!hTimer) hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
      LARGE_INTEGER li; li.QuadPart = -((LONGLONG)(ms - 1) * 10000);
      SetWaitableTimer(hTimer, &li, 0, NULL, NULL, FALSE); WaitForSingleObject(hTimer, INFINITE); }
    LARGE_INTEGER f, q1, q2; QueryPerformanceFrequency(&f); QueryPerformanceCounter(&q1); Cell safety = 0;
    while ((GetCycles() - start_time) < total) { __asm__ volatile ("pause");
        if (++safety > 2000) { QueryPerformanceCounter(&q2);
            if ((Cell)(q2.QuadPart - q1.QuadPart) > (Cell)f.QuadPart) { cpu_hz = 0; break; }
            safety = 0; q1 = q2; } } }
Cell GetSC(Cell addr) { 
    if (!addr || !TS.col) return 1;
    char *p = (char *)(addr); MemSet(p, ' ', TS.col - 1); p[TS.col - 1] = '\r';
    Cell start = GetCycles(); for(Cell i = 0; i < 100; i++) SysWrite(p, TS.col);
    Cell end = GetCycles(); return (end - start) / (TS.col * 10); }
