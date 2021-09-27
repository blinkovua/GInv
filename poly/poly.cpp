/***************************************************************************
 *   Copyright (C) 2021 by Blinkov Yu. A.                                  *
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

#include "poly.h"

namespace GInv {

int Poly::TOPlex(const Monom& a, const Monom& b) {
  int r=a.lex(b);
  if (r == 0 && a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  return r;
}

int Poly::TOPdeglex(const Monom& a, const Monom& b) {
  int r=a.deglex(b);
  if (r == 0 && a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  return r;
}

int Poly::TOPalex(const Monom& a, const Monom& b) {
  int r=a.alex(b);
  if (r == 0 && a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  return r;
}

int Poly::TOPlex2(const Monom& a, const Monom& b, const Monom& c) {
  int r=a.lex(b, c);
  if (r == 0 && a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  return r;
}

int Poly::TOPdeglex2(const Monom& a, const Monom& b, const Monom& c) {
  int r=a.deglex(b, c);
  if (r == 0 && a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  return r;
}

int Poly::TOPalex2(const Monom& a, const Monom& b, const Monom& c) {
  int r=a.alex(b, c);
  if (r == 0 && a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  return r;
}

int Poly::POTlex(const Monom& a, const Monom& b) {
  int r;
  if (a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  else
    r = a.lex(b);
  return r;
}

int Poly::POTdeglex(const Monom& a, const Monom& b) {
  int r;
  if (a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  else
    r = a.deglex(b);
  return r;
}

int Poly::POTalex(const Monom& a, const Monom& b) {
  int r;
  if (a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  else
    r = a.alex(b);
  return r;
}

int Poly::POTlex2(const Monom& a, const Monom& b, const Monom& c) {
  int r;
  if (a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  else
    r = a.lex(b, c);
  return r;
}

int Poly::POTdeglex2(const Monom& a, const Monom& b, const Monom& c) {
  int r;
  if (a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  else
    r = a.deglex(b, c);
  return r;
}

int Poly::POTalex2(const Monom& a, const Monom& b, const Monom& c) {
  int r;
  if (a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  else
    r = a.alex(b, c);
  return r;
}

void Poly::setOrder(int order) {
  switch(order) {
    case TOP_lex:
      mCmp1 = TOPlex;
      mCmp2 = TOPlex2;
      break;
    case TOP_deglex:
      mCmp1 = TOPdeglex;
      mCmp2 = TOPdeglex2;
      break;
    case TOP_alex:
      mCmp1 = TOPalex;
      mCmp2 = TOPalex2;
      break;
    case POT_lex:
      mCmp1 = POTlex;
      mCmp2 = POTlex2;
      break;
    case POT_deglex:
      mCmp1 = POTdeglex;
      mCmp2 = POTdeglex2;
      break;
    case POT_alex:
      mCmp1 = POTalex;
      mCmp2 = POTalex2;
      break;
    default:
      assert(false);
  }
}

void Poly::clear() {
  List<Term*>::Iterator i(mHead.begin());
  while(i) {
    mAllocator->destroy(i.data());
    i.del();
  }
}

void Poly::minus() {
  List<Term*>::ConstIterator i(mHead.begin());
  while(i) {
    i.data()->mCoeff.set_neg();
    ++i;
  }
  assert(assertValid());
}

void Poly::add(const Poly &a) {
  assert(mCmp1 == a.mCmp1 && mCmp2 == a.mCmp2);
  List<Term*>::ConstIterator ia(a.mHead.begin());
  List<Term*>::Iterator i(mHead.begin());
  while(i && ia)
    switch(mCmp1(i.data()->mMonom, ia.data()->mMonom)) {
    case 1:
      ++i;
      break;
    case -1:
      i.insert(new(mAllocator) Term(mAllocator, *ia.data()));
      ++i;
      ++ia;
      break;
    default:
      Integer tmp(mAllocator);
      tmp.add(i.data()->mCoeff, ia.data()->mCoeff);
      if (tmp.isZero()) {
        mAllocator->destroy(i.data());
        i.del();
      }
      else {
        i.data()->mCoeff.swap(tmp);
        ++i;
      }
      ++ia;
    }

  while(ia) {
    i.insert(new(mAllocator) Term(mAllocator, *ia.data()));
    ++i;
    ++ia;
  }
  assert(assertValid());
}

void Poly::sub(const Poly &a) {
  assert(mCmp1 == a.mCmp1 && mCmp2 == a.mCmp2);
  List<Term*>::ConstIterator ia(a.mHead.begin());
  List<Term*>::Iterator i(mHead.begin());
  while(i && ia)
    switch(i.data()->mMonom.deglex(ia.data()->mMonom)) {
    case 1:
      ++i;
      break;
    case -1:
      i.insert(new(mAllocator) Term(mAllocator, *ia.data()));
      i.data()->mCoeff.set_neg();
      ++i;
      ++ia;
      break;
    default:
      Integer tmp(mAllocator);
      tmp.sub(i.data()->mCoeff, ia.data()->mCoeff);
      if (tmp.isZero()) {
        mAllocator->destroy(i.data());
        i.del();
      }
      else {
        i.data()->mCoeff.swap(tmp);
        ++i;
      }
      ++ia;
    }

  while(ia) {
    i.insert(new(mAllocator) Term(mAllocator, *ia.data()));
    i.data()->mCoeff.set_neg();
    ++i;
    ++ia;
  }
  assert(assertValid());
}

void Poly::mult(const Poly &a) {
  assert(mCmp1 == a.mCmp1 && mCmp2 == a.mCmp2);
  if (mHead.length() == 0)
    ;
  else if (a.mHead.length() == 0)
    clear();
  else if (mHead.length() == 1 || a.mHead.length() == 1) {
    List<Term*> tmp(mAllocator);
    Monom &m=a.mHead.head()->mMonom;
    Integer &c=a.mHead.head()->mCoeff;
    List<Term*>::Iterator j(tmp.begin());
    List<Term*>::ConstIterator i;
    if (mHead.length() == 1)
      i = a.mHead.begin();
    else {
      assert(a.mHead.length() == 1);
      i = mHead.begin();
    }
    while(i) {
      j.insert(new(mAllocator) Term(mAllocator, i.data()->mMonom, m));
      j.data()->mCoeff.mult(i.data()->mCoeff, c);
      ++i;
      ++j;
    }
    mHead.swap(tmp);
  }
  else {
    List<Term*> sum(mAllocator);
    List<Term*>::ConstIterator i(mHead.begin());
    while(i) {
      List<Term*> tmp(mAllocator);
      Monom &m=i.data()->mMonom;
      Integer &c=i.data()->mCoeff;
      List<Term*>::Iterator j(tmp.begin());
      List<Term*>::ConstIterator i(mHead.begin());
      while(j) {
        j.insert(new(mAllocator) Term(mAllocator, i.data()->mMonom, m));
        j.data()->mCoeff.mult(i.data()->mCoeff, c);
        ++i;
        ++j;
      }
      ++i;

      List<Term*>::Iterator k(sum.begin());
      j = tmp.begin();
      while(k && j)
        switch(mCmp1(k.data()->mMonom, j.data()->mMonom)) {
        case 1:
          ++k;
          break;
        case -1:
          k.insert(j.get());
          ++k;
          break;
        default:
          Integer tmp(mAllocator);
          tmp.add(k.data()->mCoeff, j.data()->mCoeff);
          if (tmp.isZero()) {
            mAllocator->destroy(k.data());
            k.del();
          }
          else {
            k.data()->mCoeff.swap(tmp);
            ++k;
          }
          mAllocator->destroy(j.data());
          j.del();
        }

      while(j) {
        k.insert(j.get());
        ++k;
      }
    }
    mHead.swap(sum);
  }
  assert(assertValid());
}

void Poly::pow(int deg) {
  assert(deg >= 0);
  if (deg == 1)
    ;
  else if (deg == 0)
    clear();
  else {
    assert(deg > 1);
    Poly tmp(mAllocator, *this);
    for(--deg; deg > 0; deg--)
      tmp.mult(*this);
    swap(tmp);
  }
}

void Poly::reduction(const Poly& a) {
  assert(mCmp1 == a.mCmp1 && mCmp2 == a.mCmp2);
  assert(lm().divisiable(a.lm()));

  Monom m2(mAllocator, lm(), a.lm(), true);
  Integer tmp1(mAllocator), tmp2(mAllocator), k1(mAllocator), k2(mAllocator);
  tmp1.gcd(lc(), a.lc());
  k1.div(a.lc(), tmp1);
  k2.div(lc(), tmp1);
  k2.set_neg();

  List<Term*>::Iterator i(mHead.begin());
  List<Term*>::ConstIterator ia(a.mHead.begin());

  i.del();
  ++ia;
  while(i && ia) {
    switch(mCmp2(i.data()->mMonom, ia.data()->mMonom, m2)) {
    case 1:
      tmp1.mult(i.data()->mCoeff, k1);
      i.data()->mCoeff.swap(tmp1);
      ++i;
      break;
    case -1:
      i.insert(new(mAllocator) Term(mAllocator, ia.data()->mMonom, m2));
      i.data()->mCoeff.mult(ia.data()->mCoeff, k2);
      ++i;
      ++ia;
      break;
    default:
      tmp1.mult(i.data()->mCoeff, k1);
      tmp2.mult(ia.data()->mCoeff, k2);
      i.data()->mCoeff.add(tmp1, tmp2);
      if (i.data()->mCoeff.isZero()) {
        mAllocator->destroy(i.data());
        i.del();
      }
      else
        ++i;
      ++ia;
    }
  }
  while(i) {
    tmp1.mult(i.data()->mCoeff, k1);
    i.data()->mCoeff.swap(tmp1);
    ++i;
  }
  while(ia) {
      i.insert(new(mAllocator) Term(mAllocator, ia.data()->mMonom, m2));
      i.data()->mCoeff.mult(ia.data()->mCoeff, k2);
      ++i;
      ++ia;
  }
}

void Poly::redTail(List<Term*>::Iterator i1, const Poly& a) {
  assert(mCmp1 == a.mCmp1 && mCmp2 == a.mCmp2);
  assert(i1.data()->mMonom.divisiable(a.lm()));

  Monom m2(mAllocator, i1.data()->mMonom, a.lm(), true);
  Integer tmp1(mAllocator), tmp2(mAllocator), k1(mAllocator), k2(mAllocator);
  tmp1.gcd(i1.data()->mCoeff, a.lc());
  k1.div(a.lc(), tmp1);
  k2.div(i1.data()->mCoeff, tmp1);
  k2.set_neg();

  List<Term*>::Iterator i(mHead.begin());
  while(i != i1) {
    assert(i);
    tmp1.mult(i.data()->mCoeff, k1);
    i.data()->mCoeff.swap(tmp1);
    ++i;
  }

  List<Term*>::ConstIterator ia(a.mHead.begin());

  i.del();
  ++ia;
  while(i && ia) {
    switch(mCmp2(i.data()->mMonom, ia.data()->mMonom, m2)) {
    case 1:
      tmp1.mult(i.data()->mCoeff, k1);
      i.data()->mCoeff.swap(tmp1);
      ++i;
      break;
    case -1:
      i.insert(new(mAllocator) Term(mAllocator, ia.data()->mMonom, m2));
      i.data()->mCoeff.mult(ia.data()->mCoeff, k2);
      ++i;
      ++ia;
      break;
    default:
      tmp1.mult(i.data()->mCoeff, k1);
      tmp2.mult(ia.data()->mCoeff, k2);
      i.data()->mCoeff.add(tmp1, tmp2);
      if (i.data()->mCoeff.isZero()) {
        mAllocator->destroy(i.data());
        i.del();
      }
      else
        ++i;
      ++ia;
    }
  }
  while(i) {
    tmp1.mult(i.data()->mCoeff, k1);
    i.data()->mCoeff.swap(tmp1);
    ++i;
  }
  while(ia) {
      i.insert(new(mAllocator) Term(mAllocator, ia.data()->mMonom, m2));
      i.data()->mCoeff.mult(ia.data()->mCoeff, k2);
      ++i;
      ++ia;
  }
}

Poly::Poly(Allocator* allocator, const Poly& a):
      mAllocator(allocator),
      mHead(allocator),
      mCmp1(a.mCmp1),
      mCmp2(a.mCmp2) {
  List<Term*>::ConstIterator ia(a.mHead.begin());
  List<Term*>::Iterator i(mHead.begin());
  while(ia) {
    i.insert(new(mAllocator) Term(mAllocator, *ia.data()));
    ++ia;
    ++i;
  }
  assert(assertValid());
}

void Poly::operator=(const Poly &a) {
  assert(this != &a);
  clear();

  List<Term*>::Iterator i(mHead.begin());
  List<Term*>::ConstIterator ia(a.mHead.begin());
  while(ia) {
    i.insert(new(mAllocator) Term(mAllocator, *ia.data()));
    ++ia;
    ++i;
  }
  assert(assertValid());
}

int Poly::norm() const {
  int sum=0;
  List<Term*>::ConstIterator i(mHead.begin());
  while(i) {
    sum += i.data()->mCoeff.norm();
    ++i;
  }
  return sum;
}

// void Poly::nf(Janet &a);
// void Poly::nfTail(Janet &a);
// bool Poly::isPp() const;
// void Poly::pp();
//
// std::ostream& operator<<(std::ostream& out, const Poly &a);
//

bool Poly::assertValid() const {
  if (mHead) {
    List<Term*>::ConstIterator i(mHead.begin());
    if (i.data()->mCoeff.isZero())
      return false;
    List<Term*>::ConstIterator prev(i);
    ++i;
    while(i) {
      if (i.data()->mMonom.deglex(prev.data()->mMonom) >= 0)
        return false;
      if (i.data()->mCoeff.isZero())
        return false;
      prev = i;
      ++i;
    }
  }
  return true;
}

}