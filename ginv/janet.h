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

#ifndef GINV_JANET_H
#define GINV_JANET_H

#include "util/allocator.h"
#include "util/hilbertpoly.h"
#include "monom.h"
#include "wrap.h"

#include "config.h"

#ifdef GINV_GRAPHVIZ
  #include <sstream>
  #include <graphviz/gvc.h>
#endif // GINV_GRAPHVIZ

namespace GInv {

class Janet {
protected:
  friend class ConstIterator;
  friend class Iterator;

  struct Node {
    Monom::Variable mDeg;
    Wrap*           mWrap;
    Node*           mNextDeg;
    Node*           mNextVar;

    Node(Monom::Variable deg): mDeg(deg), mWrap(nullptr),
      mNextDeg(nullptr), mNextVar(nullptr) {}
    Node(Monom::Variable deg, Wrap* w): mDeg(deg), mWrap(w),
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

    void setNM(Monom::Variable var);
//     Monom::Variable nextDegree() const;

    bool assertValid();
  };

protected:
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
      allocator->dealloc(tmp);
    }
    void clear(Allocator* allocator);
  };

  Allocator*  mAllocator;
  int         mSize;
  Link        mRoot;

  static Link build(Link j, Allocator* allocator);
  static void clear(Link j, Allocator* allocator);
  static void buildHP(Link j, HilbertPoly& hp, HilbertPoly& tmp);

#ifdef GINV_GRAPHVIZ
  static Agnode_t* draw(Agraph_t *g, Link j, Monom::Variable var, bool NMd);
#endif // GINV_GRAPHVIZ

  static void setMNprec(Wrap *wrap, int v, ConstIterator j);
  static void setMNsucc(Wrap *wrap, int v,  ConstIterator j);

public:
  explicit Janet(Allocator* allocator):
      mAllocator(allocator),
      mSize(0),
      mRoot(nullptr) {
  }
  explicit Janet(Allocator* allocator, const Janet& a):
      mAllocator(allocator),
      mSize(a.mSize),
      mRoot(nullptr) {
    if (a.mRoot)
      mRoot = build(a.mRoot, mAllocator);
  }
  ~Janet() {
    if (mRoot)
      clear(mRoot, mAllocator);
  }

  void swap(Janet& a);

  Janet::ConstIterator begin() const { return mRoot; }
  int size() const { return mSize;}
  void buildHP(HilbertPoly& hp) const {
    assert(hp.dim() < 0);
    assert(mRoot);
    HilbertPoly tmp;
    buildHP(mRoot, hp, tmp);
  }

  const Wrap* find(const Monom &m) const;
  void insert(Wrap *wrap);
  void setMNprec(Wrap *wrap) {
    if (mRoot)
      setMNprec(wrap, 0, ConstIterator(mRoot));
  }
  void setMNsucc(Wrap *wrap) {
    if (mRoot)
      setMNsucc(wrap, 0, ConstIterator(mRoot));
  }

#ifdef GINV_GRAPHVIZ
  void draw(Agraph_t *g, bool NMd) {
    draw(g, mRoot, 0, NMd);
  }

  void draw(const char* format, const char* filename) const {
    GVC_t *gvc=gvContext();
    Agraph_t *g=agopen((char*)"Janet",  Agdirected, (Agdisc_t*)nullptr);
    std::stringstream ss;
    ss << "#Janet = " << size() << std::endl;
    HilbertPoly hp;
    buildHP(hp);
    ss << "#HP = " << hp;
    agnode(g, (char*)ss.str().c_str(), 1);
    if (mRoot)
      draw(g, mRoot, 0, false);
    gvLayout(gvc, g, (char*)"dot");
    gvRenderFilename(gvc, g, format, filename);
    gvFreeLayout(gvc, g);
    agclose(g);
  }
#endif // GINV_GRAPHVIZ
};

typedef GC<Janet> GCJanet;

}

#endif // GINV_JANET_H
