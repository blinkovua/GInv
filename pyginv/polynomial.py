import sympy
from sympy import S, Integer, gcd, lcm, cancel, expand, fraction

from monomial import *

class Poly(list):
  __var = None
  __fun = None

  @staticmethod
  def init(*args):
    Monom.init(*args)
    Poly.__var = tuple(Poly(m) for m in Monom._Monom__v)
    if not Monom._Monom__f:
      Poly.__fun = None
      return Poly.__var
    else:
      Poly.__fun = tuple(Poly(m) for m in Monom._Monom__f)
      return Poly.__var, Poly.__fun
  
  def __init__(self, *args):
    if args:
      assert len(args) == 1
      if isinstance(args[0], Monom):
        self.append([args[0], Integer(1)])
      elif isinstance(args[0], Poly):
        for m, k in args[0]:
          self.append([m, k])
      else:
        if args[0]:
          if isinstance(args[0], int):
            self.append([Monom(), Integer(args[0])])
          else:
            self.append([Monom(), args[0]])
    assert self.assertValid()

  def __str__(self):
    if not self:
      return '0'
    else:
      r = []
      for m, c in self:
        if m.degree() == 0 and m.position() < 0:
          r.append(f"{c}")
        elif c == S.One:
          r.append(f"{m}")
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
    p = self.__class__()
    for m, k in self:
      p.append([m, -k])
    return p

  def __radd__(self, other):
    return self+other

  def __add__(self, other):
    if other == S.Zero:
      return self
    if isinstance(other, int): 
      other = self.__class__(Integer(other))
    else:
      other = self.__class__(other)
    p = self.__class__()
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
        if not k.is_integer:
          k = cancel(k)
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
    if isinstance(other, int): 
      other = self.__class__(Integer(other))
    else:
      other = self.__class__(other)
    p = self.__class__()
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
        if not k.is_integer:
          k = cancel(k)
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
    if isinstance(other, int): 
      other = self.__class__(Integer(other))
    else:
      other = self.__class__(other)
    t = {}
    for m1, k1 in self:
      for m2, k2 in other:
        m, k = m1*m2, k1*k2
        if m in t:
          t[m] += k
        else:
          t[m] = k
    p = self.__class__()
    for m, k in sorted(t.items(), reverse=True):
      if k:
        p.append([m, k])
    assert p.assertValid()
    return p
  
  def __rtruediv__(self, other):
    return self/other
  
  def __truediv__(self, other):
    p = self.__class__()
    for m, k in self:
      p.append([m, k/other])
    return p

  def __pow__(self, other):
    assert other >= 0
    if other == 0:
      p = self.__class__(Integer(1))
    else:
      p = self
      for i in range(1, other):
        p = p*self
    return p

  def prolong(self, var):
    p = self.__class__()
    for m, k in self:
      p.append([m.prolong(var), k])
    assert p.assertValid()
    return p

  def diff(self, dv):
    p = self.__class__()
    for m, k in self:
      k = sympy.diff(k, dv)
      if k:
        p.append([m, k])
    assert p.assertValid()
    return p

  def mult(self, m1, k1):
    assert k1 != S.Zero
    p = self.__class__()
    for m2, k2 in self:
      p.append([m1*m2, k1*k2])
    assert p.assertValid()
    return p

  def cancel(self):#TODO
    assert self
    l = fraction(self[0][1])[1]
    for mk in self[1:]:
      l = lcm(l, fraction(mk[1])[1])
    for mk in self:
      mk[1] = cancel(mk[1]*l)
    assert self.assertValid()
    
  def pp(self):
    assert self
    g = self[0][1]
    for mk in self[1:]:
      if g == S.One:
        return
      g = gcd(g, mk[1])
    for mk in self:
      #mk[1] = ratsimp(mk[1]/g)
      mk[1] = mk[1]/g
      if not mk[1].is_integer:
        mk[1] = cancel(mk[1])
    assert self.assertValid()

  def reduction(self, i, other):
    assert id(self) != id(other)
    assert 0 <= i < len(self)
    assert self[i][0].divisible(other[0][0])
    k = gcd(self[i][1], other[0][1])
    #m2, k1, k2 = self[i][0]/other[0][0], ratsimp(other[0][1]/k), ratsimp(-self[i][1]/k)
    m2, k1, k2 = self[i][0]/other[0][0], other[0][1]/k, -self[i][1]/k
    if not k1.is_integer:
        k1 = cancel(k1)
    if not k2.is_integer:
        k2 = cancel(k2)
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
        #k = ratsimp(self[i][1]*k1 + other[j][1]*k2)
        k = self[i][1]*k1 + other[j][1]*k2
        if not k.is_integer:
          k = expand(k)
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

  def redRat(self, i, other):
    assert id(self) != id(other)
    assert 0 <= i < len(self)
    assert self[i][0].divisible(other[0][0])
    m2, k2 = self[i][0]/other[0][0], -self[i][1]/other[0][1]
    if not k2.is_integer:
        k2 = cancel(k2)
    del self[i]
    j, iend, jend = 1, len(self), len(other)
    while i < len(self) and j < jend:
      c = self[i][0].cmp(other[j][0]*m2)
      if c > 0:
        i += 1
      elif c < 0:
        self.insert(i, [other[j][0]*m2, other[j][1]*k2])
        i += 1
        j += 1
      else:
        k = self[i][1] + other[j][1]*k2
        if not k.is_integer:
          k = expand(k)
        if k == S.Zero:
          del self[i]
        else:
          self[i][1] = k
          i += 1
        j += 1
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
    i = 0
    while i < len(self):
      p = pset.find(self[i][0])
      if p:
        self.redRat(i, p)
      else:
        i += 1
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

