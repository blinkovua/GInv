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
//   while (nextDeg()) {
//     assert(nextVar());
//     Janet::Iterator((*i)->mNextVar).clear(allocator);
//     del(allocator);
//   }
//   if (nextVar())
//     Janet::Iterator((*i)->mNextVar).clear(allocator);
//   del(allocator);
  while (nextDeg()) {
    if (nextVar())
      Janet::Iterator((*i)->mNextVar).clear(allocator);
    del(allocator);
  }
  if (nextVar())
    Janet::Iterator((*i)->mNextVar).clear(allocator);
  del(allocator);
}

void Janet::ConstIterator::setNM(Monom::Variable var) {
//   while(nextDeg()) {
//     assert(nextVar());
//     nextVar().setNM(var);
//     deg();
//   }
//   if (nextVar())
//     nextVar().setNM(var);
//   else
//     wrap()->setNM(var);
  while(nextDeg()) {
    if (nextVar())
      nextVar().setNM(var);
    deg();
  }
  if (nextVar())
    nextVar().setNM(var);
  else
    wrap()->setNM(var);
}

bool Janet::ConstIterator::assertValid() {
//   while(nextDeg()) {
//     if (degree() >= nextDeg().degree())
//       return false;
//     if (!nextVar())
//       return false;
//     if (!nextVar().assertValid())
//       return false;
//     deg();
//   }
//   if (nextVar())
//     return nextVar().assertValid();
//   else
//     return wrap() != nullptr;
  while(nextDeg()) {
    if (degree() >= nextDeg().degree())
      return false;
    if (nextVar() && !nextVar().assertValid())
      return false;
    deg();
  }
  if (nextVar())
    return nextVar().assertValid();
  else
    return wrap() != nullptr;
}

#ifdef GINV_POLY_GRAPHVIZ
Agnode_t* Janet::draw(Agraph_t *g, Link j, Monom::Variable var) {
  std::stringstream ss;
  Agnode_t* r=nullptr;
  Agnode_t* prev;
  while(j) {
    Agnode_t* node;
    if (j->mWrap) {
      ss << j->mWrap;
      node = agnode(g, (char*)ss.str().c_str(), 1);
      ss.str("");

      ss << "<table border='0' cellborder='0'><tr>";
      for(int i=0; i < j->mWrap->lm().size(); i++) {
        if (j->mWrap->isGB())
          ss << "<td><B>" << j->mWrap->lm()[i] << "</B></td>";
        else
          ss << "<td>" << j->mWrap->lm()[i] << "</td>";
      }
      ss << "</tr><tr>";
      for(int i=0; i < j->mWrap->lm().size(); i++) {
        ss << "<td>";
        if (j->mWrap->NM(i))
          ss << '*';
        ss << "</td>";
      }
      ss << "</tr><tr>";
      for(int i=0; i < j->mWrap->lm().size(); i++) {
        ss << "<td>";
        if (j->mWrap->build(i))
          ss << '*';
        ss << "</td>";
      }
      ss << "</tr></table>";
      agstrfree(g, agstrdup_html(g, (char*)ss.str().c_str()));
      agsafeset(node, (char*)"label", (char*)ss.str().c_str(), (char*)"");
      agsafeset(node, (char*)"shape", (char*)"plaintext", (char*)"");
    }
    else {
      ss << j;
      node = agnode(g, (char*)ss.str().c_str(), 1);
      ss.str("");
      ss << j->mDeg;
      agsafeset(node, (char*)"label", (char*)ss.str().c_str(), (char*)"");
      agsafeset(node, (char*)"shape", (char*)"plaintext", (char*)"");
      ss.str("");
    }

    if (j->mNextVar) {
      Agnode_t* right=draw(g, j->mNextVar, var+1);
      Agedge_t* e=agedge(g, node, right, (char*)"", 1);
      agsafeset(e, (char*)"style", (char*)"dotted", (char*)""); // dashed dotted
      agsafeset(e, (char*)"dir", (char*)"forward", (char*)"");
    }

    if (r == nullptr)
      r = node;
    else {
      Agedge_t* e=agedge(g, prev, node, (char*)"", 1);
      agsafeset(e, (char*)"style", (char*)"solid", (char*)"");
      agsafeset(e, (char*)"dir", (char*)"forward", (char*)"");
    }
    prev = node;
    j = j->mNextDeg;
  }

  assert(r);
  return r;
}
#endif // GINV_POLY_GRAPHVIZ

Wrap* Janet::find(const Monom &m) const {
  assert(mRoot == nullptr || mPos == m.pos());
  Link root = mRoot;
  Wrap* wrap = nullptr;

//   if (root) {
//     Janet::ConstIterator j(root);
//     unsigned d = m.degree();
//     int var = 0;
//     if (d == 0) {
//       if (j.degree() == 0)
//         wrap = j.wrap();
//     }
//     else {
//       do {
//         while(j.degree() < m[var] && j.nextDeg())
//           j.deg();
//
//         if (j.degree() > m[var])
//           break;
//         else if (j.nextVar()) {
//           d -= m[var];
//           if (d == 0)
//             break;
//           ++var;
//           j.var();
//         }
//         else {
//           wrap = j.wrap();
//           assert(wrap != nullptr);
//           assert(wrap->lm() | m);
//           break;
//         }
//       } while(true);
//     }
//   }

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

//         j.degree() >= m[var] || j.nextDeg()
//         
//         1 j.degree() > m[var]
//         2 j.degree() == m[var] || j.nextDeg()
        
        if (j.degree() > m[var])
          break;
        else if (j.wrap()) {
          wrap = j.wrap();
          assert(wrap != nullptr);
          assert(m.divisiable(wrap->lm()));
          break;
        }
        else {
          assert(j.nextVar());
          d -= m[var];
          if (d == 0)
            break;
          ++var;
          j.var();
        }
      } while(true);
    }
  }
  return wrap;
}

