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

#define Reset       "\033[0m"                                         // СБРОСИТЬ ВСЁ (и цвета, и режимы)
#define Cls         "\033[2J\033[H"                                   // Очистить экран и в начало
#define Home        "\033[H"                                          // В начало экрана
#define HideCur     "\033[?25l"                                       // Скрыть курсор
#define ShowCur     "\033[?25h"                                       // Показать курсор
#define WrapOn      "\033[?7h"                                        // Включить перенос длинных строк
#define WrapOff     "\033[?7l"                                        // Выключить перенос строк
#define MouseX10on  "\033[?1000h"                                     // Включаем мышь
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
    #define Grey    "\033[38;2;120;120;120m"                          // Бледный нейтральный
    #define Green   "\033[38;2;34;139;34m"                            // Глубокий лесной
    #define Red     "\033[38;2;220;20;60m"                            // Насыщенный малиновый
    #define Blue    "\033[38;2;30;144;255m"                           // Яркий небесный
    #define Orange  "\033[38;2;210;105;30m"                           // Сочный оранжевый
    #define Gold    "\033[38;2;184;134;11m"                           // Светящийся золотой
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
    CellPow = 13,                                                     /* Константа определяющая буквально всё (взял 13 чтоб перекрыть сегодняшние потребности с запасом пример 8192*4096 это при графике Браэля 16384*16386 точек это 16к монитор будет желание не переписывая код сделать 32к - возьмите 14 это предел для слова процессора так как у нас utf8, вместо 13) если нужно больше 14 то придётся перейти на 32 битную архитектуру это не сложно. В любом случае это самое быстрое, что сегодня есть в мире с такими возможностями: настоящее время для клавиатуры и мыши, окна с перекрытиями без копирования данных, холст с независимыми окнами и бездна вокруг, свободное перемещение над всем этим с возможностью фиксации - входа в окна, автоматически подхватывается тема пользователя, адаптация к изменению размера терминала и масштаба шрифта (до свиданья костыли современности), при сборке автоматическая адаптация под цветность среды использования, запрашивает у процессора на всё очень мало памяти - просто осознайте 300 метров для поддержки 16к монитора уже сейчас с возможность создания полноценного оконного приложения с огромным числом наложенных окон интерфейса, создавайте быстрый софт не зависящий от процессора, видеокарты и страны - теперь можно написать один раз и сразу для всех, а что именно напишите (игра, ось или программу) зависит от Вас, ограничения отменил. */
    MaxWin = 512,                                                     // Максимальное число окон
    SKey = 256,                                                       // Буфер клавиатуры
    SizePal = 32,                                                     // Размер в байтах одной палитры (len,bytes[len])
    SizeKey = 8,                                                      // Формат ячейки буфера клавиатуры (data1 data2 tic1 tic2 UTF8[2*[1,2]bytes / 1*[3,4]bytes])
    Utf8 = 4,                                                         // Максимальная длина utf8=4
    Data_shift = CellPow + 2,                                         // 2^(CellPow + 2) Смещение между строк холста
    ADOС_shift = CellPow,                                             // 
    WinData_shift = 4,                                                // 2^4 Смещение для данных окон 16 слов - 32 байта на окно.
    Win_shift = SizeKey,                                              // 2^8 Смещение для визуальных длин строк окон
    Parse_shift = WinData_shift + 1,                                  // 2^5 Смещение для 32 палитр
    KeyBuf_shift = WinData_shift - 1,                                 // 2^3 Смещение для ячеек буфера клавиатуры
    CellLine = 1 << CellPow,                                          // 2^(CellPow + 0) Количество ячеек в строке холста (8192)
    CellStr = CellLine / 2,                                           // Количество строк холста (при 8192 - будет 4096)
    SizeDCell = CellStr * CellLine * Utf8,                            // Размер данных в ячейках холста
    SizeADCell = SizeDCell,                                           // Размер атрибутов, данных, смещения для ячейках холста
    SizeWinData = MaxWin * SizePal,                                   // Размер данных для окон
