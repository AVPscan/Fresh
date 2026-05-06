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
#define HideCur     "\033[?25l"               // Скрыть курсор
#define ShowCur     "\033[?25h"               // Показать курсор
#define WrapOn      "\033[?7h"                // Включить перенос длинных строк
#define WrapOff     "\033[?7l"                // Выключить перенос строк
#define MouseX10on  "\033[?1000h"             // Включаем мышь
#define MouseX10off "\033[?1000l"
#define AltBufOn    "\033[?1049h"
#define AltBufOff   "\033[?1049l"

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
    #define Grey    "\033[38;2;120;120;120m"  // Бледный нейтральный
    #define Green   "\033[38;2;34;139;34m"    // Глубокий лесной
    #define Red     "\033[38;2;220;20;60m"    // Насыщенный малиновый
    #define Blue    "\033[38;2;30;144;255m"   // Яркий небесный
    #define Orange  "\033[38;2;210;105;30m"   // Сочный оранжевый
    #define Gold    "\033[38;2;184;134;11m"   // Светящийся золотой
  #else
    #define Grey    "\033[38;5;244m"
    #define Green   "\033[38;5;28m"
    #define Red     "\033[38;5;160m"
    #define Blue    "\033[38;5;27m"
    #define Orange  "\033[38;5;166m"
    #define Gold    "\033[38;5;178m"
  #endif
#endif

#define CellPow 13                            // Масштаб холста 13 16к, 14 32к, 15 64к.... 
#define MAX_WIN 512                           // Максимально число окон на холсте
#define MaxSpeed (1 << (CellPow - 4))         // Максимальное ускорение курсора
#if CellPow < 15
    typedef uint16_t ugoc;
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
#define UGOC_MAX ((ugoc)~(ugoc)0)
#define GOC_MAX  ((goc)(UGOC_MAX >> 1))
#define GOC_MIN  (~GOC_MAX)
typedef void (*AFunction)(void);
typedef uintptr_t Cell;
#define SCell sizeof(Cell)
extern char     *Cdata;
extern uint8_t  *Cinfo;
extern uint8_t  *Cds;
extern ugoc     *Coffset;
extern ugoc     *Cvsw;
extern ugoc     *Ccsw;
extern ugoc     *Cdwin;
extern ugoc     *Cdcon;
extern char     *Cdpal;
extern uint8_t  *Cdkey;
extern char     *Cdmenu;
extern char     *Cdbuf;
extern char     *Cvector;
enum {
    b0 = 0x01, b1 = 0x02, b2 = 0x04, b3 = 0x08, b4 = 0x10, b5 = 0x20, b6 = 0x40, b7 = 0x80,
    b10 = 0x03, b21 = 0x06, b65 = 0x60, b76 = 0xC0, b765 = 0xE0, Fps = 0x14, On = 0x01, Off = 0x00 };
enum { K_NO, K_Ctrl_A, K_Ctrl_B, K_Ctrl_C, K_Ctrl_D, K_Ctrl_E, K_Ctrl_F, K_Ctrl_G,
    K_DEL, K_TAB, K_LF, K_Ctrl_K, K_Ctrl_L, K_ENT, K_Ctrl_N, K_Ctrl_O,
    K_Ctrl_P, K_Ctrl_Q, K_Ctrl_R, K_Ctrl_S, K_Ctrl_T, K_Ctrl_U, K_Ctrl_V, K_Ctrl_W,
    K_Ctrl_X, K_Ctrl_Y, K_Ctrl_Z, K_ESC, K_FS, K_GS, K_RS, K_US,
    K_BAC = 127, K_Ctrl_LEF, K_Ctrl_UP, K_Ctrl_RIG, K_Ctrl_DOW, K_LEF, K_UP, K_RIG,
    K_DOW, K_HOM, K_END, K_PUP, K_PDN, K_INS, K_F1, K_F2,
    K_F3, K_F4, K_F5, K_F6, K_F7, K_F8, K_F9, K_F10,
    K_F11, K_F12, K_F13, K_F14, K_F15, K_Mouse };