class PolyDiff(Poly):
  __var = None
  __fun = None

  @staticmethod
  def init(*args):
    Monom.init(*args)
    assert Monom._Monom__f
    PolyDiff.__var = sympy.symbols(Monom._Monom__var, real=True)
    PolyDiff.__fun = sympy.symbols(Monom._Monom__fun, cls=sympy.Function, real=True)
    return PolyDiff.__var, PolyDiff.__fun

  @staticmethod
  def df(*args):
    assert args[0] in PolyDiff.__fun
    m, i = [0 for i in Monom._Monom__var], 1
    while i < len(args):
        assert args[i] in PolyDiff.__var
        v = PolyDiff.__var.index(args[i])
        if i+1 == len(args) or not isinstance(args[i+1], int):
            m[v] += 1
            i += 1
        else:
            m[v] += args[i+1]
            i += 2
    return PolyDiff(Monom(m, pos=PolyDiff.__fun.index(args[0])))

  @staticmethod
  def diff2poly(a):
    if not a.args:
      return PolyDiff(a)
    else:
      if a.func == sympy.Add:
        r = PolyDiff.diff2poly(a.args[0])
        for s in a.args[1:]:
          r = r + PolyDiff.diff2poly(s)
      elif a.func == sympy.Mul:
        r = PolyDiff.diff2poly(a.args[0])
        for s in a.args[1:]:
          r = r * PolyDiff.diff2poly(s)
      elif a.func == sympy.Pow:
        r = PolyDiff(Monom())*a
      elif repr(a.func) in Monom._Monom__var:
        r = PolyDiff(Monom(Monom._Monom__var.index(repr(a.func))))
      elif repr(a.func) in Monom._Monom__fun:
        r = PolyDiff(Monom(pos=Monom._Monom__fun.index(repr(a.func))))
      else:
        assert a.func == sympy.Derivative
        assert repr(a.args[0].func) in Monom._Monom__fun
        m = [0 for i in Monom._Monom__var]
        for v, d in a.args[1:]:
          m[Monom._Monom__var.index(repr(v))] = d
        r = PolyDiff(Monom(m, pos=Monom._Monom__fun.index(repr(a.args[0].func))))
      return r       

  def __init__(self, *args):
    super().__init__(*args)

  def __str__(self):
    if not self:
      return '0'
    else:
      r = []
      for m, c in self:
        if m.degree() == 0 and m.position() < 0:
          r.append(f"{c}")
        elif c == S.One:
          r.append(f"{m.df()}")
        else:
          r.append(f"{m.df()}*{c}")
      return " + ".join(r)

  def df_sympy(self):
    if not self:
      return '0'
    else:
      r = []
      for m, c in self:
        if m.degree() == 0 and m.position() < 0:
          r.append(f"{c}")
        elif c == S.One:
          r.append(f"{m.df_sympy()}")
        else:
          r.append(f"{m.df_sympy()}*{c}")
      return " + ".join(r)
    
  def prolong(self, var):
    p1, p2 = PolyDiff(), PolyDiff()
    for m, k in self:
      try:
        if m.__bool__():
          p1.append([m.prolong(var), k])
      except TypeError:
        print(f"{self!r}")
        print(f"{type(m)}{m!r}, {type(k)}{k!r}")
        print(f"{m.position()} {sum(m)} {m.__bool__()}")
        assert not m
    for m, k in self:
      k = sympy.diff(k, PolyDiff.__var[var])
      if k:
        p2.append([m, k])
    return p1 + p2

  def NFhead(self, pset):
    while self:
      p = pset.find(self[0][0])
      if not p:
        break
      else:
        m2 = self[0][0]/p[0][0]
        for df in m2.expand():
          p = p.prolong(df)
        self.reduction(0, p)

  def NFtail(self, pset):
    assert self
    i = 1
    while i < len(self):
      p = pset.find(self[i][0])
      if not p:
        i += 1
      else:
        m2 = self[i][0]/p[0][0]
        for df in m2.expand():
          p = p.prolong(df)
        self.reduction(i, p)

  def NF(self, pset):
    i = 0
    while i < len(self):
      p = pset.find(self[i][0])
      if not p:
        i += 1
      else:
        m2 = self[i][0]/p[0][0]
        for df in m2.expand():
          p = p.prolong(df)
        self.redRat(i, p)
    return self

