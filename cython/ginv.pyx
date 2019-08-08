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
    return self.ptr.userTime()
  def sysTime(self):
    return self.ptr.sysTime()
  def realTime(self):
    return self.ptr.realTime()

  def __str__(self):
    return "GC userTime: %.2f, sysTime: %.2f, realTime: %.2f" % (\
        self.ptr.userTime(),\
        self.ptr.sysTime(),\
        self.ptr.realTime(),\
      )

cdef class allocator:
  cdef Allocator *ptr
  def __cinit__(self):
    self.ptr = new Allocator()
  def __dealloc__(self):
    del self.ptr

  @staticmethod
  def maxMemory():
    return allocator_maxMemory()
  @staticmethod
  def currMemory():
    return allocator_currMemory()
  @staticmethod
  def timer():
    return const_timer()

  def alloc(self):
    return self.ptr.alloc()
  def size(self):
    return self.ptr.size()

  def __str__(self):
    return "alloc: %d, size: %d" % (\
        self.ptr.alloc(),\
        self.ptr.size(),\
      )

static_allocator = allocator()
cdef Allocator *__allocator=(<allocator>static_allocator).ptr

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

cdef extern from "list.h" namespace "GInv":
  cdef cppclass List[T]:
    cppclass ConstIterator:
      bint not_empty "operator bool"()
      void next "operator++"()
      T& data() const

    bint not_empty "operator bool"()
    size_t length() const

    ConstIterator begin() const
    T head() const
    void push(T data)
    T pop()

ctypedef int Variable
cdef extern from "monom.h" namespace "GInv":
  cdef cppclass Monom:
    Monom(Monom&& a)
    Monom(Allocator* a, int size, int pos)
    Monom(Allocator* a, Variable v, int size, int pos)
    Monom(Allocator* a, const Monom& a, const Monom& b)
    Monom(Allocator* a, const Monom& a, const Monom& b, bool)
    Monom(Allocator* a, const Monom& a, int n)

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

cdef class const_monom:
  cdef const Monom *ptr
  def __cinit__(self):
    self.ptr = NULL

  def __nonzero__(self):
    return self.ptr != NULL
  def __len__(self):
    return self.ptr.size()
  def pos(self):
    return self.ptr.pos()
  def degree(self):
    return self.ptr.degree()
  def __getitem__(self, k):
    return self.ptr.get(k)
  def __iter__(self):
    for 0 <= i < self.ptr.size():
      yield self.ptr.get(i)
  def __str__(self):
    if self.ptr.pos() < 0:
      return "[%s]" % " ".join(str(v) for v in self)
    else:
      return "[%d:%s]" % (self.ptr.pos(), " ".join(str(v) for v in self))
  def __repr__(self):
    return "[%d %d:%s]" % (self.ptr.pos(), self.ptr.degree(), " ".join(str(v) for v in self))

  def divisiable(self, const_monom other):
    return self.ptr.divisiable(other.ptr[0])

  def __mul__(const_monom self, const_monom other):
    r = monom()
    r.ptr = new Monom(__allocator, self.ptr[0], other.ptr[0])
    return r
  def __pow__(const_monom self, int other, modulo):
    r = monom()
    r.ptr = new Monom(__allocator, self.ptr[0], other)
    return r
  def __div__(const_monom self, const_monom other):
    r = monom()
    r.ptr = new Monom(__allocator, self.ptr[0], other.ptr[0], 1)
    return r

  def lex(self, const_monom other):
    return self.ptr.lex(other.ptr[0])
  def deglex(self, const_monom other):
    return self.ptr.deglex(other.ptr[0])
  def alex(self, const_monom other):
    return self.ptr.alex(other.ptr[0])


cdef class monom(const_monom):
  def __cinit__(self):
    self.ptr = NULL
  def __dealloc__(self):
    del self.ptr

  @staticmethod
  def rand_init(int size, int deg1, int deg2):
    monom_rand_init(size, deg1, deg2)
  @staticmethod
  def next():
    r = monom()
    r.ptr = new Monom(monom_next(__allocator))
    return r

  @staticmethod
  def zero(int size, int pos):
    r = monom()
    r.ptr = new Monom(__allocator, size, pos)
    return r
  @staticmethod
  def variable(Variable v, int size, int pos):
    r = monom()
    r.ptr = new Monom(__allocator, v, size, pos)
    return r

cdef extern from "wrap.h" namespace "GInv":
  cdef cppclass Wrap:
    Wrap(Allocator* allocator, const Monom& m)
    Wrap(Allocator* allocator, Variable var, const Wrap* ansector)

    const Monom& lm() const
    const Monom& ansector()
    bool isGB()

    bool NM(int var) const
    bool NMd(int var) const
    bool build(int var) const

