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

#include "sys.h"

// --- Константы проектирования ---
#define MAX_LOGIC_PAIRS  32    
#define MAX_PAIR_STR     32   
#define MAX_TOKEN_LEN    64    
#define MAX_NOISE_WORDS  64    
#define NEST_STACK_DEPTH 128   
#define ANALYZE_QUANTA   20    

// --- Битовые маски ролей токена ---
#define ROLE_UNKNOWN    0
#define ROLE_GLOBAL     1     
#define ROLE_LOCAL      2     
#define ROLE_OP         4     
#define ROLE_BLOCK      8     
#define ROLE_NOISE      16    

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
    int         parse_ptr, current_line, nest_stack[NEST_STACK_DEPTH], stack_ptr, in_comment, in_string;
} ParseState;

typedef struct {
    Token_t*    tokens;
    Pair_t      pairs[MAX_LOGIC_PAIRS];
    ParseState  ps;
    char        eol_str[MAX_PAIR_STR];
    uint32_t    noise_hashes[MAX_NOISE_WORDS];
    int         t_count, t_cap, p_count, n_count, indent_step, eol_len;
    int         ready_to_help;      
    int         analysis;           // Флаг анализа
    int         buffer_end;         // Конец текущего буфера для анализа
    uint64_t    tokens_at_last_check; 
} LogicEngine;

// --- Вспомогательные функции работы с текстом ---
int StringBC(const char *s, int *c) {
    int b = 0, i = 0; if (!s) { if (c) *c = 0; return 0; }
    while (s[b]) { if ((s[b] & 0xC0) != 0x80) i++; b++; }
    if (c) *c = i;
    return b; }
int StrLenB(const char *s) { return s ? strlen(s) : 0; }
int StrLen(const char *s) {
    if (!s) return 0;
    const unsigned char *p = (const unsigned char *)s;
    int count = 0;
    while (*p) { if ((*p++ & 0xC0) != 0x80) count++; }
    return count; }
int CharType(const unsigned char* buf, int* len) {
    unsigned char c = *buf; int l;
    if (c < 128) { *len = 1;
        if (c >= '0' && c <= '9') return 1;
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return 2;
        if (c >= 33 && c <= 126) return 4;
        if (c == 0) return 5;
        return 3; }
    if ((c & 0xE0) == 0xC0)      l = 2;
    else if ((c & 0xF0) == 0xE0) l = 3;
        else if ((c & 0xF8) == 0xF0) l = 4;
            else { *len = 1; return 5; }
    *len = l; for (int i = 1; i < l; i++) if ((buf[i] & 0xC0) != 0x80) return 5;
    return 2; }

// --- Сердце Движка ---
uint32_t GetHash(const char *s, int len) {
    uint32_t hash = 2166136261u;
    for (int i = 0; i < len; i++) { hash ^= (uint8_t)s[i]; hash *= 16777619u; }
    return hash; }

// Детектор внешних связей: "молча" проверяет наличие файла при нахождении сигнатуры xxx.yyy
void TryLinkFile(LogicEngine *le, const char *name) {
    if (!name || !strchr(name, '.')) return;
    if (os_file_exists(name)) {
        // Здесь будет логика добавления файла в очередь фоновой индексации
        // Для "всеядности" мы просто фиксируем факт связи
    } }

int AddOrUpdateToken(LogicEngine *le, unsigned char *name, int len, int level) {
    if (len <= 0 || !name) return -1;
    TryLinkFile(le, name); // Проверка на файл-сигнатуру
    uint32_t h = GetHash(name, len); int tmp, mid, low = 0, high = le->t_count - 1;
    while (low <= high) {
        mid = low + (high - low) / 2;
        if (le->tokens[mid].hash == h) {
            if ((tmp = strcmp(le->tokens[mid].name, name)) == 0) { le->tokens[mid].count++;
                if (le->tokens[mid].weight < 255) le->tokens[mid].weight++;
                if (level < le->tokens[mid].level) le->tokens[mid].level = level;
                return mid; }
            if (tmp < 0) low = mid + 1;
            else high = mid - 1; } 
        else if (le->tokens[mid].hash < h) low = mid + 1;
            else high = mid - 1; }
    if (le->t_count >= le->t_cap) {  int new_cap = (le->t_cap == 0) ? 256 : le->t_cap * 2;
        Token_t* t_tmp = (Token_t*)os_realloc(le->tokens, new_cap * sizeof(Token_t));
        if (!t_tmp) return -1;
        le->tokens = t_tmp; le->t_cap = new_cap; }
    if (low < le->t_count) memmove(&le->tokens[low + 1], &le->tokens[low], (le->t_count - low) * sizeof(Token_t));
    le->tokens[low].name = os_strdup(name); le->tokens[low].hash = h; le->tokens[low].count = 1;
    le->tokens[low].weight = 10; le->tokens[low].level = level; le->tokens[low].role = ROLE_UNKNOWN;
    le->tokens[low].parent_idx = -1; le->tokens[low].v_width = StrLen(name); le->t_count++; return low; }

void StepAnalysis(LogicEngine *le) {
    int len, type, first_type, total_len = 0;
    unsigned char *src = (unsigned char *)(le->buffer + le->ps.read_ptr);
    unsigned char *dst = (unsigned char *)(le->buffer + le->ps.write_ptr);
    if (le->ps.read_ptr >= le->buffer_end) { le->analysis = 0; if (le->was_repaired) { SaveRepairedPart(le); le->was_repaired = 0; } }
    while (le->ps.read_ptr < le->buffer_end) { type = CharType(src, &len); if (type != 5 && type != 3) break;
        le->ps.read_ptr += len; src += len; if (type == 5) le->was_repaired = 1; }
    if (le->ps.read_ptr >= le->buffer_end) return;
    
    first_type = type; unsigned char *token_start = dst;
    while (le->ps.read_ptr < le->buffer_end) { type = CharType(src, &len);
        if (type == 5) { le->ps.read_ptr += len; src += len; le->was_repaired = 1; continue; }
        if (type != first_type) break;
        for (int i = 0; i < len; i++) *dst++ = *src++;
        le->ps.read_ptr += len; total_len += len; }
    if (first_type == 4 && type == 1 && total_len > 0) { if (*(dst - 1) == '-' || *(dst - 1) == '+') total_len--; }
    if (total_len > 0) {
        if (first_type == 1 || first_type == 2) AddOrUpdateToken(le, (char*)token_start, total_len, le->ps.stack_ptr);
        else ProcessSign(le, token_start, total_len);
        le->ps.write_ptr += total_len; } }

void HeartBeat(LogicEngine *le) {
    uint64_t start_analyze;
    while (1) {
        const char *key = GetKey(); 
        if (key) {
            if (key[0] == 27 && key[1] == 1 && key[2] == 0) break; 
            HandleInput(le, key); RenderScreen(le); }
        if (le->ps.parse_ptr < le->buffer_end) { start_analyze = os_get_ms();
            while ((os_get_ms() - start_analyze) < ANALYZE_QUANTA) if (le->analysis) StepAnalysis(le);
            if (!le->ready_to_help && le->t_count > 20) le->ready_to_help = 1; }
        delay_ms(10); } }
