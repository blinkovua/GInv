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
        Wrap *w = new(allocator) Wrap(i.wrap(), k, allocator);
        if (!j.find(w->lm()))
          j.insert(w);
        q.push(w);
        ++r;
      }
  }
  return r;
}

int main(int argc, char *argv[]) {
  const int n(6),  // число переменных
            d(5),  // максимальная степень переменной
            l(12); // число мономов
  std::random_device rd;          
//   std::default_random_engine gen(rd());
  std::default_random_engine gen;
  std::uniform_int_distribution<int> dis(0, d);
  
  Allocator allocator[1];
  Monom *var[n];
  for(int i=0; i < n; i++)
    var[i] = new(allocator) Monom(i, n,-1, allocator);

  Monom *m[l];
  for(int k=0; k < l; k++) {
    m[k] = new(allocator) Monom(n,-1, allocator);
    for(int i=0; i < n; i++) 
      *m[k] *= var[i]->pow(dis(gen));
    std::cerr << *m[k] << std::endl;
  }

  List<Wrap*> q(allocator);
  Janet j(allocator);
  for(int k=0; k < l; k++) {
    Wrap *w = new(allocator) Wrap(*m[k], allocator);
    if (!j.find(*m[k]))
      j.insert(w);
    q.push(w);
  }

  j.draw("pdf", "janet_tree0.pdf");
  int i=0;
  do {
    int k=prolong(j.begin(), j, q, allocator);
    std::cerr << k << std::endl;
    if (k == 0)
      break;
    else {
      std::stringstream ss;
      ss << "janet_tree" << ++i << ".pdf";
      j.draw("pdf", ss.str().c_str());
    }
  } while(true);
  
  for(List<Wrap*>::ConstIterator i(q.begin()); i; ++i)
    allocator->destroy(i.data());
  for(int k=0; k < l; k++)
    allocator->destroy(m[k]);
  for(int i=0; i < n; i++)
    allocator->destroy(var[n]);

  return EXIT_SUCCESS;
}
