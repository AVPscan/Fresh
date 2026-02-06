/*
 * Copyright (C) 2026 Поздняков Алексей Васильевич
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 */
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sys.h"

extern unsigned char FileBuf[DBuf+NBuf];

int main() {
  os_sync_size();
  SetInputMode(1); printf("%d %d ", TS.w, TS.h); printf(HCur SCur); fflush(stdout);
  while (1) {
    delay_ms(60);
    const char* k = GetKey();
    if (k[0] == 27 && k[1] == K_NO) continue;
    if (k[0] != 27) printf (LCur Cna "%s    ", k);
    else printf (LCur Cnu "(%d)    ", k[1]);
    fflush(stdout); if (k[0] == 27 && k[1] == K_ESC) break; }
  SetInputMode(0); printf(ShCur Crs); fflush(stdout); return 0; }

