/*
 * Copyright (C) 2026 Поздняков Алексей Васильевич
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 */
 
#include <stdio.h>
#include <time.h>               // ОБЯЗАТЕЛЬНО для struct timespec
#include "sys.h"

extern unsigned char FileBuf[DBuf+NBuf];

int main() {
  os_sync_size(); delay_ms(0); int mode = 0;
  SetInputMode(1); printf(Cam "%d %d", TS.w, TS.h); printf(HCur SCur); fflush(stdout);
  while (1) {
    struct timespec real_start, real_end;
    clock_gettime(CLOCK_MONOTONIC, &real_start);
    delay_ms(60);
    clock_gettime(CLOCK_MONOTONIC, &real_end);
    double real_ms = (real_end.tv_sec - real_start.tv_sec) * 1000.0 + 
                 (real_end.tv_nsec - real_start.tv_nsec) / 1000000.0;
    const char* k = GetKey();
    if (k[0] == 27 && k[1] == K_NO) continue;
    if (k[0] == 'p') mode = (mode + 1) & 1;
    if (k[0] != 27) printf (LCur  Cna " тоси%sбоси " Cap "%s", Button("Погнали",mode), k);
    else printf (LCur  Cna " тоси%sбоси " Cap "(%d)", Button("Погнали",mode), k[1]);
    printf(" [Ms: %f]       ", real_ms); fflush(stdout); if (k[0] == 27 && k[1] == K_ESC) break; }
  SetInputMode(0); printf(ShCur Crs); fflush(stdout); return 0; }

