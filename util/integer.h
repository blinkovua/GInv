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

#ifndef GINV_UTIL_INTEGER_H
#define GINV_UTIL_INTEGER_H

#include <cassert>

#include <gmp.h>

#include "allocator.h"
#include "config.h"

namespace GInv {

class Integer {
  static size_t buffer_size;
  static char*  buffer;

  static inline int abs(int a) {
    return (a >= 0) ? a: -a;
  }
  static inline int max(int a, int b) {
    return (a >= b) ? a: b;
  }
  static inline int min(int a, int b) {
    return (a >= b) ? b: a;
  }

  void reallocate(int size) {
    assert(size > 0);
    if (mMpz._mp_alloc < size) {
      if (mMpz._mp_alloc)
        mAllocator->dealloc(mMpz._mp_d, mMpz._mp_alloc);
      mMpz._mp_alloc = size;
      mMpz._mp_d = new(mAllocator) mp_limb_t[mMpz._mp_alloc];
    }
  }

protected:
  Allocator*   mAllocator;
  __mpz_struct mMpz;

public:
  Integer()=delete;
  explicit Integer(Allocator* allocator):
      mAllocator(allocator) {
    mpz_init(&mMpz);
  }
  Integer(const Integer &a)=delete;
  Integer(Integer &&a):
      mAllocator(a.mAllocator) {
    mMpz._mp_alloc = a.mMpz._mp_alloc;
    mMpz._mp_size = a.mMpz._mp_size;
    mMpz._mp_d = a.mMpz._mp_d;
    a.mMpz._mp_alloc = 0;
  }
  Integer(Allocator* allocator, const Integer &a):
      mAllocator(allocator) {
    mpz_init(&mMpz);
#ifdef GINV_UTIL_INTEGER_ALLOCATOR
    reallocate(abs(a.mMpz._mp_size));
#endif // GINV_UTIL_INTEGER_ALLOCATOR
    mpz_set(&mMpz, &a.mMpz);
  }
  ~Integer() {
#ifndef GINV_UTIL_INTEGER_ALLOCATOR
    mpz_clear(&mMpz);
#else
    if (mMpz._mp_alloc)
      mAllocator->dealloc(mMpz._mp_d, mMpz._mp_alloc);
#endif // GINV_UTIL_INTEGER_ALLOCATOR
  }
  Integer(Allocator* allocator, const char *str);

  void swap(Integer& a) {
    assert(this != &a);
    auto tmp=mAllocator;
    mAllocator = a.mAllocator;
    a.mAllocator = tmp;
    mpz_swap(&mMpz, &a.mMpz);
  }
  void operator=(Integer&& a) {
#ifndef GINV_UTIL_INTEGER_ALLOCATOR
    mpz_clear(&mMpz);
#else
    if (mMpz._mp_alloc)
      mAllocator->dealloc(mMpz._mp_d, mMpz._mp_alloc);
#endif // GINV_UTIL_INTEGER_ALLOCATOR
    mAllocator = a.mAllocator;
    mMpz._mp_alloc = a.mMpz._mp_alloc;
    mMpz._mp_size = a.mMpz._mp_size;
    mMpz._mp_d = a.mMpz._mp_d;

    a.mMpz._mp_alloc = 0;
  }
  void operator=(const Integer& a) {
#ifdef GINV_UTIL_INTEGER_ALLOCATOR
    reallocate(abs(a.mMpz._mp_size));
#endif // GINV_UTIL_INTEGER_ALLOCATOR
    mpz_set(&mMpz, &a.mMpz);
  }
  void set_si(signed long a) {
#ifdef GINV_UTIL_INTEGER_ALLOCATOR
    reallocate(1);
#endif // GINV_UTIL_INTEGER_ALLOCATOR
    mpz_set_si(&mMpz, a);
  }
  void set_ui(unsigned long a) {
#ifdef GINV_UTIL_INTEGER_ALLOCATOR
    reallocate(1);
#endif // GINV_UTIL_INTEGER_ALLOCATOR
    mpz_set_ui(&mMpz, a);
  }

  bool isZero() const { return mpz_sgn(&mMpz) == 0; }
  bool isOne() const { return mpz_cmp_ui(&mMpz, 1ul) == 0; }
  bool isAbsOne() const { return  mpz_cmpabs_ui(&mMpz, 1ul) == 0; }
  int sgn() const { return mpz_sgn(&mMpz); }
  int norm() const { return abs(mMpz._mp_size); }

  void set_neg() { mMpz._mp_size *= -1; }
  void add(const Integer& a);
  void add(const Integer& a, const Integer& b);
  void sub(const Integer& a);
  void sub(const Integer& a, const Integer& b);
  void mult(const Integer& a);
  void mult(const Integer& a, const Integer& b);
  bool divisible(const Integer& a) const {
    return  mpz_divisible_p (&mMpz, &a.mMpz) != 0;
  }
  void div(const Integer& a);
  void div(const Integer& a, const Integer& b);
  void gcd(const Integer& a);
  void gcd(const Integer& a, const Integer& b);
  void pow(const Integer& a, unsigned long n);

  const char* get_str(int base) const;
};

}

#endif // GINV_UTIL_INTEGER_H
