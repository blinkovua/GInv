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

// Monom::Variable Janet::ConstIterator::nextDegree() const {
//   Monom::Variable v=0;
//   do {
//     assert(nextVar());
//     var();
//     ++v;
//   } while(degree() > 0 || nextDeg());
//   return v;
// }

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
  while(*i) {
    if (nextVar())
      Janet::Iterator((*i)->mNextVar).clear(allocator);
    del(allocator);
  }
}

void Janet::ConstIterator::setNM(Monom::Variable var) {
  assert(i);
  while(nextDeg()) {
    if (nextVar())
      nextVar().setNM(var);
    if (wrap())
      wrap()->setNM(var);
    deg();
  }
  if (nextVar())
    nextVar().setNM(var);
  if (wrap())
    wrap()->setNM(var);
}

bool Janet::ConstIterator::assertValid() {
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

#ifdef GINV_GRAPHVIZ
Agnode_t* Janet::draw(Agraph_t *g, Link j, Monom::Variable var, bool NMd) {
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
      if (NMd) {
        ss << "</tr><tr>";
        for(int i=0; i < j->mWrap->lm().size(); i++) {
          ss << "<td>";
          if (j->mWrap->NMd(i))
            ss << '#';
          ss << "</td>";
        }
      }
      ss << "</tr><tr>";
      for(int i=0; i < j->mWrap->lm().size(); i++) {
        ss << "<td>";
        if (j->mWrap->build(i))
          ss << "&amp;";
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
      Agnode_t* right=draw(g, j->mNextVar, var+1, NMd);
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

//   assert(r);
  return r;
}
#endif // GINV_GRAPHVIZ

void Janet::setMNprec(Wrap *wrap, int v, ConstIterator j) {
  assert(j);
  int d=wrap->lm()[v];
  do  {
    if (j.degree() > d) {
      wrap->setNMd(v);
      break;
    }
    if (j.nextVar())
      setMNprec(wrap, v + 1, j.nextVar());
    j.deg();
  } while(j);
}

void Janet::setMNsucc(Wrap *wrap, int v,  ConstIterator j) {
//   return;
//   assert(j)
//   int d=wrap->lm()[v];
//   do  {
//     if (j.degree() <= d && nextDeg()) {
//       if (j.nextVar())
//         setMNprec(wrap, v + 1, j.nextVar())
//     }
//     if (j.nextVar());
//       setMNprec(wrap, v + 1, j.nextVar())
//     deg();
//   } while(j);
}


Wrap* Janet::find(const Monom &m) const {
  assert(mPos == m.pos());
  Link root=mRoot;
  Wrap* wrap=nullptr;

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
        assert(j);
        assert(d > 0);
        assert(var < m.size());

        while(j.degree() < m[var] && j.nextDeg())
          j.deg();

        if (j.degree() > m[var])
          break;
        else {
          d -= m[var];
          if (d == 0) {
            if (j.wrap() || (j.degree() < m[var] && !j.nextVar())) {
//             if (j.wrap()) {
//               if (j.degree() < m[var] && !j.nextVar())
//                 std::cerr << "****************" << std::endl;
              assert(j.wrap());
              wrap = j.wrap();
              assert(wrap->lm() | m);
            }
            break;
          }
          if (j.wrap()) {
            int v=0;
            for(; v < m.size(); v++) {
              if (j.wrap()->lm()[v] > m[v] ||
                (j.wrap()->lm()[v] < m[v] && j.wrap()->NM(v)))
                break;
            }
            if (v == m.size()) {
              wrap = j.wrap();
              break;
            }
          }
          if (j.nextVar()) {
            ++var;
            j.var();
          }
          else {
            assert(j.wrap());
            wrap = j.wrap();
            assert(wrap->lm() | m);
            break;
          }
        }
      } while(true);
    }
  }
  return wrap;
}

void Janet::insert(Wrap *wrap) {
  assert(wrap != nullptr);
  assert(mPos == wrap->lm().pos());
  assert(find(wrap->lm()) == nullptr);

  Link &root = mRoot;
  unsigned d = wrap->lm().degree();
  Janet::Iterator j(root);

  if (root == nullptr)
    j.build(d, 0, wrap, mAllocator);
  else {
    Monom::Variable var = 0;
    do {
      assert(j);
      assert(d > 0);
      assert(var < wrap->lm().size());

      while(j.degree() < wrap->lm()[var] && j.nextDeg())
        j.deg();

      if (j.degree() > wrap->lm()[var]) {
        wrap->setNM(var);
        j.build(d, var, wrap, mAllocator);
        break;
      }
      else if (j.degree() < wrap->lm()[var]) {
        assert(!j.nextDeg());
        if (j.wrap())
          j.wrap()->setNM(var);
        if (j.nextVar())
          Janet::ConstIterator(j).nextVar().setNM(var);
        j.deg();
        j.build(d, var, wrap, mAllocator);
        break;
      }
      else { //TODO
        assert(j.degree() == wrap->lm()[var]);
        if (j.nextDeg())
          wrap->setNM(var);
        if (j.wrap())
          for(int v=var+1; v < wrap->lm().size(); v++)
            if (wrap->lm()[v] > j.wrap()->lm()[v]) {
              j.wrap()->setNM(v);
              break;
            }
        d -= wrap->lm()[var];
        if (d == 0) {
          assert(j.wrap() == nullptr);
          j.wrap() = wrap;
          if (j.nextVar()) {
            do {
              assert(var < wrap->lm().size());
              assert(j.nextVar());
              j.var();
              ++var;
              if (j.degree() > 0 || j.nextDeg())
                wrap->setNM(var);
            } while(j.degree() == 0 && j.nextVar());
          }
          break;
        }
        else {
          if (!j.nextVar()) {
            assert(j.wrap());
            Monom::Variable v=var+1;
            while(wrap->lm()[v] == 0) {
              ++v;
              assert(v < wrap->lm().size());
            }
            j.wrap()->setNM(v);
          }
          ++var;
          j.var();
          if (!j) {
            j.build(d, var, wrap, mAllocator);
            break;
          }
        }
      }
    } while(true);
  }
  ++mSize;

  assert(find(wrap->lm()) != nullptr);
  assert(Janet::ConstIterator(root).assertValid());
}

}