enum {
    Cconvas, Cborder, CconvasB, CborderB, Cgrey, CgreyI, CgreyB, CgreyIB,
    Cgreen, CgreenI, CgreenB, CgreenIB, Cred, CredI, CredB, CredIB,
    Cblue, CblueI, CblueB, CblueIB, Corange, CorangeI, CorangeB, CorangeIB,
    Cgold, CgoldI, CgoldB, CgoldIB, Cdefault, CdefaultI, CdefaultB, CdefaultIB };
typedef struct {
    uint8_t inverse : 1;                      // бит 0      инверсия
    uint8_t bold    : 1;                      // бит 1      толстый
    uint8_t color   : 3;                      // бит 432    цвет
    uint8_t null    : 1;                      // бит 5      резерв
    uint8_t NoFull  : 1;                      // бит 6      {1} есть {0} нет данных
    uint8_t Error   : 1;                      // бит 7      {1} есть {0} нет изменений
} Info;   
typedef struct {                              //UTFinfo Render 
    uint8_t len     : 2;                      // бит 10 32  длина (0-3) + 1, игнорируем так как размер в байтах через offset
    uint8_t vis     : 2;                      // бит 32 10  визуальная ширина (0-2)
    uint8_t Dir     : 1;                      // бит 4      направление (0=LTR,1=RTL)
    uint8_t Ctrl    : 1;                      // бит 5      управляющий код
    uint8_t NoFull  : 1;                      // бит 6  {0} {1} не влезло в буфер (UTF8infoTile)
    uint8_t Error   : 1;                      // бит 7  {0} (UTF8info) {1} Structure
} Data;
typedef struct {
    uint8_t len     : 5;                      // бит 43210  длина = 1+(0-31) ascii {32...127} визуальная длина равна длине в байтах (числа)
    uint8_t format  : 2;                      // бит 65     {10} к левому {01} к правому {00}/{11} по центру
    uint8_t str     : 1;                      // бит 7      {0} UTF8 {1} Structure
} Structure;
typedef struct {
    uint8_t inverse : 1;                      // бит 0      инверсия
    uint8_t bold    : 1;                      // бит 1      толстый
    uint8_t color   : 3;                      // бит 432    цвет
    uint8_t cursor  : 1;                      // бит 5      {1} показывать {0} не показывать - курсор окна
    uint8_t nowrap  : 1;                      // бит 6      {1} включен {0} выключен авто перенос строк окна
    uint8_t sd      : 1;                      // бит 7      {1} статичное (не изменяется в размере на холсте, в байтах) {0} динамичное окно
} WF;
typedef struct {
    uint8_t vision  : 1;                      // бит 0      {1} отображается {0} не отображается
} EF;
typedef struct { goc Xr, Yr; ugoc W, H, MaxCs, MaxVs, MaxH, XCur, YCur, WFirstSR, Xc, Yc; uint16_t Layer, parent, child; uint8_t WF, EF; } WindowData;
typedef struct { uint16_t Win, Min, Max, D, S; } Canalysis;
typedef struct { uint8_t len, data[31]; } PalData;
typedef struct { uint8_t data1, tic1, data2, tic2, utf8[2][2]; } KeysBuff;
typedef struct { uint8_t CMenu, NMenu; uint16_t Win; } Menus;
enum {
    MaxWin = MAX_WIN,                                                         // Максимальное число окон
    SKey = 256,                                                               // Буфер клавиатуры на 255/510 клавиш с автоповторами
    Utf8 = 4,                                                                 // Максимальная длина utf8
    Data_shift = CellPow + 2,                                                 // Смещение между строк холста в байтах
    Offset_shift = CellPow + 1,                                               // Смещение между смещениями строк холста
    Ds_shift = CellPow,                                                       // Смещение между атрибутами строк холста
    VCsw_shift = CellPow - 1,                                                 // Смещение между окон по каждой строке холста
    Palette_shift = 5,                                                        //  32 Смещение для палитр
    Win_shift = 4,                                                            //  16 Смещение для данных окон в словах - 32 байта на окно.
    Key_shift = 3,                                                            //   8 Смещение для ячеек буфера клавиатуры
    Menu_shift = 2,                                                           //   4 Смещение для организации меню
    CellLine = 1 << CellPow,                                                  // Определение ширины холста
    CellStr = CellLine / 2,                                                   // Определение высоты холста
    ConvasArea = CellLine * CellStr,                                          // Площадь холста
    SizeCell = ConvasArea * Utf8,                                             // Размер данных для ячеек холста
    SInfo = ConvasArea,                                                       // Размер атрибутов для ячеек холста
    SDs = ConvasArea,                                                         // Размер информации о ячейках холста
    SOffset = ConvasArea * sizeof(ugoc) / 2,                                  // Размер смещений для ячеек холста
    SVsw = CellStr * MaxWin * sizeof(ugoc) / 2,                               // Размер для визуальных длин строк окон на холсте
    SCsw = CellStr * MaxWin * sizeof(ugoc) / 2,                               // Размер для числа ячеек в строках окон на холсте
    SDataWin = MaxWin * sizeof(WindowData) / 2,                               // Размер данных для окон
    SDataConvas = sizeof(Canalysis) / 2,                                      // Размер данных под разбивку холста для организации окон
    SBufPal = 32 * sizeof(PalData),                                           // Размер данных 32 палитр по 32 байта на каждую
    SBufKey = SKey * sizeof(KeysBuff),                                        // Размер данных кольцевого буфера клавиатуры
    SBufMenu = SKey * sizeof(Menus),                                          // Размер данных для событий (привязка вызова функций к событиям)
    SBuf = 8192,                                                              // Размер буфера Print/File
    SVector = SKey * sizeof(AFunction),                                       // Размер вектора прерываний
    SizeVram = SizeCell + 2 * (SInfo + SOffset + SVsw + SCsw + SDataWin + SDataConvas) + SBufPal + SBufKey + SBufMenu + SBuf + SVector};
