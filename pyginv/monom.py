# -*- coding: utf-8 -*-

class Monom(tuple):
  # abcdefghijklmnopqrstuvwxyz
  variables = []
  zero = ()

  def __init__(self, it):
    super(Monom, self).__init__(it)
    assert len(self) == len(Monom.variables)

  def __str__(self):
    if not self:
      return "1"
    else:
      return " ".join(Monom.variables[i] if self[i] == 1 else "%s^%d" % (Monom.variables[i], self[i]) for i in range(len(self)) if self[i])

  def degree(self):
    return sum(self)

  def lcm(self, other):
    return Monom(max(self[i], other[i]) for i in range(len(Monom.variables)))

  def gcd(self, other):
    return Monom(min(self[i], other[i]) for i in range(len(Monom.variables)))

  def __nonzero__(self):
    return sum(self) > 0

  def prolong(self, var):
    assert 0 <= var < len(Monom.variables)
    return Monom(self[i] + (1 if i == var else 0) for i in range(len(Monom.variables)))

  def __mul__(self, other):
    return Monom(self[i] + other[i] for i in range(len(Monom.variables)))

  def divisible(self, other):
    for i in range(len(Monom.variables)):
      if self[i] < other[i]:
        return False
    return True

  def divisibleTrue(self, other):
    i = 0
    while i < len(Monom.variables) and self[i] == other[i]:
      i += 1
    if i == len(Monom.variables) or self[i] < other[i]:
      return False
    else:
      i += 1
      while i < len(Monom.variables) and self[i] >= other[i]:
        i += 1
      return i == len(Monom.variables)

  def __div__(self, other):
    assert self.divisible(other)
    return Monom(self[i] - other[i] for i in range(len(Monom.variables)))

  def __pow__(self, other):
    assert other >= 0
    return Monom(self[i]*other for i in range(len(Monom.variables)))

  def __cmp__(self, other):
    assert False

  def lex(self, other):
    for i in range(len(Monom.variables)):
      if self[i] > other[i]:
        return 1
      elif self[i] < other[i]:
        return -1
    return 0

  def deglex(self, other):
    d1, d2 = self.degree(), other.degree()
    if d1 > d2:
      return 1
    elif d1 < d2:
      return -1
    else:
      for i in range(len(Monom.variables)-1, -1, -1):
        if self[i] < other[i]:
          return 1
        elif self[i] > other[i]:
          return -1
      return 0

  def alex(self, other):
    d1, d2 = self.degree(), other.degree()
    if d1 > d2:
      return -1
    elif d1 < d2:
      return 1
    else:
      for i in range(len(Monom.variables)):
        if self[i] > other[i]:
          return 1
        elif self[i] < other[i]:
          return -1
      return 0

if __name__ == '__main__':
  Monom.variables = ['a', 'b', 'c', 'd', 'e', 'f']
  # lex deglex alex
  Monom.__cmp__ = Monom.alex
  Monom.zero = Monom(0 for v in Monom.variables)
  for i in range(len(Monom.variables)):
    globals()[Monom.variables[i]] = Monom(0 if l != i else 1 for l in range(len(Monom.variables)))

  m1 = b*c**2
  print m1
  print repr(m1)

  m2 = a*e*f**2
  print m2, m2.prolong(3)
  print m1.lex(m2)
  print m1.deglex(m2)
  print m1.alex(m2)
  print m1 == m2
  print m1 < m2 <= m2
  a = set([m1, m2, a, b, c])
  print a

