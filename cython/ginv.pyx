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
  cdef Timer *ptr
  def __cinit__(self):
    self.ptr = new Timer()
  def __dealloc__(self):
    del self.ptr

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

cdef extern from "allocator.h" namespace "GInv":
  cdef cppclass Allocator:
    Allocator()

    @staticmethod
    size_t maxMemory()
    @staticmethod
    size_t currMemory()
    @staticmethod
    const Timer& timer()

    size_t alloc() const
    size_t size() const

cdef Allocator allocator

def allocate():
  return allocator.alloc()

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

cdef extern from "monom.h" namespace "GInv":
  cdef cppclass Monom:
    void rand_init(int size, int deg1, int deg2)
    Monom next(Allocator* allocator)
    
    #Monom(Monom&& a)
    Monom(Allocator* allocator, int size, int pos)
    Monom(Allocator* allocator, Variable v, int size, int pos)
    #Monom(Allocator* allocator, const Monom& a)
    #Monom(Allocator* allocator, Variable v, const Monom& a)
    #Monom(Allocator* allocator, const Monom& a, const Monom& b, bool div=false);
    #Monom(Allocator* allocator, const Monom& a, int n)
    #~Monom()

    bool isZero() const
    int size() const
    int pos() const
    int degree() const
    int get "operator[]"(int i) const const

    void setPos(int pos)
    bool divisiable(const Monom& a)

    int lex(const Monom& a)
    int deglex(const Monom& a)
    int alex(const Monom& a)


cdef class monom:
  cdef Monom *ptr
  #def __cinit__(self):
    #self.ptr = nullptr

  def __len__(self):
    return self.ptr.size()
  def pos(self):
    return self.ptr.pos()
  def degree(self):
    return self.ptr.degree()
  def __getitem__(self, k):
    assert 0 <= k < self.ptr.size()
    return self.ptr.get(k)
  def __str__(self):
    return "[%d;%s]" % (self.ptr.pos(), \
      " ".join(self.ptr.get(k) for k in range(self.ptr.size())))
  def __repr__(self):
    return "[%d %d;%s]" % (self.ptr.pos(), self.ptr.degree(), \
      " ".join(self.ptr.get(k) for k in range(self.ptr.size())))

  def __mul__(self, other):
    r = Monom(self.ptr, self.ptr)
    r.mult(other.ptr)
    return r
  def __pow__(self, other, n):
    r = Monom(self.ptr, self.ptr.allocator())
    r.pow(other.ptr, n)
    return r


  #__imul__
  #__ipow__

  def setPos(self, p):
    assert self.ptr.pos() == -1
    self.ptr.setPos(p)

  #Monom operator*(const Monom& a)

  def lex(self):
    return self.ptr.lex()
  def deglex(self):
    return self.ptr.deglex()
  def alex(self):
    return self.ptr.alex()
