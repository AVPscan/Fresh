#
# Fresh (C) 2026 A.Pozdnyakov GPLv3 - see LICENSE
# E-mail: avp70ru@mail.ru
# 
# Данная программа является свободным программным обеспечением: вы можете 
# распространять ее и/или изменять согласно условиям Стандартной общественной 
# лицензии GNU (GPLv3).
#

TARGET = fresh
UNAME_S := $(shell uname -s 2>/dev/null || echo Windows)
BASE_CFLAGS = -std=c11 -Os -DNDEBUG -Wall -Wextra -flto
CLANG_CFLAGS = -std=c11 -Oz -DNDEBUG -Wall -Wextra -flto
LDFLAGS =
CDFLAGS =

ifeq ($(OS),Windows_NT)
  SYS_SRC = sys_windows.c
  
  LDFLAGS += -lwinmm
  EXT = .exe
  RM = del /Q 2>NUL || rm -f
  RUN_CMD = .\\$(TARGET)$(EXT)
  GET_SIZE = wc -c < $(TARGET)$(EXT) 2>NUL || echo 0
else
ifeq ($(UNAME_S),Linux)
  BASE_CFLAGS += -D_POSIX_C_SOURCE=200809L
  CLANG_CFLAGS += -D_POSIX_C_SOURCE=200809L
  SYS_SRC = sys_linux.c
else ifeq ($(UNAME_S),Darwin)
  SYS_SRC = sys_macos.c
else
  SYS_SRC = sys_bsd.c
endif

  LDFLAGS += -s
  EXT =
  RM = rm -f
  RUN_CMD = ./$(TARGET)
  GET_SIZE = stat -c%s $(TARGET) 2>/dev/null || echo 0
endif

SOURCES = main.c engine.c $(SYS_SRC)
.PHONY: all c musl run clean size
all: clean
	@$(CC) $(BASE_CFLAGS) -o $(TARGET)$(EXT) $(SOURCES) $(LDFLAGS)
	@$(MAKE) --no-print-directory size
c: clean
	@clang $(CLANG_CFLAGS) -o $(TARGET)$(EXT) $(SOURCES) $(CDFLAGS)
	@$(MAKE) --no-print-directory size
musl: clean
	@$(CC) $(BASE_CFLAGS) -static -o $(TARGET)$(EXT) $(SOURCES) $(LDFLAGS)
	@$(MAKE) --no-print-directory size
size:
	@SIZE=$$($(GET_SIZE)); echo "$(TARGET)$(EXT) $$SIZE byte"
run: clean
	@$(CC) $(BASE_CFLAGS) -o $(TARGET)$(EXT) $(SOURCES) $(LDFLAGS)
	@$(MAKE) --no-print-directory size
	@$(RUN_CMD) || echo "(exit $$?)"
clean:
	@$(RM) $(TARGET)$(EXT) 2>/dev/null || true
