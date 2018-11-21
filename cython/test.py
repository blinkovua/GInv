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
print("userTime: %.2f, sysTime: %.2f, realTime: %.2f" % (
    t.userTime(),
    t.sysTime(),
    t.realTime()
  )
)

print(allocate())

p = randpermutation(4)
print(len(p))
for k in range(4*3*2*1):
  for i in range(4):
    print(p[i], end=" ")
  p.next()
  print()

m = monom(1, 2, 2, a)
print(len(m))
for i in range(len(m)):
  print(m[i], end=" ")
print()
print(m.degree())
print(m.pos())

print("alloc: %d, size: %d" % (
    a.alloc(),
    a.size()
  )
)

del m

print("alloc: %d, size: %d" % (
    a.alloc(),
    a.size()
  )
)
