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
    Monom.__fun = None
    Monom.__f = None
    if len(args) == 2:
      assert len(args[1]) == len(set(args[1]))
      not set(args[0]) & set(args[1])
      Monom.__fun = args[1]
      Monom.__f = tuple(Monom((0 for v in Monom.__var), pos=p)\
                              for p in range(len(Monom.__fun)))
      return Monom.__v, Monom.__f
    return Monom.__v

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

  def df_sympy(self):
    if self.__pos == -1:
      assert self.degree() == 0
      return "1"
    else:
      r = []
      for i, d in enumerate(self):
        if d:
          r.append(Monom.__var[i])
          if d > 1: r.append(f"{d}")
      u = f"{Monom.__fun[self.__pos]}({', '.join(Monom.__var)})"
      if not r:
        return u
      else:
        return f"{u}.diff({', '.join(r)})"
    
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
    
  def T(self):
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
    return f"T({m})"
   
  def T_latex(self):
    if self.__pos == -1:
      assert self.degree() == 0
      return "1"
    else:
      r = []
      for i, d in enumerate(self):
        if d == 0:
          r.append(f"{{{Monom.__var[i]}}}")
        else:
          r.append(f"{{{Monom.__var[i]}}} + {d}")
      m = "\, ".join(r)
      return f"{{{Monom.__fun[self.__pos]}}}_{{{m}}}"

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
    assert self.__pos == -1
    assert n >= 0
    r = Monom(i*n for i in self)
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
    return self.cmp(other) == 0

  def __ne__(self, other):
    return self.cmp(other) != 0

  def __hash__(self):
    return tuple.__hash__(self + (self.__pos,))

  def cmp(self, other):
    assert False

  def lex(self, other):
    for i in range(len(self)):
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
      for i in range(len(self)-1, -1, -1):
        if self[i] < other[i]:
          return 1
        elif self[i] > other[i]:
          return -1
      return 0

  def POTlex(self, other):
    if self.__pos == other.__pos:
      return self.lex(other)
    elif other.__pos == -1 or (self.__pos != -1 and self.__pos < other.__pos):
      return 1
    else:
      return -1

  def TOPlex(self, other):
    r = self.lex(other)
    if r:
      return r
    elif self.__pos == other.__pos:
      return 0
    elif other.__pos == -1 or (self.__pos != -1 and self.__pos < other.__pos):
      return 1
    else:
      return -1

  def POTdeglex(self, other):
    if self.__pos == other.__pos:
      return self.deglex(other)
    elif other.__pos == -1 or (self.__pos != -1 and self.__pos < other.__pos):
      return 1
    else:
      return -1

  def TOPdeglex(self, other):
    r = self.deglex(other)
    if r:
      return r
    elif self.__pos == other.__pos:
      return 0
    elif other.__pos == -1 or (self.__pos != -1 and self.__pos < other.__pos):
      return 1
    else:
      return -1

  def nonmult(self, other):
    assert self.__pos == other.__pos
    for i in range(len(self)):
      if self[i] < other[i]:
        return i
    assert False

  @staticmethod
  def gradus(deg, pos=-1):
    assert deg >= 0
    for i in comb(list(range(len(Monom.__var))), deg):
      r = [0 for i in range(len(Monom.__var))]
      for k in i:
        r[k] += 1
      m = Monom(r)
      m.__pos = pos
      yield m

