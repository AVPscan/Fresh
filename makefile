#
# Fresh (C) 2026 A.Pozdnyakov GPLv3 - see LICENSE
# E-mail: avp70ru@mail.ru
# 
# Данная программа является свободным программным обеспечением: вы можете 
# распространять ее и/или изменять согласно условиям Стандартной общественной 
# лицензии GNU (GPLv3).
#

TARGET = fresh

# Определение компилятора по умолчанию
CC = gcc

# Для сборки clang: make c
# Для сборки gcc: make g (или просто make)

UNAME_S := $(shell uname -s)

# Определение платформы и системного файла
ifeq ($(OS),Windows_NT)
	SYS_SRC = sys_windows.c
	EXT = .exe
	LIBS = -lkernel32 -luser32
	GET_SIZE = wc -c < $(TARGET)$(EXT)
else ifeq ($(UNAME_S),Darwin)
	SYS_SRC = sys_macos.c
	EXT =
	LIBS =
	GET_SIZE = stat -f %z $(TARGET)$(EXT)
else ifeq ($(UNAME_S),FreeBSD)
	SYS_SRC = sys_bsd.c
	EXT =
	LIBS =
	GET_SIZE = stat -f %z $(TARGET)$(EXT)
else ifeq ($(UNAME_S),OpenBSD)
	SYS_SRC = sys_bsd.c
	EXT =
	LIBS =
	GET_SIZE = stat -f %z $(TARGET)$(EXT)
else ifeq ($(UNAME_S),NetBSD)
	SYS_SRC = sys_bsd.c
	EXT =
	LIBS =
	GET_SIZE = stat -f %z $(TARGET)$(EXT)
else
	SYS_SRC = sys_linux.c
	EXT =
	LIBS =
	GET_SIZE = stat -c%s $(TARGET)$(EXT)
endif

SOURCES = main.c engine.c $(SYS_SRC)

# Базовые флаги - стандартные, без агрессивной оптимизации
BASE_CFLAGS = -std=c11 -DNDEBUG -Wall -Wextra -flto -fno-strict-aliasing -fwrapv
BASE_LDFLAGS = $(LIBS)

# Дополнительные флаги для Linux
ifneq ($(OS),Windows_NT)
	ifeq ($(UNAME_S),Linux)
		BASE_CFLAGS += -D_POSIX_C_SOURCE=200809L
	endif
endif

# Флаги для GCC
GCC_CFLAGS = -Os
GCC_LDFLAGS =

# Флаги для Clang
CLANG_CFLAGS = -Oz
CLANG_LDFLAGS =

# Общие флаги
COMMON_CFLAGS = $(BASE_CFLAGS)
COMMON_LDFLAGS = $(BASE_LDFLAGS)

# Правила сборки с разными компиляторами
.PHONY: all g c musl clean run size debug

all: g

g: CC = gcc
g: CFLAGS = $(GCC_CFLAGS) $(COMMON_CFLAGS)
g: LDFLAGS = $(GCC_LDFLAGS) $(COMMON_LDFLAGS)
g: $(SOURCES)
	@echo "🔨 Сборка GCC: $(SYS_SRC) -> $(TARGET)$(EXT)"
	$(CC) $(CFLAGS) -o $(TARGET)$(EXT) $(SOURCES) $(LDFLAGS)
	@$(MAKE) --no-print-directory size

c: CC = clang
c: CFLAGS = $(CLANG_CFLAGS) $(COMMON_CFLAGS)
c: LDFLAGS = $(CLANG_LDFLAGS) $(COMMON_LDFLAGS)
c: $(SOURCES)
	@echo "🔨 Сборка Clang: $(SYS_SRC) -> $(TARGET)$(EXT)"
	$(CC) $(CFLAGS) -o $(TARGET)$(EXT) $(SOURCES) $(LDFLAGS)
	@$(MAKE) --no-print-directory size

mac: CC = clang
mac: CFLAGS = $(CLANG_CFLAGS) $(COMMON_CFLAGS)
mac: LDFLAGS = $(CLANG_LDFLAGS) $(COMMON_LDFLAGS)
mac: $(SOURCES)
	@echo "🔨 Сборка для macOS: $(SYS_SRC) -> $(TARGET)$(EXT)"
	$(CC) $(CFLAGS) -o $(TARGET)$(EXT) $(SOURCES) $(LDFLAGS)
	@$(MAKE) --no-print-directory size

musl: CC = musl-gcc
musl: CFLAGS = $(GCC_CFLAGS) $(COMMON_CFLAGS)
musl: LDFLAGS = $(GCC_LDFLAGS) $(COMMON_LDFLAGS)
musl: $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET)$(EXT) $(SOURCES) $(LDFLAGS)
	@$(MAKE) --no-print-directory size

run: g
	@echo "▶️  Запуск..."
	@./$(TARGET)$(EXT) || true

size:
	@SIZE=$$($(GET_SIZE) 2>/dev/null || echo 0); echo "📏 Размер: $$SIZE байт"

clean:
	rm -f $(TARGET) $(TARGET).exe
	@echo "🧹 Очищено"

# Отладочная сборка с сохранением символов
debug: CC = gcc
debug: CFLAGS = -g -O0 -DDEBUG $(COMMON_CFLAGS)
debug: LDFLAGS = $(COMMON_LDFLAGS)
debug: $(SOURCES)
	@echo "🐛 Отладочная сборка"
	$(CC) $(CFLAGS) -o $(TARGET)$(EXT) $(SOURCES) $(LDFLAGS)

help:
	@echo "Доступные цели:"
	@echo "  make g      - сборка GCC (по умолчанию)"
	@echo "  make c      - сборка Clang"
	@echo "  make musl   - статическая сборка с musl (только Linux)"
	@echo "  make run    - сборка и запуск"
	@echo "  make debug  - отладочная сборка с символами"
	@echo "  make clean  - очистка"
	@echo "  make size   - показать размер"
