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

#include "bmonom.h"

namespace GInv {

BMonom::BMonom(const BMonom& a, Allocator* allocator):
    mAllocator(allocator),
    mSize(a.mDegree),
    mDegree(a.mDegree),
    mVariables(nullptr) {
  if (mDegree) {
    mVariables = new(mAllocator) Variable[mSize];
    Variable* i=mVariables;
    const Variable* ia=a.mVariables;
    const Variable* const iend=mVariables + mDegree;
    do {
      *i++ = *ia++;
    } while(i < iend);
  }
  assert(assertValid());
}

BMonom::BMonom(const BMonom& a, Variable v, Allocator* allocator):
    mAllocator(allocator),
    mSize(a.mDegree + 1),
    mDegree(mSize),
    mVariables(new(mAllocator) Variable[mSize]) {
  Variable *i=mVariables;
  if (mDegree == 1)
    *i++ = v;
  else {
    const Variable *ia=a.mVariables;
    const Variable *const iaend=a.mVariables + a.mDegree;
    while(ia < iaend && *ia > v)
      *i++ = *ia++;
    *i++ = v;
    if (ia < iaend && *ia == v) {
      ++ia;
      --mDegree;
    }
    while(ia < iaend)
      *i++ = *ia++;
  }
  assert(assertValid());
}

BMonom::BMonom(const BMonom& a, const BMonom& b, Allocator* allocator):
    mAllocator(allocator),
    mSize(a.mDegree + b.mDegree),
    mDegree(a.mDegree + b.mDegree),
    mVariables(nullptr)  {
  if (mDegree) {
    mVariables = new(mAllocator) Variable[mDegree];
    Variable *i=mVariables;
    const Variable *ia=a.mVariables,
                   *ib=b.mVariables;
    const Variable *const iaend=a.mVariables + a.mDegree,
                   *const ibend=b.mVariables + b.mDegree;
    while(ia < iaend && ib < ibend) {
      if (*ia > *ib)
        *i++ = *ia++;
      else if (*ia < *ib)
        *i++ = *ib++;
      else {
        *i++ = *ia++;
        ++ib;
        --mDegree;
      }
    }
    while(ia < iaend)
      *i++ = *ia++;
    while(ib < ibend)
      *i++ = *ib++;
  }
  assert(assertValid());
}

void BMonom::swap(BMonom& a) {
  Allocator *tmp1=mAllocator;
  mAllocator = a.mAllocator;
  a.mAllocator = tmp1;

  int tmp2=mSize;
  mSize = a.mSize;
  a.mSize = tmp2;

  tmp2 = mDegree;
  mDegree = a.mDegree;
  a.mDegree = tmp2;

  Variable* tmp3=mVariables;
  mVariables = a.mVariables;
  a.mVariables = tmp3;
}

void BMonom::operator=(const BMonom& a) {
  assert(this != &a);
  mDegree = a.mDegree;
  if (mSize < mDegree) {
    assert(mDegree > 0);
    if (mSize)
      mAllocator->dealloc(mVariables, mSize);
    mSize = mDegree;
    mVariables = new(mAllocator) Variable[mSize];
  }
  if (mDegree) {
    Variable* i=mVariables;
    const Variable* ia=a.mVariables;
    const Variable* const iend=mVariables + mDegree;
    do {
      *i++ = *ia++;
    } while(i < iend);
  }
  assert(assertValid());
}

bool BMonom::divisible(const BMonom& a) const {
  if (a.mDegree == 0)
    return true;
  if (mDegree < a.mDegree)
    return false;
  const Variable *i=mVariables,
                 *ia=a.mVariables;
  const Variable *const iend=mVariables + mDegree,
                 *const iaend=a.mVariables + a.mDegree;

  while(i < iend && ia < iaend) {
    if (*i > *ia)
      ++i;
    else if (*i < *ia)
      break;
    else {
      ++i;
      ++ia;
    }
  }
  return ia == iaend;
}

void BMonom::div(const BMonom& a, const BMonom& b) {
  assert(a.divisible(b));
  int size=a.mDegree - b.mDegree,
      degree=size;
  Variable *i=nullptr;
  if (size) {
    i = new(mAllocator) Variable[size];
    const Variable *ia=a.mVariables,
                   *ib=b.mVariables;
    const Variable *const iaend=a.mVariables + a.mDegree,
                   *const ibend=b.mVariables + b.mDegree;
    while(ib < ibend) {
      assert(ia < iaend);
      if (*ia > *ib)
        *i++ = *ia++;
      else {
        assert(*ia == *ib);
        ++ia;
        ++ib;
      }
    }
    assert(ia <= iaend);
    while(ia < iaend)
      *i++ = *ia++;
  }
  if (mSize)
    mAllocator->dealloc(mVariables, mSize);
  mSize = size;
  mDegree = degree;
  mVariables = i - degree;
  assert(assertValid());
}

int BMonom::compare(const BMonom& a) const {
  int r = 0;
  if (mDegree > a.mDegree)
    r = 1;
  else if (mDegree < a.mDegree)
    r = -1;
  else if (mDegree) {
    assert(mDegree == a.mDegree);
    const Variable *i=mVariables + mDegree - 1,
                   *ia=a.mVariables + a.mDegree - 1;
    do {
      if (*i != *ia) {
        r = (*i > *ia) ? 1: -1;
        break;
      }
      --i;
      --ia;
    } while(i >= mVariables);
  }
  return r;
}

// int BMonom::compare(const BMonom& a) const {
//   int r = 0;
//   if (mDegree != 0 || a.mDegree != 0) {
//     const Variable *i=mVariables,
//                    *ia=a.mVariables;
//     const Variable *const iend=mVariables + mDegree,
//                    *const iaend=a.mVariables + a.mDegree;
//     while(i < iend && ia < iaend && *i == *ia) {
//       ++i;
//       ++ia;
//     }
//     if (i < iend)
//       if (ia < iaend)
//         r = (*i > *ia) ? 1: -1;
//       else
//         r = 1;
//     else if (ia < iaend)
//       r = -1;
//   }
//   return r;
// }

bool BMonom::assertValid() const {
  bool r = mDegree <= mSize;
  if (r && mDegree) {
    const Variable *i=mVariables;
    const Variable *const iend=mVariables + mDegree;
    Variable prev = *i++;
    while(i < iend) {
      if (prev <= *i) {
        r = false;
        break;
      }
      prev = *i++;
    }
  }
  return r;
}

std::ostream& operator<<(std::ostream& out, const BMonom &a) {
  if (a.isZero())
    out << '1';
  else {
    out << '[' << a[0];
    for(int i=1; i < a.degree(); i++)
      out << ' ' << a[i];
    return out << ']';
  }
  return out;
}

}