#define Data(r)       (Cdata + ((r) << Data_shift))                           // адрес начала буфера строки холста (Data(r)+*Offset(c-1,r) адрес начала буфера для c,r)
#define Attr(c,r)     (Cinfo + (c) + ((r) << Ds_shift))                       // адрес атрибута ячейки холста      (Data(r)+*Offset(c,r) адрес конца буфера для c,r)
#define Ds(c,r)       (Cds + (c) + ((r) << Ds_shift))                         // адрес данных ячейки холста        (*Offset(c,r)-*Offset(c-1,r) длина в байтах для c,r)
#define Offset(c,r)   (Coffset + (c) + ((r) << Offset_shift))                 // адрес ячейки в которой смещение указывающее на конец данных в буфере строки холста
#define Vsw(n,r)      (Cvsw + (r) + ((n) << VCsw_shift))                      // адрес визуальной длины строки r окна n
#define Csw(n,r)      (Ccsw + (r) + ((n) << VCsw_shift))                      // адрес числа ячеек строки r окна n
#define Win(n)        ((WindowData*)(Cdwin + ((n) << Win_shift)))             // адрес начала данных окна n
#define Convas        (*(Canalysis*)Cdcon)                                    // адрес где организована разбивка холста
#define Palette(p)    ((PalData*)(Cdpal + ((p) << Palette_shift)))            // адрес начала кода цвета colBI[0..31]
#define KeyBuf(k)     (Cdkey + ((k) << Key_shift))                            // адрес начала ячейки в буфере клавиатуры
#define Menu(m)       ((Menus*)(Cdmenu + ((m) << Menu_shift)))                // адрес начала структуры события
#define Vector(v)     (*(AFunction*)((Cell*)Cvector + (v)))                   // адрес вектора прерывания события
typedef struct { goc LkX, LkY, MkX, MkY, RkX, RkY; uint16_t tic; uint8_t pop, push, mode, Mkey, MX, MY, key[6], Lk, Mk, Rk, Ru, Rd, cRu, cRd; } B_;
typedef struct { goc X, Y; ugoc  Xs, Ys; uint16_t dXY, Win; uint8_t Tic, Cod, oCod, Mode, Loop, Anchor, Exit, Key, up, ud, le, ri; } V_;
typedef struct { Cell addr, size; uint8_t SystemSwitch; } R_;
typedef struct { Cell Delay_ms; ugoc Rn; uint8_t SwitchRaw, SyncSize; } F_;
typedef struct { char *name; uint8_t id; } KeyIdMap;
typedef struct { ugoc col, row; } T_;
extern V_ VP;
extern B_ Buf;
extern R_ VRam;
#define ENGINE_VARS_INIT \
    char      *Cdata      = 0; \
    uint8_t   *Cinfo      = 0; \
    uint8_t   *Cds        = 0; \
    ugoc      *Coffset    = 0; \
    ugoc      *Cvsw       = 0; \
    ugoc      *Ccsw       = 0; \
    ugoc      *Cdwin      = 0; \
    ugoc      *Cdcon      = 0; \
    char      *Cdpal      = 0; \
    uint8_t   *Cdkey      = 0; \
    char      *Cdmenu     = 0; \
    char      *Cdbuf      = 0; \
    char      *Cvector    = 0; \
    V_ VP = {0,0,0,0,0,0,0,0,0,0,0,0,0,4,K_UP,K_DOW,K_LEF,K_RIG}; \
    B_ Buf = {0,0,0,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0,0},0x20,0x21,0x22,0x60,0x61,0x64,0x65}; \
    R_ VRam = {0,0,1}
