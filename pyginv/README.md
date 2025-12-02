# PyGInv: Investigation and Solution of Polynomial and Differential Systems

**Yu. A. Blinkov**  
E-mail: blinkovua@info.sgu.ru

**Abstract:**  
Computations in the PyGInv package for Gröbner bases in polynomial and differential systems and the construction of numerical schemes are presented. The results allow one to find the dimension of the solution space, numerically find all solutions in the polynomial case for zero-dimensional ideals, compute symmetries of differential equations without using special formulas, and perform transformations with difference schemes, for example, in the case of compact difference schemes. The PyGInv package is a lightweight version of GInv, written in pure Python and integrated with the SymPy computer algebra system.

**Keywords:** computer algebra, Gröbner bases, polynomial systems, differential equations

## Gröbner Bases

Historically, works aimed at creating the theory of Gröbner bases were focused on constructively solving the ideal membership problem [1-4].

An alternative approach to the Gröbner basis method was formed during the investigation of differential equations [5-7]. It was oriented towards reducing the system to a form from which the dimension of the solution space could be determined.

The difference between Gröbner bases and their extension, the involutive basis, is best demonstrated in Figure 1. In the left figure, the cones of divisible monomials are shown in different colors. As a result of fixing the monomial ordering in the general polynomial case, at the intersection points of the cones, it is necessary to compute S-polynomials relative to the current basis and add them to the basis. Upon completion of this process, a Gröbner basis in the chosen order will be obtained. Now, to determine membership, it is sufficient to compute the normal form of the polynomial relative to the current basis. In this case, the sequence of reductions is unimportant because all S-polynomials already belong to the basis. In this specific case, it is easy to calculate the Hilbert polynomial HP₈ = 3s + 10, which consists of three one-dimensional cones x^s, y^s, xy^s and a number of monomials that do not fall into these cones: 1_ℳ, x, y, x², xy, ... Consequently, the formula HP₈ = 3s + 10 is chosen such that it gives the exact number of monomials up to total degree s ≥ 8 that have no divisor among the leading monomials of the basis.

![Figure 1](divfull.png)

![Figure 2](divjanet.png)

![Figure 3](divjanetfull.png)

In the general case, such a calculation is difficult to perform. If the cones did not intersect, the computation of the Hilbert polynomial could be represented as the difference between the cone of the unit monomial 1_ℳ and the sum over all cones formed by the leading monomials of the basis.

The Janet involutive partition of variables prohibits division by variables called non-multiplicative. As a result, the cones in Figure 2 do not intersect, but there are also unfilled cones. Figure 3 shows a basis brought to involution by complementing the current basis with prolongations with respect to non-multiplicative variables. The involutive approach to constructing Gröbner bases was developed in the works [8-10].

## PyGinv

