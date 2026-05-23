/*
 * Fresh (C) 2026 A.Pozdnyakov GPLv3 - see LICENSE
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 */
 
#ifndef SYS_H
#define SYS_H

#include <stdint.h>

#define Reset       "\033[0m"                 // СБРОСИТЬ ВСЁ (и цвета, и режимы)
#define Cls         "\033[2J\033[H"           // Очистить экран и в начало
#define Home        "\033[H"                  // В начало экрана
#define ShowCur     "\033[?25h"               // Показать курсор
#define HideCur     "\033[?25l"               // Скрыть курсор
#define WrapOn      "\033[?7h"                // Включить перенос длинных строк
#define WrapOff     "\033[?7l"                // Выключить перенос строк
#define MouseX10on  "\033[?1000h"             // Включаем мышь
#define MouseX10off "\033[?1000l"             // Выключаем мышь
#define AltBufOn    "\033[?1049h"             // Включаем альтернативный буфер
#define AltBufOff   "\033[?1049l"             // Выключаем альтернативный буфер

#define CellPow   13                          // Масштаб холста 13 16к, 14 32к, 15 64к.... 
#define MAX_WIN   512                         // Максимально число окон на холсте
#define MaxSpeed  (1 << (CellPow - 4))        // Максимальное ускорение курсора
#define FFps      144                         // Частота регенерации монитора
#define Fcolour   8                           // Количество цветов на старте {максимум 32} 2 палитры
#define CFDeep    24                          // Глубина {0 3 8 24} бита {0 значит создаётся чистый цвет без наложения 8 состояний}
#if CellPow < 15                              // Создаём новый тип данных, достаточный для работы с нужным разрешением, 
    typedef uint16_t ugoc;                    // а так же константы для генератора случайных чисел {VP.Rnd текущее значение генератора} 
    typedef int16_t  goc;
    #define RNG_A 0x4F2D
    #define RNG_B 0x3A7B
#elif CellPow < 31
    typedef uint32_t ugoc;
    typedef int32_t  goc;
    #define RNG_A 0x41C64E6D
    #define RNG_B 0x3039
#else
    typedef uint64_t ugoc;
    typedef int64_t  goc;
    #define RNG_A 0x9E3779B97F4A7C15ULL
    #define RNG_B 0xBF58476D1CE4E5B9ULL
#endif
#define UGOC_MAX ((ugoc)~(ugoc)0)             // Вычисляем пределы для нового типа данных
#define GOC_MAX  ((goc)(UGOC_MAX >> 1))
#define GOC_INF  (~GOC_MAX)                   // Бесконечность!
#define GOC_MIN  (~GOC_MAX) + 1
typedef void (*AFunction)(void);
typedef uintptr_t Cell;                       // Разрядность процессора, создали абстракцию
#define SCell sizeof(Cell)

enum {
    b0 = 0x01, b1 = 0x02, b2 = 0x04, b3 = 0x08, b4 = 0x10, b5 = 0x20, b6 = 0x40, b7 = 0x80, b8 = 0x100,
    b3210 = 0x0F, b10 = 0x03, b21 = 0x06, b65 = 0x60, b76 = 0xC0, b210 = 0x07, b765 = 0xE0, 
    aB = 0x40, aI = 0x20, aC = 0x80 , aBI = 0x60, aCI = 0xA0, aCB = 0xC0, aCBI = 0xE0, aShift = 0x05,
    aColours = 0x20, On = 0x01, Off = 0x00 };
enum {
    K_NO, K_Ctrl_A, K_Ctrl_B, K_Ctrl_C, K_Ctrl_D, K_Ctrl_E, K_Ctrl_F, K_Ctrl_G,
    K_DEL, K_TAB, K_LF, K_Ctrl_K, K_Ctrl_L, K_ENT, K_Ctrl_N, K_Ctrl_O,
    K_Ctrl_P, K_Ctrl_Q, K_Ctrl_R, K_Ctrl_S, K_Ctrl_T, K_Ctrl_U, K_Ctrl_V, K_Ctrl_W,
    K_Ctrl_X, K_Ctrl_Y, K_Ctrl_Z, K_ESC, K_FS, K_GS, K_RS, K_US,
    K_BAC = 127, K_Ctrl_LEF, K_Ctrl_UP, K_Ctrl_RIG, K_Ctrl_DOW, K_LEF, K_UP, K_RIG,
    K_DOW, K_HOM, K_END, K_PUP, K_PDN, K_INS, K_F1, K_F2,
    K_F3, K_F4, K_F5, K_F6, K_F7, K_F8, K_F9, K_F10,
    K_F11, K_F12, K_F13, K_F14, K_F15, K_ALT_TAB, K_ALT_ENT, K_ALT_ESC,
    K_Mouse };
