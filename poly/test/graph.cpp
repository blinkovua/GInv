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
#include <graphviz/gvc.h>

#include "poly/janet.h"

using namespace GInv;

int main(int argc, char *argv[]) {
  Allocator allocator[1];
  Allocator allocator1[1];
  Monom var[5] = {
    {0, 5,-1, allocator},
    {1, 5,-1, allocator},
    {2, 5,-1, allocator},
    {3, 5,-1, allocator},
    {4, 5,-1, allocator},
  };

  const int n=6;
  Monom::Variable a[n][5] = {
    {1, 0, 1, 1, 4},
    {3, 0, 2, 0, 1},
    {0, 0, 4, 1, 0},
    {0, 0, 3, 0, 2},
    {0, 0, 1, 3, 1},
    {1, 0, 2, 0, 3},
  };

  Monom m[n] = {
    {5, -1, allocator},
    {5, -1, allocator},
    {5, -1, allocator},
    {5, -1, allocator},
    {5, -1, allocator},
    {5, -1, allocator},
  };

  for(int i=0; i < n; i++) {
    for(int k=0; k < 5; k++)
      m[i] *= var[k].pow(a[i][k]);
    std::cerr << m[i] << std::endl;
  }

  Wrap w[n] = {
    {m[0], allocator},
    {m[1], allocator},
    {m[2], allocator},
    {m[3], allocator},
    {m[4], allocator},
    {m[5], allocator},
  };

  Janet j(allocator);
  for(int i=0; i < n; i++)
    j.insert(&w[i]);

  j.draw("pdf", "janet_tree.pdf");
  return EXIT_SUCCESS;
}
