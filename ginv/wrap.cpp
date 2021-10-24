/***************************************************************************
 *   Copyright (C) 2019 by Blinkov Yu. A.                                  *
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

#include "wrap.h"

namespace GInv {

void Wrap::update(const Monom& m) {
  if (mLm.lex(m) != 0) {
    mLm = m;
    mAnsector = m;
    for(int i=0; i < mLm.size(); i++) {
      mNM[i] = false;
      mNMd[i] = false;
      mBuild[i] = false;
    }
  }
}

std::ostream& operator<<(std::ostream& out, const Wrap &a) {
  out << a.lm() << a.ansector() << '[';
  if (a.NM(0))
    out << '*';
  else
    out << ' ';
  for(int i=1; i < a.lm().size(); i++)
    if (a.NM(i))
      out << '*';
    else
      out << ' ';
  out << ']';
  return out;
}

}