/* ADOCell
1 info   7 есть изменение, 6 есть данные, 5 есть структура, 432 colour, 10 BI.
1 data   5{0} [data] 5{1} 7 с начала поля, 6 с конца поля, (76 {00 || 11} по центу) 5-0 длина поля ascii [1-64]
2 offset смещение от начала строки холста до начала ячейки на холсте в байтах.
WinData
4 X Y    вывод (0 0 не отображается, если (>0) привязка левым верхним углом окна относительно холста иначе (0<) привязка по модулю к вьюпорту)
4 c lc   максимальное число ячеек и визуальная длина строки в окне
4 c r    размер ширина и высота (0 безразмерна иначе визуальная ширина окна если r<0 то окно интерфейса, элементы иерархического меню (теневое окно))
2 Data   8 теневое, 7 запрет авто переноса строк, 6 включить статику окна, 5 отключить отображение курсора окна, 432 colour, 10 BI
4 Xk Yk  реальные координаты курсора внутри окна, всегда положительные так как это не вьюпорт
4 Xo Yo  координаты на холсте левого верхнего угла окна */    
    SizeVlsWin = CellStr * MaxWin * 4,                                // Размер визуальных и реальных длин строк окон - обновляются при наполнении
    SizePalBuff = SizePal * SizePal,                                  // Размер данных 8*4 [32] палитр по 32 байта на каждую
    SizeRenderWin = MaxWin * 2,                                       // Размер данных окон для рендера 
    SizeKeyBuf = SKey * SizeKey,                                      // Размер данных кольцевого буфера клавиатуры на 255/510 ячеек
    SizeBuff = 1024,                                                  // Размер буфера
    SizeVram = SizeDCell + SizeADCell + SizeWinData + SizeVlsWin + SizePalBuff + SizeRenderWin + SizeKeyBuf + SizeBuff };
enum {
    Minv = 0x01, Mbol = 0x02, Mcol = 0x1C, Mcbi = 0x1F, Msmem = 0x20,
    Mdata = 0x40, Fresh = 0x80, Free = 0x14, On = 1, Off = 0 };
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
    Cconvas, Cborder, WinsData, WinsExt , Cgrey, CgreyI, CgreyB, CgreyBI,
    Cgreen, CgreenI, CgreenB, CgreenBI, Cred, CredI, CredB, CredBI,
    Cblue, CblueI, CblueB, CblueBI, Corange, CorangeI, CorangeB, CorangeBI,
    Cgold, CgoldI, CgoldB, CgoldBI, Cdefault, CdefaultI, CdefaultB, CdefaultBI };
extern char      *Cdata;
extern uint16_t  *Cattr;
extern uint16_t  *Cwin;
extern uint16_t  *Cvlswin;
extern char      *Cpdat;
extern uint16_t  *Cdwin;
extern uint16_t  *Cdren;
extern char      *Ckbuf;
extern char      *Cvdat;
typedef struct { uint8_t info, data; uint16_t offset; } ADOCell;
typedef struct { int16_t Xrender, Yrender, Xview, Yview; uint16_t Flags, parent, child,
                 MaxCS, MaxVS, XCur, YCur, W, H, Xconvas, Yconvas; } WindowData;
