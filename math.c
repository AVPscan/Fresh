/* 
 * Fresh (C) 2026 A.Pozdnyakov GPLv3 - see LICENSE
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 * Begin 05.07.2026 in Russia
 */

//  As  (санскр. As       — основа, бытие, существовать)
// anu  (санскр. anu      — атом)
// an   (санскр. anka     — цифра)
// n    (санскр. Nimitta  — знак{овое})
// v    (санскр. Vṛddhi   — увеличение {разрядности вдвое})
typedef uintptr_t  As;                              // Бесконечность не имеет обратного 80{00{00{00{00{00{00{..}}}}}}} в
#define SCell __SIZEOF_POINTER__                    // знаковом представлении! Любая разрядность [8..2048] бит.
typedef uint8_t anu;                                // 1   anu [0..FF]
typedef int8_t nanu;                                // 1  n    [0,-7F..-1,80,+1..+7F]
//typedef struct { union { anu l[1]; anu h; }; };   // 1       возможность работать с anu как со структурой
typedef struct { anu l[1], h; } vanu;               // 2  v    [0..FFFF]
typedef struct { anu l[1]; nanu h; } vnanu;         // 2 vn    [0,+1..+7FFF,8000,-7FFF..-1]
//typedef struct { anu l[2], h; };                  // 3       [0..FFFFFF]    [17..24] бит диапазон теперь доступен
//typedef struct { anu l[2]; nanu h; };             // 3       [0,+1..+7FFFFF,800000,-7FFFFF..-1]
typedef struct { anu l[3], h; } an;                 // 4   an  [0..FFFFFFFF]
typedef struct { anu l[3]; nanu h; } nan;           // 4  n    [0,+1..+7FFFFFFF,80000000,-7FFFFFFF..-1]
//typedef struct { anu l[4:5:6]; nanu h; };         // 5-7                    [33..56] бит диапазон теперь доступен
typedef struct { anu l[7], h; } van;                // 8  v    [0..FFFFFFFFFFFFFFFF]
typedef struct { anu l[7]; nanu h; } vnan;          // 8 vn    [0,+1..+7FFFFFFFFFFFFFFF,8000000000000000,-7FFFFFFFFFFFFFFF..-1]
//typedef struct { anu l[8..254], h; };             // 9-255                  [65..2039] бит диапазон теперь доступен
//typedef struct { anu l[255], h; };                // 256     если передать на вход 0 байт в числе [8..2048] бит
typedef struct { anu l[511], h, e, r; } MatBuf;     // 514     для работы + extend,reserve!
typedef struct { anu Nim, Carry, E, N, S, T, Fa, Fb, Fe, Bm, *a, *b, *c, *o, *r, *ch, *oh, *rh; MatBuf sr, rr; } var_;
var_ Mat = {.Nim = 0};
// Vikāra                 — модификация, изменение состояния विकार
// На выходе Mat.Carry не чётрый {бит 0} (0x01) значит произошло усечение с потерей информации
// Если установлен {бит 1} (0x02) 2,3 то в результате бесконечность не учитываем знаковость представления (все биты 0 кроме старшего)
// Если установлен {бит 2} (0x04) 4,5 то в результате ноль (небытиё)
void Vikara (anu sC, anu sA, anu *c, anu *a) { Mat.S = 4;
  if (--sC <= --sA) { sA -= sC; Mat.Carry = 0; while(sC--) { Mat.S = (*c++ = *a++) ? 0 : Mat.S; }
    while(sA--) { Mat.Carry = (*a++) ? 1 : Mat.Carry; } *c = *a;
    Mat.Carry |= ((Mat.S) ? ((*c == 0x80) ? 2 : (!*c ? 4 : 1)) : 0); return; } sC -= sA; Mat.Carry = 4;
  while(sA--) { Mat.Carry = (*c++ = *a++) ? 0 : Mat.Carry; } Mat.S = 0; Mat.T = 0; *c = *a;
  Mat.Carry = (Mat.Carry) ? ((*c == 0x80) ? 2 : (!*c ? 4 : 0)) : 0; if (Mat.Nim) { if (Mat.Carry) { Mat.T = *c;
  *c = Mat.S; } else if (*c & 0x80) { Mat.S--; Mat.T--; } } while(sC--) { *++c = Mat.S; } *++c = Mat.T; }

void Add (anu s, anu *c, anu *a, anu *b) { Mat.Carry = (Mat.Carry != 0); Mat.a = a; Mat.b = b;
  if (Mat.Nim) { Mat.S = 2; Mat.T = 2; Mat.N = s; while(--Mat.N) { Mat.S = (*Mat.a++) ? 0 : Mat.S;
    Mat.T = (*Mat.b++) ? 0 : Mat.T; } Mat.S = (*Mat.a != 0x80) ? 0 : Mat.S; Mat.T = (*Mat.b != 0x80) ? 0 : Mat.T;
    if (Mat.S || Mat.T) { while(--s) { *c++ = 0; } *c = 0x80; Mat.Carry = 2; return; } Mat.a = a; Mat.b = b; }
  do { Mat.S = *Mat.a++; Mat.T = *Mat.b++; *c = Mat.S;
    Mat.Carry = ((*c += Mat.T + Mat.Carry) < Mat.S) || (Mat.Carry && (*c == Mat.S)); c++; } while(--s); }

