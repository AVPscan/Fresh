/* 
 * Math (C) 2026 A.Pozdnyakov GPLv3 - see LICENSE
 * E-mail: avp70ru@mail.ru
 * 
 * Данная программа является свободным программным обеспечением: вы можете 
 * распространять ее и/или изменять согласно условиям Стандартной общественной 
 * лицензии GNU (GPLv3).
 */

#include "sys.h"
MATH_VARS_INIT;

void FBSWAP (anu l, anu *r, anu *a) { if (l) { Mat.r = r + l; Mat.a = a + l; Mat.Fa = (l & 1); l >>= 1;
  while(l--) { Mat.Ba = *--Mat.a; Mat.Bb = *a++; *r++ = Mat.Ba; *--Mat.r = Mat.Bb; } if (Mat.Fa) { *r = *a; } } }

void FMOVB (anu l, anu *r, anu h) { Mat.Carry = 0; if (l) { Mat.Rnim = (Mat.Nim) ? (h & 0x80) ? 0xFF : 0 : 0;
  Mat.Riz = (h) ? (Mat.Nim && h == 0x80) ? 1 : 0 : 1; while(--l) { *r++ = Mat.Rnim; } *r = h; } }

void FVMOVB (anu s, anu *r, anu h, anu l) { Mat.Carry = 0; if (s) { if (--s) { Mat.Rnim = (Mat.Nim) ? (h & 0x80) ? 0xFF : 0 : 0;
  Mat.Riz = (!l) ? (Mat.Nim && h == 0x80) ? 1 : (!h) : 0; while(--s) { *r++ = Mat.Rnim; } *r++ = h; *r = l; return; } *r = l;
  Mat.Rnim = (Mat.Nim) ? (l & 0x80) ? 0xFF : 0 : 0; Mat.Riz = (l) ? (Mat.Nim && l == 0x80) ? 1 : 0 : 1; Mat.Carry = (h != Mat.Rnim); } }

void FRR (anu l, anu *r) { if (l) { r--; do { Mat.Za = *++r & 1; *r >>= 1; if (Mat.Carry) { *r |= 0x80; } Mat.Carry = Mat.Za;
    } while(--l); Mat.Carry = (Mat.Carry != 0); } }

void FRL (anu l, anu *r) { if (l) { r += l; do { Mat.Za = *--r & 0x80; *r <<= 1; if (Mat.Carry) { *r |= 1; } Mat.Carry = Mat.Za;
    } while(--l); Mat.Carry = (Mat.Carry != 0); } }

void VIKARA (anu lr, anu la, anu *r, anu *a) { Mat.Carry = 0; if ((Mat.Loop = la)) { Mat.Rnim = (Mat.Nim) ? (*a & 0x80) ? 0xFF : 0 : 0;
    Mat.a = a; if ((Mat.Riz = (*a) ? (Mat.Nim && *a == 0x80) ? 1 : 0 : 1)) while(--Mat.Loop && (Mat.Riz = (*++Mat.a) ? 0 : Mat.Riz));
    if (Mat.Riz) { if (lr) { *r++ = *a; while(--lr) *r++ = 0; } } if (!lr) return;
    if (lr >= la) { r += lr; a += la; lr -= la; do *--r = *--a; while(--la); while(lr--) { *--r = Mat.Rnim; } return; }
    la -= lr; do Mat.Carry = (*a++ == Mat.Rnim) ? Mat.Carry : 1; while(--la); Mat.Ba = *a;
    if (Mat.Nim) { Mat.Carry = ((Mat.Ba ^ Mat.Rnim) & 0x80) ? 1 : Mat.Carry; Mat.Ba &= 0x7F; Mat.Ba |= (Mat.Rnim & 0x80);
      if (Mat.Carry) { Mat.Carry--; Mat.Riz++; Mat.Rnim = 0xFF; *r++ = 0x80; while(--lr) { *r++ = 0; } return; } } *r++ = Mat.Ba;
    while(--lr) { *r++ = *++a; } return ; } Mat.Rnim = 0; Mat.Riz = 0; if (lr) { Mat.Riz++; do *r++ = 0; while(--lr); } }

