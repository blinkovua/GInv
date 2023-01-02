# distutils: language = c++

from libcpp cimport bool

cdef extern from "timer.h" namespace "GInv":
  cdef cppclass Timer:
    Timer()
    void start()
    void cont()
    void stop()

    double userTime() const
    double sysTime() const
    double realTime() const

cdef class timer:
  cdef Timer ptr[1]

  def start(self):
    self.ptr.start()
  def cont(self):
    self.ptr.cont()
  def stop(self):
    self.ptr.stop()

  def userTime(self):
    return self.ptr.userTime()
  def sysTime(self):
    return self.ptr.sysTime()
  def realTime(self):
    return self.ptr.realTime()

  def __str__(self):
    return "userTime: %.2f, sysTime: %.2f, realTime: %.2f" % (\
        self.ptr.userTime(),\
        self.ptr.sysTime(),\
        self.ptr.realTime(),\
      )

cdef extern from "allocator.h" namespace "GInv":
  cdef cppclass Allocator:
    Allocator()

    size_t alloc() const
    size_t size() const

  cdef size_t allocator_maxMemory "GInv::Allocator::maxMemory"()
  cdef size_t allocator_currMemory "GInv::Allocator::currMemory"()
  cdef void set_limit_memory "GInv::Allocator::setLimitMemory"(size_t limitMemory)
  cdef const Timer& allocator_timer "GInv::Allocator::timer"()

cdef class const_timer_GC:
  cdef const Timer* ptr
  def __cinit__(self):
    self.ptr = &allocator_timer()

  def userTime(self):
    return allocator_timer().userTime()
  def sysTime(self):
    return allocator_timer().sysTime()
  def realTime(self):
    return allocator_timer().realTime()

  def __str__(self):
    return "GC userTime: %.2f, sysTime: %.2f, realTime: %.2f" % (\
        self.ptr.userTime(),\
        self.ptr.sysTime(),\
        self.ptr.realTime(),\
      )

cdef class const_timer:
  cdef const Timer* ptr
  def __cinit__(self):
    self.ptr = NULL

  def userTime(self):
    return allocator_timer().userTime()
  def sysTime(self):
    return allocator_timer().sysTime()
  def realTime(self):
    return allocator_timer().realTime()

  def __str__(self):
    return "userTime: %.2f, sysTime: %.2f, realTime: %.2f" % (\
        self.ptr.userTime(),\
        self.ptr.sysTime(),\
        self.ptr.realTime(),\
      )

cdef class allocator:
  cdef Allocator allocator[1]

  @staticmethod
  def maxMemory():
    return allocator_maxMemory()
  @staticmethod
  def currMemory():
    return allocator_currMemory()
  @staticmethod
  def set_limit_memory(limitMemory):
    return set_limit_memory(limitMemory)
  @staticmethod
  def GC_timer():
    return const_timer_GC()




  def alloc(self):
    return self.allocator.alloc()
  def size(self):
    return self.allocator.size()

  def __str__(self):
    return "alloc: %d, size: %d" % (\
        self.allocator.alloc(),\
        self.allocator.size(),\
      )

cdef extern from "randpermutation.h" namespace "GInv":
  cdef cppclass RandPermutation_mt19937:
    RandPermutation_mt19937(int n)

    int size() const
    int get "operator[]"(int i) const
    void next()

cdef class randpermutation:
  cdef RandPermutation_mt19937 *ptr
  def __cinit__(self, int n):
    self.ptr = new RandPermutation_mt19937(n)
  def __dealloc__(self):
    del self.ptr

  def __len__(self):
    return self.ptr.size()
  def __getitem__(self, k):
    assert 0 <= k < self.ptr.size()
    return self.ptr.get(k)

  def next(self):
    self.ptr.next()

#cdef extern from "list.h" namespace "GInv":
  #cdef cppclass List[T]:
    #cppclass ConstIterator:
      #bint not_empty "operator bool"()
      #void next "operator++"()
      #T& data() const

    #bint not_empty "operator bool"()
    #size_t length() const

    #ConstIterator begin() const
    #T head() const
    #void push(T data)
    #T pop()