enum { Fblack, Fnavy, Folive, Fcyan, Ffuchsia, Fmarsala, Fochre, Fwhite, };
enum { black, navy, olive, cyan, fuchsia, marsala, ochre, white };
typedef struct {                              //UTFinfo  
    uint8_t len     : 2;                      // бит 10     длина (0-3) + 1, игнорируем так как размер в байтах через offset
    uint8_t vis     : 2;                      // бит 32     визуальная ширина (0-2)
    uint8_t Dir     : 1;                      // бит 4      направление (0=LTR,1=RTL)
    uint8_t Ctrl    : 1;                      // бит 5      управляющий код
    uint8_t ds      : 1;                      // бит 6      {0} Data {1} Structure
    uint8_t Refresh : 1;                      // бит 7      {1/0} есть изменения /нет изменений
} Data;
typedef struct {
    uint8_t len     : 4;                      // бит 3210  длина = 1+(0-15) ascii {32...126} визуальная длина равна длине в байтах (числа)
    uint8_t right   : 1;                      // бит 4      {1} к правому
    uint8_t left    : 1;                      // бит 5      {1} к левому {00}/{11} по центру (как заполнять поле структуры)
    uint8_t ds      : 1;                      // бит 6      {1} Structure {0} Data
    uint8_t Refresh : 1;                      // бит 7      {1/0} есть изменения /нет изменений
} Structure;
typedef struct {
    uint8_t col        ;                      // бит x      цвет {максимум 32 цветовых оттенка из за трёх атрибутов}
    uint8_t inverse : 1;                      // бит 5  aI  инверсия
    uint8_t bold    : 1;                      // бит 6  aB  толстый
    uint8_t cursive : 1;                      // бит 7  aC  курсив
} palette;
typedef struct { uint8_t l, d[31]; } PalBuf;
typedef struct { uint8_t data1, tic1, data2, tic2, utf8[4]; } KeyBuf; 
typedef struct { uint8_t C, N; uint16_t W; } Events;
typedef struct {
    uint8_t sd      : 1;                      // бит 0      {1} статичное (не изменяется в размере на холсте, в байтах) {0} динамичное окно
    uint8_t vision  : 1;                      // бит 1      {1} отображается {0} не отображается
    uint8_t cursor  : 1;                      // бит 2      {1} показывать {0} не показывать - курсор окна
    uint8_t nowrap  : 1;                      // бит 3      {1} включен {0} выключен авто перенос строк окна
    uint8_t wait    : 1;                      // бит 4      {1} занято заливаются данные из файла/порта {0} свободно
} EF;
typedef struct { ugoc Spd0, Spd1, Speed, Fps, Delay, Time, Ginf, Gmin, Gmax, A, B; uint16_t MWin, T[5], S[5], Tb[5]; uint8_t Deep, Colours, CellP, Ctb; } Sis;
typedef struct { ugoc W, H, CW, CH; uint16_t Win, Min, Max, D, S; uint8_t Fone, Border; } Canalysis;
typedef struct { goc Xr, Yr; ugoc W, H, MaxCs, MaxVs, MaxH, XCur, YCur, WFirstSR, Xc, Yc; uint16_t Layer, parent, child; uint8_t palette, EF; } Windows;
enum {
    SKey = 256,                                                               // Буфер клавиатуры на 255/510 клавиш с автоповторами
    CellLine = 1 << CellPow,                                                  // Определение ширины холста
    CellStr = CellLine / 2,                                                   // Определение высоты холста
    ConvasArea = CellLine * CellStr,                                          // Площадь холста
    SDCell = ConvasArea * 4,                                                  // Размер данных для ячеек холста
    SInfo = ConvasArea,                                                       // Размер атрибутов для ячеек холста (Info)
    SDs = ConvasArea,                                                         // Размер информации о ячейках холста (Data/Structure)
    SOffset = ConvasArea * sizeof(ugoc) / 2,                                  // Размер смещений для ячеек холста
    SFon = 2 * 32 * sizeof(PalBuf),                                           // Размер буфера фона (32 цветов) под 2 палитры
    SPal = 2 * 32 * 8 * sizeof(PalBuf),                                       // Размер буфера палитры (8 режимов по 32 цветов) под 2 палитры
    SKeys = SKey * sizeof(KeyBuf),                                            // Размер данных кольцевого буфера клавиатуры
    SSys = sizeof(Sis) / 2,                                                   // Размер данных под разбивку холста для организации окон
    SConvas = sizeof(Canalysis) / 2,                                          // Размер данных под разбивку холста для организации окон
    SWin = MAX_WIN * sizeof(Windows) / 2,                                     // Размер данных для окон
    SVsw = MAX_WIN * CellStr * sizeof(ugoc) / 2,                              // Размер для визуальных длин строк окон на холсте
    SCsw = MAX_WIN * CellStr * sizeof(ugoc) / 2,                              // Размер для числа ячеек в строках окон на холсте
    SEvent = SKey * sizeof(Events),                                           // Размер данных для событий (привязка вызова функций к событиям)
    SExec = SKey * sizeof(AFunction),                                         // Размер вектора событий
    SBuf = 8192,                                                              // Размер буфера Print/File
    SizeVram = SDCell + SInfo + SDs + SFon + SPal + SKeys + 2 * (SOffset + SVsw + SCsw + SWin + SSys + SConvas) + SEvent + SExec + SBuf,
    D_shift = CellPow + 2,                                                    // Смещение между строк холста в байтах
    O_shift = CellPow + 1,                                                    // Смещение между смещениями строк холста
    Ds_shift = CellPow,                                                       // Смещение между атрибутами строк холста
    VCsw_shift = CellPow - 1,                                                 // Смещение между окон по каждой строке холста
    P_shift = 5,                                                              // Смещение для палитр
    W_shift = 4,                                                              // Смещение для данных окон в словах - 32 байта на окно.
    K_shift = 3,                                                              // Смещение для ячеек буфера клавиатуры
    V_shift = 2 };                                                            // Смещение для организации событий