typedef struct { uint16_t No, N, Windows, Shadow, W, H, Xwindow, Ywindow, Xshadow, Yshadow; } Canalysis;
typedef struct { uint16_t Win[MaxWin]; } Render;
typedef struct { uint16_t MaxCS, MaxVS; } WConSrt;
typedef struct { int16_t X, Y, viewX, viewY; uint8_t Mode, dXY, Tic, Cod, oCod, Key, up, ud, le, ri, cup, cdo, cle, cri, F2, F3, F4, es; } V_;
typedef struct { uint16_t tic; int16_t LkX, LkY, MkX, MkY, RkX, RkY; char key[6]; uint8_t pop, push, mode, Mkey, MX, MY, Lk, Mk, Rk, Ru, Rd, cRu, cRd; } B_;
typedef struct { Cell addr, size; uint8_t SystemSwitch; } R_;
typedef struct { const char *name; unsigned char id; } KeyIdMap;
typedef struct { uint8_t SwitchRaw, SyncSize; Cell Delay_ms; } F_;
typedef struct { uint16_t col , row; } T_;
#define Data(r)       (Cdata + ((r) << Data_shift))                           // адрес начала буфера строки холста
#define IDO(r, c)     ((ADOCell*)(Cattr + (((r) << ADOC_shift) + (c)) << 1))  // адрес атрибута, данных и смещения ячейки холста
#define Win(n)        ((WindowData*)(Cwin + ((n) << WinData_shift)))          // адрес начала данных окна n
#define Wbv(r,n)      ((WConSrt*)(Cvlswin + (((r) << Win_shift) + (n)) << 1)) // адрес числа ячеек и визуальной длины строки окна n принадлежащих строке холста r
#define Parse(cbi)    (Cpdat + ((cbi) << Parse_shift))                        // адрес начала anci кода цвета cbi[0..31] - [8][4]
#define Convas        (*(Canalysis*)Cdwin)                                    // адрес где организована разбивка холста
#define Render(n)     *(Cdren + (n))                                          // адрес данных слоя для окна
#define KeyBuf(n)     (Ckbuf + ((n) << KeyBuf_shift))                         // адрес начала клавиши в буфере n[0..255]

extern V_ VP;
extern B_ Buf;
extern R_ VRam;
#define ENGINE_VARS_INIT \
    char      *Cdata      = 0; \
    uint16_t  *Cattr      = 0; \
    uint16_t  *Cwin       = 0; \
    uint16_t  *Cvlswin    = 0; \
    char      *Cpdat      = 0; \
    uint16_t  *Cdwin      = 0; \
    uint16_t  *Cdren      = 0; \
    char      *Ckbuf      = 0; \
    char      *Cvdat      = 0; \
    V_ VP = {1,1,0,0,0,1,0,0,0,12,K_UP,K_DOW,K_LEF,K_RIG,K_Ctrl_UP,K_Ctrl_DOW,K_Ctrl_LEF,K_Ctrl_RIG,K_F2,K_F3,K_F4,K_ESC}; \
    B_ Buf = {0,0,0,0,0,0,0,{0,0,0,0,0,0},0,0,0,0,0,0,0x20,0x21,0x22,0x60,0x61,0x64,0x65}; \
    R_ VRam = {0,0,1}
#define SYS_VARS_INIT \
    static T_ TS = {0}; \
    static F_ Flag = {1,0,0}; \
    static KeyIdMap NameId[] = { {"[A", K_UP}, {"[B", K_DOW}, {"[C", K_RIG}, {"[D", K_LEF}, \
        {"[1;5A", K_Ctrl_UP}, {"[1;5B", K_Ctrl_DOW}, {"[1;5C", K_Ctrl_RIG}, {"[1;5D", K_Ctrl_LEF}, \
        {"[M", K_Mouse}, {"[1;2P", K_F13}, {"[1;2Q", K_F14}, {"[1;2R", K_F15}, {"[15~", K_F5}, \
        {"[17~", K_F6}, {"[18~", K_F7}, {"[19~", K_F8}, {"[1~", K_HOM}, {"[2~", K_INS}, {"[20~", K_F9}, \
        {"[21~", K_F10}, {"[23~", K_F11}, {"[24~", K_F12}, {"[3~", K_DEL}, {"[4~", K_END}, {"[5~", K_PUP}, \
        {"[6~", K_PDN}, {"[F", K_END}, {"[H", K_HOM}, {"OP", K_F1}, {"OQ", K_F2}, {"OR", K_F3}, {"OS", K_F4} }
