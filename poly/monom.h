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

#include "util/allocator.h"
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

  Monom(int size, Allocator* allocator);

public:
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
  Monom(int size, int pos, Allocator* allocator);
  Monom(Variable v, int size, int pos, Allocator* allocator);
  Monom(const Monom& a, Allocator* allocator);
  Monom(const Monom& a, Variable v, Allocator* allocator);
  Monom(const Monom& a, const Monom& b, Allocator* allocator);
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
    assert(pos >= -1);
    mPos = pos;
  }

  void operator*=(const Monom& a);
  Monom operator*(const Monom& a);
  Monom pow(int n) const;
  bool operator|(const Monom& a) const;
  Monom operator/(const Monom& a);

  int lex(const Monom& a) const;
  int deglex(const Monom& a) const;
  int alex(const Monom& a) const;

  friend std::ostream& operator<<(std::ostream& out, const Monom &a);

  bool assertValid() const;
};

}

#endif // GINV_POLY_MONOM_H