_Static_assert((1 << D_shift) == CellLine * 4, "D_shift mismatch");
_Static_assert((1 << O_shift) == CellLine * sizeof(ugoc), "O_shift");
_Static_assert((1 << Ds_shift) == CellLine, "Ds_shift mismatch");
_Static_assert((1 << VCsw_shift) == CellStr * sizeof(ugoc) / 2, "VCsw_shift mismatch");
_Static_assert((1 << W_shift) == sizeof(Windows) / 2, "W_shift mismatch");
_Static_assert((1 << P_shift) == sizeof(PalBuf), "P_shift mismatch");
_Static_assert((1 << K_shift) == sizeof(KeyBuf), "K_shift mismatch");
_Static_assert((1 << V_shift) == sizeof(Events), "V_shift mismatch");
#define Data(r)       (Cdata + ((r) << D_shift))                              // адрес начала буфера строки холста
#define Info(c, r)    (Cinfo + (c) + ((r) << Ds_shift))                       // адрес данных ячейки холста
#define Cpal(c, r)    (Cds + (c) + ((r) << Ds_shift))                         // адрес данных палитры ячейки холста
#define Offset(c, r)  (Coffset + (c) + ((r) << O_shift))                      // адрес ячейки в которой смещение указывающее на конец данных в буфере строки холста
#define Start(c, r)   (Data(r) + ((c) ? *Offset((c) - 1, r) : 0))             // адрес начала буфера ячейки холста
#define Length(c, r)  ({ ugoc* _t = Offset(c,r); *_t - ((c) ? *(_t-1) : 0); })// длина ячейки холста в байтах
#define End(c, r)     (Data(r) + *Offset(c, r))                               // адрес конца буфера ячейки холста
#define AFon(f)       ((PalBuf*)(Cdfon + ((f) << P_shift)))                   // адрес начала кода фона
#define APal(c)       ((PalBuf*)(Cdpal + ((c) << P_shift)))                   // адрес начала кода цвета
#define AKey(k)       (Cdkey + ((k) << K_shift))                              // адрес начала ячейки в буфере клавиатуры
#define Sys           (*(Sis*)Cdsys)                                          // адрес полное состояние системы при входе и режимы
#define Convas        (*(Canalysis*)Cdcon)                                    // адрес где организована разбивка холста
#define Win(n)        ((Windows*)(Cdwin + ((n) << W_shift)))                  // адрес начала данных окна n
#define Vsw(n, r)     (Cvsw + (r) + ((n) << VCsw_shift))                      // адрес визуальной длины строки r окна n
#define Csw(n, r)     (Ccsw + (r) + ((n) << VCsw_shift))                      // адрес числа ячеек строки r окна n
#define Event(m)      ((Events*)(Cevent + ((m) << V_shift)))                  // адрес начала структуры события
#define Vector(v)     (*(AFunction*)((Cell*)Cexec + (v)))                     // адрес вектора прерывания события
#define Exec(v, func) Vector(v) = (((Cell)(func) < (Cell)Nop) ? Off : (func)) // сброс вектора если адрес функции раньше Nop

