/* 
 * CS:APP Data Lab 
 * 
 * Zimo Meng 2100017702
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* Copyright (C) 1991-2022 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 10.0.0.  Version 10.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2017, fifth edition, plus
   the following additions from Amendment 1 to the fifth edition:
   - 56 emoji characters
   - 285 hentaigana
   - 3 additional Zanabazar Square characters */
/* 
 * bitAnd - x&y using only ~ and | 
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
int bitAnd(int x, int y) {
  /* 4 ops */
  return ~ ((~x) | (~y));
}
/* 
 * bitConditional - x ? y : z for each bit respectively
 *   Example: bitConditional(0b00110011, 0b01010101, 0b00001111) = 0b00011101
 *   Legal ops: & | ^ ~
 *   Max ops: 8
 *   Rating: 1
 */
int bitConditional(int x, int y, int z) {
  /* 4 ops */
  return (x & y) | ((~x) & z);
}
/* 
 * byteSwap - swaps the nth byte and the mth byte
 *  Examples: byteSwap(0x12345678, 1, 3) = 0x56341278
 *            byteSwap(0xDEADBEEF, 0, 2) = 0xDEEFBEAD
 *  You may assume that 0 <= n <= 3, 0 <= m <= 3
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 25
 *  Rating: 2
 */
int byteSwap(int x, int n, int m) {
  /* 22 ops */
  int tmpn = 255 << (n << 3);
  int tmpm = 255 << (m << 3);
  int y = (x & (~tmpn)) & (~tmpm);
  int xn = (x & tmpn) >> (n << 3) & 255;
  int xm = (x & tmpm) >> (m << 3) & 255;
  return (y | (xn << (m << 3))) | (xm << (n << 3));
}
/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3 
 */
int logicalShift(int x, int n) {
  /* 6 ops */
  int tmp = ~(((1 << 31) >> n) << 1);
  return (x >> n) & tmp;
}
/* 
 * cleanConsecutive1 - change any consecutive 1 to zeros in the binary form of x.
 *   Consecutive 1 means a set of 1 that contains more than one 1.
 *   Examples cleanConsecutive1(0x10) = 0x10
 *            cleanConsecutive1(0xF0) = 0x0
 *            cleanConsecutive1(0xFFFF0001) = 0x1
 *            cleanConsecutive1(0x4F4F4F4F) = 0x40404040
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 25
 *   Rating: 4
 */
int cleanConsecutive1(int x){
  /* 9 ops */
  int tmp = ~(1 << 31);
  return (((x & (x << 1)) | (x & ((x >> 1) & tmp)))) ^ x;
}
/* 
 * countTrailingZero - return the number of consecutive 0 from the lowest bit of 
 *   the binary form of x.
 *   YOU MAY USE BIG CONST IN THIS PROBLEM, LIKE 0xFFFF0000
 *   YOU MAY USE BIG CONST IN THIS PROBLEM, LIKE 0xFFFF0000
 *   YOU MAY USE BIG CONST IN THIS PROBLEM, LIKE 0xFFFF0000
 *   Examples countTrailingZero(0x0) = 32, countTrailingZero(0x1) = 0,
 *            countTrailingZero(0xFFFF0000) = 16,
 *            countTrailingZero(0xFFFFFFF0) = 8,
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int countTrailingZero(int x){
  /* 35 ops*/
  int n = 0;
  int tmp2 = 3;
  int tmp4 = 15;
  int tmp8 = 255;
  int tmp16 = 0xffff;
  int testx16 = ((!(x&tmp16)) << 31) >> 31;
  int delta16 = testx16 & 16;
  int x16 = x >> delta16;
  int n16 = n + delta16;
  int testx8 = ((!(x16&tmp8)) << 31) >> 31;
  int delta8 = testx8 & 8;
  int x8 = x16 >> delta8;
  int n8 = n16 + delta8;
  int testx4 = ((!(x8&tmp4)) << 31) >> 31;
  int delta4 = testx4 & 4;
  int x4 = x8 >> delta4;
  int n4 = n8 + delta4;
  int testx2 = ((!(x4&tmp2)) << 31) >> 31;
  int delta2 = testx2 & 2;
  int x2 = x4 >> delta2;
  int n2 = n4 + delta2;
  int delta1 = (x2 & 1) ^ 1;
  int x1 = x2 >> delta1;
  int n1 = n2 + delta1;
  int delta0 = (x1 & 1) ^ 1;
  int n0 = n1 + delta0;
  return n0;
}
/* 
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n) {
  /* 8 ops */
  int x31 = x >> 31;
  int y = x >> n;
  int rest = (y << n) ^ x;
  return y + ((!(!rest)) & x31);
}
/* 
 * oneMoreThan - return 1 if y is one more than x, and 0 otherwise
 *   Examples oneMoreThan(0, 1) = 1, oneMoreThan(-1, 1) = 0
 *   Legal ops: ~ & ! ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int oneMoreThan(int x, int y) {
  /* 14 ops */
  int z = x + 1;
  // int judge = !((~(x+y)) | (!x) | (!y));
  int judge = !(y ^ (1 << 31));
  return (~judge)&(!(~(~z)^y));
}
/*
 * satMul3 - multiplies by 3, saturating to Tmin or Tmax if overflow
 *  Examples: satMul3(0x10000000) = 0x30000000
 *            satMul3(0x30000000) = 0x7FFFFFFF (Saturate to TMax)
 *            satMul3(0x70000000) = 0x7FFFFFFF (Saturate to TMax)
 *            satMul3(0xD0000000) = 0x80000000 (Saturate to TMin)
 *            satMul3(0xA0000000) = 0x80000000 (Saturate to TMin)
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 25
 *  Rating: 3
 */
