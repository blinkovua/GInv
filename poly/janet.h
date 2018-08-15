/***************************************************************************
 *   Copyright (C) 2017 by Blinkov Yu. A.                                  *
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

#ifndef GINV_POLY_JANET_H
#define GINV_POLY_JANET_H

#include "util/allocator.h"
#include "util/list.h"
#include "monom.h"

#include "config.h"

namespace GInv {

class Wrap {
  Monom       mLm;
  Monom       mAnsector;
  bool*       mNM;
  bool*       mBuild;

public:  
  Wrap()=delete;
  Wrap(const Wrap& a)=delete;
  Wrap(Monom m, Allocator* allocator):
      mLm(m, allocator), 
      mAnsector(m, allocator),
      mNM(new(mAllocator) bool[m.size()]),
      mBuild(new(mAllocator) bool[m.size()]) {
    for(int i=0; i < m.size(); i++) {
      mNM[i] = false;
      mBuild[i] = false;
    }
  }
  Wrap(const Wrap* ansector, int var, Allocator* allocator):
      mLm(m, v, allocator), 
      mAnsector(m, allocator),
      mNM(new(mAllocator) bool[m.size()]),
      mBuild(new(mAllocator) bool[m.size()]) {
    for(int i=0; i < m.size(); i++) {
      mNM[i] = false;
      mBuild[i] = false;
  }
  ~Wrap() {
    mAllocator->dealloc(mNM, mLm.size());
    mAllocator->dealloc(mBuild, mLm.size());
  }
};


class Janet {
  friend class ConstIterator;
  friend class Iterator;

  struct Node {
    Monom::Variable mDeg;
    Wrap*           mWrap;
    Node*           mNextDeg;
    Node*           mNextVar;

    Node(Monom::Variable deg): mDeg(deg), mWrap(nullptr),
      mNextDeg(nullptr), mNextVar(nullptr) {}
    ~Node() {}
  };

  typedef Node* Link;

public:  
  class ConstIterator {
    Link i;

  public:
    ConstIterator(Link n): i(n) {}

    void deg() { assert(i); i = i->mNextDeg; }
    void var() { assert(i); i = i->mNextVar; }
    operator bool() const { return (i != nullptr); }
    ConstIterator nextDeg() const { assert(i); return i->mNextDeg; }
    ConstIterator nextVar() const { assert(i); return i->mNextVar; }
    Wrap* wrap() const { assert(i); return i->mWrap; }
    Monom::Variable degree() const { assert(i); return i->mDeg; }

//     void prolong(ISetQ &q, int var);

    bool assertValid() const;
  };

private:  
  class Iterator {
    Link *i;

  public:
    Iterator(Link &n): i(&n) {}

    void deg() { assert(*i); i = &(*i)->mNextDeg; }
    void var() { assert(*i); i = &(*i)->mNextVar; }
    operator bool() const { return (*i != nullptr); }
    ConstIterator nextDeg() const {
      assert(*i); return (*i)->mNextDeg;
    }
    ConstIterator nextVar() const {
      assert(*i); return (*i)->mNextVar;
    }
    operator ConstIterator() const { return *i; }
    Wrap*& wrap() const { assert(*i); return (*i)->mWrap; }
    Monom::Variable degree() const { assert(i); return (*i)->mDeg; }
    void build(int d, Monom::Variable var, Wrap *wrap, Allocator* allocator);
    void del(Allocator* allocator) {
      Link tmp = *i;
      assert(tmp);
      *i = tmp->mNextDeg;
      allocator->destroy(tmp);
    }
    void clear(Allocator* allocator);
  };

  Allocator*  mAllocator;
  int         mPos;
  Link*       mRoot;

public:
  explicit Janet(Allocator* allocator):
      mAllocator(allocator),
      mPos(0),
      mRoot(nullptr) {
  }
  ~Janet() {
    if (mRoot) {
      Janet::Iterator j(mRoot);
      j.clear();
    }
  }

  Wrap* find(const Monom &m) const;
  void insert(Wrap *wrap);
  void insert(Wrap *wrap);
  void update(Wrap *wrap);
};

}

#endif // GINV_POLY_JANET_H
