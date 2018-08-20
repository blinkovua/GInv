/***************************************************************************
 *   Copyright (C) 2004 by Blinkov Yu.A.                                   *
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

#include "./janet.h"

namespace GInv {

void Janet::Iterator::build(int d, Monom::Variable var, Wrap *wrap, Allocator* allocator) {
  assert(d >= wrap->lm()[var]);
  Link r =  new(allocator) Janet::Node(wrap->lm()[var]);
  Link j = r;
  while(d > wrap->lm()[var]) {
    d -= wrap->lm()[var];
    ++var;
    j->mNextVar = new(allocator) Janet::Node(wrap->lm()[var]);
    j = j->mNextVar;
  }
  j->mWrap = wrap;

  assert(!*i || r->mDeg < (*i)->mDeg);
  r->mNextDeg = *i;
  *i = r;
}

void Janet::Iterator::clear(Allocator* allocator) {
  while (nextDeg()) {
    assert(nextVar());
    Janet::Iterator((*i)->mNextVar).clear(allocator);
    del(allocator);
  }
  del(allocator);
}

// void Janet::ConstIterator::prolong(int var) {
//   while(nextDeg()) {
//     assert(nextVar());
//     nextVar().prolong(var);
//     deg();
//   }
//   if (nextVar())
//     nextVar().prolong(q, var);
//   else
//     wrap()->setNM(var);
// }

bool Janet::ConstIterator::assertValid() {
  while(nextDeg()) {
    if (degree() >= nextDeg().degree())
      return false;
    if (!nextVar())
      return false;
    if (!nextVar().assertValid())
      return false;
    deg();
  }
  if (nextVar())
    return nextVar().assertValid();
  else
    return wrap() != nullptr;
}

#ifdef GINV_POLY_GRAPHVIZ
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
#endif // GINV_POLY_GRAPHVIZ

Wrap* Janet::find(const Monom &m) const {
  assert(mRoot == nullptr || mPos == m.pos());
  Link root = mRoot;
  Wrap* wrap = nullptr;

  if (root) {
    Janet::ConstIterator j(root);
    unsigned d = m.degree();
    int var = 0;
    if (d == 0) {
      if (j.degree() == 0)
        wrap = j.wrap();
    }
    else {
      do {
        while(j.degree() < m[var] && j.nextDeg())
          j.deg();

        if (j.degree() > m[var])
          break;
        else if (j.nextVar()) {
          d -= m[var];
          if (d == 0)
            break;
          ++var;
          j.var();
        }
        else {
          wrap = j.wrap();
          assert(wrap != nullptr);
          assert(m.divisibility(wrap->lm()));
          break;
        }
      } while(true);
    }
  }

  return wrap;
}


}

// TODO


void Janet::insert(Wrap *wrap) {
  assert(wrap != nullptr);
  assert(find(wrap->lm()) == nullptr);

  Link &root = mRoot;
  unsigned d = wrap->lm().degree();
  Janet::Iterator j(root);

  if (root == nullptr) {
    wrap->clearNM();
    j.build(d, 0, wrap);
  }
  else {
    int var = 0;
    do {
      while(j.degree() < wrap->lm()[var] && j.nextDeg())
        j.deg();

      if (j.degree() > wrap->lm()[var]) {
        wrap->setNM(var);
        j.build(d, var, wrap);
        ++var;
        wrap->clearMN(++var);
        break;
      }
      else if (j.degree() == wrap->lm()[var]) {
        if (j.nextDeg())
          wrap->setNM(var);
        else
          wrap->clearMN(var);
        d -= wrap->lm()[var];
        ++var;
        j.var();
      }
      else {
        assert(Janet::ConstIterator(j).nextVar());
        Janet::ConstIterator(j).nextVar().prolong(var);
        j.deg();
        j.build(d, var, wrap);
        wrap->clearMN(var);
        break;
      }
    } while(true);
  }
  assert(find(wrap->lm()) != nullptr);
  assert(Janet::ConstIterator(root).assertValid());
}

void Janet::insert1(Wrap *wrap) {
  assert(wrap != nullptr);
  assert(find(wrap->lm()) == nullptr);

  Link &root = mRoot;
  unsigned d = wrap->lm().degree();
  Janet::Iterator j(root);

  if (root == nullptr)
    j.build(d, 0, wrap);
  else {
    int var = 0;
    do {
      while(j.degree() < wrap->lm()[var] && j.nextDeg())
        j.deg();

      if (j.degree() > wrap->lm()[var]) {
        j.build(d, var, wrap);
        break;
      }
      else if (j.degree() == wrap->lm()[var]) {
        d -= wrap->lm()[var];
        ++var;
        j.var();
      }
      else {
        j.deg();
        j.build(d, var, wrap);
        break;
      }
    } while(true);
  }
  assert(find(wrap->lm()) != nullptr);
  assert(Janet::ConstIterator(root).assertValid());
}

void Janet::update(Wrap* wrap, ISetQ &setQ) {
  assert(wrap != nullptr);
  assert(find(wrap->lm()) != nullptr);

  Link root = mRoot[depend];
  Janet::ConstIterator j(root);
  int var = 0;
  do {
    while(j.degree() < wrap->lm()[var]) {
      assert(j.nextDeg());
      j.deg();
    }
    assert(j.degree() == wrap->lm()[var]);
    if (!j.nextDeg())
      wrap->clearBuild(var);
    else if (wrap->notBuild(var))
      setQ.insert(wrap, var);
    ++var;
    if (j.nextVar())
      j.var();
    else {
      assert(wrap == j.wrap());
      for(; var < wrap->lm().dimIndepend(); var++)
        wrap->clearBuild(var);
      break;
    }
  } while(true);
}

