# distutils: language = c++

from libcpp cimport bool
#from libcpp.utility cimport move

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
  #def __cinit__(self):
    #self.ptr = new Timer()
  #def __dealloc__(self):
    #del self.ptr

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
  cdef const Timer& allocator_timer "GInv::Allocator::timer"()

cdef class const_timer:
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

cdef class allocator:
  cdef Allocator allocator[1]

  @staticmethod
  def maxMemory():
    return allocator_maxMemory()
  @staticmethod
  def currMemory():
    return allocator_currMemory()
  @staticmethod
  def GC_timer():
    return const_timer()

  def alloc(self):
    return self.allocator.alloc()
  def size(self):
    return self.allocator.size()

  def __str__(self):
    return "alloc: %d, size: %d" % (\
        self.allocator.alloc(),\
        self.allocator.size(),\
      )

#static_allocator = allocator()
#cdef Allocator *__allocator=(<allocator>static_allocator).ptr

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

ctypedef int Variable
cdef extern from "monom.h" namespace "GInv":
  cdef cppclass Monom:
    Monom(Monom&& a)
    Monom(Allocator* a, int size, int pos)
    Monom(Allocator* a, Variable v, int size, int pos)
    Monom(Allocator* allocator, const Monom& a, RandPermutation_mt19937 &p)
    Monom(Allocator* a, const Monom& a, const Monom& b)
    Monom(Allocator* a, const Monom& a, const Monom& b, bool)
    Monom(Allocator* a, const Monom& a, int n)
    
    void setPos(int pos)

    bool isZero() const
    int size() const
    int pos() const
    int degree() const
    int get "operator[]"(int i) const

    bool divisiable(const Monom& a)

    int lex(const Monom& a)
    int deglex(const Monom& a)
    int alex(const Monom& a)

    Monom* mult(const Monom& a)
    Monom* pow(int n) const
    Monom* div(const Monom& a)

  cdef void monom_rand_init "GInv::Monom::rand_init"(int size, int deg1, int deg2)
  cdef Monom monom_next "GInv::Monom::next"(Allocator* a)

cdef class monom:
  cdef Allocator allocator[1]
  cdef Monom *ptr
  def __cinit__(self):
    self.ptr = NULL
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
    r.ptr = new Monom(monom_next(r.allocator))
    return r

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

  def setPos(self, int pos):
    assert self.ptr
    assert pos >= -1
    self.ptr.setPos(pos)
    return self

  def divisiable(self, monom other):
    assert self.ptr and other.ptr
    assert self.ptr.size() == other.ptr.size()
    return self.ptr.divisiable(other.ptr[0])
  
  def permutation(monom self, randpermutation p):
    assert self.ptr
    assert len(self) == len(p)
    r = monom()
    r.ptr = new Monom(r.allocator, self.ptr[0], p.ptr[0])
    return r

  def __mul__(monom self, monom other):
    assert self.ptr and other.ptr
    assert self.ptr.size() == other.ptr.size()
    assert self.pos() == -1 or other.pos() == -1
    r = monom()
    r.ptr = new Monom(r.allocator, self.ptr[0], other.ptr[0])
    return r
  def __pow__(monom self, int other, modulo):
    assert self.ptr != NULL and self.pos() == -1
    assert other >= 0
    r = monom()
    r.ptr = new Monom(r.allocator, self.ptr[0], other)
    return r
  def __truediv__(monom self, monom other):
    assert self.divisiable(other)
    r = monom()
    r.ptr = new Monom(r.allocator, self.ptr[0], other.ptr[0], True)
    return r

  def lex(self, monom other):
    assert self.ptr and other.ptr
    assert self.ptr.size() == other.ptr.size()
    return self.ptr.lex(other.ptr[0])
  def deglex(self, monom other):
    assert self.ptr and other.ptr
    assert self.ptr.size() == other.ptr.size()
    return self.ptr.deglex(other.ptr[0])
  def alex(self, monom other):
    assert self.ptr and other.ptr
    assert self.ptr.size() == other.ptr.size()
    return self.ptr.alex(other.ptr[0])

cdef extern from "integer.h" namespace "GInv":
  cdef cppclass Integer:
    Integer(Allocator* allocator)
    Integer(Allocator* allocator, const Integer &a)
   
#ctypedef int Order   
cdef extern from "poly.h" namespace "GInv":
  cdef cppclass Poly:
    Poly(Poly&& a)
    Poly(Allocator* allocator, int k, Variable v, int size, int pos)
    Poly(Allocator* allocator, int k, const Monom& a)
    Poly(Allocator* allocator, const Poly& a)
   
    #operator bool() const
    bool isZero() const
    int length() const
    int degree() const
    int norm() const
    const Monom& lm() const
    const Integer& lc() const

    void minus()
    void add(const Poly &a)
    void sub(const Poly &a)
    void mult(const Poly &a)
    void pow(int deg)

    void reduction(const Poly& a)
    #void nf(Janet &a)
    #void nfTail(Janet &a)
    bool isPp() const
    void pp()

cdef class poly:
  cdef Allocator allocator[1]
  cdef Poly *ptr
  def __cinit__(self, pos, order, monom a):
    assert pos in ("TOP", "POT")
    assert order in ("lex", "deglex", "alex")
    assert a.ptr
    cdef int k
    if pos  == "TOP":
      k = 0x10
    else:
      k = 0x20
    if order == "lex":
      pass
    elif order == "deglex":
      k += 1
    else:
      k += 2
    self.ptr = new Poly(self.allocator, k, a.ptr[0])
  def __dealloc__(self):
    del self.ptr
    
  def __nonzero__(self):
    return self.ptr.length() > 0
  def __len__(self):
    return self.ptr.length()


  #def __pow__(poly self, int other, modulo):
    #assert other >= 0
    #r = poly()
    #r.ptr = new Monom(r.allocator, self.ptr[0], other)
    #return r  
#cdef class wrap:
  #cdef const Wrap *ptr

  #def __nonzero__(self):
    #return self.ptr != NULL
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
  #def NMd(self, int var):
    #return self.ptr.NMd(var)
  #def build(self, int var):
    #return self.ptr.build(var)

  #def __str__(self):
    #return "%s%s[%s]" % (self.lm(), self.ansector(),\
        #"".join("*" if self.ptr.NM(var) else " " for 0 <= var < self.ptr.lm().size()))

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
