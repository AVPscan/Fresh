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
    CellPow = 13,                                                   // Константа определяющая буквально всё (взял 13 чтоб перекрыть сегодняшние потребности с запасом
                                                                    //  пример 8192*5062 это при графике Браэля 16384*20248 точек это 16к монитор будет желание не
                                                                    //  переписывая код сделать 32к - возьмите 14 это предел для слова процессора так как у нас utf8, 
                                                                    //  вместо 13) если нужно больше 14 то придётся перейти на 32 битную архитектуру это не сложно.
                                                                    //  В любом случае это самое быстрое, что сегодня есть в мире с такими возможностями:
                                                                    //   настоящее время для клавиатуры и мыши, окна с перекрытиями без копирования данных, холст
                                                                    //   с независимыми окнами и бездна вокруг, свободное перемещение над всем этим с возможностью
                                                                    //   фиксации - входа в окна, автоматически подхватывается тема пользователя, адаптация к изменению
                                                                    //   размера терминала и масштаба шрифта (до свиданья костыли современности), при сборке автоматическая
                                                                    //   адаптация под цветность среды использования, запрашивает у процессора на всё очень мало памяти - 
                                                                    //   просто осознайте 390 метров для поддержки 16к монитора уже сейчас с возможность создания
                                                                    //   полноценного оконного приложения с огромным числом наложенных окон интерфейса, создавайте быстрый
                                                                    //   софт не зависящий от процессора, видеокарты и страны - теперь можно написать один раз и сразу для
                                                                    //   всех, а что именно напишите (игра, ось или программу) зависит от Вас, я ограничения отменил.
    SKey = 256,                                                     // Буфер клавиатуры
    SizePal = 32,                                                   // Размер в байтах одной палитры (len,bytes[len])
    SizeKey = 8,                                                    // Формат ячейки буфера клавиатуры (data1 data2 tic1 tic2 UTF8[2*[1,2]bytes / 1*[3,4]bytes])
    Utf8 = 4,                                                       // Максимальная длина utf8=4
    Bit32 = 2,                                                      // 2^2 32 бита - 4 байта = 2 слова процессора
    Bit16 = 1,                                                      // 2^1 16 бит - 2 байта = 1 слово процессора
    Data_shift = CellPow + Bit32,                                   // 2^(CellPow + 2) Смещение между строк холста
    Offset_row_shift = CellPow + Bit16,                             // 2^(CellPow + 1) Смещение от начала ячейками в строке (2 байта 65536 значений с запасом)
    AVL_shift = CellPow,                                            // 2^(CellPow + 0) Смещение данных ячейки в строке (1 байт [Attr,Vis,Len])
    WinData_shift = Utf8,                                           // 2^4 Смещение для данных 256 окон 16 слов - 32 байта на окно.
    Win_shift = SizeKey,                                            // 2^8 Смещение для визуальных длин строк 256 окон
    Parse_shift = Utf8 + Bit16,                                     // 2^5 Смещение для 32 палитр
    KeyBuf_shift = Utf8 - Bit16,                                    // 2^3 Смещение для 256 ячеек буфера клавиатуры
    CellLine = 1 << CellPow,                                        // 2^(CellPow + 0) Количество ячеек в строке холста (8192)
    CellStr = CellLine * 987 / 1597,                                // Количество строк холста (при 8192 - будет 5062)
    SizeData = CellStr * CellLine * Utf8,                           // Размер данных в ячейках холста
    SizeAttr = CellStr * CellLine,                                  // Размер атрибутов данных в ячейках холста
    SizeVisLen = CellStr * CellLine,                                // Размер визуальной длины данных в ячейках холста
    SizeLen = CellStr * CellLine,                                   // Размер длины данных в ячейках холста
    SizeOffset = CellStr * CellLine * Bit32,                        // Размер смещений для данных в ячейках холста
    SizeWinData = SKey + SizePal,                                   // Размер данных 256 окон
    SizeVlsWin = CellStr * SKey * Utf8,                             // Размер данных визуальных длин строк и реальных длин строк 256 окон