// [data] = 7 битый, 6 не влезает в буфер, 5 управляющий код, 4 направление письма, 32 визуальная длина [0-2], 10 длина [0-3]+1
Cell StrLen(char *s);                                                 // Длина строки
void MemSet(void* buf, uint8_t val, Cell len);                        // Заполнение куска памяти val
void MemCpy(void* dst, void* src, Cell len);                          // Копирование куска памяти, без проверки наложения!
int8_t MemCmp(void* dst, void* src, Cell len);                        // Сравнение
void MemMove(void* dst, void* src, Cell len);                         // Перемещение куска памяти с проверкой наложения
uint8_t UTFinfo(char *s);                                             // Рассказ об utf8 возвращает [data]
uint8_t UTFinfoTile(char *s, Cell len);                               // Рассказ об utf8 возвращает [data] с учётом буфера
void Print(uint8_t n, char *str);                                     // Вывод строки в цвете палитры напрямую в терминал минуя Vram.
void InitVram(Cell addr, Cell size);                                  // Инициализация мира
Cell SystemSwitch(void);                                              // Вход/выход в мир
uint8_t PushKey(char *key);                                           // Положить клавишу в буфер [код управляющей или печатная 0xFF или 0 ошибка]
uint8_t ShowKey(uint8_t *data, uint8_t *count, char *key);            // Показать ожидаемую/получаемую клавишу
uint8_t PopKey(uint8_t *data, uint8_t *count, char *key);             // Взять клавишу из буфера [1] буфер пуст [0] видна ожидаемая/получаемая
void ForgetKey(void);                                                 // Забыть последнюю пришедшую клавишу в буфере даже ожидаемую/получаемую
uint16_t Keys(void);                                                  // Сколько клавиш в буфере
uint8_t Mouse(uint8_t key, uint8_t x, uint8_t y);                     // Обработка событий мыши с учётом рамок терминала
uint8_t GetEventKM(uint8_t *num, uint8_t *tic, uint8_t *control);     // Читаем мышь и клавиатуру, заполняем буфер при необходимости, проверка управляющих кодов.
uint8_t ViewPort(void);                                               // Полёт над пространством с возможностью приземления на холст
void WSet(uint16_t n, int16_t y, int16_t x);                          // Привязка окна к рендеру
void WTop(uint16_t n);                                                // Установить окно поверх всех кроме окон интерфейса
uint16_t _Window(int8_t col, uint8_t count, int16_t *args);           // Определение окна цветом col (col<0 теневое окно){ высотой r { визуальной шириной c } ... }
void _WData(uint16_t n, char *str, uint8_t count, int16_t *args);     // Загрузка данных в окно n согласно шаблону str с позиции курсора окна { ... }
#define Window(col, ...) _Window(col, (uint8_t)((sizeof((int16_t[]){0, ##__VA_ARGS__}) / 2) - 1), (int16_t[]){0, ##__VA_ARGS__} + 1)
#define WData(n, str, ...) _WData(n, str, (uint8_t)((sizeof((int16_t[]){0, ##__VA_ARGS__}) / 2) - 1), (int16_t[]){0, ##__VA_ARGS__} + 1)
Cell SysWrite(void *buf, Cell len);                                   // Выстрел в терминал
void SwitchRaw(void);                                                 // Включение/выключение неблокирующего ввода RealTime
void GetKey(char *b);                                                 // Читаем utf8 из порта
Cell GetRam(Cell *size);                                              // Взять память
void FreeRam(Cell addr, Cell size);                                   // Вернуть память
void SWD(Cell addr);                                                  // Установить рабочую директорию
uint16_t TermCR(uint16_t *r);                                         // Считать рамки терминала
int16_t SyncSize(Cell addr);                                          // Получить рамки терминала при необходимости стабилизировать
Cell GetCycles(void);                                                 // Тики
void Delay_ms(uint8_t ms);                                            // Адаптивная задержка, гарантия точности ms
Cell GetSC(Cell addr);                                                // Измерение пропускной способности терминала
#endif /* SYS_H */
