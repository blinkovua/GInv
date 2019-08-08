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
#include "wrap.h"

#include "config.h"

#ifdef GINV_POLY_GRAPHVIZ
  #include <sstream>
  #include <graphviz/gvc.h>
#endif // GINV_POLY_GRAPHVIZ

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
      allocator->destroy(tmp);
    }
    void clear(Allocator* allocator);
  };

  Allocator*  mAllocator;
  int         mSize;
  int         mPos;
  Link        mRoot;

#ifdef GINV_POLY_GRAPHVIZ
  static Agnode_t* draw(Agraph_t *g, Link j, Monom::Variable var, bool NMd);
#endif // GINV_POLY_GRAPHVIZ

  static void setMNprec(Wrap *wrap, int v, ConstIterator j);
  static void setMNsucc(Wrap *wrap, int v,  ConstIterator j);

public:
  explicit Janet(Allocator* allocator):
      mAllocator(allocator),
      mSize(0),
      mPos(0),
      mRoot(nullptr) {
  }
  ~Janet() {
    std::cerr << "D " << mAllocator->size() << std::endl;
    if (mRoot) {
      Janet::Iterator j(mRoot);
      j.clear(mAllocator);
    }
    std::cerr << "E " << mAllocator->size() << std::endl;
    assert(mRoot == nullptr);
  }

  Janet::ConstIterator begin() const { return mRoot; }
  int size() const { return mSize;}

  Wrap* find(const Monom &m) const;
  void insert(Wrap *wrap);
  void setMNprec(Wrap *wrap) {
    if (mRoot)
      setMNprec(wrap, 0, ConstIterator(mRoot));
  }
  void setMNsucc(Wrap *wrap) {
    if (mRoot)
      setMNsucc(wrap, 0, ConstIterator(mRoot));
  }

#ifdef GINV_POLY_GRAPHVIZ
  void draw(Agraph_t *g, bool NMd) {
    draw(g, mRoot, 0, NMd);
  }

  void draw(const char* format, const char* filename) const {
    GVC_t *gvc=gvContext();
    Agraph_t *g=agopen((char*)"Janet",  Agdirected, (Agdisc_t*)nullptr);
    std::stringstream ss;
    ss << "#Janet = " << size();
    agnode(g, (char*)ss.str().c_str(), 1);
    if (mRoot)
      draw(g, mRoot, 0, false);
    gvLayout(gvc, g, (char*)"dot");
    gvRenderFilename(gvc, g, format, filename);
    gvFreeLayout(gvc, g);
    agclose(g);
  }
#endif // GINV_POLY_GRAPHVIZ
};

}

#endif // GINV_POLY_JANET_H
