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
  #define Grey    ""
  #define Green   ""
  #define Red     ""
  #define Blue    ""
  #define Orange  ""
  #define Gold    ""
#else
  #ifdef USE_RGB
    #define Grey    "\033[38;2;120;120;120m" // Бледный нейтральный
    #define Green   "\033[38;2;34;139;34m"   // Глубокий лесной
    #define Red     "\033[38;2;220;20;60m"   // Насыщенный малиновый
    #define Blue    "\033[38;2;30;144;255m"  // Яркий небесный
    #define Orange  "\033[38;2;210;105;30m"  // Сочный оранжевый
    #define Gold    "\033[38;2;184;134;11m"  // Светящийся золотой
  #else
    #define Grey    "\033[38;5;244m"
    #define Green   "\033[38;5;28m"
    #define Red     "\033[38;5;160m"
    #define Blue    "\033[38;5;27m"
    #define Orange  "\033[38;5;166m"
    #define Gold    "\033[38;5;178m"
  #endif
#endif

#define Bold        "\033[1m"       // Жирный
#define Dim         "\033[2m"       // Тусклый
#define BoldOff     "\033[22m"      // Выключить Bold / Dim
#define Italic      "\033[3m"       // Курсив
#define ItalicOff   "\033[23m"      // Выключить курсив
#define Underline   "\033[4m"       // Подчеркивание
#define DoubleUnder "\033[21m"      // Двойное подчеркивание
#define UnderOff    "\033[24m"      // Выключить подчеркивание (любое)
#define Overline    "\033[53m"      // Надчеркивание
#define OverlineOff "\033[55m"      // Выключить надчеркивание
#define Blink       "\033[5m"       // Мигание
#define BlinkOff    "\033[25m"      // Выключить мигание
#define Invert      "\033[7m"       // Инверсия (реверс цветов)
#define InvertOff   "\033[27m"      // Выключить инверсию
#define Strike      "\033[9m"       // Зачеркивание
#define StrikeOff   "\033[29m"      // Выключить зачеркивание
#define ColorOff    "\033[39m"      // Сбросить только цвет
#define Reset       "\033[0m"       // СБРОСИТЬ ВСЁ (и цвета, и режимы)

#define Home        "\033[H"        // В начало экрана
#define HideCur     "\033[?25l"     // Скрыть курсор
#define ShowCur     "\033[?25h"     // Показать курсор
#define Cls         "\033[2J\033[H" // Очистить экран и в начало
#define SaveCur     "\033[s"        // Сохранить позицию курсора
#define LoadCur     "\033[u"        // Вернуть курсор в сохраненную позицию
#define HomeCur     "\033[G"        // Вернуть курсор в начало текущей строки
#define ClearLine   "\033[K"        // Очистить строку от курсора до конца
#define WrapOn      "\033[?7h"      // Включить перенос длинных строк
#define WrapOff     "\033[?7l"      // Выключить перенос строк
#define MouseX10on  "\033[?1000h"   // Включаем мышь
#define MouseX10off "\033[?1000l"
#define AltBufOn    "\033[?1049h"
#define AltBufOff   "\033[?1049l"

#define Off         0
#define On          1
typedef uintptr_t Cell;
#define SizeCell sizeof(Cell)
  
enum { K_NO, K_Ctrl_A, K_Ctrl_B, K_Ctrl_C, K_Ctrl_D, K_Ctrl_E, K_Ctrl_F, K_Ctrl_G,
    K_DEL, K_TAB, K_LF,  K_Ctrl_K, K_Ctrl_L, K_ENT, K_Ctrl_N, K_Ctrl_O, K_Ctrl_P,
    K_Ctrl_Q, K_Ctrl_R, K_Ctrl_S, K_Ctrl_T, K_Ctrl_U, K_Ctrl_V, K_Ctrl_W, K_Ctrl_X,
    K_Ctrl_Y, K_Ctrl_Z, K_ESC, K_FS, K_GS, K_RS, K_US, K_UP, K_Ctrl_UP, K_DOW, K_Ctrl_DOW,
    K_RIG, K_Ctrl_RIG, K_LEF, K_Ctrl_LEF, K_Mouse, K_HOM, K_END, K_PUP, K_PDN, K_INS,
    K_F1, K_F2, K_F3, K_F4, K_F5, K_F6, K_F7, K_F8, K_F9, K_F10, K_F11, K_F12, K_F13, K_F14, K_F15, K_BAC = 127 };
enum { Ccurrent = 0, CcurrentI, CcurrentB, CcurrentIB, Cdef, CdefI, CdefB,
    CdefIB, Cgrey, CgreyI, CgreyB, CgreyIB, Cgreen, CgreenI, CgreenB,
    CgreenIB, Cred, CredI, CredB, CredIB, Cblue, CblueI, CblueB, CblueIB,
    Corange, CorangeI, CorangeB, CorangeIB, Cgold, CgoldI, CgoldB, CgoldIB };

size_t StrLen(char *s);
void MemSet(void* buf, uint8_t val, size_t len);
void MemCpy(void* dst, const void* src, size_t len);
int8_t MemCmp(void* dst, const void* src, size_t len);
void MemMove(void* dst, const void* src, size_t len);
int8_t UTFinfo(unsigned char *s, uint8_t *len, uint8_t *Mrtl);
int8_t UTFinfoTile(unsigned char *s, uint8_t *len, uint8_t *Mrtl, size_t rem);

int SystemSwitch(void);
void SetColour(uint8_t col);
uint8_t ViewPort(void);

void SwitchRaw(void);
void GetKey(char *b);
size_t GetRam(size_t *size);
void FreeRam(size_t addr, size_t size);
void SWD(size_t addr);
uint16_t TermCR(uint16_t *r);
int16_t SyncSize(size_t addr, uint8_t flag);
uint64_t GetCycles(void);
void Delay_ms(uint8_t ms);
int GetSC(size_t addr);

void Print(uint8_t n, char *str);
void Show(void);
int Help(int argc, char *argv[], int flag);
#endif /* SYS_H */
