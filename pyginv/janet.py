import sympy

from polynomial import *

class Wrap:
  id = 0

  def __init__(self, *args):
    if len(args) == 1 and isinstance(args[0], Poly):
      self.poly = args[0]
      self.ansector = args[0].lm().copy()
    else:
      assert len(args) == 2 and isinstance(args[0], Wrap)
      assert args[0].prolong[args[1]] == False
      assert args[0].nonmult[args[1]] == True
      self.poly = args[0].poly.prolong(args[1])
      self.ansector = args[0].ansector.copy()
      args[0].prolong[args[1]] = True
    assert self.poly
    self.id = Wrap.id
    Wrap.id += 1
    self.lm = self.poly.lm().copy()
    self.prolong = [False for i in range(len(self.lm))]
    self.nonmult = [False for i in range(len(self.lm))]

  def __repr__(self):
    r =   [f"      id: {self.id}"]
    r.append(f"ansector: {self.ansector!r}")
    r.append(f"      lm: {self.lm!r}")
    r.append(f" prolong: {' '.join('*' if i else '_' for i in self.prolong)}")
    r.append(f" nonmult: {' '.join('*' if i else '_' for i in self.nonmult)}")
    r.append(f"    poly: {self.poly!r}")
    if len(r[-1]) > 80:
      r[-1] = f"{r[-1][:80]} ..."
    return "\n".join(r)

  def __str__(self):
    r =   [f"      id: {self.id}"]
    r.append(f"ansector: {self.ansector}")
    r.append(f"      lm: {self.lm}")
    r.append(f" prolong: {' '.join('*' if i else '_' for i in self.prolong)}")
    r.append(f" nonmult: {' '.join('*' if i else '_' for i in self.nonmult)}")
    r.append(f"    poly: {self.poly}")
    if len(r[-1]) > 80:
      r[-1] = f"{r[-1][:80]} ..."
    return "\n".join(r)

  def degree(self):
    return self.lm.degree()

  def update(self):
    assert self.poly
    if self.lm != self.poly.lm():
      self.lm = self.poly.lm().copy()
      self.ansector = self.poly.lm().copy()
      for i in range(len(self.lm)):
        self.prolong[i] = False
        self.nonmult[i] = False

  def refresh(self, other):
    assert self.poly.lm() == self.lm
    # if self.ansector.degree() < other.ansector.degree():
    if other.ansector.divisibleTrue(self.ansector):
      other.ansector = self.ansector.copy()
    # if self.lm == other.lm:
    #   if any(k > l for k, l in zip(self.ansector, other.ansector)):
    #     self.ansector = other.ansector.copy()
    
        #if self.poly.lc().is_integer and\
           #other.poly.lc().is_integer and\
           #self.poly.lc() % other.poly.lc() != 0:
          #a, b, g = sympy.gcdex(self.poly.lc(), other.poly.lc())
          #print(a, b, g)

  def isGB(self):
    return self.lm == self.ansector

  def multi(self):
    return sum(0 if i else 1 for i in self.nonmult)

  def crit1(self, other):
    return type(self.poly) == Poly and\
        all(i == 0 or j == 0 for i, j in zip(self.ansector, other.ansector))

  def crit2(self, other):
    return type(self.poly) == Poly and\
        any(i > k + l for i, k, l in zip(\
            self.lm, self.ansector, other.ansector))

  def __lt__(self, other):
    return self.lm.cmp(other.lm) < 0 or \
      (self.lm.cmp(other.lm) == 0 and self.id < other.id)

  def __gt__(self, other):
    return self.lm.cmp(other.lm) > 0 or \
      (self.lm.cmp(other.lm) == 0 and self.id > other.id)

  def __eq__(self, other):
    return self.id == other.id

  def __ne__(self, other):
    return self.id != other.id

# HilbertPoly, Cartan’s Character
# E. Cartan. Les syst`emes diff´erentiels ext´erieurs et leurs applications g´eom´etriques. Hermann, 1945
class CombRepet(list):
  __hash = {}

  def __new__ (cls, *args, **kargs):
    n, d = args[0], kargs.get('d', 0)
    if (n, d) not in CombRepet.__hash:
      CombRepet.__hash[n, d] = super().__new__(cls, *args, **kargs)
      return CombRepet.__hash[n, d]
    else:
      return super().__new__(cls, *args, **kargs)

  def __init__(self, n, d=0):
    assert n >= 0
    self.append(1)
    for i in range(1, n):
      k = sympy.Rational(n - i - d, i)
      a = [p*k for p in self]
      b = [sympy.Rational(p, i) for p in self]
      self[0] = a[0]
      for j in range(1, len(self)):
        self[j] = a[j] + b[j-1]
      self.append(b[-1])
    CombRepet.__hash[n, d] = self

  def subs(self, s):
    return sum(k*s**i for i, k in enumerate(self))