cdef extern from "integer.h" namespace "GInv":
  cdef cppclass Integer:
    Integer(Allocator* allocator)
    Integer(Allocator* allocator, const Integer &a)
    Integer(Allocator* allocator, const char *str)

    const char* get_str(int base) const

    bool isZero() const
    bool isOne() const
    bool isAbsOne() const
    int sgn() const
    int norm() const

cdef class const_integer:
  cdef const Integer *ptr
  def __cinit__(self, ):
    self.ptr = NULL

  def __nonzero__(self):
    assert self.ptr
    return not self.ptr.isZero()

  def isZero(self):
    assert self.ptr
    return self.ptr.isZero()

  def isOne(self):
    assert self.ptr
    return self.ptr.isOne()

  def isAbsOne(self):
    assert self.ptr
    return self.ptr.isAbsOne()

  def sgn(self):
    assert self.ptr
    return self.ptr.sgn()

  def norm(self):
    assert self.ptr
    return self.ptr.norm()

  def __str__(self):
    assert self.ptr
    return self.ptr.get_str(10).decode('UTF-8')
  def __repr__(self):
    assert self.ptr
    if self.ptr.sgn() == -1:
      return "-0x" + self.ptr.get_str(16).decode('UTF-8')[1:]
    else:
      return "0x" + self.ptr.get_str(16).decode('UTF-8')

ctypedef int Variable
cdef extern from "monom.h" namespace "GInv":
  cdef cppclass Monom:
    Monom(Monom&& a)
    Monom(Allocator* allocator, int size, int pos)
    Monom(Allocator* allocator, Variable v, int size, int pos)
    Monom(Allocator* allocator, const Monom& a)
    Monom(Allocator* allocator, Variable v, const Monom& a)
    Monom(Allocator* allocator, const Monom& a, const Monom& b)
    Monom(Allocator* allocator, const Monom& a, int n)

    void permutation(const Monom &a, RandPermutation_mt19937 &p)
    void setPos(int pos)

    bool isZero() const
    int size() const
    int pos() const
    int degree() const
    int get "operator[]"(int i) const

    bool divisiable(const Monom& a)

    void mult(const Monom& a);
    void div(const Monom& a);

    int gcd(const Monom& a) const;
    int lcm(const Monom& a) const;

    int lex(const Monom& a)
    int deglex(const Monom& a)
    int alex(const Monom& a)

  cdef void monom_rand_init "GInv::Monom::rand_init"(int size, int deg1, int deg2)
  cdef Monom* monom_next "GInv::Monom::rand_next"(Allocator* allocator)


cdef class const_monom:
  cdef const Monom *ptr
  def __cinit__(self):
    self.ptr = NULL

  def __nonzero__(self):
    assert self.ptr
    return self.ptr.degree() > 0
  def __len__(self):
    assert self.ptr
    return self.ptr.size()
  def pos(self):
    assert self.ptr
    return self.ptr.pos()
  def degree(self):
    assert self.ptr
    return self.ptr.degree()
  def __getitem__(self, k):
    assert self.ptr
    return self.ptr.get(k)
  def __iter__(self):
    assert self.ptr
    for 0 <= i < self.ptr.size():
      yield self.ptr.get(i)
  def __str__(self):
    assert self.ptr
    if self.ptr.pos() < 0:
      return "[%s]" % " ".join(str(v) for v in self)
    else:
      return "[%d:%s]" % (self.ptr.pos(), " ".join(str(v) for v in self))
  def __repr__(self):
    assert self.ptr
    return "[%d %d:%s]" % (self.ptr.pos(), self.ptr.degree(), " ".join(str(v) for v in self))

  def divisiable(const_monom self, const_monom other):
    assert self.ptr and other.ptr
    assert self.ptr.size() == other.ptr.size()
    return self.ptr.divisiable(other.ptr[0])

  def permutation(const_monom self, randpermutation p):
    assert self.ptr
    assert len(self) == len(p)
    r = monom()
    r.ptr = new Monom(r.allocator, self.ptr[0])
    (<Monom*>r.ptr)[0].permutation(self.ptr[0], p.ptr[0])
    return r

  def __mul__(const_monom self, const_monom other):
    assert self.ptr and other.ptr
    assert self.ptr.size() == other.ptr.size()
    assert self.pos() == -1 or other.pos() == -1
    r = monom()
    r.ptr = new Monom(r.allocator, self.ptr[0], other.ptr[0])
    return r
  def __pow__(const_monom self, int other, modulo):
    assert self.ptr != NULL and self.pos() == -1
    assert other >= 0
    r = monom()
    r.ptr = new Monom(r.allocator, self.ptr[0], other)
    return r
  def __truediv__(const_monom self, monom other):
    assert self.divisiable(other)
    r = monom()
    r.ptr = new Monom(r.allocator, self.ptr[0])
    (<Monom*>r.ptr)[0].div(other.ptr[0])
    return r

  def lex(const_monom self, const_monom other):
    assert self.ptr and other.ptr
    assert self.ptr.size() == other.ptr.size()
    return self.ptr.lex(other.ptr[0])
  def deglex(const_monom self, const_monom other):
    assert self.ptr and other.ptr
    assert self.ptr.size() == other.ptr.size()
    return self.ptr.deglex(other.ptr[0])
  def alex(const_monom self, const_monom other):
    assert self.ptr and other.ptr
    assert self.ptr.size() == other.ptr.size()
    return self.ptr.alex(other.ptr[0])

