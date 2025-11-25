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
