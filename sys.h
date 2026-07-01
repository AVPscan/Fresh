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

#define CellPow   16   // Масштаб холста [7..60] {по сути создание буфера для данных}
#define Wind      2    // Всего окон на холсте [2..65535] {окна безрамочные по сути спрайты}
#define Dynam     1    // Сколько из них динамических окон [1..Wind]
#define FHow      0    // Частота вызова обработчика таймера [Off{0}..FHz] Гц
#define FHz       500  // Десятикратная частота электросети [0,1..1000] Гц {любая точка пространства}
#define FApm      200  // Частота нажатия на клавишы [50..1000] Гц {установите больше fps монитора и всё поймёте}
#define CFDeep    24   // Глубина цвета [3 8 24] бита {8 256 2^24 максимальное число генерируемых оттенков света}
#define Fcolour   255  // Количество оттенков света на старте [1..254] {0 - чёрный 1 - белый, 2 палитры}

typedef uintptr_t  As; // Разрядность процессора
typedef uint8_t   anu; // 1   anu [0..FF]
typedef int8_t   sanu; // 1  s    [0,+1..+7F,inf,-7F..-1]
typedef uint16_t danu; // 2  d    [0..FFFF]
typedef int16_t dsanu; // 2 ds    [0,+1..+7FFF,inf,-7FFF..-1]
typedef uint32_t   an; // 4   an  [0..FFFFFFFF]
typedef int32_t   san; // 4  s    [0,+1..+7FFFFFFF,inf,-7FFFFFFF..-1]
typedef uint64_t  dan; // 8  d    [0..FFFFFFFFFFFFFFFF]
typedef int64_t  dsan; // 8 ds    [0,+1..+7FFFFFFFFFFFFFFF,inf,-7FFFFFFFFFFFFFFF..-1]
#if CellPow < 17
  typedef an    udgoc; // As      [ас]  основа, бытие             (санскр. as     — существовать)
  typedef san    dgoc; // anu     [ану́] атом, минимальная единица (санскр. anu    — атом)
  typedef danu   ugoc; // an      [ан]  число, количество         (санскр. anka   — цифра)
  typedef dsanu   goc; // goc     [гоч] пространство, место       (санскр. gocara — сфера)
#elif CellPow < 33
  typedef dan   udgoc;
  typedef dsan   dgoc;
  typedef an     ugoc;
  typedef san     goc;
#elif CellPow < 61
  typedef dan   udgoc;
  typedef dsan   dgoc;
  typedef dan    ugoc;
  typedef dsan    goc;
#endif
#define SCell sizeof(As)

enum { Off, On,
  b0 = 0x01, b1, b2 = 0x04, b3 = 0x08, b4 = 0x10, b5 = 0x20, b6 = 0x40, b7 = 0x80, b8 = 0x100,   // Битовые
  b3210 = 0x0F, b10 = 0x03, b21 = 0x06, b65 = 0x60, b76 = 0xC0, b210 = 0x07, b765 = 0xE0,        //  маски
  aD = 0x01, aB, aF = 0x04 , aC = 0x08, aU = 0x10, aI = 0x20, aS = 0x40,                         // Режимы вывода текста
  K_NO = 0, K_Ctrl_A, K_Ctrl_B, K_Ctrl_C, K_Ctrl_D, K_Ctrl_E, K_Ctrl_F, K_Ctrl_G,                // Расширенный набор ascii + все значимые клавиши
  K_DEL, K_TAB, K_LF, K_Ctrl_K, K_Ctrl_L, K_ENT, K_Ctrl_N, K_Ctrl_O,                             //       клавиатуры, K_Mouse [....] Timer диапазон
  K_Ctrl_P, K_Ctrl_Q, K_Ctrl_R, K_Ctrl_S, K_Ctrl_T, K_Ctrl_U, K_Ctrl_V, K_Ctrl_W,                //       векторов пользователя
  K_Ctrl_X, K_Ctrl_Y, K_Ctrl_Z, K_ESC, K_FS, K_GS, K_RS, K_US,                                   // Timer вектор обработчика таймера
  K_BAC = 127, K_Ctrl_LEF, K_Ctrl_UP, K_Ctrl_RIG, K_Ctrl_DOW, K_LEF, K_UP, K_RIG,                // ECD   вектор обработчика декодирование из
  K_DOW, K_HOM, K_END, K_PUP, K_PDN, K_INS, K_F1, K_F2,                                          //       Buf.Key в Buf.Dat
  K_F3, K_F4, K_F5, K_F6, K_F7, K_F8, K_F9, K_F10,                                               // RPE   вектор обработчика чтение в Buf.Key и
  K_F11, K_F12, K_F13, K_F14, K_F15, K_ALT_TAB, K_ALT_ENT, K_Mouse, Timer = 253, ECD, RPE,       //       декодирование из Buf.Key в Buf.Dat
  M_Lkey = 0x20, M_Mkey, M_Rkey, M_Rollup = 0x60, M_Rolldown, M_ShRollup = 0x64, M_ShRolldown }; // Коды мыши которые обрабатываем