cdef class monom(const_monom):
  cdef Allocator allocator[1]
  def __dealloc__(self):
    del self.ptr

  @staticmethod
  def zero(int size, int pos):
    assert size > 1 and pos >= -1
    r = monom()
    r.ptr = new Monom(r.allocator, size, pos)
    return r
  @staticmethod
  def variable(Variable v, int size, int pos):
    assert size > 1 and pos >= -1
    r = monom()
    r.ptr = new Monom(r.allocator, v, size, pos)
    return r

  @staticmethod
  def rand_init(int size, int deg1, int deg2):
    assert size > 1 and 0 < deg1 < deg2
    monom_rand_init(size, deg1, deg2)
  @staticmethod
  def next():
    r = monom()
    r.ptr = monom_next(r.allocator)
    return r

  def setPos(self, int pos):
    assert self.ptr
    assert pos >= -1
    (<Monom*>self.ptr).setPos(pos)
    return self

#ctypedef int Order
cdef extern from "poly_int.h" namespace "GInv":
  cdef cppclass PolyIntConstIterator "GInv::PolyInt::ConstIterator":
    const Integer& coeff() const
    const Monom& monom() const
    bool isNext "operator bool"() const
    void next "operator++"()

  cdef cppclass PolyInt:
    PolyInt(PolyInt&& a)
    PolyInt(Allocator* allocator, int order)
    PolyInt(Allocator* allocator, int k, const Monom& a)
    PolyInt(Allocator* allocator, const PolyInt& a)

    PolyIntConstIterator begin() const
    bool isZero() const
    int length() const
    int degree() const
    int norm() const
    const Monom& lm() const
    const Integer& lc() const
    bool comparable(const PolyInt& a) const
    int order() const
    int size() const

    void minus()
    void addI "add"(const char *hex)
    void add(const PolyInt &a)
    void subI "sub"(const char *hex)
    void sub(const PolyInt &a)
    void multI "mult"(const char *hex)
    void mult(const PolyInt &a)
    void pow(int deg)

    void reduction(const PolyInt& a)
    bool isPp() const
    void pp()

