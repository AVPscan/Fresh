
[![Linux musl static +(Cross-Platform)](https://github.com/AVPscan/Code_editor/actions/workflows/release.yml/badge.svg)](https://github.com/AVPscan/Code_editor/actions/workflows/release.yml)
---

**Sat, May 30, 2026, 08:49 — Intermediate milestones since Fresh was published on github.com.**

The initial goal was to solve the pain of UTF-8 visualization once and for all in a cross-platform way. Therefore, the core design revolves around a canvas based on the following considerations: `[1;4] bytes per character, visual length of [0;2] character cells, and a [-;+] text direction`. This led to an 8192*4 bytes line buffer to natively support 8K resolutions. However, a single line is just the beginning; multiple lines are required. Why not anchor their quantity and line size to the proportions of the Golden Ratio? This concept is baked into the project's DNA—it will never need a rewrite.

Given the architecture detailed below, software powered by Fresh features: no obsolescence, absolute portability, zero GPU dependency, minimal binary footprint, zero lag, and guaranteed RealTime execution...

**Key upgrades fully implemented during development:**

* **Braille-based rendering:** Braille as a UTF-8 segment provides a resolution of `2 dots horizontally and 4 dots vertically per character cell`. This essentially scales the resolution to 16K without changing anything. Thus, a simple `str=col/2` ratio is enough to keep multiple full-fledged screens in memory simultaneously. `For an 8K character cell screen (16K via Braille), it requires just an additional 256 MB`. Implemented directly via the terminal for demonstration purposes, it already provides any color depth for each character cell, 7 streaming attributes, and support for up to 512 windows and layers.

* **Architecture Bitness Abstraction:** Introducing `typedef uintptr_t Cell;` grants complete freedom and automated adaptation regardless of the CPU architecture's bitness.

* **Resolution Scalability (`goc`/`ugoc` types):** Realizing that `uint16_t` becomes a bottleneck when moving to 32K resolutions, I introduced `goc/ugoc` as a new scalable integer type. It provides room for `resolutions up to 2^50. For reference, 2^13 represents 8K (or 16K via Braille), 2^14 represents 16K, and so on`. This ensures total freedom and automatic adaptation to any monitor resolution.

* **Enhanced Input Processing:** Implemented a dedicated keyboard buffer. The mouse, which initially used standard X10 encoding, now supports horizontal scrolling for any mouse model. This addition fully enables real-time interaction.

* **Canvas Partitioning & Window System:** Separating the canvas into buffers/windows (`file.port.buffer...`), each maintaining its own cursor. The canvas distribution is completely automated. Visual presentation is uncoupled from data through the introduction of two rendering variables that pinpoint the window's corner anchor during each inner-loop iteration. This delivers a full-featured windowing system with zero data movement across the canvas.

* **Event-Driven Model:** A robust event model allowing direct function execution simply by binding an action to a specific keystroke or mouse event.

* **Lighting & Automated Palette Generation:** Automatic palette generation for any color depth (`3, 8, 24 bits`). The first is constructed using integer trigonometry, while the second divides the spectrum `(yielding miracles at 8 colors, and smooth grayscale gradients at 16)`. This approach uncovers deep insights into the physics of light itself. Consider this perspective: Darkness is 0, Light is 1 = a quantum. There is no "speed" as such. Mathematically stretching the range to a depth of `2^24` yields the entire color spectrum, yet we must always remember the fundamental binary foundation: it is either `Darkness` or `Light`.

* **The Concept of Infinity:** `Infinity is simply defined as being outside the current range`. Consequently, it is unsigned, just like `0`, and has always been right next to us. Shifting to another range merely reveals a new infinity...

* **Power Grid & Monitor Sync Auto-Adaptation:** Full synchronization with both power grid AC frequencies `[0.1; 1000] Hz` and monitor refresh rates `[50; 1000] Hz`. These parameters can be modified on the fly in runtime (`HotSwap and absolute-precision timer`). This ensures seamless auto-adaptation to any localized power grid standard `(which, paired with UTF-8 support, covers every country on Earth, whether you are underwater, on a ship, on land, on a train, or in an aircraft)` and any monitor refresh rate `(since we decoupled the system from resolution limitations long ago)`.

---

**Сб, 30 мая 2026 08:49 Промежуточные итоги с момента появления Fresh на github.com.**

Изначально целью было создать визуализацию без боли utf8 раз и навсегда причём кроссплатформенно и поэтому основа холст, исходя из следующих соображений `[1;4] байта на символ, визуальная длина [0;2] знакоместа и направление письменности [-;+], так появилось строка 8192*4 байт для поддержки 8к разрешений, но строка это начало, строк нужно несколько и почему бы не завязать на пропорции золотого сечения их количество и размер строки`, заложена ДНК - переписывать не нужно. С учётом нижеизложенного, программы в основе которых лежит Fresh: не устаревают, абсолютно переносимы, не нуждаются в видеокартах, минимальны в размере, не тормозят, всегда RealTime...

**Модернизации в процессе написания полностью реализованные в проекте:**

Браэль как сегмент utf8 даёт понимание `2 точки по горизонтали и 4 по вертикали для знакоместа` по сути 16к не меняя ничего, значит достаточно отношение `str=col/2` для хранения одновременно в памяти нескольких полноценных экранов `для 8к знакомест (16к браэль) берёт дополнительно 265мбайт` с учётом реализации через терминал для демонстрации уже доступны цвет любой глубины для каждого знакоместа и потоковые 7 атрибутов, 512 окон и слоёв.

Введение отвязки от разрядности процессора через `typedef uintptr_t Cell;` полная свобода и автоадаптация со стороны разрядности процессора.

Понимание, `uint16_t` уже начинает создавать проблему при переходе на 32к разрешение `goc/ugoc новый масштабируемый целочисленный тип` даёт `разрешение до 2^50 - причём сейчас 2^13 это 8к {или 16к через Браэль}, 2^14 это 16к......` полная свобода и автоадаптация со стороны разрешений мониторов.

Введение буфера клавиатуры, мышь у нас изначально была просто для X10 добавил горизонтальный скролл любой мыши, дало возможность RealTime.

Разделение холста на буфера - окна `файл.порт.буфер...` в каждом свой курсор, причём полный автомат распределения холста и отвязка от визуала введением двух переменных для рендера указывающих в какой позиции привязан угол окна на данной итерации внутреннего цикла, полноценная оконная система без перемещения данных на холсте.

Событийная модель, возможность вызова функции прямо назначением события на нажатие клавиши или мыши.

Свет и автоматическая генерация палитр любой глубины `3 8 24` первая создаётся через целочисленную тригонометрию, вторая просто разбивкой спектра `чудеса при 8 цветах а при 16 плавные оттенки серого....` даёт много информации на тему света в принципе, как Вам такое. Тьма 0, свет 1 = квант, нет скорости как таковой, пробуем математически растянуть диапазон [0,1] на глубину `2^24` даёт спектр всех цветов в диапазоне, но изначально помним либо `Тьма/Свет`.

`Бесконечность это нахождение вне диапазона` получается она беззнаковая как `0` и она всегда была рядом с нами, переходим в другой диапазон получаем новую бесконечность...

Учтена частота сети электрического тока `[0.1;1000] гц` и частота обновления монитора `[50;1000] гц` изменяемые в RunTime `HotSwap и таймер с абсолютной точностью` автоадаптация к электросети `а учитывая utf8, это все страны мира и не важно [..под водой,корабль,земля,поезд,самолёт..]` и частоте монитора `от разрешения мы давно отвязались`.

---

**Чт, 4 июня 2026 20:33**

Теперь Вам реально доступна реальная возможность увидеть расхождение частоты электоросети, если таймер отстаёт от времени в оси! SysSet(500, 1000, 24, 15, 10) - скорректируйте первое число, в примере это 500 для России `частота сети * 10`, чтоб время не отставало...

Обратите внимание в режиме альтернативной палитры при 16 цветах генерируется чёрно-белый спектр а вот данные таймера всё равно меняют цвет - вышло даже стильно как то, ниразу такого не видел но видимо чёткая математика способна удивлять всегда...

---

Визуализация сравнение размеров исполняемого файла для разных слоёв:
![Forth book](IIAbout/Compare.jpeg)
Реальность такова **1** `Musl Static` 2 `Gcc Linux` 3 `Mac/Windows` - закрывает многие споры разом.

Поясню: 1 самодостаточность 2 минимальный размер но необходимость иметь ось в фоне 3 избыточность особенно `Mac` но там по сути в базе `Unix Way` а значит всё не плохо, а вот `Windows` там в принципе бардак ибо пока используя терминал для демонстрации возможностей идей проекта обнаружил, что при передаче местоположения мыши в программу ограничения наложенные на стандарт со стороны API Windows - по X `0-90`, в частности для иных участников соревнования по X `0-222` - этот момент говорит только об одном, что эта система огромный костыль сама собой. Для ярых защитников этой поделки и знатоков `Mouse X11 SGR` - тоже костыль, полное игнорирование ansi последовательностей `попытка создать на заре интернета html подобный api` не жизнеспособен.

Событийная модель создана, событий [K_NO,  K_Ctrl_A, K_Ctrl_B, ... ,K_ALT_ENT, {K_Mouse...255}], причём событие K_NO - постоянное {на каждой итерации основного цикла}, а остальные происходят по мере поступления с клавиатуры или от мыши {при выборе пункта меню ...}. Интервал {K_Mouse ... 255} событий которые не могут прилететь с клавиатуры можно легко использовать внутри для упрощение реализации чего либо.

По сути получается очень не обычная система настоящего времени в принципе, так как динамические окна не ограничены изначально то появляются интересные возможности связанные с моим изначальным подходом к распределению холста и единственное ограничение которое я обязан реализовать - если создано более одного такого окна то пока динамическое окно наполняется данными, а значит границы буферов строк плавают и создают неопределённость при наполнении следующего подобного окна - будет блокировка пока процесс заливки данных в динамическое окно не будет завершён. Что это даст - самый просто пример загрузить разом несколько файлов имея их всегда под рукой в одном пространстве причём копирование из одного в другой тривиальная задача, думаю смогу решить момент с аккумулированием нулевых событий хотя возникнут коллизии из за общей клавы/мыши и событийного вектора - в общем попробую, а там увидим.

Релизовал математический спектр цвета для актуальных глубин 3 8 24 бита, автоматически разбивает диапазон на 7 частей получая 8 цветов. Всегода однозначно начало 0 - чёрный и конец белый цвет, а вот внутренние шесть теперь можно калибровать мониторы да и цвета при 24 глубине просто шикарные. Про 3 и 8 сами всё увидите, внутри нет костылей, всё чистая математика и даже приближение через массив для гуманитариев не помогает эти костыли вживлённые IBM при создании 256 цветов... Для меня до сих пор загадка почему не передавать тупо 3 байта для True Colour - вместо этого я вынужден его переводить в человеко понятный вид тем самум раздувая поток в 3 раза и замедляя тем самым отклик, но самое смешное видимо там куда это прилетает так же назад декодируется в три байта) Человеки, что вы наделали, хоть самому драйвер пиши, но тогда эта пуля ускорится ещё в 9 раз....

Добавил диалог с AI в папку о сути проекта (для тех кто умеет читать но не понимает код). Теперь, когда достал из структуры всё, осознал ДНК, и сделал удобную обёртку пора писать WinData и Render. 

Неделя потеряна в Департаменте (а по сути руководство "Работа России" в нашем регионе), ну ничего всё равно допишу проект!

Неделя потеряна в ЦЗН Томска, но я вернулся - теперь математика в норме, пришлось реально замедлять перемещения вьюпорта при `+ctrl` а то слишком быстро - теперь прыгаем пространствами учитывая ускорение.

## Вьюпорт мышь клавиатура изменение размеров и масштаба.
`keyboard` `up`,`down`,`left`,`rigth` `mouse` `button mouse`,`roll mouse`,`+shift roll mouse`,`+ctrl roll mouse`

<video src="https://github.com/user-attachments/assets/231a6004-a684-45f1-87d9-8ce4f11437bc" controls width="600"></video>

**Fresh** — это первая **среда исполнения приложений (Runtime Environment)**, полностью независимая от ОС, процессора, видеокарты и внешних библиотек. Это не фреймворк и не библиотека. Это **новой слой реальности** между данными и наблюдателем.

Одно приложение, собранное с Fresh, работает **одинаково** на Linux, macOS, Windows, в Docker, по SSH и на встраиваемых системах. Без доработок. Без рантаймов. Без виртуализации.

**Один статический бинарник (19 КБ)** — и ваш софт работает **везде и навсегда**.

---

## Проблема, которую вы замечали, но не могли сформулировать

Современная разработка требует:
- Выбрать ОС (и страдать от её ограничений)
- Выбрать фреймворк (Qt, GTK, Electron...)
- Тащить за собой мегабайты зависимостей
- Переписывать под каждую платформу
- Молиться, чтобы API не устарел через год

---

## Что внутри 19 КБ?

- **Графика 16K** — Весь мир{UTF8[Браэль-символы]}/Структуры ячеек  = **16K × 16K** точек[Браэль-символы] без видеокарты.

  ⚡ **Хотите больше?**
  Поменяйте константу `13` → `14` получите **32K × 32K** → ...
  (32K монитор с запасом в несколько полных экранов и 512 окнами).

- **Оконная система** — Окна одновременно слои
- **Событийная модель** — 157 Вариативных событий с клавиатуры и мыши, 98 системных событий. Нулевое событие исполняется на каждой итерации.
- **Реальный ввод** — Клава, мышь (X10) горизонтальный скролл, Real Time
- **Навигация** — Вьюпорт, адаптивное ускорение
- **Нулевые зависимости** — Никаких библиотек — только чистый C и системные вызовы

---

## Мышь

Современные мыши продают с «премиальным» горизонтальным скроллом. Fresh даёт его **любой** мыши через `shift+roll`. Без драйверов, без новых устройств.

**Четыре кода — и мышь любого года выпуска получает возможности, которые маркетологи называют «прорывом».**

---

## Производительность как вызов

Fresh не просто быстр — он **обходит ограничения железа**:

- Не нужна видеокарта (графика через шрифт терминала)
- Не нужен X11/Wayland (прямой вывод в терминал для демонстрации)
- Не нужен GPU для рендеринга (CPU + RDTSC + умный кэш)
- **Бинарник 16 КБ** добирает после запуска 265Мбайт для 16к мониторов!

На одном ядре, в текстовой консоли, по SSH — FPS ограничен только скоростью вашего терминала.

---

## Описание в папке проекта **IIAbout**

![Forth book](IIAbout/Forth%20book.jpeg)
Это СПО и один человек без начальства, без грантов, без «экосистем». Только код, который работает.

> «Вам больше не нужно знать GTK, Qt и прочие слои в ОС. А если вы не программировали — прочитайте книгу и сэкономите годы в вузе.»

---

## Лицензия

**GPLv3** — свобода использовать, изменять, распространять.

Но главная свобода — **не зависеть от производителей ОС, процессоров и стран**.

---

*P.S. Да, это всё работает даже по SSH. Да, на Raspberry Pi. Да, на 32-битном процессоре. Да, без видеокарты. Нет, это не магия. Это просто C и 35 лет правильного опыта.*

**Fresh — GitHub:** https://github.com/AVPscan/Fresh

## Сборка/Запуск

- `main.c` — пример использования
- `sys.h` — абстракции: цвет, разрядность, разрешение, холст, окна, события
- `engine.c` — реализация идеи
- `sys_*.c` — минимальные системные прослойки

```bash
make
make run


