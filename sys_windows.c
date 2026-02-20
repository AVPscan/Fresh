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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <direct.h> // Для _chdir

#include "sys.h"

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
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    static DWORD oldModeIn, oldModeOut; static uint8_t flag = 1;
    if (flag) {
        setvbuf(stdout, NULL, _IONBF, 0); 
        SetConsoleCP(65001);
        SetConsoleOutputCP(65001);
        GetConsoleMode(hIn, &oldModeIn);
        GetConsoleMode(hOut, &oldModeOut);
        DWORD newModeIn = oldModeIn; 
        newModeIn &= ~(ENABLE_QUICK_EDIT_MODE | ENABLE_WINDOW_INPUT); 
        // 2. ЯВНО ВКЛЮЧАЕМ ввод мыши и расширенные флаги (чтобы QuickEdit реально сдох)
        newModeIn |= (ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
        SetConsoleMode(hIn, newModeIn);
        SetConsoleMode(hOut, oldModeOut | ENABLE_VIRTUAL_TERMINAL_PROCESSING); flag = 0; } 
    else {
        printf( Reset ); fflush(stdout);
        FlushConsoleInputBuffer(hIn);
        SetConsoleMode(hIn, oldModeIn);
        SetConsoleMode(hOut, oldModeOut);
        setvbuf(stdout, NULL, _IOLBF, BUFSIZ); flag = 1; } }
const char* GetKey(void) {
    static unsigned char b[6]; 
    MemSet(b, 0, 6);
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD events = 0;
    INPUT_RECORD ir;
    DWORD read_count;
    if (!PeekConsoleInputW(hIn, &ir, 1, &events) || events == 0) return (char*)b;
    if (!ReadConsoleInputW(hIn, &ir, 1, &read_count) || read_count == 0) return (char*)b;
    if (ir.EventType == MOUSE_EVENT) {
      DWORD flags = ir.Event.MouseEvent.dwEventFlags;
      DWORD btn = ir.Event.MouseEvent.dwButtonState; 
      if (flags == 0 || flags == DOUBLE_CLICK || flags == MOUSE_WHEELED) {
        b[0] = 27;
        b[1] = K_Mouse;
        b[3] = 32 + (unsigned char)ir.Event.MouseEvent.dwMousePosition.X + 1;
        b[4] = 32 + (unsigned char)ir.Event.MouseEvent.dwMousePosition.Y + 1;
        if (flags == MOUSE_WHEELED) { b[2] = ( (int32_t)btn > 0 ) ? 96 : 97; } 
        else { if (btn & FROM_LEFT_1ST_BUTTON_PRESSED) b[2] = 32;
               else if (btn & RIGHTMOST_BUTTON_PRESSED) b[2] = 34;
               else if (btn & FROM_LEFT_2ND_BUTTON_PRESSED) b[2] = 33; }
        return (char*)b; }
    return (char*)b; }
    if (ir.EventType != KEY_EVENT || !ir.Event.KeyEvent.bKeyDown) return (char*)b;
    WORD vk = ir.Event.KeyEvent.wVirtualKeyCode;
    WCHAR wc = ir.Event.KeyEvent.uChar.UnicodeChar;
    DWORD controlKeyState = ir.Event.KeyEvent.dwControlKeyState;
    if (wc == 3 || (vk == 'C' && (controlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)))) {
        b[0] = 27; b[1] = K_Ctrl_C; return (char*)b; }
    int cmd = 0;
    if (vk >= VK_F1 && vk <= VK_F12) cmd = K_F1 + (vk - VK_F1);
    else {
        switch (vk) {
            case VK_UP:     cmd = (controlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) ? K_Ctrl_UP : K_UP; break;
            case VK_DOWN:   cmd = (controlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) ? K_Ctrl_DOW : K_DOW; break;
            case VK_LEFT:   cmd = (controlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) ? K_Ctrl_LEF : K_LEF; break;
            case VK_RIGHT:  cmd = (controlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) ? K_Ctrl_RIG : K_RIG; break;
            case VK_ESCAPE: cmd = K_ESC; break;
            case VK_PRIOR:  cmd = K_PUP; break;
            case VK_NEXT:   cmd = K_PDN; break;
            case VK_HOME:   cmd = K_HOM; break;
            case VK_END:    cmd = K_END; break;
            case VK_INSERT: cmd = K_INS; break;
            case VK_DELETE: cmd = K_DEL; break;
            case VK_RETURN: cmd = K_ENT; break;
            case VK_BACK:   cmd = K_BAC; break;
            case VK_TAB:    cmd = K_TAB; break; } }
    if (cmd > 0) { b[0] = 27; b[1] = (unsigned char)cmd; return (char*)b; }
    if (wc >= 32) { int len = WideCharToMultiByte(CP_UTF8, 0, &wc, 1, (char*)b, 5, NULL, NULL); return (char*)b; }
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
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
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
void Delay_ms(uint8_t ms) { if (ms > 0) Sleep((DWORD)ms); }

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