cdef class const_poly_int:
  cdef const PolyInt *ptr
  def __cinit__(self, ):
    self.ptr = NULL

  def __nonzero__(self):
    assert self.ptr
    return self.ptr.length() > 0
  def __len__(self):
    assert self.ptr
    return self.ptr.length()
  def __iter__(self):
    assert self.ptr
    cdef PolyIntConstIterator i
    i = self.ptr.begin()
    while i.isNext():
      m, c = const_monom(), const_integer()
      m.ptr, c.ptr = &i.monom(), &i.coeff()
      i.next()
      yield (m, c)
  def __str__(self):
    assert self.ptr
    if self.ptr.isZero():
      return "0"
    else:
      return "%s*%s + ..." % (str(self.lm()), str(self.lc()))
  def __repr__(self):
    assert self.ptr
    if self.ptr.isZero():
      return "0"
    else:
      return "%s*%s + ..." % (repr(self.lm()), repr(self.lc()))

  def isZero(self):
    assert self.ptr
    return self.ptr.isZero()
  def degree(self):
    assert self.ptr
    return self.ptr.degree()
  def norm(self):
    assert self.ptr
    return self.ptr.norm()

  def lm(self):
    assert self.ptr
    assert self
    r = const_monom()
    r.ptr = &self.ptr.lm()
    return r
  def lc(self):
    assert self.ptr
    assert self
    r = const_integer()
    r.ptr = &self.ptr.lc()
    return r
  def comparable(self, const_poly_int other):
    return self.ptr.comparable(other.ptr[0])

  def order(self):
    cdef int c
    c = self.ptr.order()
    if c & 0x10:
      r = "TOP"
    else:
      r = "POT"
    if c & 0x01:
      r = (r, "lex")
    elif c & 0x02:
      r = (r, "deglex")
    else:
      r = (r, "alex")
    return r
  def size(self):
    return self.ptr.size()

  def __neg__(self):
    assert self.ptr
    r = poly_int()
    r.ptr = new PolyInt(r.allocator, self.ptr[0])
    (<PolyInt*>r.ptr).minus()
    return r

  def add(const_poly_int self, other):
    assert self.ptr
    r = poly_int()
    r.ptr = new PolyInt(r.allocator, self.ptr[0])
    if isinstance(other, int):
      assert self
      (<PolyInt*>r.ptr).addI(hex(other).encode("utf-8"))
    elif isinstance(other, const_poly_int):
      assert (<const_poly_int>other).ptr and self.ptr.comparable((<const_poly_int>other).ptr[0])
      (<PolyInt*>r.ptr).add((<const_poly_int>other).ptr[0])
    else:
      return NotImplemented
    return r

  def __add__(self, other):
    if isinstance(other, const_poly_int):
      return other.add(self)
    else:
      return self.add(other)

  def sub(const_poly_int self, other):
    assert self.ptr
    r = poly_int()
    r.ptr = new PolyInt(r.allocator, self.ptr[0])
    if isinstance(other, int):
      assert self
      (<PolyInt*>r.ptr).subI(hex(other).encode("utf-8"))
    elif isinstance(other, const_poly_int):
      assert (<const_poly_int>other).ptr and self.ptr.comparable((<const_poly_int>other).ptr[0])
      (<PolyInt*>r.ptr).sub((<const_poly_int>other).ptr[0])
    else:
      return NotImplemented
    return r

  def __sub__(self, other):
    if isinstance(other, const_poly_int):
      return -other.sub(self)
    else:
      return self.add(other)

  def mul(const_poly_int self, other): #TODO
    assert self.ptr
    r = poly_int()
    r.ptr = new PolyInt(r.allocator, self.ptr[0])
    if isinstance(other, int):
      assert self
      (<PolyInt*>r.ptr).multI(hex(other).encode("utf-8"))
    elif isinstance(other, const_poly_int):
      assert (<const_poly_int>other).ptr and self.ptr.comparable((<const_poly_int>other).ptr[0])
      (<PolyInt*>r.ptr).mult((<const_poly_int>other).ptr[0])
    else:
      return NotImplemented
    return r

  def __mul__(self, other):
    if isinstance(other, const_poly_int):
      return other.mul(self)
    else:
      return self.mul(other)

  def __pow__(const_poly_int self, int other, modulo):
    assert other >= 0
    r = poly_int()
    r.ptr = new PolyInt(r.allocator, self.ptr[0])
    (<PolyInt *>r.ptr).pow(other)
    return r

  def isPp(const_poly_int self):
    assert self.ptr
    return self.ptr.isPp()

