from itertools import combinations_with_replacement as comb

class Monom(tuple):
  __var = None
  __fun = None
  __z = None
  __v = None
  __f = None

  @staticmethod
  def init(*args):
    assert len(args) in (1, 2)
    assert len(args[0]) == len(set(args[0]))
    Monom.__var = args[0]
    Monom.__z = Monom(0 for v in Monom.__var)
    Monom.__v = tuple(Monom(0 if i != j else 1 for j in range(len(Monom.__var)))\
                           for i in range(len(Monom.__var)))
    if len(args) == 2:
      assert len(args[1]) == len(set(args[1]))
      not set(args[0]) & set(args[1])
      Monom.__fun = args[1]
      Monom.__f = tuple(Monom((0 for v in Monom.__var), pos=p)\
                              for p in range(len(Monom.__fun)))

  def __new__ (cls, *args, **kargs):
    assert not kargs or 0 <= kargs['pos'] < len(Monom.__fun)
    if args:
      if isinstance(*args, int):
        assert not kargs and 0 <= args[0] < len(Monom.__var)
        return Monom.__v[args[0]]
      else:
        return super().__new__(cls, *args, **kargs)
    else:
      if not kargs:
        return Monom.__z
      else:
        return Monom.__f[kargs['pos']]


  def __init__(self, *args, **kargs):
    assert len(self) == len(Monom.__var) and all(v >= 0 for v in self)
    if not kargs:
      self.__pos = -1
    else:
      self.__pos = kargs['pos']

  def copy(self):
    r = Monom(self)
    r.__pos = self.__pos
    return r

  def __repr__(self):
    m = " ".join(str(d) for d in self)
    if self.__pos == -1:
      return f'[{m}]'
    else:
      return f'[{self.__pos};{m}]'

  def __str__(self):
    m = "*".join(f"{v}**{d}" if d > 1 else v \
                for v, d in zip(Monom.__var, self) if d >= 1)
    if self.__pos == -1:
      if m:
        return f'{m}'
      else:
        return '1'
    else:
      if m:
        return f'{Monom.__fun[self.__pos]}*{m}'
      else:
        return Monom.__fun[self.__pos]
  
  def latex(self):
    m = " ".join(f"{{{v}}}^{d}" if d > 1 else f"{{{v}}}" \
                for v, d in zip(Monom.__var, self) if d >= 1)
    if self.__pos == -1:
      if m:
        return f'{m}'
      else:
        return '1'
    else:
      if m:
        return f'{Monom.__fun[self.__pos]} {m}'
      else:
        return Monom.__fun[self.__pos]

  def df(self):
    if self.__pos == -1:
      assert self.degree() == 0
      return "1"
    else:
      r = [Monom.__fun[self.__pos]]
      for i, d in enumerate(self):
        if d:
          r.append(Monom.__var[i])
          if d > 1: r.append(f"{d}")
      m = ", ".join(r)
      return f"df({m})"

  def df_latex(self):
    if self.__pos == -1:
      assert self.degree() == 0
      return "1"
    else:
      r = " ".join([f"{{{Monom.__var[i]}}}" for i in self.expand()])
      if r:
        r = f"{{{Monom.__fun[self.__pos]}}}_{{{r}}}"
      else:
        r = f"{{{Monom.__fun[self.__pos]}}}"
      return r
    
  def gf(self):
    if self.__pos == -1:
      assert self.degree() == 0
      return "1"
    else:
      r = []
      for i, d in enumerate(self):
        if d == 0:
          r.append(Monom.__var[i])
        else:
          r.append(f"{Monom.__var[i]} + {d}")
      m = ", ".join(r)
      return f"{Monom.__fun[self.__pos]}({m})"

  def position(self):
    return self.__pos

  def degree(self):
    return sum(self)

  def lcm(self, other):
    assert self.__pos == -1 or other.__pos == -1 or self.__pos == other.__pos
    return Monom(max(i, j) for i, j in zip(self, other))

  def gcd(self, other):
    assert self.__pos == -1 or other.__pos == -1 or self.__pos == other.__pos
    return Monom(min(i, j) for i, j in zip(self, other))

  def __nonzero__(self):
    return self.__pos >= 0 or sum(self) > 0

  def __bool__(self):
    return self.__pos >= 0 or sum(self) > 0

  def prolong(self, var):
    assert 0 <= var < len(self)
    r = Monom(d + (1 if i == var else 0) for i, d in enumerate(self))
    r.__pos = self.__pos
    return r

  def __mul__(self, other):
    assert self.__pos == -1 or other.__pos == -1
    r = Monom(i + j for i, j in zip(self, other))
    r.__pos = max(self.__pos, other.__pos)
    return r

  def divisible(self, other):
    if self.__pos != other.__pos and other.__pos != -1:
      return False
    for i, j in zip(self, other):
      if i < j:
        return False
    return True

  def divisibleTrue(self, other):
    return self.divisible(other) and sum(self) > sum(other)

  def __truediv__(self, other):
    assert self.divisible(other)
    r = Monom(i - j for i, j in zip(self, other))
    if other.__pos == -1:
      r.__pos =  self.__pos
    return r

  def __pow__(self, n):
    assert n >= 0
    r = Monom(i*n for i in self)
    r.__pos = self.__pos
    return r

  def expand(self):
    r = []
    for v, d in enumerate(self):
        for i in range(d):
            r.append(v)
    return r

  def __lt__(self, other):
    return self.cmp(other) < 0

  def __le__(self, other):
    return self.cmp(other) <= 0

  def __gt__(self, other):
    return self.cmp(other) > 0

  def __ge__(self, other):
    return self.cmp(other) >= 0

  def __eq__(self, other):
    return all(i == j for i, j in zip(self, other))

  def __ne__(self, other):
    return any(i != j for i, j in zip(self, other))

  def __hash__(self):
    return tuple.__hash__(self + (self.__pos,))

  def cmp(self, other):
    assert False

  def __lex(self, other):
    for i in range(len(self)):
      if self[i] > other[i]:
        return 1
      elif self[i] < other[i]:
        return -1
    return 0

  def __deglex(self, other):
    d1, d2 = self.degree(), other.degree()
    if d1 > d2:
      return 1
    elif d1 < d2:
      return -1
    else:
      for i in range(len(self)-1, -1, -1):
        if self[i] < other[i]:
          return 1
        elif self[i] > other[i]:
          return -1
      return 0

  def POTlex(self, other):
    return (self.__pos > other.__pos) - (self.__pos < other.__pos) \
        or self.__lex(other)

  def TOPlex(self, other):
    return self.__lex(other) or \
         (self.__pos > other.__pos) - (self.__pos < other.__pos)

  def POTdeglex(self, other):
    return (self.__pos > other.__pos) - (self.__pos < other.__pos) \
        or self.__deglex(other)

  def TOPdeglex(self, other):
    return self.__deglex(other) or \
         (self.__pos > other.__pos) - (self.__pos < other.__pos)

  def nonmult(self, other):
    assert self.__pos == other.__pos
    for i in range(len(self)):
      if self[i] < other[i]:
        return i
    assert False

  @staticmethod
  def gradus(deg, pos=-1):
    assert deg >= 0 and -1 <= pos < len(Monom.__fun)
    for i in comb(list(range(len(Monom.__var))), deg):
      r = [0 for i in range(len(Monom.__var))]
      for k in i:
        r[k] += 1
      m = Monom(r)
      m.__pos = pos
      yield m

