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

void BPoly::add1(BPoly& a) {
  assert(this != &a);
  assert(mAllocator == a.mAllocator);
  List<const BMonom*>::Iterator i(mList.begin()),
                               ia(a.mList.begin());
  while(i && ia) {
    int cmp=i.data()->compare(*ia.data());
    if (cmp > 0)
      ++i;
    else if (cmp < 0) {
      i.insert(ia.get());
      ++i;
    }
    else {
      mAllocator->destroy(i.data());
      i.del();
      mAllocator->destroy(ia.data());
      ia.del();
    }
  }
  while(ia) {
    i.insert(ia.get());
    ++i;
  }
  assert(assertValid());
}

BPoly::BPoly(const ZDD::ConstIterator a, Allocator *allocator):
    mAllocator(allocator),
    mList(allocator) {

}

// void BPoly::read() {
//
// }

void BPoly::canonicalize() {
  int l=mList.length();
  if (l <= 8) {
    List<const BMonom*> tmp(mAllocator);
    List<const BMonom*>::Iterator k(tmp.begin());
    while(mList) {
      List<const BMonom*>::Iterator i(mList.begin()),
                                   j(i);
      ++i;
      int cmp=1;
      while(i) {
        cmp = j.data()->compare(*i.data());
        if (cmp > 0)
          j = i;
        else if (cmp == 0) {
          mAllocator->destroy(i.data());
          i.del();
          mAllocator->destroy(j.data());
          j.del();
          break;
        }
        ++i;
      }
      if (cmp)
        k.insert(j.get());
    }
    mList.swap(tmp);
    assert(assertValid());
  }
  else {
    int l2=l/2;
    assert(l2 > 1);
    BPoly tmp(mAllocator);
    List<const BMonom*>::Iterator i(mList.begin()),
                                 ia(tmp.mList.begin());
    while(l2 > 0) {
      ia.insert(i.get());
      ++ia;
      --l2;
    }
    canonicalize();
    tmp.canonicalize();
    add1(tmp);
  }
}

bool BPoly::tailOne() const {
  List<const BMonom*>::ConstIterator i(mList.begin()),
                                     prev;
  while(i) {
    prev = i;
    ++i;
  }
  return prev && prev.data()->degree() == 0;
}

void BPoly::mult(const BPoly& a, BMonom::Variable var) {
  assert(this != &a);
  clear();
  List<const BMonom*>::Iterator i(mList.begin());
  List<const BMonom*>::ConstIterator ia(a.mList.begin());
  while(ia) {
    i.insert(new(mAllocator) BMonom(*ia.data(), var, mAllocator));
    ++ia;
    ++i;
  }
  canonicalize();
  assert(assertValid());
}

void BPoly::mult(const BPoly& a, const BMonom& m) {
  assert(this != &a);
  clear();
  List<const BMonom*>::Iterator i(mList.begin());
  List<const BMonom*>::ConstIterator ia(a.mList.begin());
  while(ia) {
    i.insert(new(mAllocator) BMonom(*ia.data(), m, mAllocator));
    ++ia;
    ++i;
  }
  canonicalize();
  assert(assertValid());
}

void BPoly::addOne() {
  List<const BMonom*>::Iterator i(mList.begin());
  while(i) {
    if (i.data()->degree() == 0)
      break;
    ++i;
  }
  if (!i)
    i.insert(new(mAllocator) BMonom(mAllocator));
  else {
    mAllocator->destroy(i.data());
    i.del();
  }
}

void BPoly::add(const BPoly& a) {
  assert(this != &a);
  List<const BMonom*>::Iterator i(mList.begin());
  List<const BMonom*>::ConstIterator ia(a.mList.begin());
  while(i && ia) {
    int cmp=i.data()->compare(*ia.data());
    if (cmp > 0)
      ++i;
    else if (cmp == 0) {
      mAllocator->destroy(i.data());
      i.del();
      ++ia;
    }
    else if (cmp < 0) {
      i.insert(new(mAllocator) BMonom(*ia.data(), mAllocator));
      ++i;
      ++ia;
    }
  }
  while(ia) {
    i.insert(new(mAllocator) BMonom(*ia.data(), mAllocator));
    ++i;
    ++ia;
  }
  assert(assertValid());
}

