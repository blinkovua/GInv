/***************************************************************************
 *   Copyright (C) 2018 by Blinkov Yu. A.                                  *
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

#include <cstdlib>
#include <sstream>
#include <random>

#include <graphviz/gvc.h>

#include "util/list.h"
#include "poly/janet.h"

using namespace GInv;

int prolong(Janet::ConstIterator i, Janet &j, List<Wrap*> &q, Allocator* allocator) {
  int r = 0;
  while(i.nextDeg()) {
    assert(i.nextVar());
    r += prolong(i.nextVar(), j, q, allocator);
    i.deg();
  }
  if (i.nextVar())
    r += prolong(i.nextVar(), j, q, allocator);
  else {
    for(int k=0; k < i.wrap()->lm().size(); k++)
      if (i.wrap()->NM(k) && !i.wrap()->build(k)) {
        Wrap *w = new(allocator) Wrap(allocator, k, i.wrap());
        if (!j.find(w->lm())) {
          ++r;
          j.insert(w);
          std::cerr << "* ";
        }
        std::cerr << k << ' ' << w->lm() << std::endl;
        q.push(w);
      }
  }
  return r;
}

int main(int argc, char *argv[]) {
  const int n(6),   // число переменных
            d1(2),  // минимальная степень переменной
            d2(7),  // максимальная степень переменной
            l(12);  // число мономов

  Allocator allocator[1];
  List<Wrap*> Q(allocator);
  Monom::rand_init(n, d1, d2);
  for(int i=0; i < l; i++) {
    Monom m(Monom::next(allocator));
    List<Wrap*>::Iterator j(Q.begin());
    while(j) {
      if (j.data()->lm().divisiable(m)) {
        allocator->destroy(j.data());
        j.del();
      }
      else if (m.divisiable(j.data()->lm()))
        break;
      else
        ++j;
    }
    if (!j) {
      Q.push(new(allocator) Wrap(allocator, m));
      std::cerr << m << std::endl;
    }
  }
  std::cerr << Q.length() << std::endl;

  Janet janet(allocator);
  List<Wrap*> T(allocator);
  while(Q) {
    for(List<Wrap*>::Iterator j(Q.begin()); j; j.del()) {
      if (janet.find(j.data()->lm()))
        allocator->destroy(j.data());
      else {
        janet.insert(j.data());
        T.push(j.data());
      }
    }
    List<Wrap*> tmp(allocator);
    for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j)
      for(int k=0; k < j.data()->lm().size(); k++)
        if (j.data()->NM(k) && !j.data()->build(k)) {
          Wrap *w = new(allocator) Wrap(allocator, k, j.data());
          tmp.push(w);
        }
    tmp.swap(Q);
  }
  janet.draw("pdf", "janet_tree0.pdf");

  std::cerr << "Janet = " << T.length() << std::endl;
  for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j)
    allocator->destroy(j.data());

  return EXIT_SUCCESS;
}