typedef struct { anu d[4], u[4]; } KeyBuf;
typedef struct { anu F, Colour; danu Layer, parent, child; ugoc W, H, MaxCs, MaxVs, MaxH, XCur, YCur, WFirstSR, Xc, Yc; dgoc Xr, Yr; } Windows;
typedef struct { anu C, N; danu W; } Events;
typedef void (*AFunction)(void);
typedef struct { anu Res1, Res2; danu D, S, Win; ugoc W, H, CW, CH; } Canalysis;
typedef struct { anu l, d[19]; } PalBuf;

typedef struct { anu *dkey, *data, *ds, *pal; ugoc *offset, *dlwin; anu *dwin, *event, *exec, *dcon, *dpal; char *dbuf, *end;
  As off, addr, size, Save[13]; anu R, G, B, A, X, Y; dsanu C, U, Z, XZ; san Syn, Loop, Dis, XY; an Spal, RGB, XYz; dgoc Xr, Yr; } Var_;
typedef struct { anu Count, Goc, PCell, D, DS, O, V, Attr, CellP, Colours, Deep, Dynamic; danu Win, On, Apm, Hz, FTime, Rnd, Last, AF, Ink, Border,
  Fone, I[8], Su[6], Time[6], Timer[6]; char T[8]; anu Error, Loop; goc Gmin, Gmax, Speed; ugoc Ginf, UGmax, Spd0, Spd1, Mcol, Mstr, W; } Base_;
typedef struct { anu Cod, Mode, Key, ri, ud, le, up, ssc, scs, bcu, Anchor, Exit; danu Wexe; dgoc X, Y, dXY; ugoc Xs, Ys; } ViewPort_;
typedef struct { anu pop, push, Mkey, MX, MY, Ctrl, Cod, Count, Dat, Key[6], Lk, Mk, Rk, Ru, Rd, cRu, cRd; danu tic; dgoc LkX, LkY, MkX, MkY, RkX,
  RkY; } KeyMouse_;
typedef struct { As addr, size; anu SystemSwitch; } MAS_;

typedef struct { ugoc c, r; } CR_;
typedef struct { As s, ns; anu SwitchRaw; } MSnS_;
typedef struct { char *name; anu id; } KeyIdMap;

typedef struct {
  anu len     : 2; // бит 10     длина (0-3) + 1, игнорируем так как размер в байтах через offset
  anu vis     : 2; // бит 32     визуальная ширина (0-2)
  anu Dir     : 1; // бит 4      направление (0=LTR,1=RTL)
  anu Ctrl    : 1; // бит 5      управляющий код
  anu ds      : 1; // бит 6      {0} Data {1} Structure
  anu Refresh : 1; // бит 7      {1/0} есть изменения / нет изменений
} Data;
typedef struct {
  anu len     : 4; // бит 3210  длина = 1+(0-15) ascii {32...126} визуальная длина равна длине в байтах (числа)
  anu right   : 1; // бит 4      {1} к правому
  anu left    : 1; // бит 5      {1} к левому {00}/{11} по центру (как заполнять поле структуры)
  anu ds      : 1; // бит 6      {1} Structure {0} Data
  anu Refresh : 1; // бит 7      {1/0} есть изменения / нет изменений
} Structure;
typedef struct {
  anu col     : 8; // бит x      код цвета {максимум 255 цветовых оттенка 0 чёрный}
} palet;
typedef struct {
  anu sd      : 1; // бит 0      {1} статичное (не изменяется в размере на холсте, в байтах) {0} динамичное окно
  anu vision  : 1; // бит 1      {1} отображается {0} не отображается
  anu cursor  : 1; // бит 2      {1} показывать {0} не показывать - курсор окна
  anu nowrap  : 1; // бит 3      {1} включен {0} выключен авто перенос строк окна
  anu wait    : 1; // бит 4      {1} занято заливаются данные из файла/порта {0} свободно
} Flags;

