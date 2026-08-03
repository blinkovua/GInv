from sympy import *

from monomial import *

def RadauIA1(f):
    global fda_f, fda_h, fda_v, fda_clp
    fda_clp = 2
    y = Matrix([g(fda_v) + O(fda_h**fda_clp) for g in fda_f])
    y1 = Matrix([T(g(fda_v + fda_h)) + O(fda_h**fda_clp) for g in fda_f])
    dy = Matrix([(T(g(fda_v + fda_h)) - g(fda_v))/fda_h for g in fda_f])
    k1 = f(y1).expand()
    return [clip(v.removeO()) for v in expand(dy - k1)]

def GaussLegendre2(f):
    global fda_f, fda_h, fda_v, fda_clp
    fda_clp = 3
    y = Matrix([g(fda_v) + O(fda_h**fda_clp) for g in fda_f])
    y1 = Matrix([T(g(fda_v + fda_h)) + O(fda_h**fda_clp) for g in fda_f])
    dy = Matrix([(T(g(fda_v + fda_h)) - g(fda_v))/fda_h for g in fda_f])
    k1 = f(y1/2 + y/2)
    return [clip(v.removeO()) for v in expand(dy - k1)]

def CrankNicolson2(f):
    global fda_f, fda_h, fda_v, fda_clp
    fda_clp = 3
    y = Matrix([g(fda_v) + O(fda_h**fda_clp) for g in fda_f])
    y1 = Matrix([T(g(fda_v + fda_h)) + O(fda_h**fda_clp) for g in fda_f])
    dy = Matrix([(T(g(fda_v + fda_h)) - g(fda_v))/fda_h for g in fda_f])
    k1 = f(y)
    k2 = f(y1)
    return [clip(v.removeO()) for v in expand(dy - (k1 + k2)/2)]

def RadauIIA3(f):
    global fda_f, fda_h, fda_v, fda_clp
    fda_clp = 4
    y = Matrix([g(fda_v) + O(fda_h**fda_clp) for g in fda_f])
    y1 = Matrix([T(g(fda_v + fda_h)) + O(fda_h**fda_clp) for g in fda_f])
    dy = Matrix([(T(g(fda_v + fda_h)) - g(fda_v))/fda_h for g in fda_f])
    k2 = f(y1).expand()
    k1 = (-k2/3 + 4*dy/3).expand()
    return [clip(v.removeO()) for v in expand((k1 - f(y + fda_h*(5*k1/12 - k2/12)))*3/4)]

def RungeKutta4(f):
    global fda_f, fda_h, fda_v, fda_clp
    fda_clp = 5
    y = Matrix([g(fda_v) + O(fda_h**fda_clp) for g in fda_f])
    y1 = Matrix([T(g(fda_v + fda_h)) + O(fda_h**fda_clp) for g in fda_f])
    dy = Matrix([(T(g(fda_v + fda_h)) - g(fda_v))/fda_h for g in fda_f])
    k1 = f(y).expand()
    k2 = f(y + fda_h*k1/2).expand()
    k3 = f(y + fda_h*k2/2).expand()
    k4 = f(y + fda_h*k3).expand()
    return [clip(v.removeO()) for v in expand(dy - (k1 + 2*k2 + 2*k3 + k4)/6)]

def DormandPrince5(f):
    global fda_f, fda_h, fda_v, fda_clp
    fda_clp = 6
    y = Matrix([g(fda_v) + O(fda_h**fda_clp) for g in fda_f])
    y1 = Matrix([T(g(fda_v + fda_h)) + O(fda_h**fda_clp) for g in fda_f])
    dy = Matrix([(T(g(fda_v + fda_h)) - g(fda_v))/fda_h for g in fda_f])
    k1 = f(y).expand()
    k2 = f(y + fda_h*k1/5).expand()
    k3 = f(y + fda_h*(k1*3/40 + k2*9/40)).expand()
    k4 = f(y + fda_h*(k1*44/45 - k2*56/15 + k3*32/9)).expand()
    k5 = f(y + fda_h*(k1*19372/6561 - k2*25360/2187 +\
        k3*64448/6561 - k4*212/729)).expand()
    k6 = f(y + fda_h*(k1*9017/3168 - k2*355/33 + k3*46732/5247 +\
        k4*49/176 - k5*5103/18656)).expand()
    return [clip(v.removeO()) for v in expand(dy -\
        (k1*35/384 + k3*500/1113 + k4*125/192 - k5*2187/6784 + k6*11/84))]

