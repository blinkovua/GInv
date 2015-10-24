# -*- coding: utf-8 -*-

from monom import *

class Poly(list):
  @staticmethod
  def __gcd(a, b):
    while b:
      a, b = b, a%b
    return a

  def __init__(self, k=0):
    super(Poly, self).__init__()
    if k:
      self.append([Monom.zero, k])

  def __str__(self):
    if not self:
      return "0"
    else:
      lst = []
      m, k = self[0]
      if k == 1:
        lst.append(str(m))
      elif k == -1:
        lst.append("-%s" % str(m))
      elif m.degree() == 0:
        lst.append(str(k))
      else:
        lst.append("%s %s" % (k, m))
      for m, k in self[1:]:
        if k == 1:
          lst.append(" + %s" % str(m))
        elif k == -1:
          lst.append(" - %s" % str(m))
        elif m.degree() == 0:
          if k < 0:
            lst.append(" - %s" % -k)
          else:
            lst.append(" + %s" % k)
        elif k < 0:
          lst.append(" - %s %s" % (-k, m))
        else:
          lst.append(" + %s %s" % (k, m))
      return "".join(lst)

  def lm(self):
    assert self
    return self[0][0]

  def lc(self):
    assert self
    return self[0][1]

  def find(self, m):
    assert self
    return self if m.divisible(self[0][0]) else None

  def __cmp__(self, other):
    return cmp(self.lm(), other.lm())

  def __pos__(self):
    return self

  def __neg__(self):
    p = Poly()
    for m, k in self:
      p.append([m, -k])
    return p

  def __add__(self, other):
    p = Poly()
    if isinstance(other, int):
      other = Poly(other)
    i, j, iend, jend = 0, 0, len(self), len(other)
    while i < iend and j < jend:
      c = cmp(self[i][0], other[j][0])
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

  def __sub__(self, other):
    p = Poly()
    if isinstance(other, int):
      other = Poly(other)
    i, j, iend, jend = 0, 0, len(self), len(other)
    while i < iend and j < jend:
      c = cmp(self[i][0], other[j][0])
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

  def __mul__(self, other):
    if isinstance(other, int):
      other = Poly(other)
    t = {}
    for m1, k1 in self:
      for m2, k2 in other:
        m, k = m1*m2, k1*k2
        if m in t:
          t[m] += k
        else:
          t[m] = k
    p = Poly()
    for m, k in sorted(t.iteritems(), cmp=lambda a, b: -cmp(a[0], b[0])):
      if k:
        p.append([m, k])
    assert p.assertValid()
    return p

  def __pow__(self, other):
    assert other >= 0
    if other == 0:
      return Poly(1)
    else:
      return reduce(lambda x, y: x*y, (self for i in range(other)))

  #def prolong(self, var):
    #p = Poly()
    #for m, k in self:
      #p.append([m.prolong(var), k])
    #assert p.assertValid()
    #return p

  def mult(self, m1, k1):
    assert k1
    p = Poly()
    for m2, k2 in self:
      p.append([m1*m2, k1*k2])
    assert p.assertValid()
    return p

  def pp(self):
    assert self
    g = self[0][1]
    for mk in self[1:]:
      g = Poly.__gcd(g, mk[1])
      if g == 1 or g == -1:
        return
    for mk in self:
      mk[1] /= g
    assert self.assertValid()

  def reduction(self, i, other):
    assert 0 <= i < len(self)
    assert self[i][0].divisible(other[0][0])
    k = Poly.__gcd(self[i][1], other[0][1])
    m2, k1, k2 = self[i][0]/other[0][0], other[0][1]/k, -self[i][1]/k
    for mk in self[:i]:
      mk[1] *= k1
    del self[i]
    j, iend, jend = 1, len(self), len(other)
    while i < len(self) and j < jend:
      c = cmp(self[i][0], other[j][0]*m2)
      if c > 0:
        self[i][1] *= k1
        i += 1
      elif c < 0:
        self.insert(i, [other[j][0]*m2, other[j][1]*k2])
        i += 1
        j += 1
      else:
        k = self[i][1]*k1 + other[j][1]*k2
        if k == 0:
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

  @staticmethod
  def S(self, other):
    assert self and other
    m, k = self[0][0].lcm(other[0][0]), Poly.__gcd(self[0][1], other[0][1])
    assert self[0][1]%k == 0 and other[0][1]%k == 0
    return self.mult(m/self[0][0], other[0][1]/k) - other.mult(m/other[0][0], self[0][1]/k)

  def assertValid(self):
    for i in range(len(self)-1):
      if not self[i][1] or cmp(self[i][0], self[i+1][0]) <= 0:
        return False
    return not self or self[-1][1]


if __name__ == '__main__':
  Monom.variables = ['a', 'b', 'c', 'd', 'e', 'f']
  Monom.zero = Monom(0 for v in Monom.variables)
  # lex deglex
  Monom.__cmp__ = Monom.lex
  for i in range(len(Monom.variables)):
    p = Poly()
    p.append([Monom(0 if l != i else 1 for l in range(len(Monom.variables))), 1])
    globals()[Monom.variables[i]] = p

  print (b+a)*c*(b+a)
  print repr(Poly(2))
  print (Poly(2)  + a)*(c + 3) + a*b*c*2
  f = -(a - c + 1)**11
  print f
  f.reduction(2, (a - c + 1)**10*2)
  print f
  f = -(a - c + 1)**11
  f.NF((a - c + 1)**2 + b)
  print f
  print "S(a*b**2 - 2, 3*a**2*b - 4) =", Poly.S(a*b**2 - 2, Poly(3)*a**2*b - 4)
