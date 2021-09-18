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

Poly::Poly(Allocator* allocator, const Poly& a);

Poly::~Poly();

void Poly::swap(Poly& a);

void Poly::operator=(Poly &&a) {

void Poly::operator=(const Poly &a);

int Poly::norm() const;

void Poly::nf(Janet &a);
void Poly::nfTail(Janet &a);
bool Poly::isPp() const;
void Poly::pp();

std::ostream& operator<<(std::ostream& out, const Poly &a);
  
}
