from pprint import pprint

import numpy as np
from sympy import Rational, I, Abs
from scipy.linalg import schur

from ginv import *

def var2matrix(basis):
    assert Monom.cmp == Monom.TOPgrevlex
    rows, i = {}, 0
    for d in range(max(w.lm.degree() for w in basis)):
        for m in Monom.gradus(d):
            if not basis.find(m):
                rows[m] = i
                i += 1

    n, hp = len(Monom()), basis.HP()
    for i in range(1, n):
        assert hp[i] == 0
    hp0 = hp[0]

    X = [np.zeros((hp0, hp0), dtype=np.complex128) for i in range(n)]
    Xlen = [0 for i in range(n)]
    for k, v in rows.items():
        for i in range(n):
            for m, c in Poly(Monom(i)*k).NF(basis):
                if c:
                    Xlen[i] += 1
                    X[i][rows[m], v] = c.evalf(33)

    return X, Xlen

def solve_poly_system(matrices, tol=1e-10):
    """
    matrices: список матриц умножения [M_x1, M_x2, ..., M_xn]
    размером (hp0, hp0) каждая.
    """
    n_vars = len(matrices)
    hp0 = matrices[0].shape[0]

    # 1. Генерируем случайную линейную комбинацию для разделения корней
    rng = np.random.default_rng()
    weights = rng.standard_normal(n_vars) + 1j*rng.standard_normal(n_vars)
    M_combined = sum(w * M for w, M in zip(weights, matrices))

    # 2. Находим комплексное разложение Шура
    # T = Q^H * M_combined * Q, где Q - унитарная, T - треугольная
    T_rand, Q = schur(M_combined, output='complex')

    # 3. Группируем значения
    # Так как матрицы коммутируют, Q приводит их все к треугольному виду
    roots = []
    for j in range(hp0):
        point = np.empty(n_vars, dtype=np.complex128)
        for k, M in enumerate(matrices):
            # Компонента корня - это j-й диагональный элемент
            # Mi_triangular = Q.conj().T @ M @ Q
            point[k] = np.dot(Q[:, j].conj(), np.dot(M, Q[:, j]))
        roots.append(point)

    return sorted(roots, key=lambda r: np.linalg.norm(r))

def check_solve(var, polys, roots):
    res = [p for p in polys]
    for j in range(len(res)):
        for i in range(len(var)):
            res[j] = res[j].replace(f'{var[i]}', f'r__[{i}]')

    subs = []
    for r in roots:
        r__ = [Rational(f"{c.real}") + I*Rational(f"{c.imag}") for c in r]
        err = np.empty(len(res), dtype=np.complex64)
        for i, p in enumerate(res):
            err[i] = Abs(eval(p).evalf(33))
        subs.append(err)

    return subs

# Пример использования
if __name__ == "__main__":
    # Тестовый пример с кратными корнями

    var = 'x, y, z'
    polys = (
    # "(z - 1)**2*(z + 1)**4*(z**6 + z**2 - 9*z - 7)**3*(103*z + 108)**2",
    "(z - 1)**1*(z + 1)*(z**6 + z**2 - 9*z - 7)**1*(103*z + 108)**1",
    "85*y + 92*z**26 + 80*z**25 + 81*z**24 + 85*z**23 + 64*z**22 + 46*z**21 + 12*z**20 + 31*z**19 + 74*z**18 + 53*z**17 + 73*z**16 + 50*z**15 + 52*z**14 + 58*z**13 + 70*z**12 + 75*z**11 + 38*z**10 + 7*z**9 + 82*z**8 + 74*z**7 + 69*z**6 + 36*z**5 + 91*z**4 + 39*z**3 + 65*z**2 + 90*z + 45",\
    "85*x + 24*z**26 + 91*z**25 + 28*z**24 + 73*z**23 + 75*z**22 + 57*z**21 + 48*z**20 + 47*z**19 + 5*z**18 + 96*z**17 + 21*z**16 + 39*z**15 + 57*z**14 + 79*z**13 + 25*z**12 + 82*z**11 + 97*z**10 + 22*z**9 + 36*z**8 + 46*z**7 + 30*z**6 + 97*z**5 + 14*z**4 + 91*z**3 + 41*z**2 + 53*z + 56")
    Monom.cmp = Monom.TOPgrevlex
    x, y, z = Poly.init(var.split(', '))
    test = Janet()
    res = ginvBlockLow([eval(p) for p in polys],  test, level=1)
    print(f"crit1: {res[1]}")
    print(f"crit1: {res[2]}")
    print(f" time: {res[0]:.2f} sec")
    print(f"    count: {test.count()}")
    print(f"reduction: {test.reduction()}")
    print(f"       HP: {test.HP()}")

    # for w in test.GB():
    #     print(w.lm, end=", ")
    # print()

    X, Xlen = var2matrix(test)
    n, hp0 = len(X), X[0].shape[0]
    print([Xlen[i] for i in range(n)], hp0**2)
    print([f"{Xlen[i]*100/hp0**2:.0f}%" for i in range(n)])

    roots = solve_poly_system(X)
    for r in roots:
        print(np.round(r[-1], decimals=7))

    subs = check_solve(var.split(', '), polys, roots)
    for s in subs:
        print(max(s))