void Janet::insert(Wrap *wrap) {
  assert(wrap != nullptr);
  assert(find(wrap->lm()) == nullptr);

  Link &root = mRoot;
  unsigned d = wrap->lm().degree();
  Janet::Iterator j(root);

  if (root == nullptr) {
    mPos = wrap->lm().pos();
    j.build(d, 0, wrap, mAllocator);
  }
  else {
    int var = 0;
    do {
      while(j.degree() < wrap->lm()[var] && j.nextDeg())
        j.deg();

      if (j.degree() > wrap->lm()[var]) {
        wrap->setNM(var);
        j.build(d, var, wrap, mAllocator);
        ++var;
        break;
      }
      else if (j.degree() == wrap->lm()[var]) {
        if (j.nextDeg())
          wrap->setNM(var);
        d -= wrap->lm()[var];
        ++var;
        j.var();
      }
      else {
        Janet::ConstIterator(j).nextVar().setNM(var);
        j.deg();
        j.build(d, var, wrap, mAllocator);
        break;
      }
    } while(true);
  }
  if (find(wrap->lm()) == nullptr) {
    std::cerr << "error: " << wrap->lm() << std::endl;
    draw("pdf", "janet_tree1.pdf");
  }
  assert(find(wrap->lm()) != nullptr);
  assert(Janet::ConstIterator(root).assertValid());
}

void Janet::setNM(Wrap *wrap) {
  assert(wrap != nullptr);
  assert(find(wrap->lm()) == nullptr);

  Link &root = mRoot;
  unsigned d = wrap->lm().degree();
  Janet::Iterator j(root);

  if (root == nullptr) {
    mPos = wrap->lm().pos();
    j.build(d, 0, wrap, mAllocator);
  }
  else {
    int var = 0;
    do {
      while(j.degree() < wrap->lm()[var] && j.nextDeg())
        j.deg();

      if (j.degree() > wrap->lm()[var]) {
        wrap->setNM(var);
        j.build(d, var, wrap, mAllocator);
        ++var;
        break;
      }
      else if (j.degree() == wrap->lm()[var]) {
        if (j.nextDeg())
          wrap->setNM(var);
        d -= wrap->lm()[var];
        ++var;
        j.var();
      }
      else {
        Janet::ConstIterator(j).nextVar().setNM(var);
        j.deg();
        j.build(d, var, wrap, mAllocator);
        break;
      }
    } while(true);
  }
  assert(find(wrap->lm()) != nullptr);
  assert(Janet::ConstIterator(root).assertValid());
}

// void Janet::insert(Wrap *wrap) {
//   assert(wrap != nullptr);
//   assert(find(wrap->lm()) == nullptr);
//
//   Link &root = mRoot;
//   unsigned d = wrap->lm().degree();
//   Janet::Iterator j(root);
//
//   if (root == nullptr) {
// //     wrap->clearNM();
//     j.build(d, 0, wrap, mAllocator);
//   }
//   else {
//     int var = 0;
//     do {
//       while(j.degree() < wrap->lm()[var] && j.nextDeg())
//         j.deg();
//
//       if (j.degree() > wrap->lm()[var]) {
//         wrap->setNM(var);
//         j.build(d, var, wrap, mAllocator);
//         ++var;
// //         wrap->clearMN(++var);
//         break;
//       }
//       else if (j.degree() == wrap->lm()[var]) {
//         if (j.nextDeg())
//           wrap->setNM(var);
// //         else
// //           wrap->clearMN(var);
//         d -= wrap->lm()[var];
//         ++var;
//         j.var();
//       }
//       else {
//         assert(Janet::ConstIterator(j).nextVar());
//         Janet::ConstIterator(j).nextVar().setNM(var);
//         j.deg();
//         j.build(d, var, wrap, mAllocator);
// //         wrap->clearMN(var);
//         break;
//       }
//     } while(true);
//   }
// //   assert(find(wrap->lm()) != nullptr);
//   assert(Janet::ConstIterator(root).assertValid());
// }

}

// TODO



// void Janet::update(Wrap* wrap, ISetQ &setQ) {
//   assert(wrap != nullptr);
//   assert(find(wrap->lm()) != nullptr);
//
//   Link root = mRoot[depend];
//   Janet::ConstIterator j(root);
//   int var = 0;
//   do {
//     while(j.degree() < wrap->lm()[var]) {
//       assert(j.nextDeg());
//       j.deg();
//     }
//     assert(j.degree() == wrap->lm()[var]);
//     if (!j.nextDeg())
//       wrap->clearBuild(var);
//     else if (wrap->notBuild(var))
//       setQ.insert(wrap, var);
//     ++var;
//     if (j.nextVar())
//       j.var();
//     else {
//       assert(wrap == j.wrap());
//       for(; var < wrap->lm().dimIndepend(); var++)
//         wrap->clearBuild(var);
//       break;
//     }
//   } while(true);
// }

