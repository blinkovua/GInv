from pprint import pprint
import heapq, time

from janet import *

class Q(list):
  def __init__(self, *args):
    assert len(args) <= 1
    super().__init__(*args)
    heapq.heapify(self)
    self.crit1 = 0
    self.crit2 = 0

  def push(self, w):
    if isinstance(w, Wrap):
      heapq.heappush(self, w)
    else:
      for i in w:
        assert isinstance(i, Wrap)
        heapq.heappush(self, i)

  def pop(self):
    return heapq.heappop(self)

  def reduceMinDegree(self, invdiv):
    d, r = 0, []
    while self and (not r or self[0].degree() == d):
      w = self.pop()
      w1 = invdiv.findWrap(w.lm)
      if not w1:
        r.append(w)
      else:
        if w.crit1(w1):
          self.crit1 += 1
        elif w.crit2(w1):
          self.crit2 += 1
        else:
          w.refresh(w1)
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
        v.refresh(w)
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
    res = q.reduceMinDegree(invdiv)
    if res:
      res = q.autoReduce(res)
      if tp == 1:
        for r in res:
          r.poly.NFtail(invdiv)
          r.poly.pp()
      if level > 0:
        if tp == 0:
          print(", ".join(f"{w.lm}" for w in res))
        if tp == 1:
          print(", ".join(f"{w.lm.df()}" for w in res))
      q.push(invdiv.insert(res))

      for w in invdiv:
        w.poly.NFtail(invdiv)
        w.poly.pp()

  return time.time() - t, q.crit1, q.crit2


