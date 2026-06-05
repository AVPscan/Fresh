/*
 * Fresh (C) 2026 A.Pozdnyakov GPLv3 - see LICENSE
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 */
 
#include <time.h>           // nanosleep, clock_gettime
#include <termios.h>        // tcgetattr, tcsetattr
#include <fcntl.h>          // open, O_RDONLY, O_NONBLOCK
#include <unistd.h>         // read, write, chdir, close
#include <sys/ioctl.h>      // ioctl, TIOCGWINSZ
#include <sys/mman.h>       // mmap, munmap
#include <mach-o/dyld.h>    // _NSGetExecutablePath
#include <mach/mach_time.h> // mach_absolute_time
#include "sys.h"

SYS_VARS_INIT;

Cell SysWrite(void *buf, Cell len) { return (Cell)write(1, buf, len); }

void SwitchRaw(void) {
  static struct termios oldt;
  if (Flag.SwitchRaw) {
      tcgetattr(0, &oldt); struct termios newt = oldt; newt.c_lflag &= ~(ICANON | ECHO | ISIG); newt.c_iflag &= ~(ICRNL | IXON | ISTRIP);
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
  if (r == MAP_FAILED) { r = 0; l = 0; } *size = l; return (Cell)r; }
    
void FreeRam(Cell addr, Cell size) { if (addr) munmap((void*)addr, size); }

uint8_t SyncSize(Cell addr) { if (!addr) return 0;
  struct winsize ws; if (ioctl(0, TIOCGWINSZ, &ws) < 0) return 0;
  if (ws.ws_col == TS.c && ws.ws_row == TS.r) return 0;
  TS.c = ws.ws_col; TS.r = ws.ws_row; return 1; }

goc RealFps(ugoc fps) { if (!fps) { struct timespec f; clock_gettime(CLOCK_MONOTONIC_RAW, &f); Flag.s = f.tv_sec; Flag.ns = f.tv_nsec; return fps; }
  struct timespec f = {0, 1000000000L / fps}; nanosleep(&f, NULL); clock_gettime(CLOCK_MONOTONIC_RAW, &f);
  Cell t, r = ((t = (f.tv_sec - Flag.s) * 1000000000L + (f.tv_nsec - Flag.ns)) % 1000000000L); Flag.s = f.tv_sec; Flag.ns = f.tv_nsec;
  return (goc)((fps * (t / 1000000000L)) + ((r) ? (1000000000L / r) : 0) - fps); }

void SWD(Cell addr) { if (!addr) return;
  uint32_t len = 4096; char *path = (char *)(addr);
  if (_NSGetExecutablePath(path, &len) != 0) return;
  for (char *p = path + len; p > path; p--) if (*p == '/') { *p = '\0'; chdir(path); break; } }

Cell GetCycles(void) { return (Cell)mach_absolute_time(); }

Cell GetSC(Cell addr) { if (!addr || !TS.c) { return 1; } uint8_t i = 100; char *p = (char *)addr;
  MemSet(p, ' ', TS.c - 1); p[TS.c - 1] = '\r'; Cell start = GetCycles();
  while(--i) { SysWrite(p, TS.c); } return (GetCycles() - start) / (TS.c * 10); }
