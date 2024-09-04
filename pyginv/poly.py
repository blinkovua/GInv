import sympy
from sympy import S, Basic, gcd
from monom import *

class Poly(list):
  def __init__(self, *args):
    if args:
      assert len(args) <= 2
      if len(args) == 2:
        assert isinstance(args[0], Monom) and \
          (isinstance(args[1], int) or isinstance(args[1], Basic)) and args[1]
        self.append([args[0], args[1]])
      else:
        if isinstance(args[0], int) or isinstance(args[0], Basic):
          assert args[0]
          self.append([Monom(), args[0]])
        elif isinstance(args[0], Monom):
          self.append([args[0], 1])
        else:
          assert isinstance(args[0], Poly)
          for m, k in args[0]:
            self.append([m, k])
    assert self.assertValid()

  def __str__(self):
    if not self:
      return '0'
    else:
      r = []
      for m, c in self:
        if m.degree() == 0 and m.position() < 0:
          r.append(str(c))
        elif c == S.One:
          r.append(str(m))
        else:
          r.append(f"{m}*{c}")
      return " + ".join(r)

  def lm(self):
    assert self
    return self[0][0]

  def lc(self):
    assert self
    return self[0][1]

  def find(self, m):
    assert self
    return self if m.divisible(self[0][0]) else None

  def cmp(self, other):
    return self.lm().cmp(other.lm())

  def __pos__(self):
    return self

  def __neg__(self):
    p = Poly()
    for m, k in self:
      p.append([m, -k])
    return p

  def __radd__(self, other):
    return self+other

  def __add__(self, other):
    if other == S.Zero:
      return self
    if isinstance(other, int) or isinstance(other, Basic):
      other = Poly(Monom(), other)
    p = Poly()
    i, j, iend, jend = 0, 0, len(self), len(other)
    while i < iend and j < jend:
      c = self[i][0].cmp(other[j][0])
      if c > 0:
        p.append(self[i])
        i += 1
      elif c < 0:
        p.append(other[j])
        j += 1
      else:
        k = self[i][1] + other[j][1]
        if k:
          p.append([self[i][0], k])
        i += 1
        j += 1
    while i < iend:
      p.append(self[i])
      i += 1
    while j < jend:
      p.append(other[j])
      j += 1
    assert p.assertValid()
    return p

  def __rsub__(self, other):
    return self-other

  def __sub__(self, other):
    if other == S.Zero:
      return self
    if isinstance(other, int) or isinstance(other, Basic):
      other = Poly(Monom(), other)
    p = Poly()
    i, j, iend, jend = 0, 0, len(self), len(other)
    while i < iend and j < jend:
      c = self[i][0].cmp(other[j][0])
      if c > 0:
        p.append(self[i])
        i += 1
      elif c < 0:
        p.append([other[j][0], -other[j][1]])
        j += 1
      else:
        k = self[i][1] - other[j][1]
        if k:
          p.append([self[i][0], k])
        i += 1
        j += 1
    while i < iend:
      p.append(self[i])
      i += 1
    while j < jend:
      p.append([other[j][0], -other[j][1]])
      j += 1
    assert p.assertValid()
    return p

  def __rmul__(self, other):
    return self*other

  def __mul__(self, other):
    if other == S.One:
      return self
    if isinstance(other, int) or isinstance(other, Basic):
      other = Poly(Monom(), other)
    t = {}
    for m1, k1 in self:
      for m2, k2 in other:
        m, k = m1*m2, k1*k2
        if m in t:
          t[m] += k
        else:
          t[m] = k
    p = Poly()
    for m, k in sorted(t.items(), reverse=True):
      if k:
        p.append([m, k])
    assert p.assertValid()
    return p

  def __pow__(self, other):
    assert other >= 0
    if other == 0:
      p = Poly(Monom(), S.One)
    else:
      p = self
      for i in range(1, other):
        p = p*self
    return p

  def prolong(self, var):
    p = Poly()
    for m, k in self:
      p.append([m.prolong(var), k])
    assert p.assertValid()
    return p

  def diff(self, var):
    p = Poly()
    for m, k in self:
      k = sympy.diff(k, var)
      if k:
        p.append([m, k])
    assert p.assertValid()
    return p

  def mult(self, m1, k1):
    assert k1 != S.Zero
    p = Poly()
    for m2, k2 in self:
      p.append([m1*m2, k1*k2])
    assert p.assertValid()
    return p

  def pp(self):
    assert self
    g = self[0][1]
    for mk in self[1:]:
      g = gcd(g, mk[1])
      if g == S.One:
        return
    for mk in self:
      mk[1] /= g
      sympy.simplify(mk[1])
    assert self.assertValid()

  def reduction(self, i, other):
    assert id(self) != id(other)
    assert 0 <= i < len(self)
    assert self[i][0].divisible(other[0][0])
    k = gcd(self[i][1], other[0][1])
    m2, k1, k2 = self[i][0]/other[0][0], sympy.simplify(other[0][1]/k), sympy.simplify(-self[i][1]/k)
    for mk in self[:i]:
      mk[1] *= k1
    del self[i]
    j, iend, jend = 1, len(self), len(other)
    while i < len(self) and j < jend:
      c = self[i][0].cmp(other[j][0]*m2)
      if c > 0:
        self[i][1] *= k1
        i += 1
      elif c < 0:
        self.insert(i, [other[j][0]*m2, other[j][1]*k2])
        i += 1
        j += 1
      else:
        k = sympy.simplify(self[i][1]*k1 + other[j][1]*k2)
        if k == S.Zero:
          del self[i]
        else:
          self[i][1] = k
          i += 1
        j += 1
    while i < len(self):
      self[i][1] *= k1
      i += 1
    while j < jend:
      self.append([other[j][0]*m2, other[j][1]*k2])
      j += 1
    assert self.assertValid()

  def reduce(self, other):
    assert isinstance(other, Poly)
    assert self and other
    i, iend, lm = 0, len(self), other[0][0]
    while i < iend and self[i][0].cmp(lm) > 0:
      i += 1
    if i < iend and self[i][0].cmp(lm) == 0:
      self.reduction(i, other)
      assert self.assertValid()

  def NFhead(self, pset):
    while self:
      p = pset.find(self[0][0])
      if p:
        self.reduction(0, p)
      else:
        break

  def NFtail(self, pset):
    assert self
    i = 1
    while i < len(self):
      p = pset.find(self[i][0])
      if p:
        self.reduction(i, p)
      else:
        i += 1

  def NF(self, pset):
    self.NFhead(pset)
    if self:
      self.NFtail(pset)
    return self

  @staticmethod
  def S(self, other):
    assert self and other
    assert self.lm().position() == other.lm().position()
    m, k = self[0][0].lcm(other[0][0]), gcd(self[0][1], other[0][1])
    #assert self[0][1]%k == S.Zero and other[0][1]%k == S.Zero
    return self.mult(m/self[0][0], other[0][1]/k) - other.mult(m/other[0][0], self[0][1]/k)

  def assertValid(self):
    for i in range(len(self)-1):
      if self[i][1] == S.Zero or self[i][0].cmp(self[i+1][0]) <= 0:
        print(self[i], self[i+1])
        return False
    return not self or self[-1][1]