if __name__ == '__main__':
  var = ['a', 'b', 'c', 'd', 'e', 'f']
  fun = ['u', 'v', 'w']
  Monom.init(var, fun)

  print(f"{Monom()!r}")
  print(Monom())

  print(Monom(4))
  print(f"{Monom(4)!r}")
  print(f"{Monom(4)!s}")

  print(f"{Monom(pos=2)!r}")
  print(f"{Monom(pos=2)!s}")

  print(f"{Monom(pos=1)*Monom((1, 2, 3, 4, 5, 6))!r}")
  print(f"{Monom(pos=1)*Monom([1, 2, 3, 4, 5, 6])!s}")

  print(Monom((1, 2, 3, 4, 5, 6)).expand())
  m = Monom()
  for i in Monom((1, 2, 3, 4, 5, 6)).expand():
    m = m*Monom(i)
  print(f"{m!r}")
  print(f"{m!s}")

  Monom.cmp = Monom.POTlex

  for i, g in enumerate(var):
    globals()[g] = Monom(i)
  for i, g in enumerate(fun):
    globals()[g] = Monom(pos=i)


  print(a**4*b*u)

  m = Monom(pos=1).prolong(2).prolong(2).prolong(3)**2
  print(f"{m!r}")
  print(f"{m}")
  print(f"{m.df()}") # форма для производных
  print(f"{m.gf()}") # форма для разностных функций

  print(repr(Monom(pos=1)*Monom(2)*Monom(2)*Monom(3)**2))
  print(v*c*c*d**2)

  m = Monom(pos=2).prolong(4).prolong(1).prolong(3)**4
  print(f"{m!r}")
  print(f"{m}")
  print(f"{m.df()}")
  print(f"{m.gf()}")

  m = w*c*c*d**2
  print(f"{m!r}")
  print(f"{m}")
  print(f"{m.gf()}")
  print(f"{m.df()}")

  m = a*a**4*b*u*c/a/b*c
  print(f"{m!r}")
  print(f"{m}")
  print(f"{m.df()}")
  print(f"{m.gf()}")

  m1 = w*b*c**2
  print(f"{m1!r}")
  print(f"{m1}")
  print(f"{m1.df()}")
  print(f"{m1.gf()}")

  m = m1/m1
  print(f"{m!r}")
  print(f"{m}")

  m2 = a*e*f**2*Monom(pos=1)
  print(f"{m2!r}")
  print(f"{m2}")
  print(f"{m2.df()}")
  print(f"{m2.gf()}")

  print(m1.POTlex(m2))
  print(m1.TOPdeglex(m2))
  print(m2 < m1 <= m1)

  print(set([m1, m2, a, b, c]))

  print(m1.divisible(m2))
  print(m2.prolong(0).divisible(m2))
  print(m2.prolong(0).divisibleTrue(m2))
  print(m2.nonmult(m2.prolong(0)**3))

  try:
    (m2.prolong(0)**3).nonmult(m2)
  except AssertionError:
    print('AssertionError: assert self.__pos == other.__pos')
  try:
    m2.prolong(2).nonmult(m1)
  except AssertionError:
    print('AssertionError: assert False')