cdef class poly_int(const_poly_int):
  cdef Allocator allocator[1]
  def __dealloc__(self):
    del self.ptr

  @staticmethod
  def to_monom(pos, order, const_monom a):
    assert pos in ("TOP", "POT")
    assert order in ("lex", "deglex", "alex")
    assert a.ptr
    cdef int k
    if pos  == "TOP":
      k = 0x10
    else:
      k = 0x20
    if order == "lex":
      k |= 0x01
    elif order == "deglex":
      k |= 0x02
    else:
      k |= 0x04
    r = poly_int()
    r.ptr = new PolyInt(r.allocator, k, a.ptr[0])
    return r

  def reduction(poly_int self, const_poly_int other):
    assert self.ptr and other.ptr
    assert len(self) and len(other)
    assert self.ptr.comparable((<const_poly_int>other).ptr[0])
    assert self.ptr.lm().divisiable(other.ptr.lm())
    (<PolyInt *>self.ptr).reduction(other.ptr[0])
    return self

  def pp(poly_int self):
    assert self.ptr
    assert not self.ptr.isPp()
    (<PolyInt *>self.ptr).pp()
    return self

cdef extern from "poly_int.h" namespace "GInv":
  cdef cppclass WrapPolyInt:
    const Monom& lm() const
    const Monom& ansector() const
    bool isGB() const

    bool NM(int var) const
    bool build(int var) const
    const PolyInt& poly() const

cdef class wrap_poly_int:
  cdef const WrapPolyInt *ptr
  def __cinit__(self, ):
    self.ptr = NULL

  def lm(self):
    r = const_monom()
    r.ptr = &self.ptr.lm()
    return r
  def ansector(self):
    r = const_monom()
    r.ptr = &self.ptr.ansector()
    return r
  def isGB(self):
    return self.ptr.isGB()

  def NM(self, int var):
    return self.ptr.NM(var)
  def build(self, int var):
    return self.ptr.build(var)
  def poly(self):
    r = const_poly_int()
    r.ptr = &self.ptr.poly()
    return r

  def __str__(self):
    return "%s%s[%s]" % (self.lm(), self.ansector(),\
        "".join("*" if self.ptr.NM(var) else " " for 0 <= var < self.ptr.lm().size()))

cdef extern from "hilbertpoly.h" namespace "GInv":
  cdef cppclass HilbertPoly:
    HilbertPoly()
    int dim() const
    bool isZero(int k)
    const char* get "operator[]"(int k) const;

cdef extern from "janet.h" namespace "GInv":
  cdef cppclass Janet:
     int size() const
     void buildHP(HilbertPoly& hp) const
     void draw(const char* format, const char* filename) const

cdef class const_janet:
  cdef const Janet *ptr
  def __cinit__(self, ):
    self.ptr = NULL

  def size(self):
    assert self.ptr
    return self.ptr.size()
  def HilbertPoly(self):
    cdef HilbertPoly hp
    cdef int i
    assert self.ptr
    self.ptr.buildHP(hp)
    r = []
    for i in range(hp.dim()+1):
      r.append(hp.get(i).decode("utf-8"))
    return r
  def draw(self, format, filename):
    assert self.ptr
    self.ptr.draw(format.encode("utf-8"), filename.encode("utf-8"))

cdef extern from "poly_int.h" namespace "GInv":
  cdef cppclass GCListWrapPolyIntConstIterator "GInv::GCListWrapPolyInt::ConstIterator":
    WrapPolyInt* data() const
    bool isNext "operator bool"() const
    void next "operator++"()

  cdef cppclass JanetPolyInt:
    bool comparable(const PolyInt& a) const
    void push(const PolyInt& a)

    void build()

    bool isOne()

    int size() const
    GCListWrapPolyIntConstIterator begin() const
    WrapPolyInt* find(const Monom &m) const
    void nf(PolyInt& a) const
    int maxPos() const
    const Janet& janet(int pos)
    void buildHP(HilbertPoly& hp) const

    const Timer& timer() const
    int reduction() const
    int maxEqualLm() const
    int critI() const
    int critII() const
    int isZeroNf() const