typedef struct { goc LkX, LkY, MkX, MkY, RkX, RkY; uint16_t tic; uint8_t pop, push, Mkey, MX, MY, Ctrl, Cod, Count, Data, Key[6], Lk, Mk, Rk, Ru, Rd, cRu, cRd; } B_;
typedef struct { goc X, Y; ugoc Rnd, dXY, Xs, Ys; uint16_t Win, Wec; uint8_t Cod, Mode, Loop, Key, ri, ud, le, up, ssc, scs, bcu, Anchor, Exit; } V_;
typedef struct { Cell addr, size; uint8_t SystemSwitch; } R_;
typedef struct { Cell Delay; uint8_t SwitchRaw; } F_;
typedef struct { char *name; uint8_t id; } KeyIdMap;
typedef struct { ugoc col, row; } T_;
extern char     *Cdata;
extern uint8_t  *Cinfo;
extern uint8_t  *Cds;
extern ugoc     *Coffset;
extern char     *Cdfon;
extern char     *Cdpal;
extern uint8_t  *Cdkey;
extern ugoc     *Cdsys;
extern ugoc     *Cdcon;
extern ugoc     *Cdwin;
extern ugoc     *Cvsw;
extern ugoc     *Ccsw;
extern char     *Cevent;
extern char     *Cexec;
extern char     *Cdbuf;
extern V_ VP;
extern B_ Buf;
extern R_ VRam;
extern uint8_t  cR, cG, cB;
extern uint32_t cRGB;
#define ENGINE_VARS_INIT \
    char      *Cdata      = 0; \
    uint8_t   *Cinfo      = 0; \
    uint8_t   *Cds        = 0; \
    ugoc      *Coffset    = 0; \
    char      *Cdfon      = 0; \
    char      *Cdpal      = 0; \
    uint8_t   *Cdkey      = 0; \
    ugoc      *Cdsys      = 0; \
    ugoc      *Cdcon      = 0; \
    ugoc      *Cdwin      = 0; \
    ugoc      *Cvsw       = 0; \
    ugoc      *Ccsw       = 0; \
    char      *Cevent     = 0; \
    char      *Cexec      = 0; \
    char      *Cdbuf      = 0; \
    uint8_t   cR = 0, cG = 0, cB = 0; \
    uint32_t  cRGB = 0; \
    R_ VRam = {0,0,1}; \
    V_ VP = {0,0,0,0,0,0,0,0,0,0,0,9,K_RIG,K_DOW,K_LEF,K_UP,K_Ctrl_RIG,K_Ctrl_UP,K_Ctrl_LEF,K_Ctrl_DOW,K_ALT_ESC}; \
    B_ Buf = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0,0},0x20,0x21,0x22,0x60,0x61,0x64,0x65};