PyGInv is a "lightweight" version of GIinv [11, 12] (short for Gröbner INVolutive). It is written in "pure" Python and uses the SymPy computer algebra system (https://www.sympy.org/) for working with parameters. Where possible, PyGInv allows import/export of data from/to SymPy. For building the Janet tree, the Graphviz package (https://graphviz.org/) is optionally used.

Currently, PyGInv allows building Gröbner bases for polynomials, modules, linear differential equations, and linear difference schemes. In many cases, as will be shown below, it is also possible to work with nonlinear differential equations and difference schemes. PyGInv contains additional functions for investigating and solving polynomial and differential systems.

The source code, presented in four files (`monomial.py`, `polynomial.py`, `janet.py`, `ginv.py`), takes up just under 2000 lines of code. All source files and example files, including those described below, are located at https://github.com/blinkovua/GInv/tree/master/pyginv.

Using the debugged PyGInv interface, it is planned to partially transfer it to the GIinv package written in C++11, which uses its own memory manager [13] for symbolic computations and "long" arithmetic.

All computations were performed on an Intel(R) Core(TM) i7-4770K CPU @ 3.50GHz processor, with 16 GB of RAM, in Debian 12.

## Polynomials and Modules

Let us consider the purely monomial basis illustrated in Figures 1-3.

Line 1 imports PyGInv as the main module `ginv.py`.

Line 3 specifies the type of admissible term ordering. Currently implemented are `POTlex`, `TOPlex`, `POTdeglex`, and `TOPdeglex`. In their names, `lex` stands for lexicographical, and `deglex` stands for degree lexicographical by total degree. Accordingly, `POT` means Position Over Term, and `TOP` means Term Over Position.

```python
from ginv import *

Monom.cmp = Monom.TOPdeglex
x, y = Poly.init('x, y'.split(', '))
mnm = Janet()
res = ginvBlockLow((x**7*y, x**5*y**2, \
   x**4*y**3, x**2*y**4), mnm, level=1)
mnm.saveImage('mnm.pdf')
```

Line 4 defines the variables `x`, `y` with the order $x \succ y$. Line 5 defines the type of involutive division; currently, only Janet division based on the Janet binary tree [1] shown in Figures 4 and 5 is implemented. Saving the result as a Janet tree happens on line 8. Lines 9-10 call the algorithm `ginvBlockLow` implementing the block strategy described in [2], which works only with the `TOPdeglex` ordering. The algorithm `ginvMin`, implementing the minimal strategy, works with all types of orderings.

![Figure](mnm.png)

Figures 4 and 5 show different levels of output information. The figure displays only the tree structure, with monomials belonging to the reduced Gröbner basis shown in bold, and monomials additionally belonging to the minimal involutive basis [3] shown in regular font.

Below, lines 1-17 demonstrate the computation of the Gröbner basis for the standard example of cyclic roots. In this case, the resulting ideal is zero-dimensional with $HP_{6}=4s+6$.

```python
Monom.cmp = Monom.TOPdeglex
x1, x2, x3, x4 = \
  Poly.init('x1, x2, x3, x4'.split(', '))
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
```

Lines 11-17 show the output of various information related to the computation. Their result is presented below in lines 18-23.

```python
    crit1: 6
    crit2: 0
     time: 0.02 sec
    count: 7
reduction: 54
       HP: 4s + 6
```

In Figure 5, compared to the monomial below, its non-multiplicative variables and the prolongations constructed from them are additionally displayed. At the very bottom, the "ancestor" is recorded; Buchberger's criteria [4] and some types of involutive divisions [5] (not yet implemented in PyGInv) operate based on it.

![Figure](cyclic4.png)

Lines 24-34 output the Gröbner basis. The wrapper class `Wrap` used contains a lot of technical information for efficiently organizing computations.

```python
for w in cyclic4.GB():
    print(w.poly)
x3**2*x4**4 + x2*x3 + x2*x4*-1 + x3*x4 + \
  x4**2*-2
x3**3*x4**2*-1 + x3**2*x4**3*-1 + x3 + x4
x2*x4**4*-1 + x4**5*-1 + x2 + x4
x2*x3*x4**2 + x3**2*x4**2 + x2*x4**3*-1 + \
  x3*x4**3 + x4**4*-1 + -1
x2*x3**2*-1 + x3**2*x4*-1 + x2*x4**2 + x4**3
x2**2*-1 + x2*x4*-2 + x4**2*-1
x1 + x2 + x3 + x4
```

Lines 1-12 show the organization of computations using the syzygy module for the fourth cyclic roots. For this, dependent variables are introduced on line 2, with the variable order $f \succ p_1 \succ  p_2 \succ  p_3 \succ  p_4$. In modules, they play the same role as functions in differential equations and difference schemes. Internally, this is organized using a positive number that determines the position. Naturally, they should only appear linearly in expressions. In this example, we have 6 different Janet trees according to the number of dependent variables. Therefore, line 6 uses the wrapper class `Forest`. It is assumed that it will work with other types of involutive divisions.

```python
var = 'x1, x2, x3, x4'.split(', ')
fun = 'f, p1, p2, p3, p4'.split(', ')
var, fun = Poly.init(var, fun)
x1, x2, x3, x4 = var
f, p1, p2, p3, p4 = fun
cyclic4 = Forest(Janet)
res = ginvMin((
f*(x1+x2+x3+x4)-p1,\
f*(x1*x2+x2*x3+x3*x4+x4*x1)-p2,\
f*(x1*x2*x3+x2*x3*x4+x3*x4*x1+x4*x1*x2)-p3,\
f*(x1*x2*x3*x4-1)-p4), \
cyclic4, level=1)
```

To construct syzygies, only expressions not containing the dependent variable `f` are needed, so the loop on line 14 starts from position 1. For this system, an elimination ordering was not required. Lines 15-19 show the conversion of the computations into a vector form representation using SymPy.

```python
syzygy = []
for i in range(1, 5):
  for w in cyclic4[i].GB():
    syzygy.append(tuple(sympy.Add(\
*tuple(eval(f"{m.lcm(fun[k].lm())}*{c}") \
for m, c in w.poly if m.position() == k),\
   evaluate=False) for k in range(1, 5)))
syzygy
```

$[ ( x_{3}^{2} x_{4}^{4} - x_{4}^{2},   x_{2} x_{3}^{2} x_{4}^{2} - x_{2} x_{4}^{4} - x_{3}^{2} x_{4}^{3} - x_{3} x_{4}^{4} + x_{3} + x_{4}, \\  - x_{2} x_{3}^{2} x_{4} + x_{2} x_{4}^{3} + x_{4}^{4} - 1,   x_{2} x_{3}^{2} - x_{2} x_{4}^{2} + x_{3}^{2} x_{4} - x_{4}^{3}),\\  ( x_{2} x_{3}^{2} + x_{3}^{2} x_{4},   - x_{2} x_{3} - x_{2} x_{4} - x_{3}^{2} - x_{3} x_{4}, \  x_{2} + x_{4},   0), \\  ( - x_{2} - x_{4},   - x_{2}^{2} x_{4}^{2} + 1,  x_{2}^{2} x_{4} + x_{2} x_{4}^{2},  - x_{2}^{2} - 2 x_{2} x_{4} - x_{4}^{2})]$

Lines 21-28 perform a verification of the correctness of the three constructed syzygies.

```python
P = (x1+x2+x3+x4,\
x1*x2+x2*x3+x3*x4+x4*x1,\
x1*x2*x3+x2*x3*x4+x3*x4*x1+x4*x1*x2,\
x1*x2*x3*x4-1)
for syz in syzygy:
  print(sum(s*p for s, p \
    in zip(syz, P)).expand(), end=" ")
0 0 0
```

## Finding Roots

For zero-dimensional ideals, it is possible to efficiently find all roots of a system of polynomials, taking into account their multiplicity. Consider a simple example:

`{x^3 - y^2 + z - 1, y^3 - z^2 + x - 1, z^3 - x^2 + y - 1}`

with the variable ordering $x \succ y \succ z$. Under the `deglex` term order, and according to Buchberger's first criterion [1], this set is a Gröbner basis. The minimal involutive Janet basis already contains 13 elements, represented here only by their leading monomials:

`{z^3, y z^3, y^2 z^3, y^3, x z^3, x y z^3, x y^2 z^3, x y^3, x^2 z^3, x^2 y z^3, x^2 y^2 z^3, x^2 y^3, x^3}`.

By applying the static function `gradus` of the `Monom` class on line 3, one can iterate by total degree to find all monomials that are not divisible by any leading monomial of the system.

```python
rows, i = {}, 0
for d in range(8):
  for m in Monom.gradus(d):
    if not test.find(m):
      print(f"{m!r}", end=", ")
      rows[m] = i
      i += 1
```

Line 5 prints these monomials in their internal representation as a list of variable exponents. Their number exactly equals the value of the Hilbert polynomial $HP_{7}= 27$.

```
[0 0 0],
[1 0 0], [0 1 0], [0 0 1],
[2 0 0], [1 1 0], ... , [0 0 2],
[2 1 0], [2 0 1], ... , [0 2 1], [0 1 2],
[2 2 0], [2 1 1], ... , [0 2 2],
[2 2 1], [2 1 2], [1 2 2],
[2 2 2],
```

As a result, after performing all reductions with respect to the Gröbner basis (line 17, call to the `NF` function), for monomials in the variable $x$ up to degree 26 inclusive, we can only obtain monomials listed on lines 8-14. This allows us to assign each row of the matrix to $x^d$. Similarly, to construct column `b` on lines 19-21, we assign the monomial $x^27$.

```python
A=[[0 for k in range(27)] for i in range(27)]
for d in range(27):
  for m, c in (x**d).NF(test):
    A[rows[m]][d] = c
b = [0 for i in range(27)]
for m, c in (x**27).NF(test):
  b[rows[m]] = c

A, b = sympy.Matrix(A), sympy.Matrix(b)
s = sympy.linsolve((A, b)).args[0]
p = x**27-sum(x**d*s[d] for d in range(27))
```

Solving the resulting system in matrix form using SymPy, we find the form of the polynomial in the given ideal involving only one variable:

$x^{27} - 9 x^{24} + 29 x^{21} + 6 x^{19} - 53 x^{18} + 22 x^{17} - 63 x^{16} + 96 x^{15} - 149 x^{14} + 242 x^{13} - 261 x^{12} + 484 x^{11} - 545 x^{10} + 740 x^{9} - 908 x^{8} + 972 x^{7} - 1220 x^{6} + 1047 x^{5} - 1045 x^{4} + 943 x^{3} - 535 x^{2} + 422 x - 216$

The roots of this polynomial can be found using the `nroots` function from the SymPy computer algebra system.

A more efficient computation can be organized using the matrix representation of variables to numerically solve the eigenvalue problem. On lines 26-32, using the relation that multiplying the monomials from lines 8-14 by a variable reduces again to this set of monomials with respect to the Gröbner basis, we construct the matrix for this variable.

```python
dct = [{}, {}, {}]
for k, v in rows.items():
  for i in range(3):
    for m, c in Poly(Monom(i)*k).NF(test):
      if c:
        dct[i][rows[m], v] = c
X, Y, Z = (SparseMatrix(27, 27, dct[i]) for i in range(3))
X*Y == Y*X and X*Z == Z*X and Z*Y == Y*Z
True
```

The matrices have many zero elements and, despite not being symmetric, commute with each other (lines 34-35).

Using the SciPy library (https://scipy.org/) on lines 36-43, one can easily compute the eigenvalues, which will correspond exactly to the roots of the original system [2].

```python
import numpy as np
from scipy import linalg

A = np.empty((27, 27), dtype=np.float64)
for j in range(27):
  for k in range(27):
    A[j][k] = X[j, k]
print(linalg.eigvals(A))
```

The computation of the 7th cyclic roots in PyGInv takes 1225 seconds. This system of polynomials has 924 roots counting multiplicity. The calculation of the matrix representation takes 366 seconds. The matrix densities are as follows [69694, 68288, 66858, 65585, 62887, 51490, 18216], which corresponds to the percentages [8%, 8%, 8%, 8%, 7%, 6%, 2%]. The eigenvalue calculation takes only 19 seconds.

## Graph Coloring

Using Rabinowitsch's trick [1] to represent inequalities by introducing additional variables, Groebner bases can be used to solve the problem of coloring graphs with *n* colors. For example, in the case of three colors, the colors for adjacent vertices can be defined by the following polynomials:

```python
Monom.cmp = Monom.POTlex
k, y, x = Poly.init('k, y, x'.split(', '))
test = Janet()
res = ginvMin((
x*(x+1)*(x-1),
y*(y+1)*(y-1),
(x - y)*k - 1,\
), test, level=1)
```

In the last polynomial (line 7), the additional variable *k* cannot be zero, meaning the colors *x* and *y* are different. The Groebner basis, after eliminating the polynomial with *k*, has the following form:
`x³ - x`, `y² + yx + x² - 1`,
`2k - 3yx² + 2y - 2x`, where the second polynomial is the desired separating polynomial for the values *x* and *y*.

Using these relations, all adjacent vertices can be described, and by constructing a Groebner basis, all possible colorings of a given graph can be found.

Similarly, for example, for five colors, the following system can be defined:
`x(x+1)(x-1)(x+2)(x-2)`, `y(y+1)(y-1)(y+2)(y-2)`, `(x - y)k - 1`, and the form of the separating polynomial can be easily obtained:
`y⁴ + y³x + y²x² - 5y² + yx³ - 5yx + x⁴ -5x² + 4`.

Let's define the graph shown in Figure 6 (lines 10-19). Here, the first element of each tuple is a vertex, followed by its connections to other vertices.

```python
graph = (
(1, 2, 5, 8, 10),\
(2, 3, 6, 9),\
(3, 4, 7, 10),\
(4, 5, 8, 11),\
(5, 6, 12),\
(6, 7, 11),\
(7, 8, 12),\
(8, 9),\
(9, 10, 12),\
(10, 11))
```

In Python, it is very convenient to work with strings. As a result, the code below generates all the required polynomials for coloring the graph in Figure 6 with three colors.

```python
var = [f"x{vs}" for vs in range(1, 13)]
eqs = [f"{v}**3 + {v}*-1" for v in var]
for vs in graph:
  w = vs[0]
  for v in vs[1:]:
    eqs.append(f"""x{w}**2*-1 + x{w}*x{v}*-1
      + x{v}**2*-1 + 1""")
```

Constructing the Groebner basis in PyGinv took approximately 8 seconds.
The Hilbert polynomial indicates 6 distinct solutions. Let's choose `x₁₂ - 1` as a root.
The construction takes practically zero time, but a choice between two solutions remains.
By setting `x₁₀ + 1`, exactly one solution is constructed, as shown in Figure 6.

![Figure 6](color.png)
Figure 6

## DIFFERENTIAL EQUATIONS

The construction of Gröbner bases in the differential case largely coincides with the case of polynomials and modules. On line 7, the wrapper function `df` is introduced for specifying derivatives (monomials) in PyGinv. Since after executing line 4, the variables `x1`, `x2`, `x3` are ordinary SymPy variables, and the dependent variable `u` is a function, the identification with PyGinv is determined by `df`, where it is important that the variable names as strings match.

```python
Monom.cmp = Monom.TOPdeglex
var = 'x1, x2, x3'.split(', ')
fun = 'u'.split(', ')
var, fun = PolyDiff.init(var, fun)
x1, x2, x3 = var
u = fun[0]
df = PolyDiff.df
diablo = Janet()
res = ginvBlockLow((
df(u, x1, 2) + x3*df(u, x2, 2),\
df(u, x3, 2)), \
diablo, level=1)
```

The system presented above is an example of a Janet system, where if we view $x_3$ as a parameter, the constraint imposed on it by the equation on line 11 leads to a finite number of constants in the solution $HP_{5}=12$.

The minimal involutive basis is presented below.

```python
df(u, x3, 2)
df(u, x2, x3, 2)
df(u, x2, 2, x3)
df(u, x2, 3, x3)
df(u, x2, 4)
df(u, x1, x3, 2)
df(u, x1, x2, x3, 2)
df(u, x1, x2, 2, x3)
df(u, x1, x2, 3, x3)
df(u, x1, x2, 4)
df(u, x1, 2) + df(u, x2, 2)*x3
```

Using the same technique as in the polynomial case, we find all derivatives that have no divisors among the leading derivatives of the basis; there are exactly 12 of them.

```python
rows, i = {}, 0
for d in range(5):
  for m in Monom.gradus(d, pos=0):
    if not diablo.find(m):
      print(f"${m.df_latex()}$", end=", ")
      rows[m] = i
```

These derivatives can be used
${u}$,
${u}_{{x_1}}$, ${u}_{{x_2}}$, ${u}_{{x_3}}$,
${u}_{{x_1} {x_2}}$, ${u}_{{x_1} {x_3}}$, ${u}_{{x_2} {x_2}}$, ${u}_{{x_2} {x_3}}$,
${u}_{{x_1} {x_2} {x_2}}$, ${u}_{{x_1} {x_2} {x_3}}$, ${u}_{{x_2} {x_2} {x_2}}$,
${u}_{{x_1} {x_2} {x_2} {x_2}}$
to find a solution of the original system in the form of a finite Taylor series, as arbitrary constants

$C_{000} + C_{001} x_{3} + C_{010} x_{2} + C_{011} x_{2} x_{3} + C_{100} x_{1} + C_{101} x_{1} x_{3} + C_{110} x_{1} x_{2} + C_{111} x_{1} x_{2} x_{3} + C_{201} x_{1}^{2} x_{3} - C_{201} x_{2}^{2} + C_{211} x_{1}^{2} x_{2} x_{3} - \frac{C_{211} x_{2}^{3}}{3} + C_{301} x_{1}^{3} x_{3} - 3 C_{301} x_{1} x_{2}^{2} + C_{311} x_{1}^{3} x_{2} x_{3} - C_{311} x_{1} x_{2}^{3}$.

Let us consider the possibility of working with nonlinear differential equations using the example of two-dimensional Navier-Stokes equations for an incompressible fluid. We introduce additional functions to denote the nonlinear parts $u2 = u^2$, $uv = u\cdot v$, $v2 = v^2$ when writing the equations in divergence form. The representation of these equations in SymPy is shown below.

```python
t, x, y, Re = sympy.symbols('t, x, y, Re')
u,v,p,u2,uv,v2=(sympy.Function(f)(t,x,y)\
 for f in 'u,v,p,u2,uv,v2'.split(','))
eq1 = u.diff(x) + v.diff(y)
eq2=u.diff(t)+(u2+p-u.diff(x)/Re).diff(x)\
  + (uv - u.diff(y)/Re).diff(y)
eq3=v.diff(t) + (uv-v.diff(x)/Re).diff(x)\
  + (v2 + p - v.diff(y)/Re).diff(y)
```

Using the static function `diff2poly` on the line for translating from SymPy to PyGInv's internal representation on lines 16-18. Since we have introduced additional functions, which are expressed in terms of the three main ones `u`, `v`, `p`, they must not be accounted for when calculating the Hilbert polynomial. For this, the parameter `verge` has the value 3.

```python
Monom.cmp = Monom.TOPdeglex
var = 't, x, y'.split(', ')
fun = 'u, v, p, u2, uv, v2'.split(', ')
var, fun = PolyDiff.init(var, fun)
df = PolyDiff.df
diff2poly = PolyDiff.diff2poly
NS = Forest(Janet)
res = ginvBlockLow([diff2poly(eq)\
  for eq in (eq1, eq2, eq3)], \
NS, level=1)
print(f"HP: {NS.HP(verge=3)}")
HP: 2s^2 + 6s + 3
```

As a result, we obtain the following system, in which the introduced additional functions $u2$, $uv$, $v2$ are not included in the leading derivatives of the Gröbner basis.

$$
\left\lbrace 
\begin{array}{l}
\mathbf{{u}_{{x}}} + {v}_{{y}} ,\\
\mathbf{{v}_{{x} {y}}} - {u}_{{y} {y}} + {u}_{{t}} \mathrm{Re} + {p}_{{x}} \mathrm{Re} + {u2}_{{x}} \mathrm{Re} 
+ {uv}_{{y}} \mathrm{Re} , \\
\mathbf{{v}_{{x} {x}}} + {v}_{{y} {y}} - {v}_{{t}} \mathrm{Re} - {uv}_{{x}} 
\mathrm{Re} - {p}_{{y}} \mathrm{Re} 
- {v2}_{{y}} \mathrm{Re} , \\
\mathbf{{p}_{{x} {x}} }+ {u2}_{{x} {x}} + 2 {uv}_{{x} {y}} + {p}_{{y} {y}} 
+ {v2}_{{y} {y}}.
\end{array}
\right. 
$$

## Symmetries of Differential Equations

The condition for higher symmetries of the Korteweg–de Vries equation (using shorthand notation, e.g., $u_{xxx} = u_3$) can be written in the following form [1]
$$
\begin{cases}
\mathbf{u_{t}} + 6 u_1 u + u_3 = 0 \\
\mathbf{u_{\varepsilon}} - F(t, x, u, u_1, u_2, \ldots, u_5) = 0
\end{cases}
$$

This form of notation allows one to introduce a constant $\varepsilon$ into an exact solution of the Korteweg–de Vries equation by integrating the second equation.

For the second equation of the system, we introduce the total derivatives of $F$.
$$
\begin{aligned}
\frac{dF}{dt} &= F_t - F_u (6 u_1 u + u_3) - \sum F_{u_{i}} (6 u_1 u + u_3)_i\\
\frac{dF}{dx} &= F_x + F_u u_1 + \sum F_{u_{i}} u_{i+1}
\end{aligned}
$$

The integrability (compatibility) condition for this system can be expressed via the $S$-polynomial (with lexicographic ordering $t \succ x$)
$$
(\mathbf{u_{t}} + 6 u_1 u + u_3)_\varepsilon - \mathbf{u_{\varepsilon t}} + \frac{d}{dt}F = 0
$$

In this last equation, due to the Leibniz differentiation rule, the derivative with respect to $\varepsilon$ enters the integrability condition linearly.
$$
6u u_{\varepsilon 1}+ 6u_1 u_{\varepsilon}+ u_{\varepsilon 3} \frac{d}{dt}F = 0
$$

Furthermore, we can replace $u_{\varepsilon}$ by $F$ using the total derivatives of $F$.
$$
6u \frac{d}{dx}F+ 6u_1 F + \frac{d^3}{dx^3}F + \frac{d}{dt}F = 0
$$

Since $F$ itself does not depend on derivatives higher than the fifth order, we can obtain a system of linear equations for $F$ by equating the coefficients of $u_6$ and $u_7$. In SymPy, all these computations take about 30 lines of code.

As a result, we obtain a system of 5 equations, the first and third of which are very cumbersome:
$$
\begin{cases}
6 f u_{1} + f_{t} + f_{u_1u_1u_1} u_{2}^{3} + \text{88 terms} + 6 f_{x} u,\\
3 f_{u_1u_5} u_{2} + \text{4 terms} + 3 f_{uu_5} u_{1}, \\
3 f_{u_1u_1u_5} u_{2}^{2} + \text{29 terms} + 3 f_{uuu_5} u_{1}^{2}, \\
3 f_{u_1u_5u_5} u_{2} + \text{5 terms} + 3 f_{uu_5u_5} u_{1}, \\
f_{u_5u_5u_5}.
\end{cases}
$$

By constructing a Gröbner basis for the resulting system and refining the form of the solution, the explicit form of the higher symmetries can be found in a few steps.

The first construction yields the following basis (taking 8 seconds to compute in PyGinv):
* ${f}_{{u_5} {u_5}}$,
* ${f}_{{u_4}}$,
* ${f}_{{u_3} {u_5}}$,
* ${f}_{{u_3} {u_3}}$,
* ${f}_{{u_2}} - 20u_1 {f}_{{u_5}}$,
* ${f}_{{u_1} {u_5}}$,
* ${f}_{{u_1} {u_3}}$,
* ${f}_{{u_1} {u_1}}$,
* ${f}_{{u} {u_5}}$,
* ${f}_{{u} {u_3}} - 10 {f}_{{u_5}}$,
* ${f}_{{u} {u_1}} - 6 {f}_{{u_3}}$,
* ${f}_{{u} {u}} - 60u_1 {f}_{{u_5}}$,
* $-2{f}_{{x}} + \ldots$,
* $2{f}_{{t}} + \ldots$.

From this, it is clear that the solution must depend linearly on $u_5$, $u_3$, $u_1$, and therefore
$$
f = A u_5 + B u_3 + C u_1 + D,
$$
where $A, B, C, D$ depend on $t, x, u, u_2$.

The next Gröbner basis yields the following equations:
* ${A}_{{u_2}}$,
* ${A}_{{u}}$,
* ${A}_{{x}}$,
* ${A}_{{t}}$,
* ${B}_{{u_2}}$,
* ${B}_{{u}} 10 {A}$,
* ${B}_{{x}}$,
* $2 {B}_{{t}} 2 + 3{D}_{{u}}$,
* ${C}_{{u_2}} -20 {A}$,
* ${C}_{{u}} 1 - 6 {B}$,
* $-2{C}_{{x}} + {D}_{{u}}$,
* ${C}_{{t}} + 3 u {D}_{{u}} + 6{D}$,
* ${D}_{{u_2}}$,
* ${D}_{{u} {u_2}}$,
* ${D}_{{u} {u}}$,
* ${D}_{{x}}$,
* ${D}_{{t}}$.

These equations are easily integrated:
$$
\begin{aligned}
A &= A_0, \\
B &= 10 A_0 u - 3 t D_1/2 + B_0, \\
C &= 20 A_0 u_2 + x D_1/2 + 30 A_0 u^2 - 18 t D_1/2 u + 6 B_0 u - 6 t D_0 + C_0, \\
D &= D_1 u + D_0 ,
\end{aligned}
$$
using the constants $A_0$, $B_0$, $D_0$, $D_1$, $C_0$.

Collecting terms corresponding to arbitrary constants, we obtain the following symmetries:
$$
\begin{cases}
30 u^{2} u_{1} + 10 u u_{3} + 20 u_{1} u_{2} + u_{5},\\
6 u u_{1} + u_{3},\\
- 6 t u_{1} + 1,\\
- 9 t u u_{1} - \frac{3 t u_{3}}{2} + u + \frac{u_{1} x}{2},\\
u_{1}.
\end{cases}
$$

Considering that $u_3 = -u_t - 6u u_1$, the second and fourth equations can be rewritten in the form of classical symmetries:
$$
\begin{cases}
30 u^{2} u_{1} + 10 u u_{3} + 20 u_{1} u_{2} + u_{5},\\
-u_t,\\
- 6 t u_{1} + 1,\\
\frac{3 t u_{t}}{2} + u + \frac{u_{1} x}{2},\\
u_{1}.
\end{cases}
$$


## Difference Schemes

In the works [Gerdt:2006:GBG, Blinkov:2006:GRS, Blinkov:2017], an algorithmic approach based on computer algebra is applied to construct new finite-difference schemes for equations of various types. All calculations presented in these and other works can be reproduced in PyGInv.

As an example, consider the construction of compact difference schemes [Gulin] using fourth-order accuracy schemes:

$$
\frac{1}{6}f'_{i+1} + \frac{2}{3}f'_{i} + \frac{1}{6}f'_{i-1} = \frac{f_{i+1}-f_{i-1}}{2h}, \tag{*}
$$
$$
\frac{1}{12}f''_{i+1} + \frac{5}{6}f''_{i} + \frac{1}{12}f''_{i-1} = \frac{f_{i+1}-2f_{i}+f_{i-1}}{h^2}.
$$

Let us construct a difference scheme for a boundary value problem of a second-order ODE with a small parameter $\varepsilon$:

$$
\varepsilon f''_{i} + \left(1 + \varepsilon^{2}\right)f'_{i} +
 \left(1 - \varepsilon^{2}\right)f_{i}=0.
$$

Line 4 shows the shift operator `T`.

```
Monom.cmp = Monom.POTdeglex
var = 'j'.split(', ')
fun = 'fxx, fx, f'.split(', ')
T = PolySchem.T
var, fun = PolySchem.init(var, fun)
j = var[0]
fxx, fx, f = fun
BL = Forest(Janet)
res = ginvMin((\
(T(fx,j,2)/6 + 2*T(fx,j)/3 + T(fx)/6)\
  - (T(f,j,2) - T(f))/(2*h),\
(T(fxx,j,2)/12+5*T(fxx,j)/6+T(fxx)/12)\
  - (T(f,j,2) - 2*T(f,j) + T(f))/h**2,\
eps*T(fxx)+eps0*T(fx)+eps1*T(f)), BL)
```

The constructed basis immediately yields a difference scheme for the left boundary:

$$
\begin{aligned}
6 (1 + {\varepsilon^2}) {f'}_{{j}} &+ (1 + {\varepsilon^2})\frac{ 3 {f}_{{j} + 1} - 30 {f}_{{j} + 2} - 3 {f}_{{j} + 3} + 30 {f}_{{j}}}{h} + {} \\
&+ (1 - {\varepsilon^2}) \left(- 41 {f}_{{j} + 1} - 14 {f}_{{j} + 2} - {f}_{{j} + 3} - 4 {f}_{{j}}\right) + {} \\
&+ \varepsilon \frac{84 {f}_{{j} + 1} - 24 {f}_{{j} + 2} - 12 {f}_{{j} + 3} - 48 {f}_{{j}}}{h^{2}}=0,
\end{aligned}
$$

Applying continuation (shifting) along `j`, we obtain a scheme for interior points:

$$
\begin{aligned}
(1 + {\varepsilon^2}) {f'}_{{j} + 1} &+ 3(1 + {\varepsilon^2})\frac{ {f}_{{j} + 2} - {f}_{{j}}}{h} + {} \\
&+ (1 - {\varepsilon^2}) \left({f}_{{j} + 2} + 10 {f}_{{j} + 1} + {u}_{{j}}\right) + {} \\
&+ 12\varepsilon\frac{ {f}_{{j} + 2} - 2{f}_{{j} + 1}  + {f}_{{j}}}{h^{2}}=0,
\end{aligned}
$$

and for the right point of the computational interval:

$$
\begin{aligned}
6 (1 + {\varepsilon^2}) {f'}_{{j} + 3} &+ (1 + {\varepsilon^2})\frac{ 30 {f}_{{j} + 1} - 3 {f}_{{j} + 2} - 30 {f}_{{j} + 3} + 3 {f}_{{j}}}{h} + {} \\
&+ (1 - {\varepsilon^2}) \left(- 14 {f}_{{j} + 1} - 41 {f}_{{j} + 2} - 4 {f}_{{j} + 3} - {f}_{{j}}\right) + {} \\
&+ \varepsilon\frac{ - 24 {f}_{{j} + 1} + 84 {f}_{{j} + 2} - 48 {f}_{{j} + 3} - 12 {f}_{{j}}}{h^{2}}=0.
\end{aligned}
$$

Adding the relation (*) to the obtained difference schemes yields a closed system of equations. Numerical calculations showed good agreement with the exact solution for small values of $\varepsilon$.

The following code allows constructing a compact difference scheme for the Poisson equation.

```
var = 'j, k'.split(', ')
fun = 'ux, uy, u, f'.split(', ')
var, fun = PolySchem.init(var, fun)
T = PolySchem.T
j, k = var
ux, uy, u, f = fun
laplace = Forest(Janet)
res = ginvMin((
(T(uxx,j,2)/12+5*T(uxx,j)/6+T(uxx)/12)*\
  h**2 - (T(u,j,2) - 2*T(u,j) + T(u)),\
(T(uyy,k,2)/12+5*T(uyy,k)/6+T(uyy)/12)*\
  h**2 - (T(u,k,2) - 2*T(u,k) + T(u)),\
T(uxx) + T(uyy) - T(f)), \
laplace, level=1)
```

As a result, we obtain a compact scheme of 4th order accuracy:

$$
(- 20 {u}_{{j} + 1\, {k} + 1} + 4 {u}_{{j} + 1\, {k} + 2} + 4 {u}_{{j} + 1\, {k}} + 4 {u}_{{j} + 2\, {k} + 1} + {u}_{{j} + 2\, {k} + 2} + {u}_{{j} + 2\, {k}} + 4 {u}_{{j}\, {k} + 1} + {u}_{{j}\, {k} + 2} + {u}_{{j}\, {k}})/h^{2}
- (100 {f}_{{j} + 1\, {k} + 1} + 10 {f}_{{j} + 1\, {k} + 2} + 10 {f}_{{j} + 1\, {k}} + 10 {f}_{{j} + 2\, {k} + 1} + {f}_{{j} + 2\, {k} + 2} + {f}_{{j} + 2\, {k}} + 10 {f}_{{j}\, {k} + 1} + {f}_{{j}\, {k} + 2} + {f}_{{j}\, {k}})/{24} =0.
$$

## Conclusion

Currently, some modules of the PyGInv package are used to construct the first differential approximation (FDA) by building Gröbner bases for series. This has made it possible to effectively construct FDA for Runge-Kutta methods, multistep Adams-Bashforth and Adams-Moulton methods, as well as difference schemes. In the future, it is planned to integrate FDA calculation into PyGInv as a separate module.

## References

[Gordan:1900:IFB] **Gordan P.** Journal de Mathématiques Puves et Appliqués // Journal de Mathématiques Puves et Appliqués. 1900. N. 1. P. 141–156.

[Macaulay:1927:SPE] **Macaulay F.S.** Some properties of enumeration in the theory of modular systems // Proc. London Math. Soc. 1927. V. 26. P. 531–555. DOI: 10.1112/plms/s2-26.1.531

[Croebner:1939:UDE] **Gröbner W.** Über die Eliminationstheorie // Monatsh. der Math. 1939. V. 54. P. 71–78.

[BUCHBERGER2006475] **Buchberger B.** Bruno Buchberger’s PhD thesis 1965: An algorithm for finding the basis elements of the residue class ring of a zero dimensional polynomial ideal // Journal of Symbolic Computation. 2006. V. 41. N. 3. P. 475–511. DOI: 10.1016/j.jsc.2005.09.007

[Cartan:1899:SCE] **Cartan E.** Sur certaines expressions différentielles et le problème de Pfaff // Annales scientifiques de l'École Normale Supérieure Sér. 3. 1899. N. 16. P. 239–332.

[Riquier:1910:SED] **Riquier C.** Les Systèmes d'Equations aux Dérivées Partielles. Gauthier-Villars, Paris. 1910.

[Janet:1920:SED] **Janet M.** Systèmes d’équations aux dérivées partielles // Journals de mathématiques, 8e série. 1920. N. 3. P. 65–151.

[Zharkov:1996:IAI] **Zharkov A.Yu., Blinkov Yu.A.** Involution approach to investigating polynomial systems // Mathematics and Computers in Simulation. 1996. V. 42. N. 4-6. P. 323–332. DOI: 10.1016/S0378-4754(96)00006-7

[Gerdt:1998:IBP] **Gerdt V.P, Blinkov Yu.A.** Involutive Bases of Polynomial Ideals // Mathematics and Computers in Simulation. 1998. V. 45. N. 4-6. P. 519–542. DOI: 10.1016/S0378-4754(97)00127-4

[Gerdt:1998:MIB] **Gerdt V.P, Blinkov Yu.A.** Minimal Involutive Bases // Mathematics and Computers in Simulation. 1998. V. 45. N. 4-6. P. 543–560. DOI: 10.1016/S0378-4754(97)00128-6

[Blinkov:2008:SSK] **Blinkov Yu.A., Gerdt V.P.** Specialized computer algebra system GINV // Programming and Computer Software. 2008. Vol. 34. No. 2. P. 67–80. (Translation of Russian original)

[Blinkov:2024:ROP] **Blinkov Yu.A., Salpagaro