void Sub (anu s, anu *c, anu *a, anu *b) { Mat.Carry = (Mat.Carry != 0); Mat.a = a; Mat.b = b;
  if (Mat.Nim) { Mat.S = 2; Mat.T = 2; Mat.N = s; while(--Mat.N) { Mat.S = (*Mat.a++) ? 0 : Mat.S;
    Mat.T = (*Mat.b++) ? 0 : Mat.T; } Mat.S = (*Mat.a != 0x80) ? 0 : Mat.S; Mat.T = (*Mat.b != 0x80) ? 0 : Mat.T;
    if (Mat.S || Mat.T) { while(--s) { *c++ = 0; } *c = 0x80; Mat.Carry = 2; return; } Mat.a = a; Mat.b = b; }
  do { Mat.S = *Mat.a++; Mat.T = *Mat.b++; *c = Mat.S;
    Mat.Carry = ((*c -= Mat.T + Mat.Carry) > Mat.S) || (Mat.Carry && (*c == Mat.S)); c++; } while(--s); }

#define SBLEFT() Mat.E = (*b & 0x80) ? ((*b++ = (*b << 1) + Mat.E) ? 1 : 1) : ((*b++ = (*b << 1) + Mat.E) ? 0 : 0)
void Mul (anu s, anu *c, anu *a, anu *b) { Mat.o = &Mat.sr.l; Mat.r = &Mat.rr.l;
  Mat.S = 0; Mat.T = 0; Mat.c = c; Mat.a = a; Mat.b = b; Mat.N = s; while(--Mat.N) {
    Mat.S = (Mat.S) ? ((*Mat.o++ = *Mat.a++) ? Mat.S : s - Mat.N) : ((*Mat.a) ? ((*Mat.o++ = *Mat.a++) ? s - Mat.N : 0) : (Mat.a++) ? Mat.S : Mat.S);
    Mat.T = (Mat.T) ? ((*Mat.r++ = *Mat.b++) ? Mat.T : s - Mat.N) : ((*Mat.b) ? ((*Mat.r++ = *Mat.b++) ? s - Mat.N : 0) : (Mat.b++) ? Mat.T : Mat.T);
    *Mat.c++ = 0; } Mat.S = (!(Mat.Fa = (!Mat.S && (*Mat.o = *Mat.a) == 0x80) ? 2 : (!Mat.S && !(*Mat.o = *Mat.a)) ? 4 : 0)) ? s : Mat.S;
  Mat.T = (!(Mat.Fb = (!Mat.T && (*Mat.r = *Mat.b) == 0x80) ? 2 : (!Mat.T && !(*Mat.r = *Mat.b)) ? 4 : 0)) ? s : Mat.T; *Mat.c = 0; Mat.a = &Mat.sr.l;
  Mat.b = &Mat.rr.l; Mat.E = ((Mat.o - Mat.a) < (Mat.r - Mat.b)) ? 1 : ((Mat.o - Mat.a) > (Mat.r - Mat.b)) ? 0 : (*Mat.o < *Mat.r) ? 1 : 0;
  if (Mat.Fa || Mat.Fb) { Mat.Carry = 4; if (Mat.Nim && (Mat.Fa == 2 || Mat.Fb == 2)) { *Mat.c = 0x80; Mat.Carry = 2; } return; }
  Mat.Fa = (Mat.Nim && ((*Mat.o ^ *Mat.r) & 0x80)) ? 0xFF : 0; Mat.r -= Mat.a; Mat.o -= Mat.b; c += Mat.S - Mat.o + Mat.T - Mat.r;
  Mat.S = Mat.o + 1; Mat.T = Mat.r + 1; if (Mat.E) { Mat.a = Mat.b; Mat.b = &Mat.sr.l; Mat.E = Mat.S; Mat.S = Mat.T; Mat.T = Mat.E; }
  
  Mat.Fe = (Mat.S == s) ? 0 : 1; Mat.Bm = (*(Mat.a + --Mat.S) & 0x80) ? 0xFF : 0; Mat.S++; Mat.r = Mat.b + 1 + --s; Mat.c = c;
  do { if ((s = *Mat.b++)) { Mat.N = Mat.S; b = Mat.r; a = Mat.a; do { *b++ = *a++; } while(--Mat.N); *b = Mat.Bm; do {
    if (!(s & 1)) { Mat.E = 0; Mat.N = Mat.S; b = Mat.r; do { SBLEFT(); } while(--Mat.N); *b = (*b << 1) + Mat.E; }
    else { Mat.E = 0; Mat.Fb = 0; Mat.N = Mat.S; b = Mat.r; Mat.Fa = *(c = Mat.c); do {
      Mat.Fb = ((*c += *b + Mat.Fb) < Mat.Fa) || (Mat.Fb && (*c == Mat.Fa)); SBLEFT(); Mat.Fa = *++c; } while(--Mat.N);
      *b = (*b << 1) + Mat.E; Mat.Fb = (Mat.Fe) ? (*c += *b + Mat.Fb) : 0; } } while(s >>= 1); } Mat.c++; } while(--Mat.T); }