if __name__ == '__main__':
  var = 'a, b, c, d, e'.split(', ')
  print(Monom.init(var))

  var = 'a, b, c, d, e'.split(', ')
  fun = 'u, v, w'.split(', ')
  print(Monom.init(var, fun))

  var = 'a, b, c, d, e'.split(', ')
  a, b, c, d, e = Monom.init(var)
  print(a**1*b**2*c**3*d**4*e**5)

  var = 'a, b, c, d, e'.split(', ')
  fun = 'u, v, w'.split(', ')
  var, fun = Monom.init(var, fun)
  a, b, c, d, e = var
  u, v, w = fun
  m = a**1*b**2*c**3*d**4*e**5
  print(f"{m} -> {m!r}")
  for i, f in enumerate(fun):
      print(a**i*f*b*c, f" -> {a**i*f*b*c!r}")
  print(f"{Monom()} -> {Monom()!r}")   

  m = Monom()
  for i in  range(len(var)):
      m *= Monom(i)**(i+1)
  print(f"{m} -> {m!r}")
  for i in  range(len(fun)):
      print(Monom(0)**i*Monom(pos=i)*Monom(1)*Monom(2),\
            f" -> {Monom(0)**i*Monom(pos=i)*Monom(1)*Monom(2)!r}")

  m = a**1*b**2*c**3*d**4*e**5
  print(f"{m.latex()} -> {m!r}")
  for i, f in enumerate(fun):
      print((a**i*f*b*c).latex(), f" -> {a**i*f*b*c!r}")
  print(f"{Monom().latex()} -> {Monom()!r}") 

  # form for derivatives
  m = u*a**1*b**2*c**3*d**4*e**5
  print(f"{m.df()} -> {m!r}")
  for i, f in enumerate(fun):
      print((u*a**i*e*b*c).df(), f" -> {u*a**i*e*b*c!r}")
  print(f"{Monom(pos=2).df()} -> {Monom(pos=2)!r}") 

  print(f"{m.df_sympy()} -> {m!r}")
  for i, f in enumerate(fun):
      print((u*a**i*e*b*c).df_sympy(), f" -> {u*a**i*e*b*c!r}")
  print(f"{Monom(pos=2).df_sympy()} -> {Monom(pos=2)!r}") 

  print(f"{m.df_latex()} -> {m!r}")
  for i, f in enumerate(fun):
      print((u*a**i*e*b*c).df_latex(), f" -> {u*a**i*e*b*c!r}")
  print(f"{Monom(pos=2).df_latex()} -> {Monom(pos=2)!r}") 

  # form for difference functions
  m = u*a**1*b**2*c**3*d**4*e**5
  print(f"{m.T()} -> {m!r}")
  for i, f in enumerate(fun):
      print((a**i*f*b*c).T(), f" -> {a**i*f*b*c!r}")
  print(f"{Monom().T()} -> {Monom()!r}") 

  m = u*a**1*b**2*c**3*d**4*e**5
  print(f"{m.T_latex()} -> {m!r}")
  for i, f in enumerate(fun):
      print((u*a**i*e*b*c).T_latex(), f" -> {u*a**i*e*b*c!r}")
  print(f"{Monom(pos=2).T_latex()} -> {Monom(pos=2)!r}") 

  m = a*a**4*b*u*c/a/b*c
  print(f"{m} -> {m!r}")

  print(Monom((1, 2, 3, 4, 5), pos=1).expand())

  m = Monom()
  for i in Monom((1, 2, 3, 4, 5)).expand():
      m = m*Monom(i)
  print(m)

  print(m.copy()*m)

  m1 = w*b*c**2
  m2 = a*e*b**2
  print(m2)

  print(m1.POTlex(m2))

  print(m1.TOPdeglex(m2))

  Monom.cmp = Monom.POTlex # POTlex, TOPlex, POTdeglex, TOPdeglex

  print(m2 < m1 <= m1)

  print(set([m1, m2, a, b, c]))

  print(m1.divisible(m2))

  print(m2.divisible(m2))

  print(m2.divisibleTrue(m2))

  print(m2.prolong(0).divisibleTrue(m2))

  print(m2.nonmult(m2**3))

  try:
      (m2.prolong(0)**3).nonmult(m2)
  except AssertionError:
      print("1;1 0 0 0 1 2] division [1;6 0 0 0 3 6]")

  try:
      m2.prolong(2).nonmult(m1)
  except AssertionError:
      print("""[2;0 1 2 0 0 0] position=2
  [1;1 0 1 0 1 2] position=1""")

  for i in range(3):
      for m in Monom.gradus(i):
          print(f"{m!r}", end=" ")
      print(";")

  for i in range(3):
      for m in Monom.gradus(i, 2):
          print(f"{m!r}", end=" ")
      print(";")






