/*
 * Copyright (C) 2026 Поздняков Алексей Васильевич
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "sys.h"

// --- Константы проектирования 
#define MAX_LOGIC_PAIRS  32    
#define MAX_PAIR_STR     32   
#define NEST_STACK_DEPTH 128   
#define ANALYZE_QUANTA   20    
// --- Основные структуры ---

typedef struct {
    char        open[MAX_PAIR_STR];
    char        close[MAX_PAIR_STR];
    int         count, len_o, len_c;
} Pair_t;

typedef struct {
    char*       name;
    uint32_t    hash;
    int         count, weight, level, role, parent_idx, v_width;
} Token_t;

typedef struct {
    int         read_ptr;       // src в FileBuf
    int         write_ptr;      // dst (чистый поток) в FileBuf
    int         current_line;
    int         nest_stack[NEST_STACK_DEPTH];
    int         stack_ptr;
    int         in_comment, in_string;
} ParseState;

typedef struct {
    Token_t*    tokens;
    Pair_t      pairs[MAX_LOGIC_PAIRS];
    ParseState  ps;
    
    void*       file_h;         // Хендл из os_open_file
    long        file_offset;    // Смещение для os_read_file_at
    long        file_size;      
    char*       current_file;   // Имя файла (триггер для автомата)
    
    int         t_count, t_cap, p_count, analysis;
    int         buffer_end;     // Граница в FileBuf
    int         is_eof;         
    int         was_repaired;   // Флаг очистки мусора
} LogicEngine;

// --- Вспомогательные функции ---

uint32_t GetHash(const char *s, int len) {
    uint32_t hash = 2166136261u;
    for (int i = 0; i < len; i++) { hash ^= (uint8_t)s[i]; hash *= 16777619u; }
    return hash; 
}

int CharType(const unsigned char* buf, int* len) {
    unsigned char c = *buf; int l;
    if (c < 128) { *len = 1;
        if (c >= '0' && c <= '9') return 1;
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '$') return 2;
        if (c >= 33 && c <= 126) return 4;
        if (c == 0) return 5;
        return 3; 
    }
    if ((c & 0xE0) == 0xC0) l = 2;
    else if ((c & 0xF0) == 0xE0) l = 3;
    else if ((c & 0xF8) == 0xF0) l = 4;
    else { *len = 1; return 5; }
    *len = l; return 2; 
}

// --- Ядро: Работа с данными ---

int NextChunk(LogicEngine *le) {
    if (!le->current_file) return 1;
    if (!le->file_h) {
        le->file_h = os_open_file(le->current_file);
        if (!le->file_h) { le->current_file = 0; le->analysis = 0; return 1; }
        le->file_offset = 0;
        le->is_eof = 0;
        le->buffer_end = 0;
        le->ps.read_ptr = 0; }
    size_t tail = 0;
    if (le->buffer_end > 0 && le->ps.read_ptr < le->buffer_end) {
        tail = le->buffer_end - le->ps.read_ptr;
        if (tail > 0) {
            if (tail > NBuf) tail = NBuf; 
            memmove(FileBuf, FileBuf + le->ps.read_ptr, tail); 
        } 
    }

    int read = os_read_file_at(le->file_h, le->file_offset, FileBuf + tail, DBuf);
    if (read <= 0) {
        if (tail == 0) { 
            os_close_file(le->file_h); le->file_h = 0; 
            le->current_file = 0; le->analysis = 0; return 1; 
        }
        le->is_eof = 1; 
    } else le->file_offset += read;

    le->ps.read_ptr = 0;
    le->ps.write_ptr = 0; // Начинаем запись чистого потока заново в новом куске
    le->buffer_end = (int)(tail + (read > 0 ? read : 0));
    return 0; 
}

void SaveRepairedPart(LogicEngine *le) {
}

int AddOrUpdateToken(LogicEngine *le, unsigned char *name, int len, int level) {
    if (len <= 0 || !name) return -1;
    uint32_t h = GetHash((char*)name, len); 
    int tmp, mid, low = 0, high = le->t_count - 1;
    while (low <= high) {
        mid = low + (high - low) / 2;
        if (le->tokens[mid].hash == h) {
            if ((tmp = strncmp(le->tokens[mid].name, (char*)name, len)) == 0) { 
                le->tokens[mid].count++;
                if (le->tokens[mid].weight < 255) le->tokens[mid].weight++;
                return mid; 
            }
            if (tmp < 0) low = mid + 1; else high = mid - 1;
        } else if (le->tokens[mid].hash < h) low = mid + 1;
        else high = mid - 1;
    }
    if (le->t_count >= le->t_cap) {  
        int n_cap = (le->t_cap == 0) ? 256 : le->t_cap * 2;
        le->tokens = (Token_t*)os_realloc(le->tokens, n_cap * sizeof(Token_t));
        le->t_cap = n_cap; 
    }
    if (low < le->t_count) memmove(&le->tokens[low + 1], &le->tokens[low], (le->t_count - low) * sizeof(Token_t));
    
    // Копируем имя из буфера в кучу, так как FileBuf изменится
    le->tokens[low].name = (char*)os_malloc(len + 1);
    memcpy(le->tokens[low].name, name, len);
    le->tokens[low].name[len] = '\0';
    
    le->tokens[low].hash = h; le->tokens[low].count = 1;
    le->tokens[low].weight = 10; le->tokens[low].level = level; 
    le->tokens[low].role = 0; le->t_count++; 
    return low; 
}

void ProcessSign(LogicEngine *le, unsigned char *name, int len) {
    for (int i = 0; i < len; i++) {
        unsigned char c = name[i];       
        int idx = AddOrUpdateToken(le, &c, 1, le->ps.stack_ptr);
        if (idx >= 0) le->tokens[idx].role = 4; // ROLE_OP
    } 
}

// --- Ядро: Потоковый анализ ---

void StepAnalysis(LogicEngine *le) {
    if (le->ps.read_ptr >= le->buffer_end) {
        if (le->was_repaired) { SaveRepairedPart(le); le->was_repaired = 0; }
        if (NextChunk(le)) return; 
    }
    
    int len, type, first_type, total_len = 0;
    unsigned char *src = FileBuf + le->ps.read_ptr;
    unsigned char *dst = FileBuf + le->ps.write_ptr;

    // Пропуск мусора и пробелов
    while (le->ps.read_ptr < le->buffer_end) { 
        type = CharType(src, &len); 
        if (type != 5 && type != 3) break;
        if (type == 5) le->was_repaired = 1;
        else { // Копируем пробельные в чистый поток
            for (int i = 0; i < len; i++) *dst++ = *src++;
            le->ps.write_ptr += len;
        }
        le->ps.read_ptr += len; src += len; 
    }
    
    if (le->ps.read_ptr >= le->buffer_end) return;
    unsigned char *token_start = dst; // Токен берем из очищенного dst-потока
    first_type = type;
    
    if (first_type == 4 && (*src == '-' || *src == '+')) { 
        int nl; if (le->ps.read_ptr + len < le->buffer_end) {
            if (CharType(src + len, &nl) == 1) first_type = 1; 
        }
    }

    while (le->ps.read_ptr < le->buffer_end) { 
        type = CharType(src, &len);
        if (type == 5) { le->was_repaired = 1; le->ps.read_ptr += len; src += len; continue; }
        
        int collect = 0;
        if (first_type == 1) { // Число
            if (type == 1 || type == 2) collect = 1;
            else if (*src == '.') {
                int nl; if (CharType(src + 1, &nl) == 1) collect = 1;
            }
            else if (total_len > 0 && (*src == '-' || *src == '+') && (*(src-1) == 'e' || *(src-1) == 'E')) collect = 1;
            else if (total_len == 0 && type == 4) collect = 1;
        } 
        else if (first_type == 2) { if (type == 2 || type == 1) collect = 1; }
        else { if (type == first_type) collect = 1; }
        
        if (!collect) break;
        
        for (int i = 0; i < len; i++) *dst++ = *src++;
        le->ps.read_ptr += len; le->ps.write_ptr += len;
        src += len; total_len += len; 
    }

    if (total_len > 0) {
        if (first_type == 1 || first_type == 2) AddOrUpdateToken(le, token_start, total_len, le->ps.stack_ptr);
        else ProcessSign(le, token_start, total_len);
    }
}

void HandleInput(LogicEngine *le, const char *key) { }
void RenderScreen(LogicEngine *le) {
    if (le->analysis) {
        printf("\rАнализ [%s]: %ld/%ld байт. Токенов: %d (Уровень: %d)   ", 
               le->current_file, le->file_offset, le->file_size, le->t_count, le->ps.stack_ptr);
        fflush(stdout); } 
    else if (le->t_count > 0) {
        static int done = 0;
        if (done) return;
        printf("\n\n--- ОТЧЕТ АВТОМАТА ---\n");
        printf("Всего уникальных токенов: %d\n", le->t_count);
        printf("Топ токенов по весу (Level > 0):\n");
        for (int i = 0; i < le->t_count && i < 15; i++) {
            if (le->tokens[i].level > 0)
                printf("[%s] вес:%d lvl:%d cnt:%d\n", 
                       le->tokens[i].name, le->tokens[i].weight, le->tokens[i].level, le->tokens[i].count); }
        done = 1; }}

void HeartBeat(LogicEngine *le) {
    uint64_t start_analyze;
    while (1) {
        const char *key = GetKey(); 
        if (key && key[0] == 27 && key[1] == 1 && key[2] == 0) break; 
        HandleInput(le, key); 
        RenderScreen(le); 
        if (le->analysis) {
            start_analyze = os_get_ms();
            while ((os_get_ms() - start_analyze) < ANALYZE_QUANTA) if (!le->analysis) StepAnalysis(le); }
        delay_ms(10); 
    } }

int main(int argc, char *argv[]) {
    LogicEngine le = {0};
    le.current_file = "test.c";
    le.analysis = 1;
    HeartBeat(&le);
    os_free(le.tokens);
    return 0;
}
