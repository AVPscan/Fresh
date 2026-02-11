/*
 * Copyright (C) 2026 Поздняков Алексей Васильевич
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 */
 
#ifndef SYS_H
#define SYS_H

#include <stddef.h>
#include <stdint.h>

//#define USE_BW
//#define USE_RGB

#ifdef USE_BW
  #define Cnn ""
  #define Cna ""
  #define Cpr ""
  #define Cnu ""
  #define Cap ""
  #define Cam ""
#else
  #ifdef USE_RGB
    #define Cnn "\033[38;2;120;120;120m"
    #define Cna "\033[38;2;210;105;30m"
    #define Cpr "\033[38;2;184;134;11m"
    #define Cnu "\033[38;2;30;144;255m"
    #define Cap "\033[38;2;34;139;34m"
    #define Cam "\033[38;2;220;20;60m"
  #else // обычные
    #define Cnn "\033[38;5;244m" // Номер (Серый)
    #define Cna "\033[38;5;166m" // Имя (Оранжевый)
    #define Cpr "\033[38;5;178m" // Цена (Золотистый) — хорошо выделяется
    #define Cnu "\033[38;5;27m"  // Число (Синий)
    #define Cap "\033[38;5;28m"  // Рост/Плюс (Темно-зеленый)
    #define Cam "\033[38;5;160m" // Падение/Минус (Красный)
  #endif
#endif

typedef uintptr_t Cell;
#define CELL_SIZE sizeof(uintptr_t)
#define SYSTEM_SECTOR_SIZE  0
#define GLOBAL_SIZE_STR     8192
#define GLOBAL_STRING       2048
#define GLOBAL_DATA_SIZE    (GLOBAL_SIZE_STR * GLOBAL_STRING)
#define GLOBAL_ATTR_SIZE    (GLOBAL_SIZE_STR * GLOBAL_STRING)
#define GLOBAL_TOKEN_SIZE   (GLOBAL_SIZE_STR * GLOBAL_STRING) * 4
#define GLOBAL_LINE_SIZE    GLOBAL_SIZE_STR * 6
#define GLOBAL_SIZE         GLOBAL_DATA_SIZE * 6 + GLOBAL_LINE_SIZE

#define GET_BLINE(row)      (G_DATA + ((size_t)(row) << 13))
#define GET_DLINE(row)      (G_LINE + (((size_t)(row) << 1)+(size_t)(row)) << 1)
#define GET_ATTR(row,col)   (G_ATTRIBUTE + ((size_t)(row) << 13)) + (size_t)col
#define GET_TOKEN(row,col)  ((G_TOKENS + ((size_t)(row) << 13)) + (size_t)(col)<<2)

#define Crs   "\033[0m"
#define HCur  "\033[?25l"
#define ShCur "\033[?25h"
#define Cls   "\033[2J\033[H"
#define SCur  "\033[s"
#define LCur  "\033[u"
#define Cha   "\033[G"
#define ZCur  "\033[K"
#define LWOn  "\033[?7h"
#define LWOff "\033[?7l"

enum { K_NO, K_CRA, K_CRB, K_CRC, K_CRD, K_CRE, K_CRF, K_CRG, 
    K_DEL, K_TAB, K_LF,  K_CRK, K_CRL, K_ENT, K_CRN, K_CRO, K_CRP, K_CRQ, K_CRR, K_CRS, K_CRT, K_CRU, 
    K_CRV, K_CRW, K_CRX, K_CRY, K_CRZ, K_ESC, K_FS, K_GS, K_RS, K_US, K_SPA,
    K_UP, K_DOW, K_RIG, K_LEF, K_HOM, K_END, K_PUP, K_PDN, K_INS,
    K_F1, K_F2, K_F3, K_F4, K_F5, K_F6, K_F7, K_F8, K_F9, K_F10, K_F11, K_F12, K_BAC = 127 };
void* os_open_file(const char* name);
void* os_create_file(const char* name);
void  os_close_file(void* handle);
int   os_read_file(void* handle, unsigned char* buf, int len);
int   os_read_file_at(void* handle, long offset, unsigned char* buf, int len);
int   os_print_file(void* handle, const char* format, ...);
void  os_printf(const char* format, ...);
int   os_snprintf(char* buf, size_t size, const char* format, ...);
void os_memset(void* buf, int val, size_t len);
void  SetInputMode(int raw);
const char* GetKey(void);
size_t GetVram(size_t *size);
void FreeVram(void);
int GetC(void);
void SWD(void);
int os_sync_size(void);
int GetWH(int *h);
char *GetBuf(void);
const char *Button(const char *label, int active);
uint64_t get_cycles(void);
void  delay_ms(int ms);

#endif /* SYS_H */
