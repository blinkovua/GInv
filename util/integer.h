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
  static size_t sBufferSize;
  static char*  sBuffer;

#ifdef GINV_UTIL_INTEGER_ALLOCATOR  
  
  static inline int abs(int a) {
    return (a >= 0) ? a: -a;
  }
  static inline int max(int a, int b) {
    return (a >= b) ? a: b;
  }
  static inline int min(int a, int b) {
    return (a >= b) ? b: a;
  }

  void reallocate(int size);

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
    reallocate(abs(a.mMpz._mp_size));
    mpz_set(&mMpz, &a.mMpz);
  }
  Integer(Allocator* allocator, const char *str);
  ~Integer() {
    if (mMpz._mp_alloc)
      mAllocator->dealloc(mMpz._mp_d, mMpz._mp_alloc);
//       delete[] mMpz._mp_d;
  }

  void swap(Integer& a) {
    assert(this != &a);
    auto tmp=mAllocator;
    mAllocator = a.mAllocator;
    a.mAllocator = tmp;
    mpz_swap(&mMpz, &a.mMpz);
  }
  void operator=(Integer&& a) {
    if (mMpz._mp_alloc)
      mAllocator->dealloc(mMpz._mp_d, mMpz._mp_alloc);
//       delete[] mMpz._mp_d;
    mAllocator = a.mAllocator;
    mMpz._mp_alloc = a.mMpz._mp_alloc;
    mMpz._mp_size = a.mMpz._mp_size;
    mMpz._mp_d = a.mMpz._mp_d;

    a.mMpz._mp_alloc = 0;
  }
  void operator=(const Integer& a) {
    reallocate(abs(a.mMpz._mp_size));
    mpz_set(&mMpz, &a.mMpz);
  }
  void set_si(signed long a) {
    reallocate(1);
    mpz_set_si(&mMpz, a);
  }
  void set_ui(unsigned long a) {
    reallocate(1);
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
  
#else  

protected:
  __mpz_struct mMpz;

public:
  Integer()=delete;
  explicit Integer(Allocator*) { mpz_init(&mMpz); }
  Integer(const Integer &a)=delete;
  Integer(Integer &&a) {
    mMpz._mp_alloc = a.mMpz._mp_alloc;
    mMpz._mp_size = a.mMpz._mp_size;
    mMpz._mp_d = a.mMpz._mp_d;
    a.mMpz._mp_alloc = 0;
  }
  Integer(Allocator*, const Integer &a){
    mpz_init(&mMpz);
    mpz_init_set(&mMpz, &a.mMpz);
  }
  Integer(Allocator* allocator, const char *str) {
    mpz_init(&mMpz);
    int r=mpz_set_str(&mMpz, str, 0);
    assert(r == 0);
  }
  ~Integer() { mpz_clear(&mMpz); }

  void swap(Integer& a) {
    assert(this != &a);
    mpz_swap(&mMpz, &a.mMpz);
  }
  void operator=(Integer&& a) {
    mpz_set(&mMpz, &a.mMpz);
    mMpz._mp_alloc = a.mMpz._mp_alloc;
    mMpz._mp_size = a.mMpz._mp_size;
    mMpz._mp_d = a.mMpz._mp_d;

    a.mMpz._mp_alloc = 0;
  }
  void operator=(const Integer& a) { mpz_set(&mMpz, &a.mMpz); }
  void set_si(signed long a) { mpz_set_si(&mMpz, a); }
  void set_ui(unsigned long a) { mpz_set_ui(&mMpz, a); }

  bool isZero() const { return mpz_sgn(&mMpz) == 0; }
  bool isOne() const { return mpz_cmp_ui(&mMpz, 1ul) == 0; }
  bool isAbsOne() const { return  mpz_cmpabs_ui(&mMpz, 1ul) == 0; }
  int sgn() const { return mpz_sgn(&mMpz); }
  int norm() const { return abs(mMpz._mp_size); }

  void set_neg() { mMpz._mp_size *= -1; }
  void add(const Integer& a) {
    mpz_add(&mMpz, &mMpz, &a.mMpz);
  }
  void add(const Integer& a, const Integer& b) {
    assert(this != &a);
    assert(this != &b);
    mpz_add(&mMpz, &a.mMpz, &b.mMpz);
  }
  void sub(const Integer& a) {
    mpz_sub(&mMpz, &mMpz, &a.mMpz);
  }
  void sub(const Integer& a, const Integer& b) {
    assert(this != &a);
    assert(this != &b);
    mpz_sub(&mMpz, &a.mMpz, &b.mMpz);
  }
  void mult(const Integer& a) {
    mpz_mul(&mMpz, &mMpz, &a.mMpz);
  }
  void mult(const Integer& a, const Integer& b) {
    assert(this != &a);
    assert(this != &b);
    mpz_mul(&mMpz, &a.mMpz, &b.mMpz);
  }
  bool divisible(const Integer& a) const {
    return  mpz_divisible_p (&mMpz, &a.mMpz) != 0;
  }
  void div(const Integer& a) {
    assert(divisible(a));
    mpz_divexact(&mMpz, &mMpz, &a.mMpz);
  }
  void div(const Integer& a, const Integer& b) {
    assert(this != &a);
    assert(this != &b);
    assert(a.divisible(b));
    mpz_divexact(&mMpz, &a.mMpz, &b.mMpz);
  }
  void gcd(const Integer& a) {
    assert(!isZero() && !a.isZero());
    mpz_gcd(&mMpz, &mMpz, &a.mMpz);
  }
  void gcd(const Integer& a, const Integer& b) {
    assert(this != &a);
    assert(this != &b);
    assert(!a.isZero() && !b.isZero());
    mpz_gcd(&mMpz, &a.mMpz, &b.mMpz);
  }
  
#endif // GINV_UTIL_INTEGER_ALLOCATOR

  const char* get_str(int base) const;
};

}

#endif // GINV_UTIL_INTEGER_H
