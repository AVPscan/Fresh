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
// Параметры на момент сборки, можно все изменить в RunTime единственное ограничение пределы для CellPow [7..14}30}62]
#define CellPow   13    // Масштаб холста [7..62] {по сути создание буфера для данных}
#define Wind      1024  // Максимально окон на холсте [2..65535] {окна безрамочные по сути спрайты}
#define FHow      2     // Частота вызова обработчика таймера [Off{0}..FHz] Гц
#define FHz       500   // Десятикратная частота электросети [0,1..1000] Гц {любая точка пространства}
#define FApm      200   // Частота нажатия на клавишы [50..1000] Гц {установите больше fps монитора и всё поймёте}
#define CFDeep    24    // Глубина цвета [3 8 24] бита {8 256 2^24 максимальное число генерируемых оттенков света}
#define Fcolour   126   // Количество оттенков света на старте [1..126] {0 - чёрный 1 - белый, 2 палитры и метода автосоздания}
#if CellPow < 17        // Масштабирование разрешения в зависимости от размера холста(буфер)
  typedef uint32_t udgoc;
  typedef int32_t  dgoc;
  typedef uint16_t ugoc;
  typedef int16_t  goc;
#elif CellPow < 33
  typedef uint64_t udgoc;
  typedef int64_t  dgoc;
  typedef uint32_t ugoc;
  typedef int32_t  goc;
#elif CellPow < 61
  typedef uint64_t udgoc;
  typedef int64_t  dgoc;
  typedef uint64_t ugoc;
  typedef int64_t  goc;
#endif
typedef uintptr_t Cell; // Разрядность процессора
#define SCell sizeof(Cell)

enum {
  b0 = 0x01, b1 = 0x02, b2 = 0x04, b3 = 0x08, b4 = 0x10, b5 = 0x20, b6 = 0x40, b7 = 0x80, b8 = 0x100, // Битовые
  b3210 = 0x0F, b10 = 0x03, b21 = 0x06, b65 = 0x60, b76 = 0xC0, b210 = 0x07, b765 = 0xE0,             //  маски
  aD = 0x01,aB = 0x02, aF = 0x04 , aC = 0x08, aU = 0x10, aI = 0x20, aS = 0x40, On = 0x01, Off = 0x00};// Режимы вывода текста
enum { dark, snow, Maxcol = 126, last, Dark, Snow, Last = 255 };                                      // Константы цвета {0-чёрный, 1-белый} фона {128, 129}
enum {                                                                                                // Расширенный набор ascii + все значимые клавиши
  K_NO, K_Ctrl_A, K_Ctrl_B, K_Ctrl_C, K_Ctrl_D, K_Ctrl_E, K_Ctrl_F, K_Ctrl_G,                         //       клавиатуры, K_Mouse [....] Timer диапазон
  K_DEL, K_TAB, K_LF, K_Ctrl_K, K_Ctrl_L, K_ENT, K_Ctrl_N, K_Ctrl_O,                                  //       векторов пользователя
  K_Ctrl_P, K_Ctrl_Q, K_Ctrl_R, K_Ctrl_S, K_Ctrl_T, K_Ctrl_U, K_Ctrl_V, K_Ctrl_W,                     // Timer вектор обработчика таймера
  K_Ctrl_X, K_Ctrl_Y, K_Ctrl_Z, K_ESC, K_FS, K_GS, K_RS, K_US,                                        // ECD   вектор обработчика декодирование из
  K_BAC = 127, K_Ctrl_LEF, K_Ctrl_UP, K_Ctrl_RIG, K_Ctrl_DOW, K_LEF, K_UP, K_RIG,                     //       Buf.Key в Buf.Dat
  K_DOW, K_HOM, K_END, K_PUP, K_PDN, K_INS, K_F1, K_F2,                                               // RPE   вектор обработчика чтение в Buf.Key и
  K_F3, K_F4, K_F5, K_F6, K_F7, K_F8, K_F9, K_F10,                                                    //       декодирование из Buf.Key в Buf.Dat
  K_F11, K_F12, K_F13, K_F14, K_F15, K_ALT_TAB, K_ALT_ENT, K_Mouse, Timer = 253, ECD, RPE };          

typedef struct { uint8_t *dpal, *dkey, *event, *exec, *dcon; char *dbuf; uint8_t *data, *info, *ds; ugoc *offset;
  uint8_t *dwin; char *end; Cell off, addr, size, Save[12]; uint8_t R, G, B, I, F, A, X, Y; int16_t U, Z;
  int32_t Syn, Loop, Dis; uint32_t RGB, XYz; dgoc Xr, Yr; } Var_;
