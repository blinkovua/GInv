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
#include <cmath>

#include <graphviz/gvc.h>

#include "util/timer.h"
#include "util/list.h"
#include "util/randpermutation.h"
#include "poly/gb.h"

using namespace GInv;

void addQ(List<Wrap*> &Q, List<Wrap*> &Qtmp, int d1, int d2) {
  while(!Q && Qtmp)
    for(int d=d1; d <= d2 && !Q; d++)
      for(List<Wrap*>::Iterator j(Qtmp.begin()); j;)
        if (j.data()->lm().degree() != d)
          ++j;
        else {
          Q.push(j.data());
          j.del();
        }
}

int main(int argc, char *argv[]) {
  const int l=3,  // число экспериментов
            p=50,  // число перестановок
            n=50,   // число мономов
            s=10,    // число переменных
            d1=3,   // минимальная степень переменной
            d2=30;  // максимальная степень переменной

  RandPermutation_mt19937 per(s);
  Monom::rand_init(s, d1, d2);

  int sumJanet=0, sumGB=0;
  Timer timerJanet, timerGB;
  for(int i=0; i < l; i++) {
    Allocator allocator[1];
    List<Wrap*> Qbase(allocator);
    for(int i=0; i < n; i++)
      Qbase.push(new(allocator) Wrap(allocator, Monom(allocator, Monom::next(allocator))));
    std::cout << "*****" << std::endl;

    int sJanet[p], sGB[p];
    for(int p1=0; p1 < p; p1++) {
      timerJanet.cont();
      {
        Allocator allocator[1];
        List<Wrap*> Qtmp(allocator);
        for(List<Wrap*>::ConstIterator j(Qbase.begin()); j; ++j)
          Qtmp.push(new(allocator) Wrap(allocator, Monom(allocator, j.data()->lm(), per)));

        List<Wrap*> Q(allocator);
        addQ(Q, Qtmp, d1, d2);

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

          addQ(Q, Qtmp, d1, d2);
        }

        for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j) {
          Allocator allocator[1];
          for(int v=0; v < s; v++) {
            Monom m(allocator, v, j.data()->lm());
            assert(janet.find(j.data()->lm()));
          }
        }
        assert(T.length() == janet.size());
        std::cout << "Janet = " << T.length() << "  ";
        sumJanet += T.length();
        sJanet[p1] = T.length();
        for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j)
          allocator->destroy(j.data());
      }
      timerJanet.stop();

      timerGB.cont();
      {
        Allocator allocator[1];
        List<Wrap*> Qtmp(allocator);
        for(List<Wrap*>::ConstIterator j(Qbase.begin()); j; ++j)
          Qtmp.push(new(allocator) Wrap(allocator, Monom(allocator, j.data()->lm(), per)));

        List<Wrap*> Q(allocator);
        addQ(Q, Qtmp, d1, d2);

        GB gb(allocator);
        List<Wrap*> T(allocator);
        while(Q) {
          for(List<Wrap*>::Iterator j(Q.begin()); j; j.del()) {
            if (gb.find(j.data()->lm()))
              allocator->destroy(j.data());
            else {
              gb.insert(j.data());
              T.push(j.data());
            }
          }
          List<Wrap*> tmp(allocator);
          for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j)
            for(int k=0; k < j.data()->lm().size(); k++)
              if ((j.data()->NM(k) || j.data()->NMd(k)) && !j.data()->build(k)) {
                Wrap *w = new(allocator) Wrap(allocator, k, j.data());
                tmp.push(w);
              }
          tmp.swap(Q);

          addQ(Q, Qtmp, d1, d2);
        }

        for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j) {
          Allocator allocator[1];
          for(int v=0; v < s; v++) {
            Monom m(allocator, v, j.data()->lm());
            assert(gb.find(j.data()->lm()));
          }
        }
        assert(T.length() == gb.size());
        std::cout << "GB = " << T.length() << std::endl;
        sumGB += T.length();
        sGB[p1] = T.length();
        for(List<Wrap*>::ConstIterator j(T.begin()); j; ++j)
          allocator->destroy(j.data());

      }
      timerGB.stop();

      per.next();
    }

    double meanJanet=0., meanGB=0.;
    for(int p1=0; p1 < p; p1++) {
      meanJanet += sJanet[p1];
      meanGB += sGB[p1];
    }
    meanJanet /= p;
    meanGB /= p;

    double sdJanet=0., sdGB=0.;
    for(int p1=0; p1 < p; p1++) {
      sdJanet += (sJanet[p1] - meanJanet)*(sJanet[p1] - meanJanet);
      sdGB += (sGB[p1] - meanGB)*(sGB[p1] - meanGB);
    }
    sdJanet = sqrt(sdJanet/(p-1));
    sdGB = sqrt(sdGB/(p-1));

    std::cout << std::endl << std::fixed
      << "sdJanet = " << sdJanet
      << "  sdGB = " << sdGB << std::endl;

    for(List<Wrap*>::ConstIterator j(Qbase.begin()); j; ++j)
      allocator->destroy(j.data());
  }

  std::cout << "average Janet = " << std::fixed << float(sumJanet)/(l*p) << std::endl;
  std::cout << timerJanet << std::endl;
  std::cout << "   average GB = " << std::fixed << float(sumGB)/(l*p) << std::endl;
  std::cout << timerGB << std::endl;

  return EXIT_SUCCESS;
}
