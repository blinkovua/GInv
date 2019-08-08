/***************************************************************************
 *   Copyright (C) 2015 by Blinkov Yu. A.                                  *
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

#ifndef GINV_POLY_MONOM_H
#define GINV_POLY_MONOM_H

#include <utility>
#include <random>

#include "util/allocator.h"
#include "util/randpermutation.h"
#include "config.h"

namespace GInv {

class Monom {
public:
  typedef int Variable;

protected:
  Allocator*  mAllocator;
  int         mSize;
  int         mPos;
  int         mDegree;
  Variable*   mVariables;

  static int                                             sSize;
  static std::uniform_int_distribution<int>*             sDis1;
  static std::uniform_int_distribution<Monom::Variable>* sDis2;
  
  void mult1(const Monom& a);

public:
  static void rand_init(int size, int deg1, int deg2);
  static Monom next(Allocator* allocator);

  Monom()=delete;
  Monom(const Monom& a)=delete;
  Monom(Monom&& a):
      mAllocator(a.mAllocator),
      mSize(a.mSize),
      mPos(a.mPos),
      mDegree(a.mDegree),
      mVariables(a.mVariables) {
    a.mSize = 0;
  }
  Monom(Allocator* allocator, int size, int pos);
  Monom(Allocator* allocator, Variable v, int size, int pos);
  Monom(Allocator* allocator, const Monom& a);
  Monom(Allocator* allocator, const Monom& a, RandPermutation &p);
  Monom(Allocator* allocator, Variable v, const Monom& a);
  Monom(Allocator* allocator, const Monom& a, const Monom& b);
  Monom(Allocator* allocator, const Monom& a, const Monom& b, bool);
  Monom(Allocator* allocator, const Monom& a, int n);
  ~Monom() {
    if (mSize)
      mAllocator->dealloc(mVariables, mSize);
  }

  void swap(Monom& a);
  void operator=(Monom &&a) {
    assert(this != &a);
    if (mSize)
      mAllocator->dealloc(mVariables, mSize);

    mAllocator = a.mAllocator;
    mSize = a.mSize;
    mPos = a.mPos;
    mDegree = a.mDegree;
    mVariables = a.mVariables;
    a.mSize = 0;
  }
  void operator=(const Monom &a);

  operator bool() const { return mDegree > 0; }
  bool isZero() const { return mDegree == 0; }

  int size() const { return mSize; }
  int pos() const { return mPos; }
  int degree() const { return mDegree; }

  Variable operator[](int i) const {
    assert(0 <= i && i < mSize);
    return mVariables[i];
  }

  void setPos(int pos) {
    assert(mPos == -1);
    mPos = pos;
  }

  bool divisiable(const Monom& a) const;

  int lex(const Monom& a) const;
  int deglex(const Monom& a) const;
  int alex(const Monom& a) const;

  friend inline Monom operator*(Monom&& a, const Monom& b) {
    Monom r(std::move(a));
    r.mult1(b);
    return std::move(r);
  }

  friend std::ostream& operator<<(std::ostream& out, const Monom &a);

  bool assertValid() const;
};


}

#endif // GINV_POLY_MONOM_H