#define SYS_VARS_INIT \
    static T_ TS = {0}; \
    static F_ Flag = {0,1}; \
    static KeyIdMap NameId[] = { {"[A", K_UP}, {"[B", K_DOW}, {"[C", K_RIG}, {"[D", K_LEF}, \
        {"[1;5A", K_Ctrl_UP}, {"[1;5B", K_Ctrl_DOW}, {"[1;5C", K_Ctrl_RIG}, {"[1;5D", K_Ctrl_LEF}, \
        {"[M", K_Mouse}, {"[1;2P", K_F13}, {"[1;2Q", K_F14}, {"[1;2R", K_F15}, {"[15~", K_F5}, \
        {"[17~", K_F6}, {"[18~", K_F7}, {"[19~", K_F8}, {"[1~", K_HOM}, {"[2~", K_INS}, {"[20~", K_F9}, \
        {"[21~", K_F10}, {"[23~", K_F11}, {"[24~", K_F12}, {"[3~", K_DEL}, {"[4~", K_END}, {"[5~", K_PUP}, \
        {"[6~", K_PDN}, {"[F", K_END}, {"[H", K_HOM}, {"OP", K_F1}, {"OQ", K_F2}, {"OR", K_F3}, {"OS", K_F4}, \
        {"\t", K_ALT_TAB}, {"\r", K_ALT_ENT}, {"\033", K_ALT_ESC} };