#define SYS_VARS_INIT \
    static T_ TS = {0}; \
    static F_ Flag = {0,0,1,0}; \
    static KeyIdMap NameId[] = { {"[A", K_UP}, {"[B", K_DOW}, {"[C", K_RIG}, {"[D", K_LEF}, \
        {"[1;5A", K_Ctrl_UP}, {"[1;5B", K_Ctrl_DOW}, {"[1;5C", K_Ctrl_RIG}, {"[1;5D", K_Ctrl_LEF}, \
        {"[M", K_Mouse}, {"[1;2P", K_F13}, {"[1;2Q", K_F14}, {"[1;2R", K_F15}, {"[15~", K_F5}, \
        {"[17~", K_F6}, {"[18~", K_F7}, {"[19~", K_F8}, {"[1~", K_HOM}, {"[2~", K_INS}, {"[20~", K_F9}, \
        {"[21~", K_F10}, {"[23~", K_F11}, {"[24~", K_F12}, {"[3~", K_DEL}, {"[4~", K_END}, {"[5~", K_PUP}, \
        {"[6~", K_PDN}, {"[F", K_END}, {"[H", K_HOM}, {"OP", K_F1}, {"OQ", K_F2}, {"OR", K_F3}, {"OS", K_F4} }

_Static_assert(SizeCell == CellLine * CellStr * 4, "SizeCell mismatch");
_Static_assert((1 << Data_shift) == CellLine * Utf8, "Data_shift mismatch");
_Static_assert((1 << Offset_shift) == CellLine * sizeof(ugoc), "Offset_shift");
_Static_assert((1 << VCsw_shift) == CellStr * sizeof(ugoc) / 2, "VCsw_shift mismatch");
_Static_assert((1 << Win_shift) == sizeof(WindowData) / 2, "Win_shift mismatch");
_Static_assert((1 << Palette_shift) == sizeof(PalData), "Palette_shift mismatch");
_Static_assert((1 << Key_shift) == sizeof(KeysBuff), "Key_shift mismatch");
_Static_assert((1 << Menu_shift) == sizeof(Menus), "Menu_shift mismatch");