extern Var_ var;
extern Base_ Base;
extern ViewPort_ VP;
extern KeyMouse_ Buf;
extern MAS_ VRam;

extern MSnS_ Flag;
extern CR_ TS;

#define ENGINE_VARS_INIT \
  Var_ var = {0}; Base_ Base = {0}; ViewPort_ VP = {0}; KeyMouse_ Buf = {0}; MAS_ VRam = {0,0,1}; 

#define Ink(n)        ((n) > 7) ? Base.Last : Base.I[(n)]                         // Код цвета/фона  0 чёрный 1 белый [2..7] оттенки равномерно из
#define Fon(n)        ((n) > 7) ? (Base.AF + Base.Last) : (Base.AF + Base.I[(n)]) //   диапазона [8..] доп ячейка - генерируемый цвет по таймеру

#define AKey(k)       ((KeyBuf*)(var.dkey + ((k) << 3)))                          // адрес начала ячейки в буфере клавиатуры
#define Con(r)        (var.data + ((r) << Base.D))                                // адрес начала буфера строки холста
#define Info(c, r)    (var.ds + (c) + ((r) << Base.DS))                           // адрес данных ячейки холста
#define Cpal(c, r)    (var.pal + (c) + ((r) << Base.DS))                          // адрес данных палитры ячейки холста
#define Offset(c, r)  (var.offset + (c) + ((r) << Base.O))                        // адрес ячейки в которой смещение указывающее на конец данных в буфере
#define WStrVL(n, r)  (var.dlwin + (r) + ((n) * Base.W))                          // Визуальная длина строки окна
#define Win(n)        ((Windows*)(var.dwin + ((n) * sizeof(Windows))))            // адрес начала данных окна n
#define Event(m)      ((Events*)(var.event + ((m) << Base.V)))                    // адрес начала структуры события
#define Vector(a)     (*(AFunction*)((As*)var.exec + (a)))                        // адрес вектора прерывания события
#define Convas        (*(Canalysis*)var.dcon)                                     // адрес где организована разбивка холста
#define APal(c)       ((PalBuf*)(var.dpal + ((((c) << 2) + (c)) << 2)))           // адрес начала кода цвета

#define Exe(v, func)  Vector(v) = (((As)(func) < (As)Nop) ? Off : (func))         // сброс вектора если адрес функции раньше Nop
#define Start(c, r)   (Con(r) + ((c) ? *Offset((c) - 1, r) : 0))                  // адрес начала буфера ячейки холста
#define Length(c, r)  ({ ugoc* _t = Offset(c,r); *_t - ((c) ? *(_t-1) : 0); })    // длина ячейки холста в байтах
#define End(c, r)     (Con(r) + *Offset(c, r))                                    // адрес конца буфера ячейки холста

#define SYS_VARS_INIT \
  KeyIdMap NameId[] = {{"[A",K_UP},{"[B",K_DOW},{"[C",K_RIG},{"[D",K_LEF},{"[1;5A",K_Ctrl_UP},{"[1;5B",K_Ctrl_DOW},{"[1;5C",K_Ctrl_RIG}, \
    {"[1;5D",K_Ctrl_LEF},{"[M", K_Mouse},{"[1;2P",K_F13},{"[1;2Q",K_F14},{"[1;2R",K_F15},{"[15~",K_F5},{"[17~",K_F6},{"[18~",K_F7}, \
    {"[19~",K_F8},{"[1~",K_HOM},{"[2~",K_INS},{"[20~",K_F9},{"[21~",K_F10},{"[23~",K_F11},{"[24~",K_F12},{"[3~",K_DEL},{"[4~",K_END}, \
    {"[5~",K_PUP},{"[6~",K_PDN},{"[F",K_END},{"[H",K_HOM},{"OP",K_F1},{"OQ",K_F2},{"OR",K_F3},{"OS",K_F4}, \
    {"\t",K_ALT_TAB},{"\r",K_ALT_ENT}}; MSnS_ Flag = {0,0,1}; CR_ TS = {0};

