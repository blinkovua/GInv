/***************************************************************************
 *   Copyright (C) 2012 by Blinkov Yu. A.                                  *
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

#ifndef GINV_UTIL_RBTREE_H
#define GINV_UTIL_RBTREE_H

#include <cstddef>
#include <type_traits>

#include "config.h"

#ifdef GINV_UTIL_GRAPHVIZ
  #include <cstdlib>
  #include <graphviz/gvc.h>

  inline void str(char buffer[256], int a) {
    sprintf(buffer, "%d", a);
  }
#endif // GINV_UTIL_GRAPHVIZ

#include "allocator.h"

namespace GInv {

template <typename T, int compare(const T, const T)>
class RBTree {
  static_assert(std::is_integral<T>::value ||
                std::is_floating_point<T>::value ||
                std::is_pointer<T>::value,
    "typename T requires simple type: integral, floating and pointer");

public:
  typedef enum {Black=0, Red=1} Color;

protected:
  struct Node {
    const T mKey;
    Color   mColor;
    Node*   mParent;
    Node*   mLeft;
    Node*   mRight;

    inline Node(T a):
        mKey(a),
        mColor(Red),
        mParent(nullptr),
        mLeft(nullptr),
        mRight(nullptr) {
    }
    inline Node(const Node &a, Node* parent):
        mKey(a.mKey),
        mColor(a.mColor),
        mParent(parent),
        mLeft(nullptr),
        mRight(nullptr) {
    }
    inline ~Node() {}
  };

  Allocator*  mAllocator;
  Node*       mRoot;
  size_t      mCount;

  static inline Node* minimum(Node* x) {
    while(x->mLeft)
      x = x->mLeft;
    return x;
  }
  static inline Node* maximum(Node* x) {
    while(x->mRight)
      x = x->mRight;
    return x;
  }
  static inline Node* next(Node* x) {
    Node* y;
    if (x->mRight)
      y = minimum(x->mRight);
    else {
      y = x->mParent;
      while(y && x == y->mRight) {
        x = y;
        y = y->mParent;
      }
    }
    return y;
  }
  static inline Node* prev(Node* x) {
    Node* y;
    if (x->mLeft)
      y = maximum(x->mLeft);
    else {
      y = x->mParent;
      while(y && x == y->mLeft) {
        x = y;
        y = y->mParent;
      }
    }
    return y;
  }

  static size_t countBlack(const Node* node, const Node* root) {
    if (node == nullptr)
      return 0;

    size_t sum=0;
    do {
      if (node->mColor == Black)
        ++sum;
      if (node == root)
        break;
      node = node->mParent;
    } while(true);
    return sum;
  }

  void rotateLeft(Node* const x) {
    assert(x);
    Node *const y = x->mRight;

    x->mRight = y->mLeft;
    if (y->mLeft != nullptr)
      y->mLeft->mParent = x;
    y->mParent = x->mParent;

    if (x->mParent == nullptr)
      mRoot = y;
    else if (x == x->mParent->mLeft)
      x->mParent->mLeft = y;
    else
      x->mParent->mRight = y;
    y->mLeft = x;
    x->mParent = y;
  }

  void rotateRight(Node* const x) {
    assert(x);
    Node *const y = x->mLeft;

    x->mLeft = y->mRight;
    if (y->mRight != nullptr)
      y->mRight->mParent = x;
    y->mParent = x->mParent;

    if (x->mParent == nullptr)
      mRoot = y;
    else if (x == x->mParent->mRight)
      x->mParent->mRight = y;
    else
      x->mParent->mLeft = y;
    y->mRight = x;
    x->mParent = y;
  }

  static Node* copy(const Node *x, Allocator* allocator, Node* parent) {
    assert(x);
    Node* i=new(allocator) Node(*x, parent);
    Node* r=i;
    do {
      if (x->mRight)
        i->mRight = copy(x->mRight, allocator, i);
      if (x->mLeft == nullptr)
        break;
      i->mLeft = new(allocator) Node(*x->mLeft, i);
      i = i->mLeft;
      x = x->mLeft;
    } while(true);
    return r;
  }

  static void clear(Node* x, Allocator* allocator) {
    assert(x);
    do {
      if (x->mRight)
        clear(x->mRight, allocator);
      Node *tmp=x;
      x = x->mLeft;
      allocator->dealloc(tmp);
    } while(x);
  }

  Node* findNode(const T key) const {
    Node* r=mRoot;
    while(r) {
      int cmp=compare(key, r->mKey);
      if (cmp < 0)
        r = r->mLeft;
      else if (cmp > 0)
        r = r->mRight;
      else
        break;
    }
    return r;
  }
  void insertFixup(Node* x) {
    while(x != mRoot && x->mParent->mColor == Red) {
      Node* const xpp=x->mParent->mParent;

      if (x->mParent == xpp->mLeft) {
        Node* const y = xpp->mRight;
        if (y && y->mColor == Red) {
          x->mParent->mColor = Black;
          y->mColor = Black;
          xpp->mColor = Red;
          x = xpp;
        }
        else {
          if (x == x->mParent->mRight) {
            x = x->mParent;
            rotateLeft(x);
          }
          x->mParent->mColor = Black;
          xpp->mColor = Red;
          rotateRight(xpp);
        }
      }
      else {
        Node* const y=xpp->mLeft;
        if (y && y->mColor == Red) {
          x->mParent->mColor = Black;
          y->mColor = Black;
          xpp->mColor = Red;
          x = xpp;
        }
        else {
          if (x == x->mParent->mLeft) {
            x = x->mParent;
            rotateRight(x);
          }
          x->mParent->mColor = Black;
          xpp->mColor = Red;
          rotateLeft(xpp);
        }
      }
    }
    mRoot->mColor = Black;
  }
  void insertNode(Node* z) {
    Node *x=mRoot,
         *y=nullptr;
    while(x) {
      y = x;
      int cmp=compare(z->mKey, x->mKey);
      if (cmp < 0)
        x = x->mLeft;
      else {
        assert(cmp > 0);
        x = x->mRight;
      }
    }

    z->mParent = y;

    if (y == nullptr)
      mRoot = z;
    else {
      int cmp=compare(z->mKey, y->mKey);
      assert(cmp != 0);
      if (cmp < 0)
        y->mLeft = z;
      else
        y->mRight = z;
    }

    insertFixup(z);
    ++mCount;
  }
  void deleteFixup(Node* x, Node* xp) {
    assert(x == nullptr || x->mParent == xp);
    while(x != mRoot && (x == nullptr || x->mColor == Black)) {
      if (x == xp->mLeft) {
        Node* w = xp->mRight;
        if (w->mColor == Red) {
          w->mColor = Black;
          xp->mColor = Red;
          rotateLeft(xp);
          w = xp->mRight;
        }
        if ((w->mLeft == nullptr || w->mLeft->mColor == Black) &&
            (w->mRight == nullptr || w->mRight->mColor == Black)) {
          w->mColor = Red;
          x = xp;
          xp = x->mParent;
        }
        else {
          if (w->mRight == nullptr || w->mRight->mColor == Black) {
            w->mLeft->mColor = Black;
            w->mColor = Red;
            rotateRight(w);
            w = xp->mRight;
          }
          w->mColor = xp->mColor;
          xp->mColor = Black;
          if (w->mRight)
            w->mRight->mColor = Black;
          rotateLeft(xp);
          break;
        }
      }
      else {
        Node* w = xp->mLeft;
        if (w->mColor == Red) {
          w->mColor = Black;
          xp->mColor = Red;
          rotateRight(xp);
          w = xp->mLeft;
        }
        if ((w->mRight == nullptr || w->mRight->mColor == Black) &&
            (w->mLeft == nullptr || w->mLeft->mColor == Black)) {
          w->mColor = Red;
          x = xp;
          xp = x->mParent;
        }
        else {
          if (w->mLeft == nullptr || w->mLeft->mColor == Black) {
            w->mRight->mColor = Black;
            w->mColor = Red;
            rotateLeft(w);
            w = xp->mLeft;
          }
          w->mColor = xp->mColor;
          xp->mColor = Black;
          if (w->mLeft)
            w->mLeft->mColor = Black;
          rotateRight(xp);
          break;
        }
      }
    }
    if (x)
      x->mColor = Black;
  }
  Node* deleteNode(Node* z) {
    Node *y;
    if (z->mLeft == nullptr || z->mRight == nullptr)
      y = z;
    else {
      y = z->mRight;
      while (y->mLeft)
        y = y->mLeft;
      (T&)z->mKey = y->mKey;
    }
    assert(y->mLeft == nullptr || y->mRight == nullptr);

    Node *x;
    if (y->mLeft)
      x = y->mLeft;
    else
      x = y->mRight;
    Node *xp=y->mParent;
    if (x)
      x->mParent = xp;

    if (y->mParent == nullptr)
      mRoot = x;
    else {
      if (y == y->mParent->mLeft)
        y->mParent->mLeft = x;
      else
        y->mParent->mRight = x;
    }

    if (y->mColor == Black)
      deleteFixup(x, xp);

    --mCount;

    return y;
  }

#ifdef GINV_UTIL_GRAPHVIZ
  static Agnode_t* drawLeaf(Agraph_t *g) {
    char buffer[256];
    static int leaf=0;
    ++leaf;
    sprintf(buffer, "nullptr%d", leaf);
    Agnode_t* r = agnode(g, buffer, 1);
    agsafeset(r, (char*)"label", (char*)"nullptr", (char*)"");
    agsafeset(r, (char*)"shape", (char*)"box", (char*)"");
    return r;
  }

  static Agnode_t* draw(Agraph_t *g, RBTree::Node* j) {
    char buffer[256];
    Agnode_t* r;
    if (!j)
      r = drawLeaf(g);
    else {
      str(buffer, j->mKey);
      r = agnode(g, buffer, 1);
      if (j->mColor == Red)
        agsafeset(r, (char*)"color", (char*)"firebrick1", (char*)"");
      Agnode_t* node=r;
      do {
        Agnode_t* left=draw(g, j->mLeft);
        Agnode_t* right;
        if (!j->mRight)
          right = drawLeaf(g);
        else {
          str(buffer, j->mRight->mKey);
          right = agnode(g, buffer, 1);
          if (j->mRight->mColor == Red)
            agsafeset(right, (char*)"color", (char*)"firebrick1", (char*)"");
        }
        Agedge_t *e=agedge(g, node, left, (char*)"", 1);
        agsafeset(e, (char*)"style", (char*)"solid", (char*)"");
        agsafeset(e, (char*)"dir", (char*)"forward", (char*)"");
        e = agedge(g, node, right, (char*)"", 1);
        agsafeset(e, (char*)"style", (char*)"solid", (char*)"");
        agsafeset(e, (char*)"dir", (char*)"forward", (char*)"");

        node = right;
        j = j->mRight;
      } while(j);
    }
    return r;
  }
#endif // GINV_UTIL_GRAPHVIZ

public:
  class ConstIterator {
    friend RBTree<T, compare>;
    RBTree::Node* mLink;

  public:
    inline ConstIterator():
        mLink(nullptr) {
    }
    inline ConstIterator(RBTree::Node* node):
        mLink(node) {
    }
    inline ~ConstIterator() {}

    inline operator bool() const { return mLink != nullptr; }
    inline void operator=(const ConstIterator& a) { mLink = a.mLink; }
    inline void operator--() {
      assert(mLink != nullptr);
      mLink = RBTree::prev(mLink);
    }
    inline void operator++() {
      assert(mLink != nullptr);
      mLink = RBTree::next(mLink);
    }

    inline const T key() const {
      assert(mLink);
      return mLink->mKey;
    }
    inline RBTree::Color color() const {
      assert(mLink);
      return mLink->mColor;
    }
    inline ConstIterator left() const {
      assert(mLink);
      return mLink->mLeft;
    }
    inline ConstIterator right() const {
      assert(mLink);
      return mLink->mRight;
    }
  };

  inline RBTree(RBTree&& a):
      mAllocator(a.allocator),
      mRoot(a.mRoot),
      mCount(a.mCount) {
    a.mRoot = nullptr;
    a.mCount = 0;    
  }
  inline RBTree(Allocator* allocator):
      mAllocator(allocator),
      mRoot(nullptr),
      mCount(0) {
    assert(assertValid());
  }
  inline RBTree(const RBTree& a, Allocator* allocator):
      mAllocator(allocator),
      mRoot((a.mRoot) ? copy(a.mRoot, allocator, nullptr): nullptr),
      mCount(a.mCount) {
    assert(assertValid());
  }
  inline ~RBTree() {
    if (mRoot)
      clear(mRoot, mAllocator);
  }

  inline void clear() {
    if (mRoot)
      clear(mRoot, mAllocator);
    mRoot = nullptr;
    mCount = 0;
    assert(assertValid());
  }
  inline void swap(RBTree &a) {
    auto tmp1=mAllocator;
    mAllocator = a.mAllocator;
    a.mAllocator = tmp1;

    auto tmp2=mRoot;
    mRoot = a.mRoot;
    a.mRoot = tmp2;

    auto tmp3=mCount;
    mCount = a.mCount;
    a.mCount = tmp3;
  }
  inline void operator=(const RBTree &a) {
    assert(this != &a);
    if (mRoot)
      clear(mRoot, mAllocator);
    mRoot = copy(a.mRoot, mAllocator, nullptr);
    mCount = a.mCount;
    assert(assertValid());
  }

  inline operator bool() const { return mRoot != nullptr; }
  inline int length() const { return mCount; }

  inline const T maximumKey() const {
    assert(mRoot != nullptr);
    return maximum(mRoot)->mKey;
  }
  inline const T minimumKey() const {
    assert(mRoot != nullptr);
    return minimum(mRoot)->mKey;
  }

  inline ConstIterator root() const {
    return mRoot;
  }
  inline ConstIterator maximum() const {
    if (mRoot == nullptr)
      return nullptr;
    else
      return maximum(mRoot);
  }
  inline ConstIterator minimum() const {
    if (mRoot == nullptr)
      return nullptr;
    else
      return minimum(mRoot);
  }
  inline ConstIterator find(const T key) const {
    return findNode(key);
  }
  ConstIterator findPrev(const T key) const {
    if (mRoot == nullptr)
      return mRoot;
    else {
      assert(mRoot);
      Node* i=mRoot;
      do {
        if (compare(key, i->mKey) <= 0) {
          if (i->mLeft == nullptr)
            return prev(i);
          i = i->mLeft;
        }
        else {
          if (i->mRight == nullptr)
            return i;
          i = i->mRight;
        }
      } while(true);
    }
  }
  ConstIterator findNext(const T key) const {
    if (mRoot == nullptr)
      return mRoot;
    else {
      assert(mRoot);
      Node* i=mRoot;
      do {
        if (compare(key, i->mKey) >= 0) {
          if (i->mRight == nullptr)
            return next(i);
          i = i->mRight;
        }
        else {
          if (i->mLeft == nullptr)
            return i;
          i = i->mLeft;
        }
      } while(true);
    }
  }

  inline void insert(const T key) {
    assert(findNode(key) == nullptr);
    Node *z = new(mAllocator) Node(key);
    insertNode(z);
    assert(findNode(key) != nullptr);
    assert(assertValid());
  }
  inline void remove(const T key) {
    Node *z=findNode(key);
    assert(z != nullptr);
    Node *y=deleteNode(z);
    mAllocator->destroy(y);
    assert(findNode(key) == nullptr);
    assert(assertValid());
  }
  inline void remove(ConstIterator i) {
    assert(i.mLink != nullptr);
    Node *y=deleteNode(i.mLink);
    mAllocator->destroy(y);
    assert(assertValid());
  }

  bool assertValid() const {
    if (mRoot == nullptr || mCount == 0)
      return mRoot == nullptr && mCount == 0;

    if (mRoot->mColor == Red)
      return false;

    Node *i=minimum(mRoot);
    size_t len=countBlack(i, mRoot);
    for (; i; i = next(i)) {
      if (i->mColor == Red)
        if ((i->mLeft && i->mLeft->mColor == Red) ||
            (i->mRight && i->mRight->mColor == Red))
          return false;

      if (i->mLeft && compare(i->mLeft->mKey, i->mKey) > 0)
        return false;
      if (i->mRight && compare(i->mRight->mKey, i->mKey) < 0)
        return false;

      if ((i->mLeft == nullptr || i->mRight == nullptr) &&
          countBlack(i, mRoot) != len)
        return false;
    }

    return true;
  }

#ifdef GINV_UTIL_GRAPHVIZ
  void draw(const char* format, const char* filename) const {
    GVC_t *gvc=gvContext();
    Agraph_t *g=agopen((char*)"RBTree",  Agdirected, (Agdisc_t*)nullptr);
    draw(g, mRoot);
    gvLayout(gvc, g, (char*)"dot");
    gvRenderFilename(gvc, g, format, filename);
    gvFreeLayout(gvc, g);
    agclose(g);
  }
#endif // GINV_UTIL_GRAPHVIZ
};

}

#endif // GINV_UTIL_RBTREE_H