void VMul (anu s, anu *c, anu *a, anu *b) { Mat.o = &Mat.sr.l; Mat.r = &Mat.rr.l; Mat.S = 2; Mat.T = 2; Mat.c = c; Mat.a = a; Mat.b = b;
  Mat.N = s; Mat.oh = Mat.o + 1 + --s; Mat.rh = Mat.r + 1 + s; Mat.ch = Mat.c + 1 + s++; while(--Mat.N) {
    Mat.S = (*Mat.o++ = *Mat.a++) ? 0 : Mat.S; Mat.T = (*Mat.r++ = *Mat.b++) ? 0 : Mat.T; *Mat.oh++ = 0; *Mat.rh++ = 0; *Mat.c++ = 0;
    *Mat.ch++ = 0; } Mat.S = (Mat.S && (*Mat.o = *Mat.a) == 0x80) ? 2 : (Mat.S && !(*Mat.o = *Mat.a)) ? 4 : 0;
  Mat.T = (Mat.T && (*Mat.r = *Mat.b) == 0x80) ? 2 : (Mat.T && !(*Mat.r = *Mat.b)) ? 4 : 0; *Mat.c = 0; *Mat.ch = 0;
  if (Mat.S || Mat.T) { Mat.Carry = 4; if (Mat.Nim && (Mat.S == 2 || Mat.T == 2)) { *Mat.ch = 0x80; Mat.Carry = 2; } return; }
  }

void Div (anu s, anu *c, anu *a, anu *b, anu *r) { Mat.r = &Mat.rr.l; Mat.S = 2; Mat.T = 2; Mat.c = c; Mat.a = a; Mat.b = b; Mat.o = r;
  Mat.N = s; while(--Mat.N) { Mat.T = (*Mat.r++ = *Mat.b++) ? 0 : Mat.T; Mat.S = (*Mat.c++ = *Mat.a++) ? 0 : Mat.S;
    *Mat.o++ = 0; } Mat.T = (Mat.T && (*Mat.r = *Mat.b) == 0x80) ? 2 : (Mat.T && !(*Mat.r = *Mat.b)) ? 4 : 0;
  Mat.S = (Mat.S && (*Mat.c = *Mat.a) == 0x80) ? 2 : (Mat.S && !(*Mat.c = *Mat.a)) ? 4 : 0;
  if (Mat.S || Mat.T) { Mat.Carry = 4; if (Mat.Nim) { if (Mat.S == Mat.T) { *c = 1; *Mat.c = 0; Mat.Carry = 0; }
    else if (Mat.T == 4) { *Mat.c = 0x80; Mat.Carry = 2; } else if (Mat.T == 2 && !Mat.S) { while(--s) *c++ = 0; } } return; }
  }

void VDiv (anu s, anu *c, anu *a, anu *b, anu *r) { Mat.r = &Mat.rr.l; Mat.S = 2; Mat.T = 2; Mat.c = c; Mat.a = a; Mat.b = b; Mat.o = r;
  Mat.N = s; Mat.oh = Mat.a + 1 + --s; Mat.rh = Mat.c + 1 + s++; Mat.ch = Mat.rh; while(--Mat.N) { Mat.T = (*Mat.r++ = *Mat.b++) ? 0 : Mat.T;
    Mat.S = (*Mat.c++ = *Mat.a++) ? 0 : Mat.S; Mat.S = (*Mat.ch++ = *Mat.oh++) ? 0 : Mat.S; *Mat.o++ = 0; }
  Mat.T = (Mat.T && (*Mat.r = *Mat.b) == 0x80) ? 2 : (Mat.T && !(*Mat.r = *Mat.b)) ? 4 : 0; Mat.S = (*Mat.c = *Mat.a) ? 0 : Mat.S;
  Mat.S = (Mat.S && (*Mat.ch = *Mat.oh) == 0x80) ? 2 : (Mat.S && !(*Mat.ch)) ? 4 : 0;
  if (Mat.S || Mat.T) { Mat.Carry = 4; if (Mat.Nim) { if (Mat.S == Mat.T) { *c = 1; *Mat.ch = 0; Mat.Carry = 0; } else if (Mat.T == 4) { 
    *Mat.ch = 0x80; Mat.Carry = 2; } else if (Mat.T == 2 && !Mat.S) { while(--s) { *c++ = 0; *Mat.rh++ = 0; } } } return; }
  }