Cell StrLen(char *s);                                                 // Длина строки
void MemSet(void* buf, uint8_t val, Cell len);                        // Заполнение куска памяти val
void MemCpy(void* dst, void* src, Cell len);                          // Копирование куска памяти, без проверки наложения!
int8_t MemCmp(void* dst, void* src, Cell len);                        // Сравнение
void MemMove(void* dst, void* src, Cell len);                         // Перемещение куска памяти с проверкой наложения
uint8_t UTFinfo(uint8_t *s);                                          // Рассказ об utf8 возвращает Data
uint8_t UTFinfoTile(uint8_t *s, Cell len);                            // Рассказ об utf8 возвращает Data с учётом буфера
uint8_t PushKey(uint8_t c, uint8_t *key);                             // Положить клавишу в буфер [код управляющей или печатная 0xFF или 0 ошибка]
uint8_t ShowKey(uint8_t *data, uint8_t *count, uint8_t *key);         // Показать ожидаемую/получаемую клавишу
uint8_t PopKey(uint8_t *data, uint8_t *count, uint8_t *key);          // Взять клавишу из буфера [1] буфер пуст [0] видна ожидаемая/получаемая
ugoc Keys(void);                                                      // Сколько клавиш в буфере
void Print(uint8_t pal, char *str);                                   // Вывод строки в палитре напрямую игнорируя Fresh.
void InitVram(Cell addr, Cell size);                                  // Инициализация мира
Cell SystemSwitch(void);                                              // Вход/выход в мир
uint8_t MoveConvas(ugoc *sx, ugoc *sy, goc *cx, goc *cy, goc dx, goc dy); // Взаимосвязь перемещения по холсту и экранных координат
uint8_t MoveScreen(ugoc *sx, ugoc *sy, goc *cx, goc *cy, goc mx, goc my); // Взаимосвязь изменения экранных координат(мышью) и холста
uint8_t Mouse(uint8_t c, uint8_t key, uint8_t x, uint8_t y);          // Обработка событий мыши с учётом рамок терминала
uint8_t GetEventKM(uint8_t *num, uint8_t *tic, uint8_t *control);     // Читаем мышь и клавиатуру, заполняем буфер при необходимости, проверка управляющих кодов.
void Nop(void);                                                       // Заглушка, пустая функция
void Anchor(void);                                                    // Вход в окно {Выход с окна}
void Bye(void);                                                       // Выход из мира
uint8_t ViewPort(void);                                               // Полёт над пространством с возможностью приземления на холст
void WinTop(ugoc n);                                                  // Установить окно поверх всех (игнорирует теневые)
void Adaptive(void);                                                  // Адаптивно показать окно {Спрятать окно}
void _WinView(uint16_t n, uint8_t count, goc *args);                  // Привязка окна к рендеру
uint16_t _Window(int8_t col, uint8_t count, ugoc *args);              // Определение цвета окна col при col<0 статичное окно
void _WSet(uint16_t n, uint8_t cur, uint8_t count, AFunction *args);  // Управление отображением курсора и авто переносом строк в окне
void _WData(uint16_t n, char *str, uint8_t count, ugoc *args);        // Загрузка данных в окно n согласно шаблону str с позиции курсора окна { ... }
void IRnd(void);                                                      // Инициализация генератора случайных чисел
void SRnd(ugoc n);                                                    // Принудительно задать стартовое значение генератору случайных чисел
ugoc Rand(ugoc n);                                                    // Случайное число [0...(n-1)]
Cell SysWrite(void *buf, Cell len);                                   // Выстрел в терминал
void SwitchRaw(void);                                                 // Включение/выключение неблокирующего ввода RealTime
void GetKey(uint8_t *b);                                              // Читаем utf8 из порта
Cell GetRam(Cell *size);                                              // Взять память
void FreeRam(Cell addr, Cell size);                                   // Вернуть память
void SWD(Cell addr);                                                  // Установить рабочую директорию
ugoc TermCR(ugoc *r);                                                 // Считать рамки терминала
uint8_t SyncSize(Cell addr);                                          // Получить рамки терминала при необходимости стабилизировать
Cell GetCycles(void);                                                 // Тики
void Delay_ms(uint8_t ms);                                            // Адаптивная задержка, гарантия точности ms
Cell GetSC(Cell addr);                                                // Измерение пропускной способности терминала
#define WinView(n, ...) _WinView(n, (uint8_t)((sizeof((goc[]){0, ##__VA_ARGS__}) / sizeof(goc)) - 1), (goc[]){0, ##__VA_ARGS__} + 1)
#define Window(col, ...) _Window(col, (uint8_t)((sizeof((ugoc[]){0, ##__VA_ARGS__}) / sizeof(ugoc)) - 1), (ugoc[]){0, ##__VA_ARGS__} + 1)
#define WinSet(n, cur, ...) _WSet(n, cur, (uint8_t)((sizeof((AFunction[]){0, ##__VA_ARGS__}) / sizeof(AFunction)) - 1), (AFunction[]){0, ##__VA_ARGS__} + 1)
#define WinData(n, str, ...) _WData(n, str, (uint8_t)((sizeof((ugoc[]){0, ##__VA_ARGS__}) / sizeof(ugoc)) - 1), (ugoc[]){0, ##__VA_ARGS__} + 1)
#endif /* SYS_H */
