from itertools import chain
from sympy import *

from monomial import *

func_poly = set([Add, Mul, Pow])
def compact(f):
  def cmpct(a):
    if not a.args:
      return a
    else:
      if a.func in func_poly:
        return a.func(*tuple(cmpct(s) for s in a.args))
      elif a.func != Derivative:
        return Symbol(f"{a.func}")
      else:
        m = []
        for xi, k in a.args[1:]:
          m.append(f"{xi}"*k)
        return Symbol(f"{a.args[0].func}_{{{''.join(m)}}}", real=True)
  return cmpct(f)

def init(f, v):
  global sym_f, sym_v, sym_DF, sym_fun, sym_f1, sym_var
  sym_f, sym_v, sym_DF = f, v, set()
  Monom.cmp = Monom.TOPgrevlex
  Monom.init([str(v) for v in sym_v], [str(f) for f in sym_f])
  sym_fun = dict(zip(sym_f, (Monom(pos=i) for i in range(len(sym_f)))))
  sym_f1 = dict(zip((i for i in range(len(sym_f))), sym_f))
  sym_var = dict(zip(sym_v, (Monom(i) for i in range(len(sym_v)))))

def totalD(a, eps, toD):
  if not a.args:
    return a
  else:
    if a.func != Derivative:
      return a.func(*(totalD(s, eps, toD) for s in a.args))
    elif (eps, 1) not in a.args:
      return a
    else:
      assert a.args[0] in sym_f
      r = sym_f[a.args[0]]
      for v, l in a.args[1:]:
        if v != eps:
          for i in range(l):
            r = toD[v](r)
      return r

def df2m(a):
  assert a.func == Derivative
  m = sym_fun[a.args[0]]
  for xi in a.args[1:]:
    if isinstance(xi, Symbol):
      m = m*sym_var[xi]
    else:
      m = m*sym_var[xi[0]]**xi[1]
  return m

def m2df(m):
  r = sym_f1[m.position()]
  for i in range(len(m)):
    r = r.diff(sym_v[i], m[i])
  return r

def findDiv(a, d):
  r = None
  def find(a, r):
    if a.args:
      if a.func == Derivative and a.args[0] in sym_fun:
        m = df2m(a)
        if m.divisible(d) and (not r or m.cmp(r) > 0):
          r = m
      else:
        for s in a.args:
          r = find(s, r)
    return r
  return find(a, r)

def NF(f, df, G):
  assert len(df) == len(G)
  ms = [df2m(d) for d in df]
  while True:
    r = None
    for l in range(len(ms)):
      r = findDiv(f, ms[l])
      if r:
        break
    if not r:
      break
    m = m2df(r)
    deg = f.as_poly(m).degree()
    assert deg >= 0
    if deg > 1:
      c = f.coeff(m, deg)*m**(deg - 1)
    else:
      c = f.coeff(m, deg)
    d = []
    for v, i in zip(sym_v, r/ms[l]):
      if i: d.append(v)
      if i > 1: d.append(i)
    if d:
      f = f - G[l].diff(*d)*c/G[l].coeff(df[l])
    else:
      f = f - G[l]*c/G[l].coeff(df[l])
    f = f.expand()
  return f

def lstD(a, toU):
  if not a.args:
    return a
  else:
    if a in toU:
      return toU[a]
    elif a.func != Derivative:
      return a.func(*(lstD(s, toU) for s in a.args))
    elif a.args[0] not in sym_f:
      return a
    else:
      sym_DF.add(a)
      return a

def to_eqs(a, var):
  for x in var:
    r = []
    for b in a:
      if b:
        r.append([b.coeff(x, i) for i in range(b.as_poly(x).degree()+1) if b.coeff(x, i)])
    a = chain(*r)
  return list(a)

if __name__ == '__main__':
  eps, t, x, y = symbols('ε, t, x, y', real=True)
  U = Function('u')(eps, t, x, y)

  u, ut, ux, uy = symbols('u, u_t, u_x, u_y', real=True)
  F = Function('f')(t, x, y, u, ut, ux, uy)

  eq1 = U.diff(t, x) + U.diff(x)**2*U.diff(x, 2) + U.diff(x, 3) + U.diff(y, 2)
  print(compact(eq1))
  print(compact(eq1.diff(eps).expand())**2)

  init({U: F}, (t, x, y))

  def Dt(a):
    return a.diff(t) + a.diff(u)*U.diff(t) + a.diff(ut)*U.diff(t, t)\
      + a.diff(ux)*U.diff(x, t) + a.diff(uy)*U.diff(y, t)
  def Dx(a):
    return a.diff(x) + a.diff(u)*U.diff(x) + a.diff(ut)*U.diff(t, x)\
      + a.diff(ux)*U.diff(x, x) + a.diff(uy)*U.diff(y, x)
  def Dy(a):
    return a.diff(y) + a.diff(u)*U.diff(y) + a.diff(ut)*U.diff(t, y)\
      + a.diff(ux)*U.diff(x, y) + a.diff(uy)*U.diff(y, y)

  toD = {
    t: Dt,\
    x: Dx,\
    y: Dy,\
  }
  eq2 = totalD(eq1.diff(eps), eps, toD).expand()
  print(compact(eq2))

  eq3 = NF(eq2, [U.diff(t, x)], [eq1])
  print(compact(eq3))

  toU = {
    U.diff(t): ut,\
    U.diff(x): ux,\
    U.diff(y): uy,\
    U: u,\
  }
  eq4 = lstD(eq3, toU).expand()
  print(compact(eq4))
  print([compact(df) for df in sym_DF])

  eq5 = to_eqs([eq4], sym_DF)
  for eq in eq5:
    print(compact(eq))

