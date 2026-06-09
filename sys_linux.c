/*
 * Fresh (C) 2026 A.Pozdnyakov GPLv3 - see LICENSE
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 */
 
#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include <time.h>      // nanosleep, clock_gettime
#include <termios.h>   // tcgetattr, tcsetattr
#include <fcntl.h>     // fcntl, O_NONBLOCK
#include <unistd.h>    // read, write, chdir, readlink
#include <sys/mman.h>  // mmap, munmap
#include <sys/ioctl.h> // ioctl, TIOCGWINSZ
#include "sys.h"

SYS_VARS_INIT;

Cell SysWrite(void *buf, Cell len) { return (Cell)write(1, buf, len); }

void SwitchRaw(void) {
  static struct termios oldt;
  if (Flag.SwitchRaw) {
    tcgetattr(0, &oldt); struct termios newt = oldt; newt.c_lflag &= ~(ICANON | ECHO | ISIG ); newt.c_iflag &= ~(ICRNL | IXON | ISTRIP);
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

goc RealFps(ugoc fps) { if (!fps) { struct timespec f; clock_gettime(CLOCK_MONOTONIC_COARSE, &f); Flag.s = f.tv_sec; Flag.ns = f.tv_nsec; return fps; }
  struct timespec f = {0, 1000000000L / fps}; nanosleep(&f, NULL); clock_gettime(CLOCK_MONOTONIC_COARSE, &f);
  Cell t, r = ((t = (f.tv_sec - Flag.s) * 1000000000L + (f.tv_nsec - Flag.ns)) % 1000000000L); Flag.s = f.tv_sec; Flag.ns = f.tv_nsec;
  return (goc)((fps * (t / 1000000000L)) + ((r) ? (1000000000L / r) : 0) - fps); }

Cell GetRam(Cell *size) { if (!*size) return 0;
  Cell l = (*size + 0xFFF) & ~0xFFF; void *r = mmap(0, l, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (r == MAP_FAILED) { r = 0; l = 0; } *size = l; return (Cell)r; }
  
void FreeRam(Cell addr, Cell size) { if (addr) munmap((void*)addr, size); }

uint8_t SyncSize(void) { if (!VRam.addr) return Off;
  struct winsize ws; if (ioctl(0, TIOCGWINSZ, &ws) < Off) return Off;
  if (ws.ws_col == TS.c && ws.ws_row == TS.r) return Off;
  TS.c = ws.ws_col; TS.r = ws.ws_row; return On; }

extern char **environ;
void SWD(void) { if (!VRam.addr) return;
  char *path = (char*)(var.dbuf); Cell len = readlink("/proc/self/exe", path, 1024); if (len <= 0) return;
  path[len] = 0; if (MemCmp(path, "/nix/store", 10) == 0) {
    for (char **env = environ; *env != NULL; env++) { char *e = *env;
      if (e[0] == 'H' && e[1] == 'O' && e[2] == 'M' && e[3] == 'E' && e[4] == '=') { chdir(e + 5); return; } }
    return; }
  for (char *p = path + len; p > path; p--) if (*p == '/') { *p = '\0'; chdir(path); break; } }

Cell GetCycles(void) {
  Cell lo, hi; __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
  #if __SIZEOF_POINTER__ > 4
    return ((Cell)lo + ((Cell)hi << 32)); }
  #else
    return lo; }
  #endif

Cell GetSC(void) { if (!VRam.addr || !TS.c) { return 1; } uint8_t i = 100; char *p = (char *)VRam.addr;
  MemSet(p, ' ', TS.c - 1); p[TS.c - 1] = '\r'; Cell start = GetCycles();
  while(--i) { SysWrite(p, TS.c); } return (GetCycles() - start) / (TS.c * 10); }

