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

#include "poly_int.h"

namespace GInv {

int PolyInt::TOPlex(const Monom& a, const Monom& b) {
  int r=a.lex(b);
  if (r == 0 && a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  return r;
}

int PolyInt::TOPdeglex(const Monom& a, const Monom& b) {
  int r=a.deglex(b);
  if (r == 0 && a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  return r;
}

int PolyInt::TOPalex(const Monom& a, const Monom& b) {
  int r=a.alex(b);
  if (r == 0 && a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  return r;
}

int PolyInt::TOPlex2(const Monom& a, const Monom& b, const Monom& c) {
  int r=a.lex(b, c);
  if (r == 0 && a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  return r;
}

int PolyInt::TOPdeglex2(const Monom& a, const Monom& b, const Monom& c) {
  int r=a.deglex(b, c);
  if (r == 0 && a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  return r;
}

int PolyInt::TOPalex2(const Monom& a, const Monom& b, const Monom& c) {
  int r=a.alex(b, c);
  if (r == 0 && a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  return r;
}

int PolyInt::POTlex(const Monom& a, const Monom& b) {
  int r;
  if (a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  else
    r = a.lex(b);
  return r;
}

int PolyInt::POTdeglex(const Monom& a, const Monom& b) {
  int r;
  if (a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  else
    r = a.deglex(b);
  return r;
}

int PolyInt::POTalex(const Monom& a, const Monom& b) {
  int r;
  if (a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  else
    r = a.alex(b);
  return r;
}

int PolyInt::POTlex2(const Monom& a, const Monom& b, const Monom& c) {
  int r;
  if (a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  else
    r = a.lex(b, c);
  return r;
}

int PolyInt::POTdeglex2(const Monom& a, const Monom& b, const Monom& c) {
  int r;
  if (a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  else
    r = a.deglex(b, c);
  return r;
}

int PolyInt::POTalex2(const Monom& a, const Monom& b, const Monom& c) {
  int r;
  if (a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  else
    r = a.alex(b, c);
  return r;
}

void PolyInt::setOrder(int order) {
  switch(order) {
    case TOP | lex:
      mCmp1 = TOPlex;
      mCmp2 = TOPlex2;
      break;
    case TOP | deglex:
      mCmp1 = TOPdeglex;
      mCmp2 = TOPdeglex2;
      break;
    case TOP | alex:
      mCmp1 = TOPalex;
      mCmp2 = TOPalex2;
      break;
    case POT | lex:
      mCmp1 = POTlex;
      mCmp2 = POTlex2;
      break;
    case POT | deglex:
      mCmp1 = POTdeglex;
      mCmp2 = POTdeglex2;
      break;
    case POT | alex:
      mCmp1 = POTalex;
      mCmp2 = POTalex2;
      break;
    default:
      assert(false);
  }
}

void PolyInt::clear() {
  List<Term*>::Iterator i(mHead.begin());
  while(i) {
    mAllocator->destroy(i.data());
    i.del();
  }
}

PolyInt::PolyInt(Allocator* allocator, Monom::Variable v, const PolyInt& a):
      mAllocator(allocator),
      mOrder(a.mOrder),
      mSize(a.mSize),
      mHead(allocator),
      mCmp1(a.mCmp1),
      mCmp2(a.mCmp2) {
  List<Term*>::ConstIterator ia(a.mHead.begin());
  List<Term*>::Iterator i(mHead.begin());
  while(ia) {
    i.insert(new(mAllocator) Term(mAllocator, v, *ia.data()));
    ++ia;
    ++i;
  }
  assert(assertValid());
}

PolyInt::PolyInt(Allocator* allocator, const PolyInt& a):
      mAllocator(allocator),
      mOrder(a.mOrder),
      mSize(a.mSize),
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

void PolyInt::swap(PolyInt& a) {//TODO
  assert(compare(a));
  auto tmp=mAllocator;
  mAllocator = a.mAllocator;
  a.mAllocator = tmp;

  mHead.swap(a.mHead);
}

void PolyInt::operator=(PolyInt &&a) {//TODO
  assert(this != &a);
  clear();
  swap(a);
}


void PolyInt::operator=(const PolyInt &a) {
  assert(this != &a);
  clear();

  mOrder = a.mOrder;
  mSize = a.mSize;
  mCmp1 = a.mCmp1;
  mCmp2 = a.mCmp2;

  List<Term*>::Iterator i(mHead.begin());
  List<Term*>::ConstIterator ia(a.mHead.begin());
  while(ia) {
    i.insert(new(mAllocator) Term(mAllocator, *ia.data()));
    ++ia;
    ++i;
  }
  assert(assertValid());
}

int PolyInt::norm() const {
  int sum=0;
  List<Term*>::ConstIterator i(mHead.begin());
  while(i) {
    sum += i.data()->mCoeff.norm();
    ++i;
  }
  return sum;
}


void PolyInt::minus() {
  List<Term*>::ConstIterator i(mHead.begin());
  while(i) {
    i.data()->mCoeff.set_neg();
    ++i;
  }
  assert(assertValid());
}

void PolyInt::add(const char* hex) {
  assert(mHead);
  List<Term*>::Iterator i(mHead.begin());
  assert(i);
  int size=i.data()->mMonom.size();
  while(i && (i.data()->mMonom.degree() > 0 ||
              i.data()->mMonom.pos() >= 0))
    ++i;
  if (!i)
    i.insert(new(mAllocator) Term(mAllocator, size, hex));
  else {
    assert(i.data()->mMonom.degree() == 0 &&
           i.data()->mMonom.pos() == -1);
    Integer a(mAllocator, hex);
    i.data()->mCoeff.add(a);
    if (i.data()->mCoeff.isZero()) {
      mAllocator->destroy(i.data());
      i.del();
    }
  }
  assert(assertValid());
}

void PolyInt::add(const PolyInt &a) {
  assert(compare(a));
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
      i.data()->mCoeff.add(ia.data()->mCoeff);
      if (!i.data()->mCoeff.isZero())
        ++i;
      else {
        mAllocator->destroy(i.data());
        i.del();
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

void PolyInt::sub(const char* hex) {
  assert(mHead);
  List<Term*>::Iterator i(mHead.begin());
  assert(i);
  int size=i.data()->mMonom.size();
  while(i && (i.data()->mMonom.degree() > 0 ||
              i.data()->mMonom.pos() >= 0))
    ++i;
  if (!i) {
    i.insert(new(mAllocator) Term(mAllocator, size, hex));
    i.data()->mCoeff.set_neg();
  }
  else {
    assert(i.data()->mMonom.degree() == 0 &&
           i.data()->mMonom.pos() == -1);
    Integer a(mAllocator, hex);
    i.data()->mCoeff.sub(a);
    if (i.data()->mCoeff.isZero()) {
      mAllocator->destroy(i.data());
      i.del();
    }
  }
  assert(assertValid());
}

void PolyInt::sub(const PolyInt &a) {
  assert(compare(a));
  List<Term*>::ConstIterator ia(a.mHead.begin());
  List<Term*>::Iterator i(mHead.begin());
  while(i && ia)
    switch(mCmp1(i.data()->mMonom, ia.data()->mMonom)) {
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
      i.data()->mCoeff.sub(ia.data()->mCoeff);
      if (!i.data()->mCoeff.isZero())
        ++i;
      else {
        mAllocator->destroy(i.data());
        i.del();
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

void PolyInt::mult(const char* hex) {
  if (mHead.length() == 0)
    ;
  else {
    Integer c(mAllocator, hex);
    if (c.isZero())
      clear();
    else {
      List<Term*>::ConstIterator i(mHead.begin());
      while(i) {
        i.data()->mCoeff.mult(c);
        ++i;
      }
    }
  }
  assert(assertValid());
}

void PolyInt::mult(const PolyInt &a) {
  assert(compare(a));
  if (mHead.length() == 0)
    ;
  else if (a.mHead.length() == 0)
    clear();
  else if (mHead.length() == 1 || a.mHead.length() == 1) {
    List<Term*> tmp(mAllocator);
    List<Term*>::Iterator j(tmp.begin());
    if (mHead.length() == 1) {
      Monom &m=mHead.head()->mMonom;
      Integer &c=mHead.head()->mCoeff;

      List<Term*>::ConstIterator i(a.mHead.begin());
      while(i) {
        j.insert(new(mAllocator) Term(mAllocator, i.data()->mMonom, m));
        j.data()->mCoeff.mult(i.data()->mCoeff, c);
        ++i;
        ++j;
      }
    }
    else {
      assert(a.mHead.length() == 1);
      Monom &m=a.mHead.head()->mMonom;
      Integer &c=a.mHead.head()->mCoeff;

      List<Term*>::ConstIterator i(mHead.begin());
      while(i) {
        j.insert(new(mAllocator) Term(mAllocator, i.data()->mMonom, m));
        j.data()->mCoeff.mult(i.data()->mCoeff, c);
        ++i;
        ++j;
      }
    }
    mHead.swap(tmp);
  }
  else {
    List<Term*> sum(mAllocator);
    List<Term*>::ConstIterator i(mHead.begin());
    assert(i);
    do {
      Monom &m=i.data()->mMonom;
      Integer &c=i.data()->mCoeff;

      List<Term*> tmp(mAllocator);
      List<Term*>::Iterator j(tmp.begin());
      List<Term*>::ConstIterator l(a.mHead.begin());
      while(l) {
        j.insert(new(mAllocator) Term(mAllocator, l.data()->mMonom, m));
        j.data()->mCoeff.mult(l.data()->mCoeff, c);
        ++l;
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
          k.data()->mCoeff.add(j.data()->mCoeff);
          if (!k.data()->mCoeff.isZero())
            ++k;
          else {
            mAllocator->destroy(k.data());
            k.del();
          }
          mAllocator->destroy(j.data());
          j.del();
        }

      while(j) {
        k.insert(j.get());
        ++k;
      }
    } while(i);
    mHead.swap(sum);
  }
  assert(assertValid());
}

void PolyInt::pow(int deg) {
  assert(deg >= 0);
  if (deg == 1)
    ;
  else if (deg == 0)
    clear();
  else {
    assert(deg > 1);
    PolyInt tmp(mAllocator, *this);
    for(--deg; deg > 0; deg--)
      tmp.mult(*this);
    swap(tmp);
  }
}

void PolyInt::reduction(const PolyInt& a) {
  assert(mCmp1 == a.mCmp1 && mCmp2 == a.mCmp2);
  assert(lm().divisiable(a.lm()));

  Monom m2(mAllocator, lm());
  m2.div(a.lm());

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
      i.data()->mCoeff.mult(k1);
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
    i.data()->mCoeff.mult(k1);
    ++i;
  }
  while(ia) {
    i.insert(new(mAllocator) Term(mAllocator, ia.data()->mMonom, m2));
    i.data()->mCoeff.mult(ia.data()->mCoeff, k2);
    ++i;
    ++ia;
  }
}

void PolyInt::redTail(List<Term*>::Iterator i1, const PolyInt& a) {
  assert(mCmp1 == a.mCmp1 && mCmp2 == a.mCmp2);
  assert(i1.data()->mMonom.divisiable(a.lm()));

  Monom m2(mAllocator, lm());
  m2.div(a.lm());

  Integer tmp1(mAllocator), tmp2(mAllocator), k1(mAllocator), k2(mAllocator);
  tmp1.gcd(i1.data()->mCoeff, a.lc());
  k1.div(a.lc(), tmp1);
  k2.div(i1.data()->mCoeff, tmp1);
  k2.set_neg();

  List<Term*>::Iterator i(mHead.begin());
  while(i != i1) {
    assert(i);
    i.data()->mCoeff.mult(k1);
    ++i;
  }

  List<Term*>::ConstIterator ia(a.mHead.begin());

  i.del();
  ++ia;
  while(i && ia) {
    switch(mCmp2(i.data()->mMonom, ia.data()->mMonom, m2)) {
    case 1:
      i.data()->mCoeff.mult(k1);
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
    i.data()->mCoeff.mult(k1);
    ++i;
  }
  while(ia) {
      i.insert(new(mAllocator) Term(mAllocator, ia.data()->mMonom, m2));
      i.data()->mCoeff.mult(ia.data()->mCoeff, k2);
      ++i;
      ++ia;
  }
}

// void PolyInt::nf(Janet &a);
// void PolyInt::nfTail(Janet &a);

bool PolyInt::isPp() const {
  if (!mHead)
    return true;
  else {
    List<Term*>::ConstIterator i(mHead.begin());
    if (i.data()->mCoeff.isAbsOne())
      return true;
    Integer g(mAllocator, i.data()->mCoeff);
    ++i;
    while(i) {
      g.gcd(i.data()->mCoeff);
      if (g.isAbsOne())
        return true;
      ++i;
    }
    return false;
  }
}


void PolyInt::pp() {
  assert(!isPp());
  if (mHead) {
    List<Term*>::ConstIterator i(mHead.begin());
    if (i.data()->mCoeff.isAbsOne())
      return;
    Integer g(mAllocator, i.data()->mCoeff);
    ++i;
    while(i) {
      g.gcd(i.data()->mCoeff);
      if (g.isAbsOne())
        return;
      ++i;
    }

    i = mHead.begin();
    while(i) {
      i.data()->mCoeff.div(g);
      ++i;
    }
  }
}


std::ostream& operator<<(std::ostream& out, const PolyInt &a) {
  PolyInt::ConstIterator i(a.begin());
  if (!i)
    out << '0';
  else {
    assert(i);
    out << i.monom() << '*' << i.coeff().get_str(10);
    ++i;
    while(i) {
      out << " + "<< i.monom() << '*' << i.coeff().get_str(10);
      ++i;
    }
  }
  return out;
}



bool PolyInt::assertValid() const {
  if (mHead) {
    List<Term*>::ConstIterator i(mHead.begin());
    if (i.data()->mCoeff.isZero())
      return false;
    List<Term*>::ConstIterator prev(i);
    ++i;
    while(i) {
      if (mCmp1(i.data()->mMonom, prev.data()->mMonom) >= 0)
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