cdef class wrap:
  cdef const Wrap *ptr

  def __nonzero__(self):
    return self.ptr != NULL
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
  def NMd(self, int var):
    return self.ptr.NMd(var)
  def build(self, int var):
    return self.ptr.build(var)

  def __str__(self):
    return "%s%s[%s]" % (self.lm(), self.ansector(),\
        "".join("*" if self.ptr.NM(var) else " " for 0 <= var < self.ptr.lm().size()))

cdef extern from "mcompletion.h" namespace "GInv":
  cdef cppclass MCompletion:
    MCompletion(Allocator* allocator, int (*cmp)(const Wrap&, const Wrap&))

    void push(const Monom& m)
    void build();

    Wrap* find(const Monom& m) const
    const List[Wrap*] T() const
    int length() const

  cdef int mcompletion_lex "GInv::MCompletion::lex"(const Wrap& a, const Wrap& b)
  cdef int mcompletion_deglex "GInv::MCompletion::deglex"(const Wrap& a, const Wrap& b)
  cdef int mcompletion_alex "GInv::MCompletion::alex"(const Wrap& a, const Wrap& b)
  cdef int mcompletion_lexGB "GInv::MCompletion::lexGB"(const Wrap& a, const Wrap& b)
  cdef int mcompletion_deglexGB "GInv::MCompletion::deglexGB"(const Wrap& a, const Wrap& b)
  cdef int mcompletion_alexGB "GInv::MCompletion::alexGB"(const Wrap& a, const Wrap& b)
  cdef int mcompletion_GB "GInv::MCompletion::GB"(const Wrap& a, const Wrap& b)

cdef class mcompletion:
  cdef Allocator *allocator
  cdef MCompletion *ptr
  def __cinit__(self, compare):
    self.allocator = new Allocator()
    if compare == "janet":
      self.ptr = new MCompletion(self.allocator, mcompletion_lex)
    elif compare == "deglex":
      self.ptr = new MCompletion(self.allocator, mcompletion_deglex)
    elif compare == "alex":
      self.ptr = new MCompletion(self.allocator, mcompletion_alex)
    elif compare == "lexGB":
      self.ptr = new MCompletion(self.allocator, mcompletion_lexGB)
    elif compare == "deglexGB":
      self.ptr = new MCompletion(self.allocator, mcompletion_deglexGB)
    elif compare == "alexGB":
      self.ptr = new MCompletion(self.allocator, mcompletion_alexGB)
    elif compare == "GB":
      self.ptr = new MCompletion(self.allocator, mcompletion_GB)
    else:
      raise NameError(compare + " not defined in constructor mcompletion()")
  def __dealloc__(self):
    del self.ptr
    del self.allocator

  def push(self, const_monom m):
    self.ptr.push(m.ptr[0])

  def build(self):
    self.ptr.build()

  def find(self, const_monom m):
    r = wrap()
    r.ptr = self.ptr.find(m.ptr[0])
    return r

  def __len__(self):
    return self.ptr.length()
  def __iter__(self):
    i = self.ptr.T().begin()
    while i.not_empty():
      w = wrap()
      w.ptr = i.data()
      i.next()
      yield w

cdef extern from "janet.h" namespace "GInv":
  cdef cppclass Janet:
    cppclass ConstIterator:
      void deg()
      void var()
      bint not_empty "operator bool"()
      ConstIterator nextDeg() const
      ConstIterator nextVar() const
      Wrap* wrap() const
      Variable degree() const

    Janet(Allocator* allocator)
    ConstIterator begin() const
    int size() const

    Wrap* find(const Monom &m)
    void insert(Wrap *wrap)

    void draw(const char* format, const char* filename) const

cdef extern from "mcompletion.h" namespace "GInv":
  cdef cppclass MJanet:
    MJanet(Allocator* allocator)

    void push(const Monom& m)
    void build()

    const Janet& tree() const

cdef class mjanet:
  cdef Allocator *allocator
  cdef MJanet *ptr
  def __cinit__(self):
    self.allocator = new Allocator()
    self.ptr = new MJanet(self.allocator)
  def __dealloc__(self):
    del self.ptr
    del self.allocator

  def push(self, const_monom m):
    self.ptr.push(m.ptr[0])

  def build(self):
    self.ptr.build()

  def find(self, const_monom m):
    r = wrap()
    r.ptr = self.ptr.tree().find(m.ptr[0])
    return r
  def __len__(self):
    return self.ptr.tree().size()
  def draw(self, format, filename):
    self.ptr.tree().draw(format.encode('utf8'), filename.encode('utf8'))

#def lprint(a):
    #if isinstance(a, list):
        #for i in a:
            #yield from lprint(i)
    #else:
        #yield a
