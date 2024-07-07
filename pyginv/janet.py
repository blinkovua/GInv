from monom import *

class Janet:
  def __init__(self):
    self.pos = None
    self.var = None
    self.tree = []
  
  def insert(self):
    assert not self.pos or self.lm().
    pass


if __name__ == '__main__':
  variables = ['a', 'b', 'c', 'd', 'e', 'f']
  Monom.variables = len(variables)+1
  # POTlex TOPlex POTdeglex TOPdeglex
  Monom.cmp = Monom.POTlex
  Monom.zero = Monom(0 for v in range(Monom.variables))
  for i, v in enumerate(variables):
    globals()[v] = Monom(0 if l-1 != i else 1 for l in range(Monom.variables))

  m1 = b*c**2