class HilbertPoly(list):
  def __init__(self, n, init=True):
    assert n > 0
    if init:
      self[:] = CombRepet(n+1)
    else:
      for i in range(n+1):
        self.append(0)

  def __isub__(self, other):
    assert isinstance(other, CombRepet)
    assert len(self) >= len(other)
    for i in range(len(other)):
      self[i] -= other[i]
    return self

  def __iadd__(self, other):
    assert isinstance(other, HilbertPoly)
    assert len(self) == len(other)
    for i in range(len(self)):
      self[i] += other[i]
    return self
  
  def __str__(self):
    r = f"{sum(s*sympy.symbols('s')**i for i, s in enumerate(self))}"
    return r.replace('**', '^').replace('*', '')

  def subs(self, s):
    return sum(k*s**i for i, k in enumerate(self))

class Janet:
  def __init__(self):
    self.__reduction = 0
    self.__count = 0
    self.root = []
    
  def is_forest(self):
    return False

  def findWrap(self, m):
    if self.root:
      assert self.pos == m.position()
      r, d, v = self.root, m.degree(), 0
      while r and d >= 0: # TODO
        if len(r)-1 <= m[v] and isinstance(r[-1], Wrap):
            self.__reduction += 1
            return r[-1]
        r, d, v = r[min(len(r)-1, m[v])], d - m[v], v + 1
      return r if isinstance(r, Wrap) else None

  def find(self, m):
    w = self.findWrap(m)
    return w.poly if w else None

  def __iter__(self):
    def traversal(r):
      if r:
        for n in r[:-1]:
          yield from traversal(n)
        if isinstance(r[-1], Wrap):
          yield r[-1]
        else:
          yield from traversal(r[-1])
    return traversal(self.root)

  def GB(self):
    def traversal(r):
      if r:
        for n in r[:-1]:
          yield from traversal(n)
        if isinstance(r[-1], Wrap):
          if r[-1].isGB(): yield r[-1]
        else:
          yield from traversal(r[-1])
    return traversal(self.root)

  def __insert(self, w, lst):
    assert isinstance(w, Wrap)
    assert not self.findWrap(w.lm)

    if not self.root:
      self.pos = w.lm.position()
    assert self.pos == w.lm.position()

    def setNonmut(r, v):
      if r:
        for n in r[:-1]:
          setNonmut(n, v)
        if isinstance(r[-1], Wrap):
          r[-1].nonmult[v] = True
        else:
          setNonmut(r[-1], v)

    def remove(r):
      if r:
        for n in r[:-1]:
          remove(n)
        if isinstance(r[-1], Wrap):
          lst.append(r[-1])
        else:
          remove(r[-1])

    r, d, v = self.root, w.degree(), 0
    while True:
      if len(r)-1 > w.lm[v]:
        w.nonmult[v] = True
      elif len(r)-1 < w.lm[v]:
        setNonmut(r, v)
        for i in range(w.lm[v]+1 - len(r)):
          r.append([])
        assert len(r)-1 == w.lm[v]
      assert isinstance(r[w.lm[v]], list)

      if d > w.lm[v]:
        r, d, v = r[w.lm[v]], d - w.lm[v], v + 1
      else:
        assert d == w.lm[v]
        remove(r[w.lm[v]:])
        del r[w.lm[v]:]
        w.nonmult[v] = False
        r.append(w)
        break
    self.__count += 1
    return lst

  def insert(self, q):
    lst = []
    for w in q:
      self.__insert(w, lst)
    self.__count -= len(lst)
    return lst

  def setNonmut(self, m):
    def setnm(r):
      if r:
        for n in r[:-1]:
          setnm(n)
        if isinstance(r[-1], Wrap):
          r[-1].nonmult[r[-1].lm.nonmult(m)] = True
        else:
          setnm(n)
    setnm(self.root)

  def degMinProlong(self):
    d = 0
    for w in self:
      for v, f in enumerate(w.nonmult):
        if f and not w.prolong[v]:
          d = min(d, w.degree()) if d else w.degree()
    return d

  def prolongDeg(self, d):
    r = []
    for w in self:
      if d == 0 or d == w.degree():
        for v, f in enumerate(w.nonmult):
          if f and not w.prolong[v]:
            r.append(Wrap(w, v))
    return r

  def prolongMonom(self, m):
    r = []
    for w in self:
      for v, f in enumerate(w.nonmult):
        if f and not w.prolong[v] and w.lm.prolong(v) <= m:
          r.append(Wrap(w, v))
    return r

  def prolongAll(self):
    r = []
    for w in self:
      for v, f in enumerate(w.nonmult):
        if f and not w.prolong[v]:
          r.append(Wrap(w, v))
    return r
  
  def reduction(self):
    return self.__reduction

  def count(self):
    return self.__count
    
  def HP(self):
    hp = HilbertPoly(len(Monom._Monom__var))
    for w in self:
      hp -= CombRepet(w.multi() + 1, w.degree())
    return hp

  def saveImage(self, filename, level=0):
    try:
      import gv
    except Exception:
      print("установите модуль 'gv' для рисования диаграмм с помощью 'graphviz'")
      return
    g = gv.graph('Janet tree')
    gv.setv(g, 'nojustify', 'true')
    gv.setv(g, 'fontname', 'monospace')
    deg = 0
    def draw(r, n1, s):
      nonlocal deg
      for i, n in enumerate(r):
        if isinstance(n, Wrap):
          deg = max(deg, n.degree())
          n2 = gv.node(g, f'{id(n)}')
          add = ""
          if level:
            if n.lm.position() < 0:
              indent = ' '
            else:
              indent = ' '*(2 + len(repr(n.lm.position())))
            if level >= 1:
              add += f"<BR/>{indent}{' '.join('*' if i else '_' for i in n.nonmult)}"
            if level >= 2:
              add += f"<BR/>{indent}{' '.join('*' if i else '_' for i in n.prolong)}"
            if level >= 3:
              add += f"<BR/>{n.ansector!r}"
          if n.isGB():
            gv.setv(n2, 'label', f'<<B>{n.lm!r}</B>{add}>')
          else:
            gv.setv(n2, 'label', f"<{n.lm!r}{add}>")

          gv.setv(n2, 'shape', 'plaintext')
          gv.setv(n2, 'fontsize', '10')
          if n1:
            e = gv.edge(n1, n2)
            gv.setv(e, 'dir', 'forward')
            gv.setv(e, 'style', s)
            s = 'solid'
          n1 = n2
        elif n:
          n2 = gv.node(g, f'{id(n)}')
          gv.setv(n2, 'label', f'{i}')
          gv.setv(n2, 'shape', 'plaintext')
          gv.setv(n2, 'fontsize', '10')
          if n1:
            e = gv.edge(n1, n2)
            gv.setv(e, 'dir', 'forward')
            gv.setv(e, 'style', s)
            s = 'solid'
          n1 = n2
          draw(n, n1, 'dotted')
    draw(self.root, None, 'solid')

    mark = gv.node(g, 'mark')
    gv.setv(mark, 'label', f'<<B>#Janet = {self.count()}<BR/>HP_{{{deg}}} = {self.HP()}</B>>')
    gv.setv(mark, 'shape', 'plaintext')
    gv.setv(mark, 'fontsize', '12')
    gv.layout(g, 'dot')
    gv.render(g, filename.split('.')[-1], filename)

