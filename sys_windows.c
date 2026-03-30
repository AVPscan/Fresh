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
#include <io.h>
#include "sys.h"

SYS_VARS_INIT;

Cell SysWrite(void *buf, Cell len) { return (Cell)_write(1, buf, (unsigned int)len); }

void SwitchRaw(void) {
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE); HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); CONSOLE_CURSOR_INFO ci;
    static DWORD oldModeIn, oldModeOut; static UINT oldCP, oldOutCP;  
    if (Flag.SwitchRaw) {
        oldCP = GetConsoleCP(); oldOutCP = GetConsoleOutputCP();
        GetConsoleMode(hIn, &oldModeIn); GetConsoleMode(hOut, &oldModeOut);
        SetConsoleCP(65001); SetConsoleOutputCP(65001); CONSOLE_CURSOR_INFO cinfo;
        GetConsoleCursorInfo(hOut, &cinfo); cinfo.bVisible = FALSE; SetConsoleCursorInfo(hOut, &cinfo);
        DWORD newModeIn = oldModeIn & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT | ENABLE_QUICK_EDIT_MODE);
        newModeIn |= (ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT | ENABLE_VIRTUAL_TERMINAL_INPUT);
        SetConsoleMode(hIn, newModeIn);
        SetConsoleMode(hOut, oldModeOut | ENABLE_VIRTUAL_TERMINAL_PROCESSING); Flag.SwitchRaw--; }
    else {
        FlushConsoleInputBuffer(hIn); SetConsoleCP(oldCP); SetConsoleOutputCP(oldOutCP); SetConsoleMode(hIn, oldModeIn);
        CONSOLE_CURSOR_INFO cinfo; GetConsoleCursorInfo(hOut, &cinfo); cinfo.bVisible = TRUE;
        SetConsoleCursorInfo(hOut, &cinfo); SetConsoleMode(hOut, oldModeOut); Flag.SwitchRaw++; } }

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

size_t GetRam(size_t *size) { if (!*size) return 0;
    size_t l = (*size + 0xFFF) & ~0xFFF; void *r = VirtualAlloc(NULL, l, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!r) l = 0;
    *size = l; return (size_t)r; }
    
void FreeRam(size_t addr, size_t size) { (void)size; if (addr) VirtualFree((void*)addr, 0, MEM_RELEASE); }

void SWD(size_t addr) { if (!addr) return;
    char *path = (char *)(addr); DWORD len = GetModuleFileNameA(NULL, path, 1024); if (len == 0) return;
    for (char *p = path + len; p > path; p--) if (*p == '\\' || *p == '/') { *p = '\0'; SetCurrentDirectoryA(path); break; } }

uint16_t TermCR(uint16_t *r) { *r = TS.row; return TS.col; }

int16_t SyncSize(size_t addr) {
    if (!addr) return 0;
    static HANDLE hOut = NULL; 
    if (!hOut) hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hOut, &csbi)) return 0;
    uint16_t w = csbi.srWindow.Right - csbi.srWindow.Left;
    uint16_t h = csbi.srWindow.Bottom - csbi.srWindow.Top;
    if (w == TS.col - 1 && h == TS.row - 1) return 0;
    if (Flag.SyncSize) { uint8_t stable = 3; if (w < TS.col - 1 || h < TS.row - 1) Print(Cconvas,Cls);
      while (stable--) { Delay_ms(3);
        if (GetConsoleScreenBufferInfo(hOut, &csbi)) {
          if ((csbi.srWindow.Right - csbi.srWindow.Left) != w || (csbi.srWindow.Bottom - csbi.srWindow.Top) != h) { 
            w = csbi.srWindow.Right - csbi.srWindow.Left; h = csbi.srWindow.Bottom - csbi.srWindow.Top; stable = 3; } } } }
    TS.col = w + 1; TS.row = h + 1; Flag.SyncSize = 1; return 1; }

Cell GetCycles(void) {
    LARGE_INTEGER li; QueryPerformanceCounter(&li); return (Cell)li.QuadPart; }
    
Cell GetSC(Cell addr) {
    if (!addr || !TS.col) return 1;
    char *p = (char *)(addr); MemSet(p, ' ', TS.col - 1); p[TS.col - 1] = '\r';
    LARGE_INTEGER start, end, freq; QueryPerformanceFrequency(&freq); QueryPerformanceCounter(&start);
    for(Cell i = 0; i < 100; i++) SysWrite(p, TS.col);
    QueryPerformanceCounter(&end);
    return (Cell)((end.QuadPart - start.QuadPart) * 1000 / (TS.col * 10)); }
    
void Delay_ms(uint8_t ms) {
    static LARGE_INTEGER freq, start, after_sleep; static uint64_t total_target = 0;
    if (!Flag.Delay_ms) { QueryPerformanceFrequency(&freq); Flag.Delay_ms = 1; }
    if (ms == 0) { SwitchToThread(); return; }
    LARGE_INTEGER now; QueryPerformanceCounter(&now);
    if (ms > 2) { Sleep(ms - 2);
        QueryPerformanceCounter(&after_sleep);
        uint64_t elapsed = after_sleep.QuadPart - now.QuadPart; uint64_t target = (freq.QuadPart * ms) / 1000;
        if (elapsed < target) {
            while (1) {
                QueryPerformanceCounter(&after_sleep); if ((uint64_t)(after_sleep.QuadPart - now.QuadPart) >= target) break;
                __asm__ volatile ("pause"); } } }
    else {
        uint64_t target = now.QuadPart + (freq.QuadPart * ms) / 1000;
        while (1) {
            QueryPerformanceCounter(&now); if ((uint64_t)now.QuadPart >= target) break;
            __asm__ volatile ("pause"); } } }