if __name__ == '__main__':
  sympy.init_printing()

  var = ['a', 'b', 'c', 'd', 'e', 'f']
  fun = ['u', 'v', 'w']
  Monom.init(var, fun)

  Monom.cmp = Monom.POTlex

  for i, g in enumerate(var):
    globals()[g] = Poly(Monom(i))
  for i, g in enumerate(fun):
    globals()[g] = Poly(Monom(pos=i))

  print(repr(Poly()))
  print(Poly())

  alpha, beta, tau = sympy.symbols('alpha, beta, tau', real=True)
  print(alpha)

  print(repr(Poly(21546)))
  print(Poly(21546))

  print(repr(Poly(tau**4*21546)))
  print(Poly(tau**4*21546))


  h = u*c**5 + f*a*b*tau*1236537 + d*alpha
  print(repr(h))

  g = Poly(h)
  print(g)

  print(g != Poly(h))

  print(h.lm(), h.lc())

  print(g.prolong(4))

  print(repr(g.diff(tau)))
  print(g.diff(tau))

  g.reduction(0, h)
  print(g)

  h *= (4*b**4 + f*tau + beta)**11
  print(h)

  h.NFtail(4*b**4 + f*tau + beta)
  print(h)

  h.NFhead(4*b**4 + f*tau + beta)
  print(h)

  h = (4*b**4 + a*b*c**2 + f*tau + beta)
  h.reduce(a*b*c**2 + f*tau)
  print(h)

  h = (4*b**4 + a*b*c**2 + f*tau + beta)
  h.reduce(a*b*c + f*tau)
  print(h)