typedef struct { uint8_t Count, Goc, PCell, CellP, Deep, Colours, D, DS, O, P, K, V, Inc, Fone, Border, Attr;
  uint16_t Win, On, Apm, Hz, FTime, Rnd, Su[6], Time[6], Timer[6]; char Sep, T[9]; goc Gmin, Gmax, Speed;
  ugoc Ginf, UGmax, Spd0, Spd1, Mcol, Mstr; } Base_;

typedef struct { uint8_t l, d[31]; } PalBuf;
typedef struct { uint8_t d[4], u[4]; } KeyBuf;
typedef struct { uint8_t C, N; uint16_t W; } Events;
typedef void (*AFunction)(void);
typedef struct { uint8_t Res1, Res2; uint16_t Min, Max, D, S, Win; ugoc W, H, CW, CH; } Canalysis;
typedef struct { uint8_t col, F; uint16_t Layer, parent, child; ugoc W, H, MaxCs, MaxVs, MaxH, XCur, YCur, WFirstSR,
  Xc, Yc; dgoc Xr, Yr; } Windows;
typedef struct { ugoc c, r; } CR_;
typedef struct { Cell addr, size; uint8_t SystemSwitch; } MAS_;
typedef struct { Cell s, ns; uint8_t SwitchRaw; } MSnS_;
typedef struct { char *name; uint8_t id; } KeyIdMap;
typedef struct { uint8_t pop, push, Mkey, MX, MY, Ctrl, Cod, Count, Dat, Key[6], Lk, Mk, Rk, Ru, Rd, cRu, cRd;
  uint16_t tic; dgoc LkX, LkY, MkX, MkY, RkX, RkY; } KeyMouse_;
typedef struct { uint8_t Res1, Cod, Mode, Loop, Key, ri, ud, le, up, ssc, scs, bcu, Anchor, Exit; uint16_t Win, Wec;
  dgoc X, Y, dXY; ugoc Xs, Ys; } ViewPort_;

typedef struct {                            //UTFinfo  
  uint8_t len     : 2;                      // бит 10     длина (0-3) + 1, игнорируем так как размер в байтах через offset
  uint8_t vis     : 2;                      // бит 32     визуальная ширина (0-2)
  uint8_t Dir     : 1;                      // бит 4      направление (0=LTR,1=RTL)
  uint8_t Ctrl    : 1;                      // бит 5      управляющий код
  uint8_t ds      : 1;                      // бит 6      {0} Data {1} Structure
  uint8_t Refresh : 1;                      // бит 7      {1/0} есть изменения / нет изменений
} Data;
typedef struct {
  uint8_t len     : 4;                      // бит 3210  длина = 1+(0-15) ascii {32...126} визуальная длина равна длине в байтах (числа)
  uint8_t right   : 1;                      // бит 4      {1} к правому
  uint8_t left    : 1;                      // бит 5      {1} к левому {00}/{11} по центру (как заполнять поле структуры)
  uint8_t ds      : 1;                      // бит 6      {1} Structure {0} Data
  uint8_t Refresh : 1;                      // бит 7      {1/0} есть изменения / нет изменений
} Structure;
typedef struct {
  uint8_t col        ;                      // бит x      код цвета {максимум 128 цветовых оттенка} 
  uint8_t colFon  : 1;                      // бит 7      {1/0} есть данные / нет данных
} palet;
typedef struct {
  uint8_t sd      : 1;                      // бит 0      {1} статичное (не изменяется в размере на холсте, в байтах) {0} динамичное окно
  uint8_t vision  : 1;                      // бит 1      {1} отображается {0} не отображается
  uint8_t cursor  : 1;                      // бит 2      {1} показывать {0} не показывать - курсор окна
  uint8_t nowrap  : 1;                      // бит 3      {1} включен {0} выключен авто перенос строк окна
  uint8_t wait    : 1;                      // бит 4      {1} занято заливаются данные из файла/порта {0} свободно
} Flags;

extern MAS_ VRam;
extern ViewPort_ VP;
extern KeyMouse_ Buf;
extern CR_ TS;
extern MSnS_ Flag;
extern Base_ Base;
extern Var_ var;

