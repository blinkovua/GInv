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

#include "integer.h"

namespace GInv {

size_t Integer::sBufferSize=4096*16;
char*  Integer::sBuffer=new char[Integer::sBufferSize];

#ifdef GINV_UTIL_INTEGER_ALLOCATOR  
  void Integer::reallocate(int size) {
    assert(size > 0);
    if (mMpz._mp_alloc < size) {
      if (mMpz._mp_alloc)
        mAllocator->dealloc(mMpz._mp_d, mMpz._mp_alloc);
//         delete[] mMpz._mp_d;
      mMpz._mp_alloc = size;
      mMpz._mp_d = new(mAllocator) mp_limb_t[mMpz._mp_alloc];
//       mMpz._mp_d = new mp_limb_t[mMpz._mp_alloc];
    }
  }

Integer::Integer(Allocator* allocator, const char *str):
      mAllocator(allocator) {
  mpz_init(&mMpz);
  __mpz_struct tmp;
  mpz_init(&tmp);
  int r=mpz_set_str(&tmp, str, 0);
  assert(r == 0);
  reallocate(abs(tmp._mp_size));
  mpz_set(&mMpz, &tmp);
  mpz_clear(&tmp);
}

void Integer::add(const Integer& a) {
  Integer tmp(mAllocator);
  tmp.reallocate(max(abs(mMpz._mp_size), abs(a.mMpz._mp_size)) + 1);
  mpz_add(&tmp.mMpz, &mMpz, &a.mMpz);
  swap(tmp);
}

void Integer::add(const Integer& a, const Integer& b) {
  assert(this != &a);
  assert(this != &b);
  reallocate(max(abs(a.mMpz._mp_size), abs(b.mMpz._mp_size)) + 1);
  mpz_add(&mMpz, &a.mMpz, &b.mMpz);
}

void Integer::sub(const Integer& a) {
  Integer tmp(mAllocator);
  tmp.reallocate(max(abs(mMpz._mp_size), abs(a.mMpz._mp_size)) + 1);
  mpz_sub(&tmp.mMpz, &mMpz, &a.mMpz);
  swap(tmp);
}


void Integer::sub(const Integer& a, const Integer& b) {
  assert(this != &a);
  assert(this != &b);
  reallocate(max(abs(a.mMpz._mp_size), abs(b.mMpz._mp_size)) + 1);
  mpz_sub(&mMpz, &a.mMpz, &b.mMpz);
}

void Integer::mult(const Integer& a) {
  Integer tmp(mAllocator);
  tmp.reallocate(abs(mMpz._mp_size) + abs(a.mMpz._mp_size));
  mpz_mul(&tmp.mMpz, &mMpz, &a.mMpz);
  swap(tmp);
}

void Integer::mult(const Integer& a, const Integer& b) {
  assert(this != &a);
  assert(this != &b);
  reallocate(abs(a.mMpz._mp_size) + abs(b.mMpz._mp_size));
  mpz_mul(&mMpz, &a.mMpz, &b.mMpz);
}

void Integer::div(const Integer& a) {
  assert(divisible(a));
  Integer tmp(mAllocator);
  tmp.reallocate(abs(mMpz._mp_size) - abs(a.mMpz._mp_size) + 1);
  mpz_divexact(&tmp.mMpz, &mMpz, &a.mMpz);
  swap(tmp);
}

void Integer::div(const Integer& a, const Integer& b) {
  assert(this != &a);
  assert(this != &b);
  assert(a.divisible(b));
  reallocate(abs(a.mMpz._mp_size) - abs(b.mMpz._mp_size) + 1);
  mpz_divexact(&mMpz, &a.mMpz, &b.mMpz);
}

void Integer::gcd(const Integer& a) {
  assert(!isZero() && !a.isZero());
  Integer tmp(mAllocator);
  tmp.reallocate(min(abs(mMpz._mp_size), abs(a.mMpz._mp_size)));
  mpz_gcd(&tmp.mMpz, &mMpz, &a.mMpz);
  swap(tmp);
}

void Integer::gcd(const Integer& a, const Integer& b) {
  assert(this != &a);
  assert(this != &b);
  assert(!a.isZero() && !b.isZero());
  reallocate(min(abs(a.mMpz._mp_size), abs(b.mMpz._mp_size)));
  mpz_gcd(&mMpz, &a.mMpz, &b.mMpz);
}

#endif // GINV_UTIL_INTEGER_ALLOCATOR


const char* Integer::get_str(int base) const {
  assert(2 <= base && base <= 62);
  if (mMpz._mp_size == 0) {
    sBuffer[0] = '0';
    sBuffer[1] = '\0';
  }
  else {
    size_t size= mpz_sizeinbase(&mMpz, base) + 2;
    if (size > sBufferSize) {
      delete[] sBuffer;
      sBufferSize = (size*15/10/4096 + 1)*4096;
      sBuffer=new char[sBufferSize];
    }
    assert(size <= sBufferSize);
    char *res=mpz_get_str(sBuffer, base, &mMpz);
    assert(sBuffer == res);
  }
  return sBuffer;
}

}
