/***************************************************************************
 *   Copyright (C) 2017 by Blinkov Yu. A.                                  *
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

#ifndef GINV_GB_H
#define GINV_GB_H

#include "janet.h"

#include "config.h"

#ifdef GINV_GRAPHVIZ
  #include <sstream>
  #include <graphviz/gvc.h>
#endif // GINV_GRAPHVIZ

namespace GInv {

class GB {
  static const int sMaxDegree;

  Allocator*       mAllocator;
  int              mSize;
  int              mMaxDegree;
  Janet**          mForest;

public:
  explicit GB(Allocator* allocator);
  ~GB();

  int size() const { return mSize;}
  const Janet* operator[](int i) const {
    assert(0 <= i && i < mMaxDegree);
    return mForest[i];
  }

  Wrap* find(const Monom &m) const;
  void insert(Wrap *wrap);

#ifdef GINV_GRAPHVIZ
  void draw(const char* format, const char* filename) const {
    GVC_t *gvc=gvContext();
    Agraph_t *g=agopen((char*)"GB",  Agdirected, (Agdisc_t*)nullptr);
    std::stringstream ss;
    ss << "#GB = " << size();
    agnode(g, (char*)ss.str().c_str(), 1);
    for(int i=0; i < mMaxDegree; i++)
      if (mForest[i])
        mForest[i]->draw(g, true);
    gvLayout(gvc, g, (char*)"dot");
    gvRenderFilename(gvc, g, format, filename);
    gvFreeLayout(gvc, g);
    agclose(g);
  }
#endif // GINV_GRAPHVIZ
};

}

#endif // GINV_GB_H