As StrLen(char *s);                                          // Длина строки
void MemSet(void* buf, anu val, As len);                     // Заполнение куска памяти val
void MemMove(void* dst, void* src, As len);                  // Перемещение куска памяти с проверкой наложения
void MemCpy(void* dst, void* src, As len);                   // Копирование куска памяти, без проверки наложения!
sanu MemCmp(void* dst, void* src, As len);                   // Сравнение
void UTFinfoTile(anu *s, As len);                            // Рассказ об utf8 возвращает Buf.Cod = Data с учётом буфера
void UTFinfo(anu *s);                                        // Рассказ об utf8 возвращает Buf.Cod = Data
void PushKey(void);                                          // Положить клавишу в буфер Buf.key
anu ShowKey(void);                                           // Показать ожидаемую/получаемую клавишу Buf.key Buf.Dat Buf.Count
anu PopKey(void);                                            // Взять клавишу из буфера ожидаемая/получаемая Buf.key Buf.Cod = Data; Buf.Count;
ugoc Key(void);                                              // Сколько клавиш в буфере
danu D96(danu *a, danu d);                                   // Остаток от деления 96 битного числа
void CSTime(an add);                                         // Сформировать строку времени увеличив при этом таймер на add
void IRnd(void);                                             // Инициализация генератора случайных чисел
dsanu Rand(dsanu n);                                         // Случайное число [0...(n-1)]
sanu Fsin(dsanu u);                                          // Синус      полный круг 360 градусов [0...511] шаг ~0,7 градуса
sanu Fcos(dsanu u);                                          // Косинус    для всего диапазона дают [-127...+127]
sanu Ftg(dsanu u);                                           // Тангенс    бесконечность [-128] (для sanu дианазон [-128,-127,ноль,127])
sanu Fctg(dsanu u);                                          // Котангенс  так как 0 и -128 не имеют обратных чисел!
danu CreateCA(danu c, anu a, char *Buf);                     // Создать с адреса ansi последовательность цвета, атрибутов и вернуть её длину
void Print(danu n, anu m, char *str);                        // Вывод строки в палитре и атрибуте напрямую игнорируя Fresh
void GenFC(danu c, anu deep);                                // Установить по индексу c[0...127], cR cG cB - фон и цвет в палитру Base.Deep
void SetSeparator(char s);                                   // Установить разделитель в формат времени
void SetBorder(anu on, danu b);                              // Установить цвет бордюра и осчистить экран
void SetPalette(anu set);                                    // Установить палитру [0..1]
void SwitchPalette(void);                                    // Переключить палитру
void GenRGB(anu mode, danu c, danu n);                       // Сгенерировать RGB позиции (с) из диапазона до (n) включительно методом (Off/On)
void GenLast(dsanu c);                                       // Сгенерировать цвет и фон, по углу, в позицию last текущей палитры методом sinus
void GenPalettes(void);                                      // Автогенерация оттенков света в палитры двумы методами
void ColourInit(danu c, danu d);                             // Установка переменных цвета Colours Deep
As HowSize(anu c, anu d, danu w, As a);                      // Расчёт общего размера среды
void InitVram(anu c, anu d, danu w, danu h, danu z, danu a); // Инициализация мира CellPower Dynamic Win How Hz Apm
As SystemSwitch(void);                                       // Вход/выход в мир
void MoveNorm(dgoc x, dgoc y);                               // Нормализация перемещения
void MoveConvas(dgoc dx, dgoc dy);                           // Взаимосвязь перемещения по холсту и экранных координат
anu MoveScreen(dgoc mx, dgoc my);                            // Взаимосвязь изменения экранных координат(мышью) и холста
void Free(void);                                             // Одна итерация Fresh
void Encode(void);                                           // Декодировать Buf.Key в Buf.Dat
void RPEncode(void);                                         // Прочитать событие из порта 0 и декодировать Buf.Key в Buf.Dat
void Nop(void);                                              // Заглушка, пустая функция
void Anchor(void);                                           // Вход в окно {Выход с окна}
void SwitchCur(void);                                        // Переключить отображение курсора вьюпорта
void Bye(void);                                              // Выход из мира
void WSwitch(void);                                          // Показать окно {Спрятать окно}
void WASwitch(void);                                         // Адаптивно показать окно {Спрятать окно}
void WDown(void);                                            // Ротация динамических окон
void WUp(void);                                              // Ротация динамических окон в обратном направлении
void WTop(danu n);                                           // Установить окно выше остальных подобных
void _WView(danu n, anu count, dgoc *args);                  // Привязать окно на холсте либо на экране(статическое), при Off{,Off} не отображать
danu _Window(anu mode, anu col, anu c, udgoc *a);            // Создание окна с палитрой col при col<0 статичное окно
void _WExecs(danu n, anu cur, anu c, AFunction *a);          // Настройка статического окна привязка функций к кодам клавиш
void _WSet(danu n, anu c, anu *a);                           // Настройка окна включение/отключение {Cursor{,Warp}}
void _SEvents(anu c, anu *a);                                // Запомнить вектор системный событий
void _SExec(anu c, AFunction *a);                            // Привязать вектор системных событий к функциям
void _SKeys(anu c, anu *a);                                  // Задать клавиши управления вьюпортом в обратном порядке
void _SMouse(anu c, anu *a);                                 // Задать коды управления мышью
void _FSet(anu cp, anu c, danu *a);                          // Изменить CellPower {,Win{,How{,Hz{,Fps}}}}
void _CSet(anu c, danu *a);                                  // Изменить {colours{,deep}}
void _WData(danu n, char *str, anu c, udgoc *a);             // Загрузка данных в окно n согласно шаблону str с позиции курсора окна { ... }
#define WView(n, ...) _WView(n, (anu)((sizeof((dgoc[]){0, ##__VA_ARGS__}) / sizeof(dgoc)) - 1), (dgoc[]){0, ##__VA_ARGS__} + 1)
#define Window(mode, col, ...) _Window(mode, col, (anu)((sizeof((udgoc[]){0, ##__VA_ARGS__}) / sizeof(udgoc)) - 1), (udgoc[]){0, ##__VA_ARGS__} + 1)
#define WDynamic(col, ...) _Window(1, col, (anu)((sizeof((udgoc[]){0, ##__VA_ARGS__}) / sizeof(udgoc)) - 1), (udgoc[]){0, ##__VA_ARGS__} + 1)
#define WStatic(col, ...) _Window(0, col, (anu)((sizeof((udgoc[]){0, ##__VA_ARGS__}) / sizeof(udgoc)) - 1), (udgoc[]){0, ##__VA_ARGS__} + 1)
#define WExec(n, cur, ...) _WExecs(n, cur, (anu)((sizeof((AFunction[]){0, ##__VA_ARGS__}) / sizeof(AFunction)) - 1), (AFunction[]){0, ##__VA_ARGS__} + 1)
#define WSet(n, ...) _WSet(n, (anu)((sizeof((anu[]){0, ##__VA_ARGS__}) / sizeof(anu)) - 1), (anu[]){0, ##__VA_ARGS__} + 1)
#define Even(...) _SEvents((anu)((sizeof((anu[]){0, ##__VA_ARGS__}) / sizeof(anu)) - 1), (anu[]){0, ##__VA_ARGS__} + 1)
#define Exec(...) _SExec((anu)((sizeof((AFunction[]){0, ##__VA_ARGS__}) / sizeof(AFunction)) - 1), (AFunction[]){0, ##__VA_ARGS__} + 1)
#define Keys(...) _SKeys((anu)((sizeof((anu[]){0, ##__VA_ARGS__}) / sizeof(anu)) - 1), (anu[]){0, ##__VA_ARGS__} + 1)
#define Mouse(...) _SMouse((anu)((sizeof((anu[]){0, ##__VA_ARGS__}) / sizeof(anu)) - 1), (anu[]){0, ##__VA_ARGS__} + 1)
#define Fresh(cp, ...) _FSet(cp, (anu)((sizeof((danu[]){0, ##__VA_ARGS__}) / sizeof(danu)) - 1), (danu[]){0, ##__VA_ARGS__} + 1)
#define Colour(...) _CSet((anu)((sizeof((danu[]){0, ##__VA_ARGS__}) / sizeof(danu)) - 1), (danu[]){0, ##__VA_ARGS__} + 1)
#define WData(n, str, ...) _WData(n, str, (anu)((sizeof((udgoc[]){0, ##__VA_ARGS__}) / sizeof(udgoc)) - 1), (udgoc[]){0, ##__VA_ARGS__} + 1)
As SysWrite(void *buf, As len);                              // Выстрел в терминал
void SwitchRaw(void);                                        // Включение/выключение неблокирующего ввода RealTime
void GetKey(anu *b);                                         // Читаем utf8 из порта
goc Real(ugoc fps);                                          // Сколько реально прошло в ожидании
As GetRam(As *size);                                         // Взять память
void FreeRam(As addr, As size);                              // Вернуть память
anu SyncSize(void);                                          // Обновить рамки терминала
void SWD(void);                                              // Установить рабочую директорию
#endif /* SYS_H */