void BPoly::OR(BMonom::Variable var, bool negation) {
  if (negation) { // a*var + var + 1
    // a*var
    List<const BMonom*>::ConstIterator i(mList.begin());
    while(i) {
      const BMonom* m=i.data();
      i.data() = new(mAllocator) BMonom(*m, var, mAllocator);
      mAllocator->destroy(m);
      ++i;
    }
    // a*var + var
    mList.push(new(mAllocator) BMonom(var, mAllocator));
    // a*var + var + 1
    mList.push(new(mAllocator) BMonom(mAllocator));
  }
  else { // a*var + a + var
    // a*var + a
    List<const BMonom*>::Iterator i(mList.begin());
    while(i) {
      const BMonom* m1=i.data();
      const BMonom* m2=new(mAllocator) BMonom(*m1, var, mAllocator);
      if (m1->degree() == m2->degree()) {
        mAllocator->destroy(m1);
        mAllocator->destroy(m2);
        i.del();
      }
      else {
        ++i;
        i.insert(m2);
        ++i;
      }
    }
    // a*var + a + var
    mList.push(new(mAllocator) BMonom(var, mAllocator));
  }
  canonicalize();
}

bool BPoly::assertValid() const {
  if (mList) {
    List<const BMonom*>::ConstIterator i(mList.begin());
    assert(i);
    const BMonom *prev=i.data();
    ++i;
    while(i) {
      if (prev->compare(*i.data()) <= 0)
        return false;
      prev = i.data();
      ++i;
    }
  }
  return true;
}

std::ostream& operator<<(std::ostream& out, const BPoly &a) {
  if (!a)
    out << '0';
  else {
    BPoly::ConstIterator i(a.begin());
    out << i.monom();
    ++i;
    while(i) {
      out << " + " << i.monom();
      ++i;
    }
  }
  return out;
}
/*
AutoReduce::~AutoReduce() {
  for(int i=0; i < mSize; i++) {
    Container::ConstIterator j(mContainer[i].begin());
    while(j) {
      delete j.data();
      ++j;
    }
  }
  delete[] mContainer;
}

void AutoReduce::add(DegRevLexGC* p) {
  assert(p && *p && !p->isOne());
  assert(0 < p->lm()[0] && p->lm()[0] <= mSize);
  Container::Iterator j(mContainer[p->lm()[0]-1].begin());
  while(j) {
    if (p->lm().compare(j.data()->lm()) > 0)
      break;
    ++j;
  }
  j.insert(p);
}

BPoly* AutoReduce::find(const BPoly::BMonom* m) const {
  for(int i=0; i < m->degree(); i++) {
    Container::ConstIterator j(mContainer[(*m)[i]-1].begin());
    while(j) {
      if (m->divisible(j.data()->lm())) {
        return j.data();
      }
      ++j;
    }
  }
  return nullptr;
}

bool AutoReduce::autoReduce() {
  bool reduce=true;
  while(reduce) {
    reduce = false;
    for(int i=mSize-1; i >= 0; i--)
      if (mContainer[i]) {
        Container tmp;
//         std::cout << "i = " << i << "  " << mContainer[i].length()  << " -> ";
        Container::Iterator j(mContainer[i].begin());
        while(j) {
          DegRevLexGC *p=j.data();
          j.del();
          reduce = reduce || find(&p->lm());
          p->nf(*this);
          if (p->isZero())
            delete p;
          else if (p->isOne()) {
            delete p;
            return false;
          }
          else
            tmp.push(p);
        }
        while(tmp)
          add(tmp.pop());
  //       std::cout << mContainer[i].length() << std::endl;
      }

    for(int i=0; i < mSize; i++) {
      Container::ConstIterator j(mContainer[i].begin());
      while(j) {
        j.data()->nfTail(*this);
        ++j;
      }
    }
//     reduce = false;
  }

  for(int i=0; i < mSize; i++) {
    Container::Iterator j(mContainer[i].begin());
    while(j) {
      if (j.data()->length() <= 2)
        ++j;
      else {
        delete j.data();
        j.del();
      }
    }
  }

  return true;
}*/

}