void FADD (anu l, anu *r, anu *a, anu *b) { if (l) { Mat.Riz = 0; Mat.Rnim = 0; Mat.Za = !((*a ^ *b) & 0x80);
  Mat.Loop = l; Mat.Carry = (Mat.Carry != 0); if (Mat.Nim) { Mat.Fa = (*a == 0x80); Mat.Fb = (*b == 0x80);
    if (Mat.Fa || Mat.Fb) { Mat.a = a; Mat.b = b; while(--Mat.Loop && (Mat.Fa || Mat.Fb)) {
        Mat.Fa = (Mat.Fa) ? (*++Mat.a == 0) : Mat.Fa; Mat.Fb = (Mat.Fb) ? (*++Mat.b == 0) : Mat.Fb; }
      if (!Mat.Loop) { Mat.Riz++; Mat.Rnim--; *r++ = 0x80; while(--l) { *r++ = 0; } return; } Mat.Loop = l; } }
  r += l; a += l; b += l; Mat.Br = 0; do { Mat.Bb = *--b; Mat.Ba = Mat.Bb + Mat.Carry; *--r = *--a + Mat.Ba;
    Mat.Carry = (*r < Mat.Ba) || (Mat.Ba < Mat.Bb); Mat.Br |= *r; } while(--l);
  if (Mat.Nim) { if (Mat.Carry && Mat.Za) { Mat.Riz++; Mat.Rnim--; *r++ = 0x80; while(--Mat.Loop) *r++ = 0; }
    else { Mat.Rnim = (*r & 0x80) ? 0xFF : 0; } Mat.Carry = 0; } if (!Mat.Br) { Mat.Riz = 1; Mat.Rnim = 0; } } }

void FSUB (anu l, anu *r, anu *a, anu *b) { if (l) { Mat.Riz = 0; Mat.Rnim = 0; Mat.Za = ((*a ^ *b) & 0x80);
  Mat.Loop = l; Mat.Carry = (Mat.Carry != 0); if (Mat.Nim) { Mat.Fa = (*a == 0x80); Mat.Fb = (*b == 0x80);
    if (Mat.Fa || Mat.Fb) { Mat.a = a; Mat.b = b; while(--Mat.Loop && (Mat.Fa || Mat.Fb)) {
        Mat.Fa = (Mat.Fa) ? (*++Mat.a == 0) : Mat.Fa; Mat.Fb = (Mat.Fb) ? (*++Mat.b == 0) : Mat.Fb; }
      if (!Mat.Loop) { Mat.Riz++; Mat.Rnim--; *r++ = 0x80; while(--l) { *r++ = 0; } return; } Mat.Loop = l; } }
  r += l; a += l; b += l; Mat.Br = 0; do { Mat.Bb = *--b; Mat.Ba = Mat.Bb + Mat.Carry; *--r = *--a - Mat.Ba;
    Mat.Carry = (*r > Mat.Ba) || (Mat.Ba < Mat.Bb); Mat.Br |= *r; } while(--l);
  if (Mat.Nim) { if (Mat.Carry && Mat.Za) { Mat.Riz++; Mat.Rnim--; *r++ = 0x80; while(--Mat.Loop) *r++ = 0; }
    else { Mat.Rnim = (*r & 0x80) ? 0xFF : 0; } Mat.Carry = 0; } if (!Mat.Br) { Mat.Riz = 1; Mat.Rnim = 0; } } }

