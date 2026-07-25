/* 
 * Fresh (C) 2026 A.Pozdnyakov GPLv3 - see LICENSE
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 */
//Begin 05.07.2026 in Russia
//  As  (санскр. As       — основа, бытие, существовать)
// anu  (санскр. anu      — атом)
// an   (санскр. anka     — цифра)
// n    (санскр. Nimitta  — знак{овое})
// v    (санскр. Vṛddhi   — увеличение {разрядности вдвое})
typedef uintptr_t  As;                              // Бесконечность не имеет обратного 80{00{00{00{00{00{00{..}}}}}}} и
#define SCell __SIZEOF_POINTER__                    // есть только в знаковом представлении! Любая разрядность [8..] бит.
typedef uint8_t anu;                                // 1   anu [0..FF]
//typedef uint8_t nanu;                             // 1  n    [0,-7F..-1,80,+1..+7F]
//typedef struct { union { anu h; anu l[1]; }; };   // 1       возможность работать с anu как со структурой
typedef struct { anu h, l[1]; } vanu;               // 2  v    [0..FFFF]
//typedef struct { anu h, l[1]; } vnanu             // 2 vn    [0,+1..+7FFF,8000,-7FFF..-1]
//typedef struct { anu h, l[2]; };                  // 3       [0..FFFFFF]    [17..24] бит диапазон теперь доступен
//typedef struct { anu h, l[2]; };                  // 3       [0,+1..+7FFFFF,800000,-7FFFFF..-1]
typedef struct { anu h, l[3]; } an;                 // 4   an  [0..FFFFFFFF]
//typedef struct { anu h, l[3]; } nan;              // 4  n    [0,+1..+7FFFFFFF,80000000,-7FFFFFFF..-1]
//typedef struct { anu h, l[4..6]; };               // 5-7
//typedef struct { anu h, l[4..6]; };               // 5-7n                   [33..56] бит диапазон теперь доступен
typedef struct { anu h, l[7]; } van;                // 8  v    [0..FFFFFFFFFFFFFFFF]
//typedef struct { anu h, l[7]; } vnan;             // 8 vn    [0,+1..+7FFFFFFFFFFFFFFF,8000000000000000,-7FFFFFFFFFFFFFFF..-1]
//typedef struct { anu h, l[0..254]; };             // 1-255
//typedef struct { anu h, l[0..254]; };             // 1-255n                 [8..2040] бит диапазон теперь доступен
typedef struct { anu h, l[255]; } MatBuf;           // 256....................[8..2048] для умножения {сдвиговый регистр}
typedef struct { anu Nim,   // Единственный рычаг знаковости (0 - беззнаковое). Типов данных НЕТ. Тип - воля создателя!
  Carry, IZ, Rnim, Long, Fa, Fb, Za, Zb, Br, Ba, Bb, *r, *a, *b, *e; MatBuf Ho, Lo, Sr; } var_;
var_ Mat = {.Nim = 0};

// Vikāra                 — модификация, изменение состояния विकार
// lr=0 la=0    сброс флагов        Mat.IZ,          Mat.Rnim, Mat.Carry = 0;
// lr=0 la!=0   анализ a            y знак{Mat.Rnim} x {if (Mat.IZ) {0 - ноль / FF - бесконечность}}, Mat.Carry = 0;
// lr!=0 la=0   создание нуля в r   1                0, Mat.Carry = 0;
// lr!=0 la!=0  преобразование      при уменьшении размера числа если отброшенная часть не пуста, то Mat.Carry = 1;
void Vikara (anu lr, anu la, anu *r, anu *a) { Mat.Carry = 0;
  if (!la) { Mat.IZ = 1; Mat.Rnim = 0; if (!lr) Mat.IZ--; else { do *r++ = 0; while(--lr); } return; }
  Mat.a = a; Mat.Long = la; Mat.Rnim = (Mat.Nim) ? (*a & 0x80) ? 0xFF : 0 : 0;
  if ((Mat.IZ = (*a) ? (Mat.Nim && *a == 0x80) ? 1 : 0 : 1)) while(Mat.IZ && --la) Mat.IZ = (*++a) ? 0 : Mat.IZ;
  if (!lr) { return; } if (Mat.IZ) { *r++ = (Mat.Rnim) ? 0x80 : 0; while(--lr) *r++ = 0; return }
  if (lr >= Mat.Long) { r += lr; Mat.a += Mat.Long; lr -= Mat.Long; do *--r = *--Mat.a; while(--Mat.Long);
    while(lr--) *--r = Mat.Rnim; return; }
  Mat.Long -= lr; do Mat.Carry = (*Mat.a++ == Mat.Rnim) ? Mat.Carry : 1; while(--Mat.Long); Mat.Ba = *Mat.a;
  if (Mat.Nim) { Mat.Carry = ((Mat.Ba ^ Mat.Rnim) & 0x80) ? 1 : Mat.Carry; Mat.Ba &= 0x7F; Mat.Ba |= (Mat.Rnim & 0x80); }
  *r++ = Mat.Ba; while(--lr) *r++ = *++Mat.a; }