class JanetCache(Janet):
  def __init__(self, *args):
    super().__init__(*args)
    self.сache = {}

  def find(self, m):
    w = self.findWrap(m)
    if not w:
      return None
    else:
      m1 = m/w.lm
      if not m1:
        r = w.poly
      else:
        if m in self.сache:
          r = self.сache[m]
        else:
          r = w.poly.mult(m1, sympy.S.One)
        r.NFtail(self)
        r.pp()
        self.сache[m] = r
        #print(m1, end=" ")
    return r

class Forest(list):
  def __init__(self, InvDiv):
    assert len(Monom._Monom__fun) > 0
    for i in range(len(Monom._Monom__fun)):
      self.append(InvDiv())

  def is_forest(self):
    return True
  
  def findWrap(self, m):
    return self[m.position()].findWrap(m) if m.position() >= 0 else None
  
  def find(self, m):
    return self[m.position()].find(m) if m.position() >= 0 else None

  def __iter__(self):
    for idiv in super().__iter__():
      for i in idiv:
        yield i
       
  def GB(self):
    for idiv in super().__iter__():
      for i in idiv.GB():
        yield i
              
  def insert(self, q):
    lst = []
    for w in q:
      lst.extend(self[w.lm.position()].insert([w]))
    return lst

  def degMinProlong(self):
    d = 0
    for idiv in super().__iter__():
      d1 = idiv.degMinProlong()
      if d1 > 0:
        if d == 0:
          d = d1
        else:
          d = min(d, d1)
    return d

  def prolongDeg(self, d):
    r = []
    for idiv in super().__iter__():
      r.extend(idiv.prolongDeg(d))
    return r

  def prolongMonom(self, m):
    r = []
    for idiv in super().__iter__():
      r.extend(idiv.prolongMonom(m))
    return r

  def prolongAll(self):
    r = []
    for idiv in super().__iter__():
      r.extend(idiv.prolongAll())
    return r
  
  def reduction(self):
    return sum(idiv.reduction() for idiv in super().__iter__())

  def count(self):
    return sum(idiv.count() for idiv in super().__iter__())
  
  def HP(self, verge=-1):
    hp = HilbertPoly(len(Monom._Monom__var), init=False)
    for k, h in enumerate(super().__iter__()):
      if k == verge:
        break
      hp += h.HP()
    return hp
      
