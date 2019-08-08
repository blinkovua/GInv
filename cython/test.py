from pprint import pprint, pformat

from unittest import TestCase
#from nose.tools import assert_equal

from ginv import *

#import unittest

#class TestStringMethods(unittest.TestCase):

    #def setUp(self):
        #self.widget = Widget('The widget')

    #def test_upper(self):
        #self.assertEqual('foo'.upper(), 'FOO')

    #def test_isupper(self):
        #self.assertTrue('FOO'.isupper())
        #self.assertFalse('Foo'.isupper())

    #def test_split(self):
        #s = 'hello world'
        #self.assertEqual(s.split(), ['hello', 'world'])
        ## check that s.split fails when the separator is not a string
        #with self.assertRaises(TypeError):
            #s.split(2)

#if __name__ == '__main__':
    #unittest.main()


t = timer()
t.start()
fac = 1
for i in range(1, 10000):
  fac *= i
t.stop()
print(t)

a = allocator()
print(a)
print(allocator.maxMemory())
print(allocator.currMemory())
print(allocator.timer())

p = randpermutation(4)
s = set()
for k in range(4*3*2*1):
  s.add(tuple(p[i] for i in range(4)))
  p.next()
pprint(s)
print(len(s))

m = [monom.variable(i, 5, -1) for i in range(5)]
print(len(m[4]))
for i in range(len(m[4])):
  print(m[4][i], end=" ")
print()
m1 = m[0]**4*m[1]*m[4]**4
for d in m1:
  print(d, end="-")
print()
print(m1)
#w = wrap(a, m1)
#print(w.lm())
#print(w)
#m1.setPos(3)
print(m1.pos())
print(repr(m1))
print(m1.degree())
print(a)
print(m1.divisiable(m[4]**4))

print(allocator.maxMemory())
print(allocator.currMemory())
print(allocator.timer())

monom.rand_init(6, 2, 9)
for i in range(15):
  m = monom.next()
  m *= m
  print(m)

division = ("lexGB", "alexGB", "GB")
x = [monom.variable(i, 3, -1) for i in range(3)]
lst = (x[0]**2*x[2], x[0]*x[1], x[0]*x[2], x[2]**2)
for d in division:
  basis = mcompletion(d)
  for m in lst:
    basis.push(m)

  basis.build()
  print("\n", d)
  for w in basis:
    print(w)
  print("-"*10, d)
  del basis

j = mjanet()
b = mcompletion("janet")
monom.rand_init(3, 2, 9)
for i in range(6):
  m = monom.next()
  print(m)
  j.push(m)
  b.push(m)
j.draw("pdf", "janet1.pdf")
j.build()
b.build()
#for w in j:
  #print(w)
#print("j"*10)
for w in b:
  print(w)
print("b"*10)
print(len(j), "==", len(b))
j.draw("pdf", "janet.pdf")
del b
del j

print(allocator.maxMemory())
print(allocator.currMemory())
print(allocator.timer())