void Add (anu l, anu *r, anu *a, anu *b) {
  Mat.Carry = (Mat.Carry != 0); if (Mat.Nim) { Mat.Fa = (*a == 0x80); Mat.Fb = (*b == 0x80);
    if (Mat.Fa || Mat.Fb) { Mat.Long = l; Mat.a = a; Mat.b = b; while(--Mat.Long && (Mat.Fa || Mat.Fb)) {
        Mat.Fa = (Mat.Fa) ? (*++Mat.a == 0) : Mat.Fa; Mat.Fb = (Mat.Fb) ? (*++Mat.b == 0) : Mat.Fb; }
      if (Mat.Fa || Mat.Fb) { Mat.IZ = 1; Mat.Rnim = 0xFF; *r++ = 0x80; while(--l) { *r++ = 0; } return; } } }
  r += l; a += l; b += l; do { *--r = *--a + *--b + Mat.Carry; Mat.Carry = (*r < *b); } while(--l); }

void Sub (anu l, anu *r, anu *a, anu *b) {
   Mat.Carry = (Mat.Carry != 0); if (Mat.Nim) { Mat.Fa = (*a == 0x80); Mat.Fb = (*b == 0x80);
    if (Mat.Fa || Mat.Fb) { Mat.Long = l; Mat.a = a; Mat.b = b; while(--Mat.Long && (Mat.Fa || Mat.Fb)) {
        Mat.Fa = (Mat.Fa) ? (*++Mat.a == 0) : Mat.Fa; Mat.Fb = (Mat.Fb) ? (*++Mat.b == 0) : Mat.Fb; }
      if (Mat.Fa || Mat.Fb) { Mat.IZ = 1; Mat.Rnim = 0xFF; *r++ = 0x80; while(--l) { *r++ = 0; } return; } } }
  r += l; a += l; b += l; do { *--r = *--a - *--b - Mat.Carry; Mat.Carry = (*r > *b + Mat.Carry); } while(--l); }

void Mul (anu l, anu *r, anu *a, anu *b) { if (l) { Mat.Fa = 0; Mat.Fb = 0; Mat.Ba = 1; Mat.Bb = 1; Mat.Long = l;
    Mat.Iz = 0; Mat.Za = (Mat.Nim) ? (*a & 0x80) ? 0xFF : 0 : 0; Mat.Zb = (Mat.Nim) ? (*b & 0x80) ? 0xFF : 0 : 0;
    Mat.Rnim = Mat.Za ^ Mat.Zb; Mat.r = r; r += l; Mat.e = r + l; a += l; b += l; Mat.a = &Mat.Ho; Mat.b = &Mat.Lo;
    do { Mat.Fa = (Mat.Fa) ? Mat.Fa : (*--a) ? l : Mat.Fa; Mat.Fb = (Mat.Fb) ? Mat.Fb : (*--b) ? l : Mat.Fb;
      Mat.Ba = (Mat.Fa) ? (Mat.Za) ? !(*Mat.a++ = ~*a + Mat.Ba) : (*Mat.a++ = *a) : Mat.Ba;
      Mat.Bb = (Mat.Fb) ? (Mat.Zb) ? !(*Mat.b++ = ~*b + Mat.Bb) : (*Mat.b++ = *b) : Mat.Bb;
      *--r = 0; *--Mat.e = 0; } while(--l); Mat.Ba = Mat.a - &Mat.Ho; Mat.Bb = Mat.b - &Mat.Lo;
    if (Mat.Ba < 2 || Mat.Bb < 2) { if (!Mat.Ba || !Mat.Bb) { Mat.Iz++; Mat.Rnim = 0; return; }
      if (Mat.Nim && (*(Mat.a - 1) == 0x80 || *(Mat.b - 1) == 0x80)) { *r = 0x80; Mat.Iz++; Mat.Rnim = 0xFF; return; } }
    Mat.e = Mat.r + Mat.Fa + Mat.Fb; Mat.Ba = 0; Mat.Bb = 0; do { Mat.Ba = (Mat.Ba) ? Mat.Ba : (*--Mat.a); 
      Mat.Bb = (Mat.Bb) ? Mat.Bb : (*--Mat.b); Mat.Fa = (Mat.Ba) ? Mat.Fa : --Mat.Fa; Mat.Fb = (Mat.Bb) ? Mat.Fb : --Mat.Fb;
      } while(!(Mat.Ba && Mat.Bb)); Mat.Ba = *Mat.a; Mat.Bb = *Mat.b; Mat.a = &Mat.Ho; Mat.b = &Mat.Lo;
    if (Mat.Fb > Mat.Fa || (Mat.Fb == Mat.Fa && Mat.Bb > Mat.Ba)) { Mat.a = Mat.b; Mat.b = &Mat.Ho; l = Mat.Fa;
      Mat.Fa = Mat.Fb; Mat.Fb = l; } *(Mat.b + Mat.Fb++) = 0;
    
  } }

void Div (anu l, anu *r, anu *a, anu *b, anu *e) {
  }

void VDiv (anu l, anu *r, anu *a, anu *b, anu *e) {
  }