void FMUL (anu l, anu *r, anu *a, anu *b) { if (l) { Mat.Fa = 0; Mat.Fb = 0; Mat.Ba = 1; Mat.Bb = 1; Mat.Loop = l;
    Mat.Riz = 0; Mat.Za = (*a & 0x80) ? 0xFF : 0; Mat.Zb = (*b & 0x80) ? 0xFF : 0; Mat.Rnim = Mat.Za ^ Mat.Zb;
    Mat.r = r; r += l; Mat.e = r + l; a += l; b += l; Mat.a = &Mat.Ho.h; Mat.b = &Mat.Lo.h;
    do { Mat.Fa = (*--a) ? (Mat.Fa) ? Mat.Fa : l : Mat.Fa; Mat.Fb = (*--b) ? (Mat.Fb) ? Mat.Fb : l : Mat.Fb;
      Mat.Ba = (Mat.Fa) ? (Mat.Za) ? !(*Mat.a++ = ~*a + Mat.Ba) : (*Mat.a++ = *a) : Mat.Ba;
      Mat.Bb = (Mat.Fb) ? (Mat.Zb) ? !(*Mat.b++ = ~*b + Mat.Bb) : (*Mat.b++ = *b) : Mat.Bb;
      *--r = 0; *--Mat.e = 0; } while(--Mat.Loop); Mat.Ba = Mat.a - &Mat.Ho.h; Mat.Bb = Mat.b - &Mat.Lo.h;
    Mat.VLongl = l + l; Mat.VLongh = (Mat.VLongl < l); Mat.e = Mat.r + Mat.Fa + Mat.Fb;
    if (Mat.Ba < 2 || Mat.Bb < 2) { Mat.Riz++; Mat.Rnim = 0; if (!Mat.Ba || !Mat.Bb) return;
      if (Mat.Nim && (*(Mat.a - 1) == 0x80 || *(Mat.b - 1) == 0x80)) { *r = 0x80; Mat.Rnim--; return; } }
    Mat.Ba = 0; Mat.Bb = 0; do { Mat.Ba = (Mat.Ba) ? Mat.Ba : (*--Mat.a); Mat.Bb = (Mat.Bb) ? Mat.Bb : (*--Mat.b);
      Mat.Fa = (Mat.Ba) ? Mat.Fa : --Mat.Fa; Mat.Fb = (Mat.Bb) ? Mat.Fb : --Mat.Fb; } while(!(Mat.Ba && Mat.Bb));
    Mat.Ba = *Mat.a; Mat.Bb = *Mat.b; Mat.a = &Mat.Ho.h; Mat.b = &Mat.Lo.h;
    if (Mat.Fb > Mat.Fa || (Mat.Fb == Mat.Fa && Mat.Bb > Mat.Ba)) { Mat.a = Mat.b; Mat.b = &Mat.Ho.h; l = Mat.Fa;
      Mat.Fa = Mat.Fb; Mat.Fb = l; } *(Mat.b + Mat.Fb++) = 0; r = Mat.e;
    do { if ((Mat.Ba = *--Mat.a)) { a = &Mat.Sr.h; b = Mat.b; l = Mat.Fb; do *a++ = *b++; while(--l);
        do { Mat.Zb = 0; b = &Mat.Sr.h; l = Mat.Fb; if (!(Mat.Ba & 1)) do { Mat.Br = (*b << 1) + Mat.Zb;
            Mat.Zb = ((*b & 0x80) == 0x80); *b++ = Mat.Br; } while(--l);
          else { a = r; Mat.Za = 0; do { Mat.Br = *b + Mat.Za; *--a += Mat.Br; Mat.Za = (*a < Mat.Br) || (Mat.Br < *b);
            Mat.Br = (*b << 1) + Mat.Zb; Mat.Zb = ((*b & 0x80) == 0x80); *b++ = Mat.Br; } while(--l); } 
          } while(Mat.Ba >>= 1); } --r; } while(--Mat.Fa);
    if (Mat.Rnim) { Mat.Br = 1; do { Mat.Ba = *--Mat.e; Mat.Br = !(*Mat.e = ~Mat.Ba + Mat.Br); } while(Mat.e > Mat.r); }
    Mat.Rnim = (Mat.Nim) ? Mat.Rnim : 0; } }

void FDIV (anu l, anu *r, anu *a, anu *b, anu *e) { if (l) { Mat.Fa = 0; Mat.Fb = 0; Mat.Ba = 1; Mat.Bb = 1;
    Mat.Riz = 0; Mat.Za = (*a & 0x80) ? 0xFF : 0; Mat.Zb = (*b & 0x80) ? 0xFF : 0; Mat.Rnim = Mat.Za ^ Mat.Zb;
    Mat.Loop = l; r += l; a += l; b += l; e += l; Mat.a = &Mat.Ho.h; Mat.b = &Mat.Lo.h;
    do { Mat.Fa = (*--a) ? (Mat.Fa) ? Mat.Fa : l : Mat.Fa; Mat.Fb = (*--b) ? (Mat.Fb) ? Mat.Fb : l : Mat.Fb;
      Mat.Ba = (Mat.Fa) ? (Mat.Za) ? !(*Mat.a++ = ~*a + Mat.Ba) : (*Mat.a++ = *a) : Mat.Ba;
      Mat.Bb = (Mat.Fb) ? (Mat.Zb) ? !(*Mat.b++ = ~*b + Mat.Bb) : (*Mat.b++ = *b) : Mat.Bb;
      *--r = 0; *--e = (Mat.Fb) ? 0 : (Mat.Fa) ? *Mat.a : 0; } while(--l);
    Mat.Ba = Mat.a - &Mat.Ho.h; Mat.Bb = Mat.b - &Mat.Lo.h; if (Mat.Ba < 2 || Mat.Bb < 2) { Mat.Riz++; Mat.Rnim = 0;
       if (!Mat.Ba || !Mat.Bb || (Mat.Nim && *b == 0x80)) return;
       if (Mat.Nim && (*a == 0x80)) { *r = 0x80; Mat.Rnim--; return; } } e += Mat.Bb - 1; Mat.a = &Mat.Ho.h + Mat.Bb - 1;
    Mat.b = &Mat.Lo.h; Mat.Fa = 1; while(!(*Mat.b & Mat.Fa)) { *e = (*Mat.a & Mat.Fa) ? *e | Mat.Fa : *e; *Mat.a &= ~Mat.Fa;
      Mat.Fa <<= 1; }
  } }

void FVDIV (anu l, anu *r, anu *a, anu *b, anu *e) {
  (void)l; (void)r; (void)a; (void)b; (void)e;
  }