def AdamsBashforth4(f):
    global fda_f, fda_h, fda_v, fda_clp
    fda_clp = 5
    y = [Matrix([T(g(fda_v + i*fda_h))+ O(fda_h**fda_clp)\
        for g in fda_f]) for i in range(4)]
    dy = Matrix([(T(g(fda_v + 4*fda_h)) - T(g(fda_v + 3*fda_h)))/fda_h for g in fda_f])
    return [clip(v.removeO()) for v in expand(dy -\
        (55*f(y[3]) - 59*f(y[2]) + 37*f(y[1]) - 9*f(y[0]))/24)]

def AdamsBashforth5(f):
    global fda_f, fda_h, fda_v, fda_clp
    fda_clp = 6
    y = [Matrix([T(g(fda_v + i*fda_h))+ O(fda_h**fda_clp)\
        for g in fda_f]) for i in range(5)]
    dy = Matrix([(T(g(fda_v + 5*fda_h)) - T(g(fda_v + 4*fda_h)))/fda_h for g in fda_f])
    return [clip(v.removeO()) for v in expand(dy -\
        (1901*f(y[4]) - 2774*f(y[3]) + 2616*f(y[2])\
            - 1274*f(y[1]) + 251*f(y[0]))/720)]

def AdamsMoulton4(f):
    global fda_f, fda_h, fda_v, fda_clp
    fda_clp = 5
    y = [Matrix([T(g(fda_v + i*fda_h))+ O(fda_h**fda_clp)\
        for g in fda_f]) for i in range(5)]
    dy = Matrix([(T(g(fda_v + 4*fda_h)) - T(g(fda_v + 3*fda_h)))/fda_h for g in fda_f])
    return [clip(v.removeO()) for v in expand(dy -\
        (251*f(y[4]) + 646*f(y[3]) - 264*f(y[2])\
            + 106*f(y[1]) - 19*f(y[0]))/720)]

def AdamsMoulton5(f):
    global fda_f, fda_h, fda_v, fda_clp
    fda_clp = 6
    y = [Matrix([T(g(fda_v + i*fda_h))+ O(fda_h**fda_clp)\
        for g in fda_f]) for i in range(6)]
    dy = Matrix([(T(g(fda_v + 5*fda_h)) - T(g(fda_v + 4*fda_h)))/fda_h for g in fda_f])
    return [clip(v.removeO()) for v in expand(dy -\
        (475*f(y[5]) + 1427*f(y[4]) - 798*f(y[3])\
            + 482*f(y[2]) - 173*f(y[1]) + 27*f(y[0]))/1440)]

def init(f, v, h):
    global fda_f, fda_v, fda_h, fda_fun, fda_var

    fda_f, fda_v, fda_h = f, v, h
    Monom.cmp = Monom.TOPdeglex
    Monom.init([str(fda_v)], [str(g(fda_v)) for g in fda_f])
    fda_var = Monom(0)
    fda_fun = dict((fda_f[i](fda_v), Monom(pos=i)) for i in range(len(fda_f)))

def T(f):
    global fda_clp, fda_v, fda_h
    return sum(\
        diff(f, fda_h, j).subs(fda_h, 0).doit()*fda_h**j/\
                 (factorial(j))
        for j in range(fda_clp))

def clip(f):
    f = f.expand()
    return SeriesTrunc([f.coeff(fda_h, i) for i in range(fda_clp)])

def df2m(a):
    if a.func != Derivative:
        m = fda_fun[a]
    else:
        m = fda_fun[a.args[0]]
        for xi in a.args[1:]:
            if isinstance(xi, Symbol):
                m = m*fda_var
            else:
                m = m*fda_var**xi[1]
    return m

def m2df(m):
    return fda_f[m.position()](fda_v).diff(fda_v, m[0])

def findDiv(a, d):
    r = None
    def find(a, r):
        if a.args:
            if (a in fda_fun) or \
                    (a.func == Derivative and a.args[0] in fda_fun):
                m = df2m(a)
                if m.divisible(d) and (not r or m.cmp(r) > 0):
                    r = m
            else:
                for s in a.args:
                    r = find(s, r)
        return r
    return find(a, r)

def reduction(f1, f2, m, c, shift):
    assert shift < fda_clp
    r = [f1[i] for i in range(shift)]
    if not m:
        for i in range(shift, fda_clp):
            r.append(expand(f1[i] - f2[i-shift]*c))
    else:
        for i in range(shift, fda_clp):
            r.append(expand(f1[i] - f2[i-shift].diff(*m)*c))
    return r