#define ENGINE_VARS_INIT \
  Var_ var = {0}; \
  Base_ Base = {0,0,0,CellPow,CFDeep,Fcolour,0,0,0,0,0,0,dark,Snow,Dark,0,Wind,FHow,FApm,FHz,0,1, \
    {0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},':',"00000000\0",0,0,0,0,0,0,0,0,0}; \
  ViewPort_ VP = {0,0,0,0,9,K_RIG,K_DOW,K_LEF,K_UP,K_Ctrl_RIG,K_Ctrl_UP,K_Ctrl_LEF,K_Ctrl_DOW,K_F1,0,0,0,0,0,0,0}; \
  KeyMouse_ Buf = {0,0,0,0,0,0,0,0,0,{0,0,0,0,0,0},0x20,0x21,0x22,0x60,0x61,0x64,0x65,0,0,0,0,0,0,0}; \
  MAS_ VRam = {0,0,1}; 
    
#define APal(c)       ((PalBuf*)(var.dpal + ((c) << Base.P)))                 // адрес начала кода цвета
#define AKey(k)       ((KeyBuf*)(var.dkey + ((k) << Base.K)))                 // адрес начала ячейки в буфере клавиатуры
#define Event(m)      ((Events*)(var.event + ((m) << Base.V)))                // адрес начала структуры события
#define Vector(a)     (*(AFunction*)((Cell*)var.exec + (a)))                  // адрес вектора прерывания события
#define Convas        (*(Canalysis*)var.dcon)                                 // адрес где организована разбивка холста
#define Con(r)        (var.data + ((r) << Base.D))                            // адрес начала буфера строки холста
#define Info(c, r)    (var.info + (c) + ((r) << Base.DS))                     // адрес данных ячейки холста
#define Cpal(c, r)    (var.ds + (c) + ((r) << Base.DS))                       // адрес данных палитры ячейки холста
#define Offset(c, r)  (var.offset + (c) + ((r) << Base.O))                    // адрес ячейки в которой смещение указывающее на конец данных в буфере строки
#define Win(n)        ((Windows*)(var.dwin + ((n) * sizeof(Windows))))        // адрес начала данных окна n
#define Exe(v, func)  Vector(v) = (((Cell)(func) < (Cell)Nop) ? Off : (func)) // сброс вектора если адрес функции раньше Nop
#define Start(c, r)   (Data(r) + ((c) ? *Offset((c) - 1, r) : 0))             // адрес начала буфера ячейки холста
#define Length(c, r)  ({ ugoc* _t = Offset(c,r); *_t - ((c) ? *(_t-1) : 0); })// длина ячейки холста в байтах
#define End(c, r)     (Data(r) + *Offset(c, r))                               // адрес конца буфера ячейки холста
    
#define SYS_VARS_INIT \
  KeyIdMap NameId[] = {{"[A",K_UP},{"[B",K_DOW},{"[C",K_RIG},{"[D",K_LEF},{"[1;5A",K_Ctrl_UP},{"[1;5B",K_Ctrl_DOW},{"[1;5C",K_Ctrl_RIG},{"[1;5D",K_Ctrl_LEF}, \
    {"[M", K_Mouse},{"[1;2P",K_F13},{"[1;2Q",K_F14},{"[1;2R",K_F15},{"[15~",K_F5},{"[17~",K_F6},{"[18~",K_F7},{"[19~",K_F8}, \
    {"[1~",K_HOM},{"[2~",K_INS},{"[20~",K_F9},{"[21~",K_F10},{"[23~",K_F11},{"[24~",K_F12},{"[3~",K_DEL},{"[4~",K_END}, \
    {"[5~",K_PUP},{"[6~",K_PDN},{"[F",K_END},{"[H",K_HOM},{"OP",K_F1},{"OQ",K_F2},{"OR",K_F3},{"OS",K_F4}, \
    {"\t",K_ALT_TAB},{"\r",K_ALT_ENT}}; \
  MSnS_ Flag = {0,0,1}; CR_ TS = {0};