/*
4 X Y   вывод (0 0 привязка окна к холсту иначе >0 привязка к левому верхнему углу окна терминала 0< привязка к правому нижнему углу окна терминала)
4 c r   размер ширина и высота (0 безразмерна иначе визуальная ширина окна r<0 теневое окно, выделяется снизу вверх)
4 Xc Yc смещение от начала окна, организация прокрутки вертикальной и горизонтальной в любом направлении
4 Xk Yk координаты курсора внутри визуального окна
4 Xo Yo координаты на холсте левого верхнего угла окна
4 Xl Yl координаты на холсте правого нижнего угла окна
4 cvmax clmax максимальная визуальная ширина строки окна и максимальная ширина строки окна в байтах
1 k	0 корень - окно родитель, иначе номер окна родителя
1 n     окно ребёнок - продолжение авто создание при заполнения родителя, при возможности создать подобное.
1 color окна 3 бита 2 атрибута
1 data	3 бита: 7 - запрет авто переноса строк; 6 - включить статику окна(теневое); 5 - отключить отображение курсора окна.
Итог 32байта n*32=n*2^5! */
    SizePalBuff = SizePal * SizePal,                                // Размер данных 32 палитр (8 цветов 2 атрибута [32 байта])
    SizeKeyBuf = SKey * SizeKey,                                    // Размер данных кольцевого буфера клавиатуры на 255/510 значений
    SizeVBuff = CellLine * CellLine / 2,                            // Размер видео буфера для формирования картинки в рамках терминала
    SizeVram = SizeData + SizeAttr + SizeVisLen + SizeLen + SizeOffset + SizeWinData + SizeVlsWin + SizePalBuff + SizeKeyBuf + SizeVBuff };
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
    Cgold, CgoldI, CgoldB, CgoldBI, Cborder, Cconvas, LastAttr };   // [data] = 7 не UTF8 6 не влезает в буфер 5 управляющий код 4 направление письма
                                                                    // 32 визуальная длина 0-2[3 управляющий] 10 длина 0-3 [1-4].
Cell StrLen(char *s);                                               // Длина строки
void MemSet(void* buf, uint8_t val, Cell len);                      // Заполнение куска памяти val
void MemCpy(void* dst, void* src, Cell len);                        // Копирование куска памяти, без проверки наложения!
int8_t MemCmp(void* dst, void* src, Cell len);                      // Сравнение
void MemMove(void* dst, void* src, Cell len);                       // Перемещение куска памяти с проверкой наложения
uint8_t UTFinfo(char *s);                                           // Рассказ об utf8 возвращает [data]
uint8_t UTFinfoTile(char *s, Cell len);                             // Рассказ об utf8 возвращает [data] с учётом буфера
void Print(uint8_t n, char *str);                                   // Вывод строки в цвете палитры напрямую в терминал минуя Vram.
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
void Show(void);                                                    // Для тестирования библиотеки
void Window(uint8_t n, uint8_t col, int16_t c, int16_t r);          // Определение окна n, цветом col, визуальной шириной c, высотой r (r<0 теневое окно)
void WSet(uint8_t n, int16_t c, int16_t r);                         // Привязка к рендеру ([1,1] левый верхний -> [-1,-1] от нижнего правого <- [0,0] холст)
void _WConst(uint8_t n, char *str, uint8_t count, int16_t *args);   // Данные формата str, с позиции курсора в окно n(цифры в формате - ширина резиновой структуры)
void _WData(uint8_t n, char *str, uint8_t count, int16_t *args);    // Загрузка данных в окно n согласно шаблону str с позиции курсора окна.
#define WConst(n, str, ...) _WConst(n, str, (uint8_t)((sizeof((int16_t[]){0, ##__VA_ARGS__}) / 2) - 1), (int16_t[]){0, ##__VA_ARGS__} + 1)
#define WData(n, str, ...) _WData(n, str, (uint8_t)((sizeof((int16_t[]){0, ##__VA_ARGS__}) / 2) - 1), (int16_t[]){0, ##__VA_ARGS__} + 1)
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
#endif /* SYS_H */