def NF(f, df, G, head=False):
    assert len(df) == len(G)
    ms = [df2m(d) for d in df]
    for i in range(0 if head else 1, fda_clp):
        t = 0
        if f[i]:
            while True:
                r = None
                for l in range(len(ms)):
                    r = findDiv(f[i], ms[l])
                    if r:
                        break
                if not r:
                    break
                m = m2df(r)
                deg = f[i].as_poly(m).degree()
                c = f[i].coeff(m, deg)
                if deg > 1:
                    c = f[i].coeff(m, deg)*m**(deg - 1)
                m = r/ms[l]
                d = []
                if m[0] > 0:
                    d.append(fda_v)
                    if m[0] > 1:
                        d.append(m[0])

                f = reduction(f, G[l], tuple(d), c/G[l][0].coeff(df[l]), i)
    return SeriesTrunc(f)


def compact(f):
    def cmpct(a):
        if not a.args:
            return a
        else:
            if a in fda_fun:
                return Symbol("%s" % a.func, real=True)
            elif a.func == Derivative and a.args[0] in fda_fun:
                m = []
                for xi in a.args[1:]:
                    if isinstance(xi, Symbol):
                        m.append(str(xi))
                    else:
                        m.append(str(xi[0])*xi[1])
                return Symbol("%s_{%s}" % (a.args[0].func, "".join(m)), real=True)
            else:
                return a.func(*tuple(cmpct(s) for s in a.args))
    return cmpct(f)

class SeriesTrunc(tuple):
    def prn(self, p=None):
        if p:
            for i in range(fda_clp):
                print("%s^%d =>" % (fda_h,  i))
                display(compact(self[i]).collect(p, factor))
        else:
            for i in range(fda_clp):
                print("%s^%d =>" % (fda_h,  i))
                display(compact(self[i]).factor())


    def prnlatex(self, p=None):
        if p:
            print(latex(compact(self[0]).collect(p, factor)))
            print(r"+%s\left(" % fda_h)
            print(latex(compact(self[1]).collect(p, factor)))
            print(r"\right)")
            for i in range(2, fda_clp):
                print(r"+%s^%d\left(" % (fda_h, i))
                print(latex(compact(self[i]).collect(p, factor)))
                print(r"\right)")
        else:
            print(latex(compact(self[0]).factor()))
            print(r"+%s\left(" % fda_h)
            print(latex(compact(self[1]).factor()))
            print(r"\right)")
            for i in range(2, fda_clp):
                print(r"+%s^%d\left(" % (fda_h, i))
                print(latex(compact(self[i]).factor()))
                print(r"\right)")

class FDA(tuple):
    def __new__(cls, F):
        global fda_f, fda_h, fda_v, fda_clp
        assert len(F) == len(fda_f)

        lm = tuple(g(fda_v).diff(fda_v) for g in fda_f)
        for r, m in zip(F, lm):
            assert len(r) == fda_clp
            assert r[0].coeff(m)
            pass

        # Формируем генератор или список для элементов кортежа
        elements = [SeriesTrunc(NF(r, lm, F, head=False)) for r in F]

        # Передаем элементы в базовый класс tuple через __new__
        r =  super(FDA, cls).__new__(cls, elements)
        r.lm = lm
        return r

    def NF(self, F):
        return SeriesTrunc(NF(clip(F), self.lm, self, head=True))


if __name__ == '__main__':
    eps, t, h = symbols(r'\varepsilon, t, h', real=True)
    u, u1 = symbols('u, u1', cls=Function)

    eq = u(t).diff(t, 2)  + u(t) + eps*u(t)**3
    print(eq)
    print()

    eq1 = (u(t).diff(t)**2/2 + u(t)**2/2 + eps*u(t)**4/4) - h
    print(expand(eq1.diff(t) - eq*u(t).diff(t)))
    print()

    init((u, u1), t, h)

    def f(y):
        return Matrix([\
            y[1],\
            -y[0] - eps*y[0]**3,\
        ])

    # F = RadauIA1(f)
    # F = GaussLegendre2(f)
    # F = CrankNicolson2(f)
    # F = RadauIIA3(f)
    # F = RungeKutta4(f)
    # F = DormandPrince5(f)
    # F = AdamsBashforth4(f)
    # F = AdamsBashforth5(f)
    # F = AdamsMoulton4(f)
    F = AdamsMoulton5(f)

    for r in F:
        r.prnlatex(eps)
        print()

    fda = FDA(F)

    for r in fda:
        r.prnlatex(eps)
        print()

    F3 = ((T(u1(t+h))**2/2 + T(u(t+h))**2/2 + eps*T(u(t+h))**4/4) -\
          (T(u1(t))**2/2 + T(u(t))**2/2 + eps*T(u(t))**4/4))/h
    f3 = fda.NF(F3)
    f3.prnlatex(eps)
    print()
