/*
 * Fresh (C) 2026 A.Pozdnyakov GPLv3 - see LICENSE
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
#include <sys/mman.h>  // mmap, munmap
#include <sys/ioctl.h> // ioctl, TIOCGWINSZ
#include "sys.h"

SYS_VARS_INIT;
extern char **environ;
void IRnd(void) { Flag.Rn = (ugoc)Flag.Delay_ms | On; }
void SRnd(ugoc n) { Flag.Rn = n; }
ugoc Rand(ugoc n) { return (ugoc)(((Cell)(Flag.Rn = (ugoc)(RNG_A * Flag.Rn + RNG_B)) * n) >> (sizeof(ugoc) * 8)); }

Cell SysWrite(void *buf, Cell len) { return (Cell)write(1, buf, len); }

void SwitchRaw(void) {
  static struct termios oldt;
  if (Flag.SwitchRaw) {
    tcgetattr(0, &oldt); struct termios newt = oldt; newt.c_lflag &= ~(ICANON | ECHO | ISIG);
    tcsetattr(0, TCSANOW, &newt); fcntl(0, F_SETFL, O_NONBLOCK); Flag.SwitchRaw--; } 
  else { tcsetattr(0, TCSANOW, &oldt); fcntl(0, F_SETFL, 0); Flag.SwitchRaw++; } }
  
void GetKey(uint8_t *b) {
  uint8_t *p = b, c, len = 6; while (len--) b[len] = 0;
  if (read(0, p, 1) <= 0) { *p = K_ESC; return; }
  c = *p; if (c > 127) {
    len = (c >= 0xF0) ? 4 : (c >= 0xE0) ? 3 : (c >= 0xC0) ? 2 : 1;
    while (--len) read(0, ++p, 1);
    return; }
  if (c > 31 && c < 127) return;
  *p++ = K_ESC; *p = c; if (c != K_ESC) return; 
  uint8_t *s1, *s2, j = (uint8_t)(sizeof(NameId)/sizeof(KeyIdMap));
  if (read(0, p, 1) > 0) { s1 = p; while (((s1 - p) < 5) && (read(0, ++s1, 1) > 0)) if (*s1 > 63) break;
    if (*s1 < 64) while((read(0,&c,1) > 0) && (c < 64));
    while(j--) { s2 = (uint8_t*)NameId[j].name;
      if (*p != *s2) continue;
      s1 = p; while (*++s1 == *++s2 && *s2);
      if (!*s2) { *p = NameId[j].id; break; } }
    if (j == (uint8_t)~Off) *p = Off;
    if (*p++ == (uint8_t)K_Mouse) { len = 3; while(len--) read(0, p++, 1); } } }
    
Cell GetRam(Cell *size) { if (!*size) return 0;
  Cell l = (*size + 0xFFF) & ~0xFFF; void *r = mmap(0, l, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (r == MAP_FAILED) { r = 0; l = 0; }
  *size = l; return (Cell)r; }
  
void FreeRam(Cell addr, Cell size) { if (addr) munmap((void*)addr, size); }

void SWD(Cell addr) { if (!addr) return;
  char *path = (char *)(addr); Cell len = readlink("/proc/self/exe", path, 1024); if (len <= 0) return;
  path[len] = '\0';
  if (MemCmp(path, "/nix/store", 10) == 0) {
    for (char **env = environ; *env != NULL; env++) { char *e = *env;
      if (e[0] == 'H' && e[1] == 'O' && e[2] == 'M' && e[3] == 'E' && e[4] == '=') { chdir(e + 5); return; } }
    return; }
  for (char *p = path + len; p > path; p--) if (*p == '/') { *p = '\0'; chdir(path); break; } }
  
ugoc TermCR(ugoc *r) { *r = TS.row; return TS.col; }

uint8_t SyncSize(Cell addr) { if (!addr) return 0;
  struct winsize ws, cur; if (ioctl(0, TIOCGWINSZ, &ws) < 0) return 0;
  if (ws.ws_col == TS.col && ws.ws_row == TS.row) return 0;
  if (Flag.SyncSize) { uint8_t stable = 3; if (ws.ws_col < TS.col || ws.ws_row < TS.row) Print(Cconvas,Cls);
    while (stable--) { Delay_ms(3);
      if (ioctl(0, TIOCGWINSZ, &cur) >= 0) if (cur.ws_col != ws.ws_col || cur.ws_row != ws.ws_row) { ws = cur; stable = 3; } } }
  TS.col = ws.ws_col; TS.row = ws.ws_row; Flag.SyncSize = 1; return 1; }
  
Cell GetCycles(void) {
  Cell lo, hi; __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
  #if __SIZEOF_POINTER__ > 4
    return (lo + (hi << 32)); }
  #else
    return lo; }
  #endif
  
Cell GetSC(Cell addr) { if (!addr || !TS.col) return 1;
  char *p = (char *)(addr); MemSet(p, ' ', TS.col - 1); p[TS.col - 1] = '\r';
  Cell start = GetCycles(); for(Cell i = 0; i < 100; i++) SysWrite(p, TS.col);
  Cell end = GetCycles(); return (end - start) / (TS.col * 10); }
  
void Delay_ms(uint8_t ms) {
  if (!Flag.Delay_ms) { struct timespec ts = {0, 10000000L}; Cell start = GetCycles();
    nanosleep(&ts, NULL); Flag.Delay_ms = (GetCycles() - start) * 100; if (Flag.Delay_ms == 0) Flag.Delay_ms++; }
  Cell total_cycles = (Cell)ms * (Flag.Delay_ms / 1000); Cell start_time = GetCycles();
  if (ms > 2) { struct timespec sleep_ts = {0, (ms - 1) * 1000000L}; nanosleep(&sleep_ts, NULL); }
  struct timespec check_start; clock_gettime(CLOCK_MONOTONIC_COARSE, &check_start); Cell safety = 0;
  while ((GetCycles() - start_time) < total_cycles) { __asm__ volatile("pause");
    if (++safety > 2000) { struct timespec now; clock_gettime(CLOCK_MONOTONIC_COARSE, &now);
      if (now.tv_sec > check_start.tv_sec) { Flag.Delay_ms = 0; break; }
      safety = 0; } } }
