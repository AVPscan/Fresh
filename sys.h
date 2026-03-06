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

#include <stdint.h>

#define Reset       "\033[0m"       // СБРОСИТЬ ВСЁ (и цвета, и режимы)
#define Cls         "\033[2J\033[H" // Очистить экран и в начало
#define Home        "\033[H"        // В начало экрана
#define HideCur     "\033[?25l"     // Скрыть курсор
#define ShowCur     "\033[?25h"     // Показать курсор
#define WrapOn      "\033[?7h"      // Включить перенос длинных строк
#define WrapOff     "\033[?7l"      // Выключить перенос строк
#define MouseX10on  "\033[?1000h"   // Включаем мышь
#define MouseX10off "\033[?1000l"
#define AltBufOn    "\033[?1049h"
#define AltBufOff   "\033[?1049l"

//#define USE_BW
#define USE_RGB
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

typedef uintptr_t Cell;
#define SizeCell sizeof(Cell)

enum { 
    CellPow = 13,
    Utf8 = 4,
    CellLine = 1 << CellPow,
    String = CellLine * 987 / 1597,
    SKey = 256,                     // [256] Ring buffer
    SizePal = 32,                   // [32] colour anci
    SizeKey = Utf8 + 4,             // [8]( len UTF8[4 byte] vlen mrtl tic )
    
    SizeData = String * CellLine * 4,
    SizeOffset = String * CellLine * 2,
    SizeAttr = String * CellLine,
    SizeVizLen = String * CellLine,
    SizeLen = String * CellLine,
    SizePalBuff = SizePal * SizePal,
    SizeKeyBuf = SKey * SizeKey,
    SizeVBuff = CellLine * CellLine / 2,
    SizeVram = SizeData + SizeOffset + SizeAttr + SizeVizLen + SizeLen + SizePalBuff + SizeKeyBuf + SizeVBuff,
    
    Data_shift = CellPow + 2,
    Offset_row_shift = CellPow + 1,
    Parse_shift = Utf8 + 1,
    KeyBuf_shift = Utf8 - 1 };

enum {                              // Attr(y,x)       структура атрибута
    Minv = 0x01,                    // invers         0(0 нет)
    Mbol = 0x02,                    // bold           1(0 нет)
    Mcol = 0x1C,                    // color 0-7      2(0...7) цвет 0-6 холст 7 вокруг
    Mcbi = 0x1F,                    //                34 
    Msmem = 0x20,                   // shape memory   5(0 нет) память формы, резиновые структуры [просто лень затирать хвосты] - усложнение рендера но это просто удобно и 67 ускоряют однако
    Mdata = 0x40,                   // bata           6(0 нет) есть ли данные в ячейке
    Fresh = 0x80,                   // degree         7(0 нет) есть ли изменения в ячейке
                                    //                 клавиатура реальное время и буфер на 256 клавиш если прозевали
    Free = 0x14,                    // pause          20мс не гудим даже на одноядерном
    AutoR = 0x19,                   // auto repeat    25 тиков и ещё одна такая же кнопка в буфер залетела, но там компрессия а значит спать на клавиатуре отныне можно
                                    //                 мышь реальное время, просто читаем из Buf.[Mkey, MX, MY, LkX, LkY, MkX, MkY, RkX, RkY]
    On = 1,
    Off = 0 };

enum { K_NO,
    K_Ctrl_A, K_Ctrl_B, K_Ctrl_C, K_Ctrl_D, K_Ctrl_E, K_Ctrl_F, K_Ctrl_G, K_DEL,
    K_TAB, K_LF, K_Ctrl_K, K_Ctrl_L, K_ENT, K_Ctrl_N, K_Ctrl_O, K_Ctrl_P,
    K_Ctrl_Q, K_Ctrl_R, K_Ctrl_S, K_Ctrl_T, K_Ctrl_U, K_Ctrl_V, K_Ctrl_W, K_Ctrl_X,
    K_Ctrl_Y, K_Ctrl_Z, K_ESC, K_FS, K_GS, K_RS, K_US, K_UP,
    K_Ctrl_UP, K_DOW, K_Ctrl_DOW, K_RIG, K_Ctrl_RIG, K_LEF, K_Ctrl_LEF, K_Mouse,
    K_HOM, K_END, K_PUP, K_PDN, K_INS, K_F1, K_F2, K_F3,
    K_F4, K_F5, K_F6, K_F7, K_F8, K_F9, K_F10, K_F11,
    K_F12, K_F13, K_F14, K_F15, K_BAC = 127, K_Max = K_F15 + 1 };

enum { 
    Cdefault, CdefaultI, CdefaultB, CdefaultBI, Cgrey, CgreyI, CgreyB, CgreyBI,
    Cgreen, CgreenI, CgreenB, CgreenBI, Cred, CredI, CredB, CredBI,
    Cblue, CblueI, CblueB, CblueBI, Corange, CorangeI, CorangeB, CorangeBI,
    Cgold, CgoldI, CgoldB, CgoldBI, Cborder, Cconvas, LastAttr };

Cell SysWrite(void *buf, Cell len);

Cell StrLen(char *s);
void MemSet(void* buf, uint8_t val, Cell len);
void MemCpy(void* dst, void* src, Cell len);
int8_t MemCmp(void* dst, void* src, Cell len);
void MemMove(void* dst, void* src, Cell len);
uint8_t UTFinfo(char *s, uint8_t *len, uint8_t *Mrtl);
uint8_t UTFinfoTile(char *s, uint8_t *len, uint8_t *Mrtl, Cell rem);
void InitVram(Cell addr, Cell size);
uint8_t GetBufKey(uint8_t *len, uint8_t *vlen, uint8_t *mrtl, uint8_t *count, char *key);
uint8_t Key(uint8_t *num, uint8_t *tic, uint8_t *control);
void ShowC(void);
uint8_t ViewPort(void);
Cell SystemSwitch(void);

void SwitchRaw(void);
void GetKey(char *b);
Cell GetRam(Cell *size);
void FreeRam(Cell addr, Cell size);
void SWD(Cell addr);
uint16_t TermCR(uint16_t *r);
int16_t SyncSize(Cell addr, uint8_t flag);
Cell GetCycles(void);
void Delay_ms(uint8_t ms);
Cell GetSC(Cell addr);

void Print(uint8_t n, char *str);
void Show(void);
#endif /* SYS_H */
