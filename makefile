#
# Fresh (C) 2026 A.Pozdnyakov GPLv3 - see LICENSE
# E-mail: avp70ru@mail.ru
# 
# Данная программа является свободным программным обеспечением: вы можете 
# распространять ее и/или изменять согласно условиям Стандартной общественной 
# лицензии GNU (GPLv3).
#

TARGET = fresh

# Определяем ОС
UNAME_S := $(shell uname -s 2>/dev/null || echo Windows)

# Базовые флаги (без -flto! он раздувает musl)
BASE_CFLAGS = -std=c11 -Os -DNDEBUG -Wall -Wextra -flto
LDFLAGS =

ifeq ($(OS),Windows_NT)
	SYS_SRC = sys_windows.c
	EXT = .exe
	RM = del /Q 2>NUL || rm -f
	RUN_CMD = .\\$(TARGET)$(EXT)
	GET_SIZE = wc -c < $(TARGET)$(EXT) 2>NUL || echo 0
else ifeq ($(UNAME_S),Linux)
	SYS_SRC = sys_linux.c
	EXT =
	RM = rm -f
	RUN_CMD = ./$(TARGET)
	GET_SIZE = stat -c%s $(TARGET) 2>/dev/null || echo 0
	BASE_CFLAGS += -D_POSIX_C_SOURCE=200809L
else ifeq ($(UNAME_S),Darwin)
	SYS_SRC = sys_macos.c
	EXT =
	RM = rm -f
	RUN_CMD = ./$(TARGET)
	GET_SIZE = stat -f %z $(TARGET) 2>/dev/null || echo 0
else ifeq ($(UNAME_S),FreeBSD)
	SYS_SRC = sys_bsd.c
	EXT =
	RM = rm -f
	RUN_CMD = ./$(TARGET)
	GET_SIZE = stat -f %z $(TARGET) 2>/dev/null || echo 0
else ifeq ($(UNAME_S),OpenBSD)
	SYS_SRC = sys_bsd.c
	EXT =
	RM = rm -f
	RUN_CMD = ./$(TARGET)
	GET_SIZE = stat -f %z $(TARGET) 2>/dev/null || echo 0
else ifeq ($(UNAME_S),NetBSD)
	SYS_SRC = sys_bsd.c
	EXT =
	RM = rm -f
	RUN_CMD = ./$(TARGET)
	GET_SIZE = stat -f %z $(TARGET) 2>/dev/null || echo 0
else
	SYS_SRC = sys_linux.c
	EXT =
	RM = rm -f
	RUN_CMD = ./$(TARGET)
	GET_SIZE = stat -c%s $(TARGET) 2>/dev/null || echo 0
endif

SOURCES = main.c engine.c $(SYS_SRC)

.PHONY: musl all run clean size

# Обычная сборка
all: clean
	@$(CC) $(BASE_CFLAGS) -o $(TARGET)$(EXT) $(SOURCES) $(LDFLAGS)
	@$(MAKE) --no-print-directory size

# Статическая сборка с musl (без -flto!)
musl: clean
	@$(CC) $(BASE_CFLAGS) -o $(TARGET)$(EXT) $(SOURCES) $(LDFLAGS)
	@$(MAKE) --no-print-directory size

# Только показать размер
size:
	@SIZE=$$($(GET_SIZE)); echo "$(TARGET)$(EXT) $$SIZE byte"

# Запуск
run: clean
	@$(CC) $(BASE_CFLAGS) -o $(TARGET)$(EXT) $(SOURCES) $(LDFLAGS)
	@$(MAKE) --no-print-directory size
	@$(RUN_CMD) || echo "(exit $$?)"

# Очистка
clean:
	@$(RM) $(TARGET)$(EXT) 2>/dev/null || true
