import sympy
from sympy import S, Basic, gcd
from monom import *

class Poly(list):
  def __init__(self):
    super().__init__()

  def copy(self):
    p = Poly()
    for m, k in self:
      p.append([m, k])
    return p

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
      tmp = Poly()
      tmp.append([Monom.zero, other])
      other = tmp
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
      tmp = Poly()
      tmp.append([Monom.zero, other])
      other = tmp
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
      tmp = Poly()
      tmp.append([Monom.zero, other])
      other = tmp
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
      p = Poly()
      p.append([Monom.zero, S.One])
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
    assert 0 <= i < len(self)
    assert self[i][0].position() == other[0][0].position()
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
  import sympy
  x, y, z = sympy.symbols('x y z')

  variables = ['a', 'b', 'c', 'd', 'e', 'f']
  Monom.variables = len(variables) + 1
  # POTlex TOPlex POTdeglex TOPdeglex POTalex TOPalex
  Monom.cmp = Monom.POTlex
  Monom.zero = Monom(0 for v in range(Monom.variables))
  for i, v in enumerate(variables):
    p = Poly()
    p.append([Monom(0 if l-1 != i else 1 for l in range(Monom.variables)), 1])
    globals()[v] = p

  print((b+a)*c*(b+a))
  print((a + y)*(z + c) + a*b*c*2)
  f = -(a - c + x)**11
  print(f)
  f.reduction(2, (2*c + x + a)**10*2)
  print(f)
  f = -(a - c + 1)**11
  f.NF((a - c + 1)**2 + b)
  print(f)
  print("S(a*b**2 - 2, 3*a**2*b - 4) =", Poly.S(a*b**2 - 2, a**2*b*3 - 4))
