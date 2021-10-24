/***************************************************************************
 *   Copyright (C) 2021 by Blinkov Yu. A.                                  *
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

#ifndef GINV_POLY_H
#define GINV_POLY_H

#include "monom.h"

#include "config.h"

namespace GInv {

class Poly {
public:
  enum Pos {TOP=0x10,
            POT=0x20
  };
  enum Order {lex=   0x01,
              deglex=0x02,
              alex=  0x04
  };

protected:
  static int TOPlex(const Monom& a, const Monom& b);
  static int TOPdeglex(const Monom& a, const Monom& b);
  static int TOPalex(const Monom& a, const Monom& b);

  static int TOPlex2(const Monom& a, const Monom& b, const Monom& c);
  static int TOPdeglex2(const Monom& a, const Monom& b, const Monom& c);
  static int TOPalex2(const Monom& a, const Monom& b, const Monom& c);

  static int POTlex(const Monom& a, const Monom& b);
  static int POTdeglex(const Monom& a, const Monom& b);
  static int POTalex(const Monom& a, const Monom& b);

  static int POTlex2(const Monom& a, const Monom& b, const Monom& c);
  static int POTdeglex2(const Monom& a, const Monom& b, const Monom& c);
  static int POTalex2(const Monom& a, const Monom& b, const Monom& c);

  int mOrder;
  int mSize;
  int (*mCmp1)(const Monom&, const Monom&);
  int (*mCmp2)(const Monom&, const Monom&, const Monom&);

  void setOrder(int order);

public:
  Poly()=delete;
  Poly(const Poly& a):
      mOrder(a.mOrder),
      mSize(a.mSize),
      mCmp1(a.mCmp1),
      mCmp2(a.mCmp2) {
  }
  Poly(Poly&& a):
      mOrder(a.mOrder),
      mSize(a.mSize),
      mCmp1(a.mCmp1),
      mCmp2(a.mCmp2) {
  }
  Poly(int order, int size):
      mOrder(order),
      mSize(size) {
    assert(size > 0);
    setOrder(order);
  }
  ~Poly() {}

  void swap(Poly& a);
  void operator=(Poly &&a);
  void operator=(const Poly &a);

  int order() const { return mOrder; }
  int size() const { return mSize; }

  bool comparable(const Poly& a) const {
    return mSize == a.mSize && mOrder == a.mOrder;
  }
};

}

#endif // GINV_POLY_H
