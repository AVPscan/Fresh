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

int main() {
  SetInputMode(1); printf(HCur SCur); fflush(stdout);
  while (1) {
    delay_ms(60);
    const char* k = GetKey();
    if (k[0] == 27 && k[1] == K_NOP) continue;
    if (k[0] != 27) printf ( Cna Lcur "%s    ", k);
    else printf (Cnu Lcur "%d    ", k[1]);
    if (k[0] == 27 && k[1] == K_ESC) break; }
  SetInputMode(1); printf(ShCur Cce); fflush(stdout);
  return 0; }
