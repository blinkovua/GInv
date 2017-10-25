/***************************************************************************
 *   Copyright (C) 2016 by Blinkov Yu. A.                                  *
 *   BlinkovUA@info.sgu.ru                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "./integer.h"

namespace GInv {

size_t Integer::buffer_size=4096;
char*  Integer::buffer=new char[Integer::buffer_size];

int Integer::set_str(const char *str, int base) {
#ifndef GINV_POLY_INTEGER_ALLOCATOR
  int r=mpz_set_str(&mMpz, str, base);
#else
  __mpz_struct tmp;
  mpz_init(&tmp);
  int r=mpz_set_str(&tmp, str, base);
  if (r == 0) {
    reallocate(abs(tmp._mp_size));
    mpz_set(&mMpz, &tmp);
  }
  mpz_clear(&tmp);
#endif // GINV_POLY_INTEGER_ALLOCATOR
  return r;
}

void Integer::add(const Integer& a, const Integer& b) {
  assert(this != &a);
  assert(this != &b);
#ifdef GINV_POLY_INTEGER_ALLOCATOR
  reallocate(max(abs(a.mMpz._mp_size), abs(b.mMpz._mp_size)) + 1);
#endif // GINV_POLY_INTEGER_ALLOCATOR
  mpz_add(&mMpz, &a.mMpz, &b.mMpz);
}

void Integer::sub(const Integer& a, const Integer& b) {
  assert(this != &a);
  assert(this != &b);
#ifdef GINV_POLY_INTEGER_ALLOCATOR
  reallocate(max(abs(a.mMpz._mp_size), abs(b.mMpz._mp_size)) + 1);
#endif // GINV_POLY_INTEGER_ALLOCATOR
  mpz_sub(&mMpz, &a.mMpz, &b.mMpz);
}


void Integer::mult(const Integer& a, const Integer& b) {
  assert(this != &a);
  assert(this != &b);
#ifdef GINV_POLY_INTEGER_ALLOCATOR
  reallocate(abs(a.mMpz._mp_size) + abs(b.mMpz._mp_size));
#endif // GINV_POLY_INTEGER_ALLOCATOR
  mpz_mul(&mMpz, &a.mMpz, &b.mMpz);
}

void Integer::div(const Integer& a, const Integer& b) {
  assert(this != &a);
  assert(this != &b);
  assert(a.divisible(b));
#ifdef GINV_POLY_INTEGER_ALLOCATOR
  reallocate(abs(a.mMpz._mp_size) - abs(b.mMpz._mp_size) + 1);
#endif // GINV_POLY_INTEGER_ALLOCATOR
  mpz_divexact(&mMpz, &a.mMpz, &b.mMpz);
}

void Integer::gcd(const Integer& a, const Integer& b) {
  assert(this != &a);
  assert(this != &b);
  assert(a.divisible(b));
#ifdef GINV_POLY_INTEGER_ALLOCATOR
  reallocate(abs(a.mMpz._mp_size) - abs(b.mMpz._mp_size) + 1);
#endif // GINV_POLY_INTEGER_ALLOCATOR
  mpz_divexact(&mMpz, &a.mMpz, &b.mMpz);
}

const char* Integer::get_str(int base) const {
  assert(2 <= base && base <= 62);
  if (mMpz._mp_size == 0) {
    buffer[0] = '0';
    buffer[1] = '\0';
  }
  else {
    size_t size= mpz_sizeinbase(&mMpz, base) + 2;
    if (size > buffer_size) {
      delete[] buffer;
      buffer_size = (size/4096 + 1)*4096;
      buffer=new char[buffer_size];
    }
    assert(size <= buffer_size);
    char *res=mpz_get_str(buffer, base, &mMpz);
    assert(buffer == res);
  }
  return buffer;
}

}