class PolySchem(Poly):
  __var = None
  __fun = None

  @staticmethod
  def init(*args):
    Monom.init(*args)
    assert Monom._Monom__f
    PolySchem.__var = sympy.symbols(Monom._Monom__var, real=True)
    PolySchem.__fun = sympy.symbols(Monom._Monom__fun, cls=sympy.Function, real=True)
    return PolySchem.__var, PolySchem.__fun

  @staticmethod
  def T(*args):
    assert args[0] in PolySchem.__fun
    m, i = [0 for i in Monom._Monom__var], 1
    while i < len(args):
        assert args[i] in PolySchem.__var
        v = PolySchem.__var.index(args[i])
        if i+1 == len(args) or not isinstance(args[i+1], int):
            m[v] += 1
            i += 1
        else:
            m[v] += args[i+1]
            i += 2
    return PolySchem(Monom(m, pos=PolySchem.__fun.index(args[0])))

  def __init__(self, *args):
    super().__init__(*args)

  def __str__(self):
    if not self:
      return '0'
    else:
      r = []
      for m, c in self:
        if m.degree() == 0 and m.position() < 0:
          r.append(f"{c}")
        elif c == S.One:
          r.append(f"{m.T()}")
        else:
          r.append(f"{m.T()}*{c}")
      return " + ".join(r)
  
