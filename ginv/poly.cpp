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

#include "poly.h"

namespace GInv {

int Poly::TOPlex(const Monom& a, const Monom& b) {
  int r=a.lex(b);
  if (r == 0 && a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  return r;
}

int Poly::TOPdeglex(const Monom& a, const Monom& b) {
  int r=a.deglex(b);
  if (r == 0 && a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  return r;
}

int Poly::TOPalex(const Monom& a, const Monom& b) {
  int r=a.alex(b);
  if (r == 0 && a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  return r;
}

int Poly::TOPlex2(const Monom& a, const Monom& b, const Monom& c) {
  int r=a.lex(b, c);
  if (r == 0 && a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  return r;
}

int Poly::TOPdeglex2(const Monom& a, const Monom& b, const Monom& c) {
  int r=a.deglex(b, c);
  if (r == 0 && a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  return r;
}

int Poly::TOPalex2(const Monom& a, const Monom& b, const Monom& c) {
  int r=a.alex(b, c);
  if (r == 0 && a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  return r;
}

int Poly::POTlex(const Monom& a, const Monom& b) {
  int r;
  if (a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  else
    r = a.lex(b);
  return r;
}

int Poly::POTdeglex(const Monom& a, const Monom& b) {
  int r;
  if (a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  else
    r = a.deglex(b);
  return r;
}

int Poly::POTalex(const Monom& a, const Monom& b) {
  int r;
  if (a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  else
    r = a.alex(b);
  return r;
}

int Poly::POTlex2(const Monom& a, const Monom& b, const Monom& c) {
  int r;
  if (a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  else
    r = a.lex(b, c);
  return r;
}

int Poly::POTdeglex2(const Monom& a, const Monom& b, const Monom& c) {
  int r;
  if (a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  else
    r = a.deglex(b, c);
  return r;
}

int Poly::POTalex2(const Monom& a, const Monom& b, const Monom& c) {
  int r;
  if (a.pos() != b.pos())
    r = (a.pos() > b.pos()) ? 1: -1;
  else
    r = a.alex(b, c);
  return r;
}

void Poly::setOrder(int order) {
  switch(order) {
    case TOP | lex:
      mCmp1 = TOPlex;
      mCmp2 = TOPlex2;
      break;
    case TOP | deglex:
      mCmp1 = TOPdeglex;
      mCmp2 = TOPdeglex2;
      break;
    case TOP | alex:
      mCmp1 = TOPalex;
      mCmp2 = TOPalex2;
      break;
    case POT | lex:
      mCmp1 = POTlex;
      mCmp2 = POTlex2;
      break;
    case POT | deglex:
      mCmp1 = POTdeglex;
      mCmp2 = POTdeglex2;
      break;
    case POT | alex:
      mCmp1 = POTalex;
      mCmp2 = POTalex2;
      break;
    default:
      assert(false);
  }
}

void Poly::swap(Poly& a) {
  assert(this != &a);

  auto tmp1=mOrder;
  mOrder = a.mOrder;
  a.mOrder = tmp1;

  auto tmp2=mSize;
  mSize = a.mSize;
  a.mSize = tmp2;

  auto tmp3=mCmp1;
  mCmp1 = a.mCmp1;
  a.mCmp1 = tmp3;

  auto tmp4=mCmp2;
  mCmp2 = a.mCmp2;
  a.mCmp2 = tmp4;
}

void Poly::operator=(Poly &&a) {
  assert(this != &a);

  mOrder = a.mOrder;
  mSize = a.mSize;
  mCmp1 = a.mCmp1;
  mCmp2 = a.mCmp2;
}


void Poly::operator=(const Poly &a) {
  assert(this != &a);

  mOrder = a.mOrder;
  mSize = a.mSize;
  mCmp1 = a.mCmp1;
  mCmp2 = a.mCmp2;
}


}