int satMul3(int x) {
  /* 15 ops */
  int x2 = x + x;
  int isoverflow2 = (x2 ^ x) >> 31;
  int x3 = x2 + x;
  int isoverflow3 = (x3 ^ x) >> 31;
  int isoverflow = isoverflow2 | isoverflow3;
  int ifoverflow = (1 << 31) ^ (~(x >> 31));
  return (isoverflow & ifoverflow) | ((~isoverflow) & x3);
}
/* 
 * subOK - Determine if can compute x-y without overflow
 *   Example: subOK(0x80000000,0x80000000) = 1,
 *            subOK(0x80000000,0x70000000) = 0, 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int subOK(int x, int y) {
  /* 9 ops */
  int _y = (~y) + 1;
  int z = x + _y;
  int isoverflow = ((x ^ y) & (x ^ z)) >> 31;
  return (~isoverflow) & 1;
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  /* 14 ops */
  int _x = (~x) + 1;
  int z = y + _x;
  int sgnjudge = ((y ^ 0) | (x ^ (~0))) >> 31;
  int negajudge = ((y ^ x) | (z ^ 0)) >> 31;
  return !(sgnjudge & negajudge);
}
/*
 * trueThreeFourths - multiplies by 3/4 rounding toward 0,
 *   avoiding errors due to overflow
 *   Examples: trueThreeFourths(11) = 8
 *             trueThreeFourths(-9) = -6
 *             trueThreeFourths(1073741824) = 805306368 (no overflow)
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 4
 */
int trueThreeFourths(int x)
{
  /* 14 ops */
  int sgnx = x >> 31;
  int xdiv4 = x >> 2;
  int rest = (xdiv4 << 2) ^ x;
  return xdiv4 + xdiv4 + xdiv4 + ((rest + rest + rest) >> 2) + ((!(!rest)) & sgnx);
}
/* 
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf) {
  /* 18 ops */
  int sgn = 1 << 31;
  int tmp = sgn >> 8;
  int sgnuf = uf & sgn;
  int f = uf & (~(tmp));
  int e = uf & ((~sgn)& (tmp));
  int res = uf + (1 << 23);
  if(e==0){
    return sgnuf | (f << 1);
  }
  if(e==(255 << 23)){
    return uf;
  }
  if(e==(254 << 23)){
    return tmp & res;
  }
  return res;
}
/* 
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
  /* 30 ops */
  unsigned absx = x >= 0 ? x : (-x);
  unsigned tmp = 1 << 31;
  unsigned sgnx = x & tmp;
  int last1 = -1;
  unsigned y = absx;
  int delta;
  unsigned e;
  unsigned f;
  unsigned del1;
  unsigned _delta;
  unsigned test;
  unsigned delf;
  if(x){
    while(y){
      last1++;
      y = y >> 1;
    }
    delta = 23-last1;
    e = (last1 + 127) << 23;
    del1 = absx & (~(1 << last1));
    if(delta >=0){
      f = del1 << delta;
    }
    else{
      _delta = -delta;
      delf = (0xff >> (8-_delta)) & del1;
      test = 1 << (_delta-1);
      f = del1 >> _delta;
      if(delf > test||(delf == test && (f & 1))){
        f = f + 1;
      }
    }
    return sgnx + e + f;
  }
  return 0;
}
/* 
 * float_f2i - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int float_f2i(unsigned uf) {
  /* 16 ops */
  int sgnx = uf >> 31;
  int e = ((uf >> 23) & 0xff);
  int f = (uf << 9) >> 9;
  int absx;
  int m;
  int n;
  int leftf;
  if(e <= 126) return 0;
  if(e<158){
    m = e-127;
    n = 23-m;
    f = f + (1 << 23);
    if(m<=23){
      leftf = f >> n;
      absx = leftf;
    }
    else 
      absx = f << (m-23);
    return sgnx?(-absx):absx;
  }
  else return 0x80000000u;
}
/* 
 * float_pwr2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned float_pwr2(int x) {
  /* 10 ops */
    if(x < -149) return 0;
    if(x<-126) return 1 << (x+149);
    if(x>127) return 255 << 23;
    return (x+127) << 23;
}

