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

#define Reset       "\033[0m"                                       // СБРОСИТЬ ВСЁ (и цвета, и режимы)
#define Cls         "\033[2J\033[H"                                 // Очистить экран и в начало
#define Home        "\033[H"                                        // В начало экрана
#define HideCur     "\033[?25l"                                     // Скрыть курсор
#define ShowCur     "\033[?25h"                                     // Показать курсор
#define WrapOn      "\033[?7h"                                      // Включить перенос длинных строк
#define WrapOff     "\033[?7l"                                      // Выключить перенос строк
#define MouseX10on  "\033[?1000h"                                   // Включаем мышь
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
    #define Grey    "\033[38;2;120;120;120m"                        // Бледный нейтральный
    #define Green   "\033[38;2;34;139;34m"                          // Глубокий лесной
    #define Red     "\033[38;2;220;20;60m"                          // Насыщенный малиновый
    #define Blue    "\033[38;2;30;144;255m"                         // Яркий небесный
    #define Orange  "\033[38;2;210;105;30m"                         // Сочный оранжевый
    #define Gold    "\033[38;2;184;134;11m"                         // Светящийся золотой
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
    SKey = 256,                                                     // [256] Ring buffer
    SizePal = 32,                                                   // [32] buffer colour anci
    SizeKey = 8,                                                    // [8] (data1 data2 tic1 tic2 UTF8[4 byte])
    Utf8 = 4,                                                       // Max length utf8
    Bit32 = 2,                                                      // 2^2 4 bytes
    Bit16 = 1,                                                      // 2^1 2 bytes
    CellPow = 13,                                                   // 2^CellPow
    Data_shift = CellPow + Bit32,                                   // 2^(CellPow + 2) Offset Utf8 per cell
    Offset_row_shift = CellPow + Bit16,                             // 2^(CellPow + 1) Offset 2 bytes per cell offset
    AVL_shift = CellPow,                                            // 2^(CellPow) Offset attr,vlen,len
    Win_shift = SizeKey,                                            // 2^8 Offset 2 bytes window string length
    Parse_shift = Utf8 + Bit16,                                     // 2^5 Offset 32 bytes per palette cell
    KeyBuf_shift = Utf8 - Bit16,                                    // 2^3 Offset 8 bytes per keyboard buffer cell
    CellLine = 1 << CellPow,                                        // Max cell utf8 in line
    CellStr = CellLine * 987 / 1597,                                // Max cell utf8 string
    SizeData = CellStr * CellLine * Utf8,                           // Data array cell uft8
    SizeAttr = CellStr * CellLine,                                  // Attribute array cell utf8
    SizeVisLen = CellStr * CellLine,                                // Visual length array cell utf8
    SizeLen = CellStr * CellLine,                                   // Length array cell utf8
    SizeOffset = CellStr * CellLine * 2,                            // Offset cell utf8 in line
    SizeVlsWin = CellStr * SKey * 2,                                // Visual line length in the window [256]
    SizePalBuff = SizePal * SizePal,                                // Palette array [8*4][32]
    SizeKeyBuf = SKey * SizeKey,                                    // Keyboard buffer [256][8]
    SizeVBuff = CellLine * CellLine / 2,                            // Video buffer
    SizeVram = SizeData + SizeAttr + SizeVisLen + SizeLen + SizeOffset + SizeVlsWin + SizePalBuff + SizeKeyBuf + SizeVBuff };
enum {                                                              // Attr(y,x)     структура атрибута
    Minv = 0x01,                                                    // invers         0(0 нет)
    Mbol = 0x02,                                                    // bold           1(0 нет)
    Mcol = 0x1C,                                                    // color 0-7      2(0...7) цвет 0-6 холст 7 вокруг
    Mcbi = 0x1F,                                                    //                34 
    Msmem = 0x20,                                                   // shape memory   5(0 нет) память формы, резиновые структуры [просто лень затирать хвосты] 
                                                                    //                усложнение рендера но это просто удобно и 67 ускоряют однако
    Mdata = 0x40,                                                   // bata           6(0 нет) есть ли данные в ячейке или пробел[ы]
    Fresh = 0x80,                                                   // degree         7(0 нет) есть ли изменения в ячейке
    Free = 0x14,                                                    // pause          20мс не гудим даже на одноядерном
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
// [data] = 7 не UTF8 6 не влезает в буфер 5 управляющий код 4 направление письма 32 визуальная длина 0-2[3 управляющий] 10 длина 0-3 [1-4].
Cell StrLen(char *s);                                               // Длина строки
void MemSet(void* buf, uint8_t val, Cell len);                      // Заполнение куска памяти val
void MemCpy(void* dst, void* src, Cell len);                        // Копирование куска памяти, без проверки наложения!
int8_t MemCmp(void* dst, void* src, Cell len);                      // Сравнение
void MemMove(void* dst, void* src, Cell len);                       // Перемещение куска памяти с проверкой наложения
uint8_t UTFinfo(char *s);                                           // Рассказ об utf8 возвращает [data]
uint8_t UTFinfoTile(char *s, Cell len);                             // Рассказ об utf8 возвращает [data] с учётом буфера
void Print(uint8_t n, char *str);                                   // Для отладки
void InitVram(Cell addr, Cell size);                                // Инициализация мира
Cell SystemSwitch(void);                                            // Вход/выход в мир
uint8_t PushKey(char *key);                                         // Положить клавишу в буфер [код управляющей или печатная 0xFF или 0 ошибка]
uint8_t ShowKey(uint8_t *data, uint8_t *count, char *key);          // Показать ожидаемую/получаемую клавишу
uint8_t PopKey(uint8_t *data, uint8_t *count, char *key);           // Взять клавишу из буфера [1] буфер пуст [0] видна ожидаемая/получаемая
void ForgetKey(void);                                               // Забыть последнюю пришедшую клавишу в буфере даже ожидаемую/получаемую
uint16_t Keys(void);                                                // Сколько клавиш в буфере
uint8_t Mouse(uint8_t key, uint8_t x, uint8_t y);                   // Обработка событий мыши с учётом рамок терминала
uint8_t GetEventKM(uint8_t *num, uint8_t *tic, uint8_t *control);   // Читаем мышь и клавиатуру, заполняем буфер при необходимости, проверка управляющих кодов.
uint8_t ViewPort(void);                                             // Полёт над пространством с возможностью приземления на холст

Cell SysWrite(void *buf, Cell len);                                 // Выстрел в терминал
void SwitchRaw(void);                                               // Включение/выключение неблокирующего ввода RealTime
void GetKey(char *b);                                               // Читаем utf8 из порта
Cell GetRam(Cell *size);                                            // Взять память
void FreeRam(Cell addr, Cell size);                                 // Вернуть память
void SWD(Cell addr);                                                // Установить рабочую директорию
uint16_t TermCR(uint16_t *r);                                       // Считать рамки терминала
int16_t SyncSize(Cell addr);                                        // Получить рамки терминала при необходимости стабилизировать
Cell GetCycles(void);                                               // Тики
void Delay_ms(uint8_t ms);                                          // Адаптивная задержка, гарантия точности ms
Cell GetSC(Cell addr);                                              // Измерение пропускной способности терминала

void Show(void);                                                    // Для отладки
#endif /* SYS_H */