Cell StrLen(char *s);                                                 // Длина строки
void MemSet(void* buf, uint8_t val, Cell len);                        // Заполнение куска памяти val
void MemMove(void* dst, void* src, Cell len);                         // Перемещение куска памяти с проверкой наложения
void MemCpy(void* dst, void* src, Cell len);                          // Копирование куска памяти, без проверки наложения!
int8_t MemCmp(void* dst, void* src, Cell len);                        // Сравнение
void UTFinfoTile(uint8_t *s, Cell len);                               // Рассказ об utf8 возвращает Buf.Cod = Data с учётом буфера
void UTFinfo(uint8_t *s);                                             // Рассказ об utf8 возвращает Buf.Cod = Data
void PushKey(void);                                                   // Положить клавишу в буфер Buf.key
uint8_t ShowKey(void);                                                // Показать ожидаемую/получаемую клавишу Buf.key Buf.Data Buf.Count
uint8_t PopKey(void);                                                 // Взять клавишу из буфера {1/0} видна ожидаемая/получаемая Buf.key Buf.Cod = Data; Buf.Count;
ugoc Keys(void);                                                      // Сколько клавиш в буфере
void IRnd(void);                                                      // Инициализация генератора случайных чисел
ugoc Rand(ugoc n);                                                    // Случайное число [0...(n-1)]
int8_t Fsin(int16_t u);                                               // Синус      полный круг 360 градусов [0...511] шаг ~0,7 градуса
int8_t Fcos(int16_t u);                                               // Косинус    для всего диапазона дают [-127...+127]
int8_t Ftg(int16_t u);                                                // Тангенс    бесконечность [-128] (для int8_t дианазон [-128,-127,ноль,127] 256 значений)
int8_t Fctg(int16_t u);                                               // Котангенс  так как 0 и -128 не имеют обратных чисел!
void Print(uint8_t pal, char *str);                                   // Вывод строки в палитре напрямую игнорируя Fresh.
void BPrint(uint8_t border, char *str);                               // Вывод строки с фоном напрямую игнорируя Fresh.
void SetColour(uint8_t c,  uint8_t deep);                             // Установить по индексу c[0...31], cR cG cB - фон и цвет с режимами в палитру
void SwitchPal(void);                                                 // Переключить палитру
void SetPalette(uint8_t set);                                         // Установить палитру {0/1},deep глубина цвета
void SysInit(void);                                                   // Установка переменных среды
void InitVram(Cell addr, Cell size);                                  // Инициализация мира
Cell SystemSwitch(void);                                              // Вход/выход в мир
void MoveConvas(goc dx, goc dy);                                      // Взаимосвязь перемещения по холсту и экранных координат
uint8_t MoveScreen(goc mx, goc my);                                   // Взаимосвязь изменения экранных координат(мышью) и холста
uint8_t ViewPort(void);                                               // Полёт над пространством с возможностью приземления на холст
void RPEncode(void);                                                  // Проситать событие из порта 0 и декодировать UTF8 Buf.Data
void Nop(void);                                                       // Заглушка, пустая функция
void Anchor(void);                                                    // Вход в окно {Выход с окна}
void Bye(void);                                                       // Выход из мира
void WSwitch(void);                                                   // Показать окно {Спрятать окно}
void WASwitch(void);                                                  // Адаптивно показать окно {Спрятать окно}
void WinDown(void);                                                   // Ротация динамических окон
void WinUp(void);                                                     // Ротация динамических окон в обратном направлении
void WinTop(ugoc n);                                                  // Установить окно выше остальных подобных
void _WView(uint16_t n, uint8_t count, goc *args);                    // Привязать окно на холсте(динамическое) либо на экране(статическое), при Off{,Off} не отображать
uint16_t _Window(uint8_t t, int8_t col, uint8_t count, ugoc *args);   // Создание окна с палитрой col при col<0 статичное окно
void _WEvent(uint16_t n, uint8_t cur, uint8_t count, AFunction *args);// Настройка статического окна привязка функций к кодам клавиш
void _WSet(uint16_t n, uint8_t count, uint8_t *args);                 // Настройка окна включение/отключение {Cursor{,Warp}}
void _SEvent(uint8_t count, uint8_t *args);                           // Запомнить вектор системный событий
void _SExec(uint8_t count, AFunction *args);                          // Привязать вектор системных событий к функциям
void _VKeys(uint8_t count, uint8_t *args);                            // Задать клавиши управления вьюпортом в обратном порядке
void _WData(uint16_t n, char *str, uint8_t count, ugoc *args);        // Загрузка данных в окно n согласно шаблону str с позиции курсора окна { ... }
Cell SysWrite(void *buf, Cell len);                                   // Выстрел в терминал
void SwitchRaw(void);                                                 // Включение/выключение неблокирующего ввода RealTime
void GetKey(uint8_t *b);                                              // Читаем utf8 из порта
Cell GetRam(Cell *size);                                              // Взять память
void FreeRam(Cell addr, Cell size);                                   // Вернуть память
void SWD(Cell addr);                                                  // Установить рабочую директорию
ugoc TermCR(ugoc *r);                                                 // Считать рамки терминала
ugoc GetDelay(void);                                                  // Считать колибровачные данные
uint8_t SyncSize(Cell addr);                                          // Обновить рамки терминала при необходимости стабилизировать
Cell GetCycles(void);                                                 // Тики
void Delay(ugoc n);                                                   // Адаптивная задержка, гарантия точности ms
Cell GetSC(Cell addr);                                                // Измерение пропускной способности терминала
#define WinView(n, ...) _WView(n, (uint8_t)((sizeof((goc[]){0, ##__VA_ARGS__}) / sizeof(goc)) - 1), (goc[]){0, ##__VA_ARGS__} + 1)
#define Window(t, col, ...) _Window(t, col, (uint8_t)((sizeof((ugoc[]){0, ##__VA_ARGS__}) / sizeof(ugoc)) - 1), (ugoc[]){0, ##__VA_ARGS__} + 1)
#define WinEvent(n, cur, ...) _WEvent(n, cur, (uint8_t)((sizeof((AFunction[]){0, ##__VA_ARGS__}) / sizeof(AFunction)) - 1), (AFunction[]){0, ##__VA_ARGS__} + 1)
#define WinSet(n, ...) _WSet(n, (uint8_t)((sizeof((uint8_t[]){0, ##__VA_ARGS__}) / sizeof(uint8_t)) - 1), (uint8_t[]){0, ##__VA_ARGS__} + 1)
#define Events(...) _SEvent((uint8_t)((sizeof((uint8_t[]){0, ##__VA_ARGS__}) / sizeof(uint8_t)) - 1), (uint8_t[]){0, ##__VA_ARGS__} + 1)
#define Execs(...) _SExec((uint8_t)((sizeof((AFunction[]){0, ##__VA_ARGS__}) / sizeof(AFunction)) - 1), (AFunction[]){0, ##__VA_ARGS__} + 1)
#define VKeys(...) _VKeys((uint8_t)((sizeof((uint8_t[]){0, ##__VA_ARGS__}) / sizeof(uint8_t)) - 1), (uint8_t[]){0, ##__VA_ARGS__} + 1)
#define WinData(n, str, ...) _WData(n, str, (uint8_t)((sizeof((ugoc[]){0, ##__VA_ARGS__}) / sizeof(ugoc)) - 1), (ugoc[]){0, ##__VA_ARGS__} + 1)
#endif /* SYS_H */