cdef class janet_poly_int:
  cdef JanetPolyInt *ptr
  def __cinit__(self, ):
    self.ptr = new JanetPolyInt()
  def __dealloc__(self):
    del self.ptr

  def __nonzero__(self):
    assert self.ptr
    return self.ptr.size() > 0
  def __len__(self):
    assert self.ptr
    return self.ptr.size()
  def __iter__(self):
    assert self.ptr
    cdef GCListWrapPolyIntConstIterator i
    i = self.ptr.begin()
    while i.isNext():
      r = wrap_poly_int()
      r.ptr = i.data()
      i.next()
      yield r

  def comparable(self, const_poly_int other):
    return self.ptr.comparable(other.ptr[0])

  def push(self, const_poly_int other):
    assert self.ptr.comparable(other.ptr[0])
    self.ptr.push(other.ptr[0])
    return self

  def build(self):
    self.ptr.build()

  def isOne(self):
    return self.ptr.isOne()

  def find(self, const_monom m):
    cdef WrapPolyInt* w
    w = self.ptr.find(m.ptr[0])
    if w == NULL:
      return None
    else:
      r = wrap_poly_int()
      r.ptr = w
      return r

  def nf(self, const_poly_int p):
    self.ptr.comparable(p.ptr[0])
    self.ptr.nf((<PolyInt *>p.ptr)[0])
    return p

  def maxPos(self):
    return self.ptr.maxPos()
  def janet(self, pos):
    assert -1 <= pos <= self.ptr.maxPos()
    r = const_janet()
    r.ptr = &self.ptr.janet(pos)
    return r

  def HilbertPoly(self):
    assert not self.ptr.isOne()
    assert self.ptr.size() > 0
    cdef HilbertPoly hp
    cdef int i
    assert self.ptr
    self.ptr.buildHP(hp)
    r = []
    for i in range(hp.dim()+1):
      r.append(hp.get(i).decode("utf-8"))
    return r

  def timer(self):
    r = const_timer()
    r.ptr = &self.ptr.timer()
    return r
  def reduction(self):
    return self.ptr.reduction()
  def maxEqualLm(self):
    return self.ptr.maxEqualLm()
  def critI(self):
    return self.ptr.critI()
  def critII(self):
    return self.ptr.critII()
  def isZeroNf(self):
    return self.ptr.isZeroNf()

  #def janet(self, int pos):
    #assert -1 <= pos <= self.ptr.maxPos()
    #r = janet_poly_int()
    #r.ptr = &self.ptr.janet(pos)
    #return r

  #def lm(self):
    #r = const_monom()
    #r.ptr = &self.ptr.lm()
    #return r
  #def ansector(self):
    #r = const_monom()
    #r.ptr = &self.ptr.ansector()
    #return r
  #def isGB(self):
    #return self.ptr.isGB()

  #def NM(self, int var):
    #return self.ptr.NM(var)
  #def build(self, int var):
    #return self.ptr.build(var)
  #def poly(self):
    #r = poly_int()
    #r.ptr = &self.ptr.poly()
    #return r


#cdef extern from "basis.h" namespace "GInv":
  #cdef cppclass Basis:
    #Basis()

    #bool compare(const PolyInt& a)
    #void push(const PolyInt& a)

    #void build();

    #GCListWrapPolyIntConstIterator begin() const
    #const JanetPolyInt& janet() const

    #const Timer& timer() const
    #int reduction() const

#cdef class basis:
  #cdef Basis *ptr
  #def __cinit__(self):
    #self.ptr = new Basis()
  #def __dealloc__(self):
    #del self.ptr

  #def compare(self, const_poly_int a):
    #return self.ptr.compare(a.ptr[0])

