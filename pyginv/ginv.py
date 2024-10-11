
from pprint import pprint
import heapq, time

from janet import *

class Q(list):
  def __init__(self, *args):
    assert len(args) <= 1
    super().__init__(*args)
    heapq.heapify(self)

  def push(self, w):
    if isinstance(w, Wrap):
      heapq.heappush(self, w)
    else:
      for i in w:
        assert isinstance(i, Wrap)
        heapq.heappush(self, i)

  def pop(self):
    return heapq.heappop(self)

  def reduce(self, invdiv):
    d, r = 0, []
    while self and (not r or self[0].degree() == d):
      w = self.pop()
      w.poly.NFhead(invdiv)
      if w.poly:
        w.update()
        w.poly.pp()
        if not r:
          d = w.degree()
        elif d > w.degree():
          for i in r:
            self.push(i)
          d, r = w.lm.degree(), []
        r.append(w)
    for i in r:
      i.poly.NFtail(invdiv)
      i.poly.pp()
    return r

  def autoReduce(self, q):
    assert q
    def get_min(l):
      i = 0
      for j in range(1, len(l)):
        if l[i] > l[j]:
          i = j
      w = l[i]
      del l[i]
      return w

    d, r = q[0].degree(), []
    while q:
      w = get_min(q)
      l = []
      for v in q:
        v.poly.reduce(w.poly)
        if v.poly:
          v.update()
          d = min(d, v.degree())
          l.append(v)
      q = l
      r.append(w)
    res = []
    for w in r:
      if w.degree() != d:
        self.push(w)
      else:
        w.poly.pp()
        res.append(w)

    return res

def ginvBlockLow(pset, invdiv, level=0):
  assert pset
  assert Monom.cmp == Monom.TOPdeglex
  if type(pset[0]) == Poly:
    tp = 0
  elif type(pset[0]) == PolyDiff:
    tp = 1

  t = time.time()
  q = Q(Wrap(p) for p in pset)
  while True:
    j = q[0].degree() if q else 0
    i = invdiv.degMinProlong()
    if level > 0:
      print(f"prolong {i}, Q {j}")
    if i == 0 and j == 0:
      break
    if j == 0 or 0 < i < j:
      q.push(invdiv.prolong(i))
    res = q.reduce(invdiv)
    if res:
      res = q.autoReduce(res)
      if level > 0:
        if tp == 1:
          for w in res:
            print(f"{w.lm}", end=", ")
            print()
        if tp == 2:
          for w in res:
            print(f"{w.lm.df()}", end=", ")
            print()
      q.push(invdiv.insert(res))
  return time.time() - t


