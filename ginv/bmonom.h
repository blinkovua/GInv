/***************************************************************************
 *   Copyright (C) 2016 by Blinkov Yu. A. and Fokin P. V.                  *
 *   BlinkovUA@info.sgu.ru and fokinpv@gmail.com                           *
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

#ifndef GINV_BMONOM_H
#define GINV_BMONOM_H

#include <cassert>

#include "util/list.h"
#include "config.h"

namespace GInv {

class BMonom {
public:
  typedef int Variable;

protected:
  Allocator*  mAllocator;
  int         mSize;
  int         mDegree;
  Variable*   mVariables;

public:
  BMonom() = delete;
  BMonom(const BMonom& a) = delete;
  BMonom(Allocator* allocator):
      mAllocator(allocator),
      mSize(0),
      mDegree(0),
      mVariables(nullptr) {}
  BMonom(Variable v, Allocator* allocator):
      mAllocator(allocator),
      mSize(1),
      mDegree(1),
      mVariables(new(mAllocator) Variable[mSize]) {
    *mVariables = v;
  }
  BMonom(const BMonom& a, Allocator* allocator);
  BMonom(const BMonom& a, Variable v, Allocator* allocator);
  BMonom(const BMonom& a, const BMonom& b, Allocator* allocator);
  ~BMonom() {
    if (mSize)
      mAllocator->dealloc(mVariables, mSize);
  }

  void swap(BMonom& a);
  void operator=(const BMonom &a);

  operator bool() const { return mDegree > 0; }
  bool isZero() const { return mDegree == 0; }

  int degree() const { return mDegree; }
  Variable operator[](int i) const {
    assert(0 <= i && i < mDegree);
    return mVariables[i];
  }

  bool divisible(const BMonom& a) const;
  void div(const BMonom& a, const BMonom& b);

  int compare(const BMonom& a) const;

  friend std::ostream& operator<<(std::ostream& out, const BMonom &a);

  bool assertValid() const;
};

}

#endif // GINV_BMONOM_H