#cdef extern from "mcompletion.h" namespace "GInv":
  #cdef cppclass MCompletion:
    #MCompletion(Allocator* allocator, int (*cmp)(const Wrap&, const Wrap&))

    #void push(const Monom& m)
    #void build();

    #Wrap* find(const Monom& m) const
    #const List[Wrap*] T() const
    #int length() const

  #cdef int mcompletion_lex "GInv::MCompletion::lex"(const Wrap& a, const Wrap& b)
  #cdef int mcompletion_deglex "GInv::MCompletion::deglex"(const Wrap& a, const Wrap& b)
  #cdef int mcompletion_alex "GInv::MCompletion::alex"(const Wrap& a, const Wrap& b)
  #cdef int mcompletion_lexGB "GInv::MCompletion::lexGB"(const Wrap& a, const Wrap& b)
  #cdef int mcompletion_deglexGB "GInv::MCompletion::deglexGB"(const Wrap& a, const Wrap& b)
  #cdef int mcompletion_alexGB "GInv::MCompletion::alexGB"(const Wrap& a, const Wrap& b)
  #cdef int mcompletion_GB "GInv::MCompletion::GB"(const Wrap& a, const Wrap& b)

#cdef class mcompletion:
  #cdef Allocator *allocator
  #cdef MCompletion *ptr
  #def __cinit__(self, compare):
    #self.allocator = new Allocator()
    #if compare == "janet":
      #self.ptr = new MCompletion(self.allocator, mcompletion_lex)
    #elif compare == "deglex":
      #self.ptr = new MCompletion(self.allocator, mcompletion_deglex)
    #elif compare == "alex":
      #self.ptr = new MCompletion(self.allocator, mcompletion_alex)
    #elif compare == "lexGB":
      #self.ptr = new MCompletion(self.allocator, mcompletion_lexGB)
    #elif compare == "deglexGB":
      #self.ptr = new MCompletion(self.allocator, mcompletion_deglexGB)
    #elif compare == "alexGB":
      #self.ptr = new MCompletion(self.allocator, mcompletion_alexGB)
    #elif compare == "GB":
      #self.ptr = new MCompletion(self.allocator, mcompletion_GB)
    #else:
      #raise NameError(compare + " not defined in constructor mcompletion()")
  #def __dealloc__(self):
    #del self.ptr
    #del self.allocator

  #def push(self, const_monom m):
    #self.ptr.push(m.ptr[0])

  #def build(self):
    #self.ptr.build()

  #def find(self, const_monom m):
    #r = wrap()
    #r.ptr = self.ptr.find(m.ptr[0])
    #return r

  #def __len__(self):
    #return self.ptr.length()
  #def __iter__(self):
    #i = self.ptr.T().begin()
    #while i.not_empty():
      #w = wrap()
      #w.ptr = i.data()
      #i.next()
      #yield w

#cdef extern from "janet.h" namespace "GInv":
  #cdef cppclass Janet:
    #cppclass ConstIterator:
      #void deg()
      #void var()
      #bint not_empty "operator bool"()
      #ConstIterator nextDeg() const
      #ConstIterator nextVar() const
      #Wrap* wrap() const
      #Variable degree() const

    #Janet(Allocator* allocator)
    #ConstIterator begin() const
    #int size() const

    #Wrap* find(const Monom &m)
    #void insert(Wrap *wrap)

    #void draw(const char* format, const char* filename) const

#cdef extern from "mcompletion.h" namespace "GInv":
  #cdef cppclass MJanet:
    #MJanet(Allocator* allocator)

    #void push(const Monom& m)
    #void build()

    #const Janet& tree() const

#cdef class mjanet:
  #cdef Allocator *allocator
  #cdef MJanet *ptr
  #def __cinit__(self):
    #self.allocator = new Allocator()
    #self.ptr = new MJanet(self.allocator)
  #def __dealloc__(self):
    #del self.ptr
    #del self.allocator

  #def push(self, const_monom m):
    #self.ptr.push(m.ptr[0])

  #def build(self):
    #self.ptr.build()

  #def find(self, const_monom m):
    #r = wrap()
    #r.ptr = self.ptr.tree().find(m.ptr[0])
    #return r
  #def __len__(self):
    #return self.ptr.tree().size()
  #def draw(self, format, filename):
    #self.ptr.tree().draw(format.encode('utf8'), filename.encode('utf8'))

#def lprint(a):
    #if isinstance(a, list):
        #for i in a:
            #yield from lprint(i)
    #else:
        #yield a