Cell StrLen(char *s);                                                 // Длина строки
void MemSet(void* buf, uint8_t val, Cell len);                        // Заполнение куска памяти val
void MemMove(void* dst, void* src, Cell len);                         // Перемещение куска памяти с проверкой наложения
void MemCpy(void* dst, void* src, Cell len);                          // Копирование куска памяти, без проверки наложения!
int8_t MemCmp(void* dst, void* src, Cell len);                        // Сравнение
void UTFinfoTile(uint8_t *s, Cell len);                               // Рассказ об utf8 возвращает Buf.Cod = Data с учётом буфера
void UTFinfo(uint8_t *s);                                             // Рассказ об utf8 возвращает Buf.Cod = Data
void PushKey(void);                                                   // Положить клавишу в буфер Buf.key
uint8_t ShowKey(void);                                                // Показать ожидаемую/получаемую клавишу Buf.key Buf.Dat Buf.Count
uint8_t PopKey(void);                                                 // Взять клавишу из буфера ожидаемая/получаемая Buf.key Buf.Cod = Data; Buf.Count;
ugoc Key(void);                                                       // Сколько клавиш в буфере
void ASu(uint32_t add);                                               // Прибавить к Timer[] число
void CSu(void);                                                       // Создание сумматора равного Дни:Время + Таймер
uint16_t DSu(uint16_t d);                                             // Остаток от деления сумматора на делитель
void Time(void);                                                      // Сформировать строку времени согласно длинне
void IRnd(void);                                                      // Инициализация генератора случайных чисел
int16_t Rand(int16_t n);                                              // Случайное число [0...(n-1)]
int8_t Fsin(int16_t u);                                               // Синус      полный круг 360 градусов [0...511] шаг ~0,7 градуса
int8_t Fcos(int16_t u);                                               // Косинус    для всего диапазона дают [-127...+127]
int8_t Ftg(int16_t u);                                                // Тангенс    бесконечность [-128] (для int8_t дианазон [-128,-127,ноль,127])
int8_t Fctg(int16_t u);                                               // Котангенс  так как 0 и -128 не имеют обратных чисел!
uint8_t CreateCA(uint8_t n, uint8_t m, char *dst);                    // Создать с адреса ansi последовательность цвета, атрибутов и вернуть её длину
void Print(uint8_t n, uint8_t m, char *str);                          // Вывод строки в палитре и атрибуте напрямую игнорируя Fresh
void GenFC(uint8_t c, uint8_t deep);                                  // Установить по индексу c[0...127], cR cG cB - фон и цвет в палитру согласно Base.Deep
void SetSeparator(char s);                                            // Установить разделитель в формат времени
void SetBorder(uint8_t on, uint8_t b);                                // Установить цвет бордюра и осчистить экран
void SetPalette(uint8_t set);                                         // Установить палитру [0..1]
void SwitchPalette(void);                                             // Переключить палитру
void GenRGB(uint8_t mode, uint16_t c, uint16_t n);                    // Сгенерировать RGB позиции (с) из диапазона до (n) включительно методом (Off/On)
void GenLast(int16_t c);                                              // Сгенерировать цвет и фон, по углу, в позицию last текущей палитры методом Off {sin}
void GenPalette(uint8_t set);                                         // Автогенерация оттенков света в палитру
void ColourInit(uint8_t c, uint8_t d);                                // Установка переменных цвета Colours Deep
Cell HowSize(uint8_t c, uint16_t w, Cell addr);                       // Расчёт общего размера среды
Cell InitVram(uint8_t c,uint16_t w,uint16_t o,uint16_t h,uint16_t a); // Инициализация мира CellPower Win How Hz Apm
Cell SystemSwitch(void);                                              // Вход/выход в мир
void MoveNorm(dgoc x, dgoc y);                                        // Нормализация перемещения
void MoveConvas(dgoc dx, dgoc dy);                                    // Взаимосвязь перемещения по холсту и экранных координат
uint8_t MoveScreen(dgoc mx, dgoc my);                                 // Взаимосвязь изменения экранных координат(мышью) и холста
void Free(void);                                                      // Одна итерация Fresh
void Encode(void);                                                    // Декодировать Buf.Key в Buf.Dat
void RPEncode(void);                                                  // Прочитать событие из порта 0 и декодировать Buf.Key в Buf.Dat
void Nop(void);                                                       // Заглушка, пустая функция
void Anchor(void);                                                    // Вход в окно {Выход с окна}
void SwitchCur(void);                                                 // Переключить отображение курсора вьюпорта
void Bye(void);                                                       // Выход из мира
void WSwitch(void);                                                   // Показать окно {Спрятать окно}
void WASwitch(void);                                                  // Адаптивно показать окно {Спрятать окно}
void WDown(void);                                                     // Ротация динамических окон
void WUp(void);                                                       // Ротация динамических окон в обратном направлении
void WTop(uint16_t n);                                                // Установить окно выше остальных подобных
void _WView(uint16_t n, uint8_t count, dgoc *args);                   // Привязать окно на холсте либо на экране(статическое), при Off{,Off} не отображать
uint16_t _Window(uint8_t t, int8_t col, uint8_t c, udgoc *a);         // Создание окна с палитрой col при col<0 статичное окно
void _WExecs(uint16_t n, uint8_t cur, uint8_t c, AFunction *a);       // Настройка статического окна привязка функций к кодам клавиш
void _WSet(uint16_t n, uint8_t c, uint8_t *a);                        // Настройка окна включение/отключение {Cursor{,Warp}}
void _SEvents(uint8_t c, uint8_t *a);                                 // Запомнить вектор системный событий
void _SExec(uint8_t c, AFunction *a);                                 // Привязать вектор системных событий к функциям
void _SKeys(uint8_t c, uint8_t *a);                                   // Задать клавиши управления вьюпортом в обратном порядке
void _FSet(uint8_t cp, uint8_t c, uint16_t *a);                       // Изменить CellPower {,Win{,How{,Hz{,Fps}}}}
void _CSet(uint8_t c, uint8_t *a);                                    // Изменить {colours{,deep}}
void _WData(uint16_t n, char *str, uint8_t c, udgoc *a);              // Загрузка данных в окно n согласно шаблону str с позиции курсора окна { ... }
#define WView(n, ...) _WView(n, (uint8_t)((sizeof((dgoc[]){0, ##__VA_ARGS__}) / sizeof(dgoc)) - 1), (dgoc[]){0, ##__VA_ARGS__} + 1)
#define Window(t, col, ...) _Window(t, col, (uint8_t)((sizeof((udgoc[]){0, ##__VA_ARGS__}) / sizeof(udgoc)) - 1), (udgoc[]){0, ##__VA_ARGS__} + 1)
#define WExec(n, cur, ...) _WExecs(n, cur, (uint8_t)((sizeof((AFunction[]){0, ##__VA_ARGS__}) / sizeof(AFunction)) - 1), (AFunction[]){0, ##__VA_ARGS__} + 1)
#define WSet(n, ...) _WSet(n, (uint8_t)((sizeof((uint8_t[]){0, ##__VA_ARGS__}) / sizeof(uint8_t)) - 1), (uint8_t[]){0, ##__VA_ARGS__} + 1)
#define Even(...) _SEvents((uint8_t)((sizeof((uint8_t[]){0, ##__VA_ARGS__}) / sizeof(uint8_t)) - 1), (uint8_t[]){0, ##__VA_ARGS__} + 1)
#define Exec(...) _SExec((uint8_t)((sizeof((AFunction[]){0, ##__VA_ARGS__}) / sizeof(AFunction)) - 1), (AFunction[]){0, ##__VA_ARGS__} + 1)
#define Keys(...) _SKeys((uint8_t)((sizeof((uint8_t[]){0, ##__VA_ARGS__}) / sizeof(uint8_t)) - 1), (uint8_t[]){0, ##__VA_ARGS__} + 1)
#define Fresh(cp, ...) _FSet(cp, (uint8_t)((sizeof((uint16_t[]){0, ##__VA_ARGS__}) / sizeof(uint16_t)) - 1), (uint16_t[]){0, ##__VA_ARGS__} + 1)
#define Colour(...) _CSet((uint8_t)((sizeof((uint8_t[]){0, ##__VA_ARGS__}) / sizeof(uint8_t)) - 1), (uint8_t[]){0, ##__VA_ARGS__} + 1)
#define WData(n, str, ...) _WData(n, str, (uint8_t)((sizeof((udgoc[]){0, ##__VA_ARGS__}) / sizeof(udgoc)) - 1), (udgoc[]){0, ##__VA_ARGS__} + 1)
Cell SysWrite(void *buf, Cell len);                                   // Выстрел в терминал
void SwitchRaw(void);                                                 // Включение/выключение неблокирующего ввода RealTime
void GetKey(uint8_t *b);                                              // Читаем utf8 из порта
goc Real(ugoc fps);                                                   // Сколько реально прошло в ожидании
Cell GetRam(Cell *size);                                              // Взять память
void FreeRam(Cell addr, Cell size);                                   // Вернуть память
uint8_t SyncSize(void);                                               // Обновить рамки терминала
void SWD(void);                                                       // Установить рабочую директорию
#endif /* SYS_H */