if __name__ == '__main__':
  from ginv import *
  sympy.init_printing()

  var = 'a, b, c, d, e'.split(', ')
  print(Poly.init(var))

  var = 'a, b, c, d, e'.split(', ')
  fun = 'u, v, w'.split(', ')
  print(Poly.init(var, fun))

  Monom.cmp = Monom.POTlex # POTlex, TOPlex, POTdeglex, TOPdeglex

  var = 'a, b, c, d, e'.split(', ')
  a, b, c, d, e = Poly.init(var)
  3*a**1*b**2*c**3*d**4*e**5 + (2*a**5*b**2 + 13)*e**3 + 5

  var = 'a, b, c, d, e'.split(', ')
  fun = 'u, v, w'.split(', ')
  var, fun = Poly.init(var, fun)
  a, b, c, d, e = var
  u, v, w = fun
  p = 3*a**1*b**2*c**3*d**4*e**5 + (2*u*a**5*b**2 + 13)*e**3 + 5 + w
  print(f"{p} \n-> {p!r}")
  print(f"{Poly()} -> {Poly()!r}")

  Monom.cmp = Monom.TOPlex
  p = 3*a**1*b**2*c**3*d**4*e**5 + (2*u*a**5*b**2 + 13)*e**3 + 5 + w
  print(f"{p} \n-> {p!r}")
  print(f"{Poly()} -> {Poly()!r}")

  Monom.cmp = Monom.TOPdeglex
  p = 3*a**1*b**2*c**3*d**4*e**5 + (2*u*a**5*b**2 + 13)*e**3 + 5 + w
  print(f"{p} \n-> {p!r}")
  print(f"{Poly()} -> {Poly()!r}")

  par = sympy.symbols('alpha, beta, tau, delta, epsilon, zeta', real=True)
  print(par)

  p = Poly(par[2]**4*21546/par[3])
  print(f"{p} -> {p!r}")

  p.cancel()
  print(f"{p} -> {p!r}")

  h = u*c**5 + a**2*b*par[2]*1236537 + d*par[0]
  print(f"{h} \n-> {h!r}")

  g = Poly(h)
  print(g != h)
  print(g == h)
  print(g > h)

  print(h.lm(), h.lc())

  p = g.prolong(4)
  print(f"{p} \n-> {p!r}")

  p = g.diff(par[2])
  print(f"{p} \n-> {p!r}")

  g.reduction(0, h)
  print(g)

  h *= (4*b**4 + e*par[2] + par[1])**9
  print(h)

  h.NFtail(4*b**4 + e*par[2] + par[1])
  print(h)

  h.NFhead(4*b**4 + e*par[2] + par[1])
  print(h)

  h = (4*b**4 + a*b*c**2 + e*par[2] + par[1])
  h.reduce(a*b*c**2 + e*par[2])
  print(h)

  h = (4*b**4 + a*b*c**2 + e*par[2] + par[1])
  h.reduce(a*b*c + e*par[2])
  print(h)

  Monom.cmp = Monom.TOPdeglex
  x, y = Poly.init('x, y'.split(', '))
  mnm = Janet()
  res = ginvBlockLow((x**7*y, x**5*y**2, x**4*y**3, x**2*y**4), \
  mnm, level=1)
  print(f"HP: {mnm.HP()}")
  mnm.saveImage('mnm.png')

  Monom.cmp = Monom.TOPdeglex
  x1, x2, x3, x4 = Poly.init('x1, x2, x3, x4'.split(', '))
  cyclic4 = Janet()
  res = ginvBlockLow((
  x1+x2+x3+x4,\
  x1*x2+x2*x3+x3*x4+x4*x1,\
  x1*x2*x3+x2*x3*x4+x3*x4*x1+x4*x1*x2,\
  x1*x2*x3*x4-1), \
  cyclic4, level=1)
  print(f"    crit1: {res[1]}")
  print(f"    crit2: {res[2]}")
  print(f"     time: {res[0]:.2f} sec")
  print(f"    count: {cyclic4.count()}")
  print(f"reduction: {cyclic4.reduction()}")
  print(f"       HP: {cyclic4.HP()}")
  cyclic4.saveImage('cyclic4.pdf', level=3)

  for w in cyclic4.GB():
    print(w.poly)

  x1, x2, x3, x4 = sympy.symbols('x1, x2, x3, x4')
  for w in cyclic4.GB():
    print(sympy.Add(*tuple(eval(f"{m}*{c}") for m, c in w.poly), evaluate=False))

  Monom.cmp = Monom.TOPdeglex
  var = 'x1, x2, x3, x4'.split(', ')
  fun = 'f, p1, p2, p3, p4'.split(', ')
  var, fun = Poly.init(var, fun)
  x1, x2, x3, x4 = var
  f, p1, p2, p3, p4 = fun
  cyclic4 = Forest(Janet)
  res = ginvMin((
  f*(x1+x2+x3+x4) - p1,\
  f*(x1*x2+x2*x3+x3*x4+x4*x1) - p2,\
  f*(x1*x2*x3+x2*x3*x4+x3*x4*x1+x4*x1*x2) - p3,\
  f*(x1*x2*x3*x4-1) - p4), \
  cyclic4, level=1)
  print(f"    crit1: {res[1]}")
  print(f"    crit2: {res[2]}")
  print(f"     time: {res[0]:.2f} sec")
  print(f"    count: {cyclic4.count()}")
  print(f"reduction: {cyclic4.reduction()}")
  print(f"       HP: {cyclic4[0].HP()}")
  cyclic4[0].saveImage('cyclic4_syzygy.png', level=3)

  x1, x2, x3, x4 = sympy.symbols('x1, x2, x3, x4')
  f, p1, p2, p3, p4 = sympy.symbols('f, p1, p2, p3, p4')
  for w in cyclic4.GB():
    print(eval(str(w.poly)).collect([f, p1, p2, p3, p4]))
