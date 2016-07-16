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

#ifndef GINV_INTEGER_INTEGER_H
#define GINV_INTEGER_INTEGER_H

#include <cassert>

#include <gmp.h>

#include "config.h"
#include "util/allocator.h"

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

  void reallocate(int size) {
    if (mMpz._mp_alloc < size) {
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
#ifndef GINV_INTEGER_ALLOCATOR
    mpz_init(&mMpz);
#else
    mMpz._mp_alloc = 1;
    mMpz._mp_d = new(mAllocator) mp_limb_t[mMpz._mp_alloc];
    mMpz._mp_size = 0;
#endif // GINV_INTEGER_ALLOCATOR
  }
  Integer(const Integer &a)=delete;
  Integer(const Integer &a, Allocator* allocator):
      mAllocator(allocator) {
#ifndef GINV_INTEGER_ALLOCATOR
    mpz_init(&mMpz);
#else
    mMpz._mp_alloc = max(abs(a.mMpz._mp_size), 1);
    mMpz._mp_d = new(mAllocator) mp_limb_t[mMpz._mp_alloc];
#endif // GINV_INTEGER_ALLOCATOR
    mpz_set(&mMpz, &a.mMpz);
  }
  ~Integer() {
#ifndef GINV_INTEGER_ALLOCATOR
    mpz_clear(&mMpz);
#else
    mAllocator->dealloc(mMpz._mp_d, mMpz._mp_alloc);
#endif // GINV_INTEGER_ALLOCATOR
  }

  void swap(Integer& a) {
    assert(this != &a);
    auto tmp=mAllocator;
    mAllocator = a.mAllocator;
    a.mAllocator = tmp;
    mpz_swap(&mMpz, &a.mMpz);
  }
  void operator=(const Integer& a) {
#ifdef GINV_INTEGER_ALLOCATOR
    reallocate(abs(a.mMpz._mp_size));
#endif // GINV_INTEGER_ALLOCATOR
    mpz_set(&mMpz, &a.mMpz);
  }
  void set_si(signed long a) { mpz_set_si(&mMpz, a); }
  void set_ui(unsigned long a) { mpz_set_ui(&mMpz, a); }
  int set_str(const char *str, int base=0);

  bool isZero() const { return mpz_sgn(&mMpz) == 0; }
  bool isOne() const { return mpz_cmp_ui(&mMpz, 1ul) == 0; }
  bool isAbsOne() const { return  mpz_cmpabs_ui(&mMpz, 1ul) == 0; }
  int sgn() const { return mpz_sgn(&mMpz); }

  void add(const Integer& a, const Integer& b);
  void sub(const Integer& a, const Integer& b);
  void mult(const Integer& a, const Integer& b);
  bool divisible(const Integer& a) const {
    return  mpz_divisible_p (&mMpz, &a.mMpz) != 0;
  }
  void div(const Integer& a, const Integer& b);
  void gcd(const Integer& a, const Integer& b);
  void pow(const Integer& a, unsigned long n);

  const char* get_str(int base) const;
};

}

#endif // GINV_INTEGER_INTEGER_H