if __name__ == '__main__':
  from pprint import pprint
  sympy.init_printing()

  var = ['a', 'b', 'c', 'd', 'e', 'f']
  fun = ['u', 'v', 'w']
  Monom.init(var, fun)

  Monom.cmp = Monom.POTlex

  for i, g in enumerate(var):
    globals()[g] = Poly(Monom(i))
  for i, g in enumerate(fun):
    globals()[g] = Poly(Monom(pos=i))

  alpha, beta, tau = sympy.symbols('alpha, beta, tau', real=True)
  h = u*c**5 + f*a*b*tau*1236537 + d*alpha
  print(h)
  print(f"{h!r}")

  w1 = Wrap(h)
  w1.nonmult[3] = True
  w2 = Wrap(w1, 3)
  w2.nonmult[4] = True

  print(w1)
  print()
  print(w2)

  print(repr(w1))
  print()
  print(repr(w2))

  print(CombRepet(5))
  print(CombRepet(5, 2))
  for i in range(7):
    print(i, CombRepet(6, 3).subs(i))

  var = ['a', 'b', 'c', 'd', 'e']
  fun = []
  Monom.init(var, fun)
  for i, g in enumerate(var):
      globals()[g] = Poly(Monom(i))

  pommaret = Janet()
  q = pommaret.insert(Wrap(p) for p in (\
  a**2, a*b, b**2, a*c, b*c, c**2))
  assert q == []
  pommaret.saveImage('pommaret.pdf', level=3)

  assert pommaret.degMinProlong() == 2
  q = pommaret.prolong(2)
  print(len(q))
  print([w for w in q if not pommaret.find(w.lm)])

  var = ['a', 'b', 'c', 'd']
  fun = []
  Monom.init(var, fun)
  for i, g in enumerate(var):
    globals()[g] = Poly(Monom(i))

  cyclic4 = Janet()
  q = cyclic4.insert(Wrap(p) for p in (\
    a, b**2, b*c**2, b*c*d**2,\
    b*d**4, c**2*d**4, c**3*d**2))
  assert q == []
  cyclic4.saveImage('cyclic4.pdf', level=3)

  for i in range(2, 7):
    assert cyclic4.degMinProlong() == i
    q = cyclic4.prolong(i)
    print(f"prolong {i}, len({len(q)}), {[w for w in q if not cyclic4.find(w.lm)]}")

  lst = [Wrap(p) for p in (c**2, d**4)]
  q = cyclic4.insert(lst)
  pprint(q)
  cyclic4.saveImage('cyclic41.pdf', level=3)

  lst1 = [Wrap(lst[0], 0), Wrap(lst[0], 1),\
    Wrap(lst[1], 0), Wrap(lst[1], 1), Wrap(lst[1], 2)]
  q = cyclic4.insert([w for w in lst1 if not cyclic4.find(w.lm)])
  pprint(q)
  cyclic4.saveImage('cyclic42.pdf', level=3)

  lst2 = [Wrap(lst1[-1], 0), Wrap(lst1[-1], 1), Wrap(lst1[-1], 2)]
  q = cyclic4.insert([w for w in lst2 if not cyclic4.find(w.lm)])
  pprint(q)
  cyclic4.saveImage('cyclic43.pdf', level=3)

  var = ['a', 'b', 'c']
  fun = []
  Monom.init(var, fun)
  Monom.cmp = Monom.TOPdeglex
  for i, g in enumerate(var):
    globals()[g] = Poly(Monom(i))
  perm3 = Janet()
  q = perm3.insert(Wrap(p) for p in (a**3, b**3, c**3))
  assert q == []

  for i in range(3, 8):
    print(perm3.degMinProlong())
    assert perm3.degMinProlong() == i
    q = perm3.prolong(i)
    print(f"prolong {i}")
    pprint(q)
    perm3.insert(w for w in q if not perm3.find(w.lm))

  perm3.saveImage('perm3.pdf', level=3)

  for w in perm3:
    if w.lm == w.ansector:
      print()
      print(w)

