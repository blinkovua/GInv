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

#include "monom.h"

namespace GInv {

int Monom::sSize=0;
std::uniform_int_distribution<int>* Monom::sDis1=nullptr;
std::uniform_int_distribution<Monom::Variable>* Monom::sDis2=nullptr;

void Monom::rand_init(int size, int deg1, int deg2) {
  assert(size > 0);
  assert(0 < deg1 && deg1 <= deg2);
  delete sDis1;
  delete sDis2;
  sDis1 = new std::uniform_int_distribution<int>(deg1, deg2);
  sSize = size;
  sDis2 = new std::uniform_int_distribution<Variable>(0, sSize-1);
}

void Monom::rand_next(Monom& a) {
  assert(sDis1 && sDis2 && sSize);
  static std::random_device rd;
  static std::mt19937 gen(rd());
  assert(a.mSize == sSize);

  Variable* i=a.mVariables;
  const Variable* const iend=a.mVariables + a.mSize;
  do {
    *i++ = 0;
  } while(i < iend);

  int deg=(*sDis1)(gen);
  a.mDegree = deg;
  do {
    int var=(*sDis2)(gen);
    assert(0 <= var && var < sSize);
    a.mVariables[var]++;
    --deg;
  } while(deg);
  assert(a.assertValid());
}

Monom* Monom::rand_next(Allocator* allocator) {
  Monom* r=new Monom(allocator, sSize, -1);
  rand_next(*r);
  return r;
}

Monom::Monom(Allocator* allocator, int size, int pos):
    mAllocator(allocator),
    mSize(size),
    mPos(pos),
    mDegree(0),
    mVariables(new(allocator) Variable[mSize]) {
  assert(mSize > 0);
  assert(mPos >= -1);
  Variable* i=mVariables;
  const Variable* const iend=mVariables+mSize;
  do {
    *i++ = 0;
  } while(i < iend);
  assert(assertValid());
}

Monom::Monom(Allocator* allocator, Variable v, int size, int pos):
    mAllocator(allocator),
    mSize(size),
    mPos(pos),
    mDegree(1),
    mVariables(new(allocator) Variable[size]) {
  assert(mSize > 0);
  assert(mPos >= -1);
  assert(0 <= v && v < mSize);
  Variable* i=mVariables;
  const Variable* const iend=mVariables+mSize;
  do {
    *i++ = 0;
  } while(i < iend);
  ++mVariables[v];
  assert(assertValid());
}

Monom::Monom(Allocator* allocator, const Monom& a):
    mAllocator(allocator),
    mSize(a.mSize),
    mPos(a.mPos),
    mDegree(a.mDegree),
    mVariables(new(allocator) Variable[a.mSize]) {
  Variable* i=mVariables;
  const Variable* const iend=mVariables+mSize;
  const Variable* ia=a.mVariables;
//   for(int k=0; k < mSize; k++)
//     mVariables[k] = a.mVariables[k];
  do {
    *i++ = *ia++;
  } while(i < iend);
  assert(assertValid());
}

Monom::Monom(Allocator* allocator, Variable v, const Monom& a):
    mAllocator(allocator),
    mSize(a.mSize),
    mPos(a.mPos),
    mDegree(a.mDegree+1),
    mVariables(new(allocator) Variable[a.mSize]) {
  assert(0 <= v && v < mSize);
  Variable* i=mVariables;
  const Variable* const iend=mVariables+mSize;
  const Variable* ia=a.mVariables;
  do {
    *i++ = *ia++;
  } while(i < iend);
  ++mVariables[v];
  assert(assertValid());
}

Monom::Monom(Allocator* allocator, const Monom& a, const Monom& b):
    mAllocator(allocator),
    mSize(a.mSize),
    mPos((a.mPos == -1) ? b.mPos: a.mPos),
    mDegree(a.mDegree+b.mDegree),
    mVariables(new(allocator) Variable[a.mSize]) {
  assert(a.mSize == b.mSize);
  assert(a.mPos == -1 || b.mPos == -1);
  Variable* i=mVariables;
  const Variable* const iend=mVariables+mSize;
  const Variable *ia=a.mVariables,
                 *ib=b.mVariables;
  if (b.mPos >= 0) {
    assert(a.mPos == -1);
    mPos = b.mPos;
  }
  do {
    *i++ = *ia++ + *ib++;
  } while(i < iend);
  assert(assertValid());
}

Monom::Monom(Allocator* allocator, const Monom& a, int n):
    mAllocator(allocator),
    mSize(a.mSize),
    mPos(a.mPos),
    mDegree(a.mDegree*n),
    mVariables(new(allocator) Variable[a.mSize]) {
  assert(n >= 0);
  Variable *i=mVariables;
  const Variable* const iend=mVariables+mSize;
  const Variable *ia=a.mVariables;
  do {
    *i++ = (*ia++)*n;
  } while(i < iend);
  assert(assertValid());
}

void Monom::swap(Monom& a) {
  assert(this != &a);
  assert(mSize == a.mSize);

  auto tmp1=mAllocator;
  mAllocator = a.mAllocator;
  a.mAllocator = tmp1;

  auto tmp2=mSize;
  mSize = a.mSize;
  a.mSize = tmp2;

  tmp2 = mPos;
  mPos = a.mPos;
  a.mPos = tmp2;

  tmp2 = mDegree;
  mDegree = a.mDegree;
  a.mDegree = tmp2;

  auto tmp3=mVariables;
  mVariables = a.mVariables;
  a.mVariables = tmp3;
}

void Monom::operator=(const Monom &a) {
  assert(mSize == a.mSize);
  mPos = a.mPos;
  mDegree = a.mDegree;
  Variable* i=mVariables;
  const Variable* const iend=mVariables+mSize;
  const Variable* ia=a.mVariables;
  do {
    *i++ = *ia++;
  } while(i < iend);
}

void Monom::permutation(const Monom &a, RandPermutation &p) {
  assert(mSize == a.size());
  assert(mSize == p.size());
  mDegree = a.mDegree;
  for(int v=0; v < mSize; v++)
    mVariables[v] = a.mVariables[p[v]];
  assert(assertValid());
}

bool Monom::divisiable(const Monom& a) const {
  assert(mSize == a.mSize);
  bool r=a.mPos == -1 || mPos == a.mPos;
  if (r) {
    const Variable *i=mVariables,
                   *ia=a.mVariables;
    const Variable* const iend=mVariables+mSize;
    do {
      r = *i++ >= *ia++;
    } while(r && i < iend);
  }
  return r;
}

void Monom::mult(const Monom& a) {
  assert(mSize == a.mSize);
  assert(mPos == -1 || a.mPos == -1);
  if (mPos == -1)
    mPos = a.mPos;
  mDegree +=a.mDegree;
  Variable *i=mVariables;
  const Variable* const iend=mVariables+mSize;
  const Variable* ia=a.mVariables;
  do {
    *i++ += *ia++;
  } while(i < iend);
  assert(assertValid());
}

void Monom::div(const Monom& a) {
  assert(divisiable(a));
  mDegree -=a.mDegree;
  Variable *i=mVariables;
  const Variable* const iend=mVariables+mSize;
  const Variable* ia=a.mVariables;
  do {
    *i++ -= *ia++;
  } while(i < iend);
  assert(assertValid());
}

int Monom::gcd(const Monom& a) const {
  assert(mSize == a.mSize);
  assert(mPos == -1 || a.mPos == -1);
  int r=0;
  for(int i=0; i < mSize; i++)
    r += (mVariables[i] >= a.mVariables[i]) ? mVariables[i]: a.mVariables[i];
  return r;
}

int Monom::lcm(const Monom& a) const {
  assert(mSize == a.mSize);
  assert(mPos == -1 || a.mPos == -1);
  int r=0;
  for(int i=0; i < mSize; i++)
    r += (mVariables[i] <= a.mVariables[i]) ? mVariables[i]: a.mVariables[i];
  return r;

}

int Monom::lex(const Monom& a) const {
  assert(mSize == a.mSize);
  int r=0;
  const Variable *i=mVariables,
                 *ia=a.mVariables;
  const Variable* const iend=mVariables+mSize;
  do {
    if (*i != *ia) {
      r = (*i > *ia) ? 1: -1;
      break;
    }
    ++i;
    ++ia;
  } while(i < iend);
  return r;
}

int Monom::deglex(const Monom& a) const {
  assert(mSize == a.mSize);
  int r=0;
  if (mDegree > a.mDegree)
    r = 1;
  else if (mDegree < a.mDegree)
    r = -1;
  else if (mDegree) {
    assert(mDegree == a.mDegree);
    const Variable *i=mVariables + mSize - 1,
                   *ia=a.mVariables + mSize - 1;
    do {
      if (*i != *ia) {
        r = (*i > *ia) ? -1: 1;
        break;
      }
      --i;
      --ia;
    } while(i >= mVariables);
  }
  return r;
}

int Monom::alex(const Monom& a) const {
  assert(mSize == a.mSize);
  int r=0;
  if (mDegree > a.mDegree)
    r = -1;
  else if (mDegree < a.mDegree)
    r = 1;
  else if (mDegree) {
    assert(mDegree == a.mDegree);
    const Variable *i=mVariables,
                   *ia=a.mVariables;
    const Variable* const iend=mVariables+mSize;
    do {
      if (*i != *ia) {
        r = (*i > *ia) ? 1: -1;
        break;
      }
      ++i;
      ++ia;
    } while(i < iend);
  }
  return r;
}


int Monom::lex(const Monom& a, const Monom& b) const {
  assert(mSize == a.mSize && mSize == b.mSize);
  assert(a.mPos == -1 || b.mPos == -1);
  int r=0;
  const Variable *i=mVariables,
                 *ia=a.mVariables,
                 *ib=b.mVariables;
  const Variable* const iend=mVariables+mSize;
  do {
    if (*i != *ia + *ib) {
      r = (*i > *ia + *ib) ? 1: -1;
      break;
    }
    ++i;
    ++ia;
    ++ib;
  } while(i < iend);
  return r;
}

int Monom::deglex(const Monom& a, const Monom& b) const {
  assert(mSize == a.mSize && mSize == b.mSize);
  assert(a.mPos == -1 || b.mPos == -1);
  int r=0;
  if (mDegree > a.mDegree + b.mDegree)
    r = 1;
  else if (mDegree < a.mDegree + b.mDegree)
    r = -1;
  else if (mDegree) {
    assert(mDegree == a.mDegree + b.mDegree);
    const Variable *i=mVariables + mSize - 1,
                   *ia=a.mVariables + mSize - 1,
                   *ib=b.mVariables + mSize - 1;
    do {
      if (*i != *ia + *ib) {
        r = (*i > *ia + *ib) ? -1: 1;
        break;
      }
      --i;
      --ia;
      --ib;
    } while(i >= mVariables);
  }
  return r;
}

int Monom::alex(const Monom& a, const Monom& b) const {
  assert(mSize == a.mSize && mSize == b.mSize);
  assert(a.mPos == -1 || b.mPos == -1);
  int r=0;
  if (mDegree > a.mDegree + b.mDegree)
    r = -1;
  else if (mDegree < a.mDegree + b.mDegree)
    r = 1;
  else if (mDegree) {
    assert(mDegree == a.mDegree + b.mDegree);
    const Variable *i=mVariables,
                   *ia=a.mVariables,
                   *ib=b.mVariables;
    const Variable* const iend=mVariables+mSize;
    do {
      if (*i != *ia + *ib) {
        r = (*i > *ia + *ib) ? 1: -1;
        break;
      }
      ++i;
      ++ia;
      ++ib;
    } while(i < iend);
  }
  return r;
}


std::ostream& operator<<(std::ostream& out, const Monom &a) {
  out << '[';
  if (a.pos() >=0)
    out << a.pos() << ':';
  out << a[0];
  for(int i=1; i < a.size(); i++)
    out << ' ' << a[i];
  out << ']';
  return out;
}

bool Monom::assertValid() const {
  bool r=mSize > 0;
  r = r && mPos >= -1;
  if (r) {
    const Variable* i=mVariables;
    const Variable* const iend=mVariables+mSize;
    Variable sum=0;
    do {
      sum += *i++;
    } while(i < iend);
    r = r && sum == mDegree;
  }
  return r;
}

}
