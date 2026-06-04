/*
 * Fresh (C) 2026 A.Pozdnyakov GPLv3 - see LICENSE
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

void GetKey(uint8_t *b) {
  uint8_t *p = b, c, len = 6; while (len--) *(p + len) = 0;
  HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE); DWORD ev = 0; GetNumberOfConsoleInputEvents(hIn, &ev);
  if (ev == 0) { *p = 27; return; }
  _read(0, p, 1); c = *p; if (c > 127) {
    len = (c >= 0xF0) ? 4 : (c >= 0xE0) ? 3 : (c >= 0xC0) ? 2 : 1;
    while (--len) _read(0, ++p, 1);
    return; }
  if (c > 31 && c < 127) return;
  *p++ = 27; *p = c; if (c != 27) return; 
  uint8_t *s1, *s2, j = (uint8_t)(sizeof(NameId)/sizeof(KeyIdMap));
  GetNumberOfConsoleInputEvents(hIn, &ev); if (ev == 0) return;
  _read(0, p, 1); s1 = p; while ((s1 - p) < 5) {
    GetNumberOfConsoleInputEvents(hIn, &ev); if (ev == 0) break;
    if (_read(0, ++s1, 1) <= 0) break; 
    if (*s1 > 63) break; }
  if (*s1 < 64) do { GetNumberOfConsoleInputEvents(hIn, &ev); if (ev == 0) break;
                     if (_read(0, &c, 1) <= 0) break; 
                     } while (c < 64);
  while(j--) { s2 = (uint8_t*)NameId[j].name; if (*p != *s2) continue;
    s1 = p; while (*++s1 == *++s2 && *s2);
    if (!*s2) { *p = NameId[j].id; break; } }
  if (j == (uint8_t)~Off) *p = Off;
  if (*p++ == (uint8_t)K_Mouse) { len = 3; while(len--) _read(0, p++, 1); } }

Cell GetRam(Cell *size) { if (!*size) return 0;
  Cell l = (*size + 0xFFF) & ~0xFFF; void *r = VirtualAlloc(NULL, l, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  if (!r) { l = 0; } *size = l; return (Cell)r; }
    
void FreeRam(Cell addr, Cell size) { (void)size; if (addr) VirtualFree((void*)addr, 0, MEM_RELEASE); }

void SWD(Cell addr) { if (!addr) return;
  char *path = (char *)(addr); DWORD len = GetModuleFileNameA(NULL, path, 1024); if (len == 0) return;
  for (char *p = path + len; p > path; p--) if (*p == '\\' || *p == '/') { *p = '\0'; SetCurrentDirectoryA(path); break; } }

ugoc TermCR(ugoc *r) { *r = TS.row; return TS.col; }
ugoc GetNs(void) { return (ugoc)Flag.ns; }

uint8_t SyncSize(Cell addr) {
  if (!addr) return 0;
  static HANDLE hOut = NULL; if (!hOut) hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO csbi; if (!GetConsoleScreenBufferInfo(hOut, &csbi)) return 0;
  uint16_t w = csbi.srWindow.Right - csbi.srWindow.Left, h = csbi.srWindow.Bottom - csbi.srWindow.Top;
  if (w == TS.col - 1 && h == TS.row - 1) return 0;
  TS.col = w + 1; TS.row = h + 1; return 1; }

Cell GetCycles(void) { LARGE_INTEGER li; QueryPerformanceCounter(&li); return (Cell)li.QuadPart; }

Cell GetSC(Cell addr) {
  if (!addr || !TS.col) return 1;
  char *p = (char *)(addr); MemSet(p, ' ', TS.col - 1); p[TS.col - 1] = '\r';
  LARGE_INTEGER start, end, freq; QueryPerformanceFrequency(&freq); QueryPerformanceCounter(&start);
  for(Cell i = 0; i < 100; i++) SysWrite(p, TS.col);
  QueryPerformanceCounter(&end); return (Cell)((end.QuadPart - start.QuadPart) * 1000 / (TS.col * 10)); }

goc RealFps(ugoc fps) { LARGE_INTEGER n,f;   
    if (fps) { Sleep(1000 / fps); QueryPerformanceCounter(&n); Cell t = (n.QuadPart - Flag.s) * 1000000000L + Flag.ns; Flag.ns = t % 1000000000L;
      Flag.s = n.QuadPart; return (goc)((fps * (t / 1000000000L)) + (Flag.ns ? (1000000000L / Flag.ns) : 0) - fps); }
    QueryPerformanceFrequency(&f); QueryPerformanceCounter(&n); Flag.s = n.QuadPart; Flag.ns = f.QuadPart; return fps; }
