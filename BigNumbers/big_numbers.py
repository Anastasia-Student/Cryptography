import math
from time import time
BASE = 32

# конвентирование
def from_string(s):
  ret = [int(s[max(x-8, 0):x], 16) for x in range(len(s), 0, -8)] #по 8 с конца записываем в ret от менее значимой к более значимой
  return [] if len(ret) == 1 and ret[0] == 0 else ret

# деконвентирование
def to_string(a):
  ret = f'{a[-1]:X}' #записываем последний элемент в 16 системе
  return ret + ''.join(f'{a[-x]:08X}' for x in range(2, len(a)+1)) #записываем остальные дописывая нули спереди (если нужно)

# сложение
def add(a, b):
  ret = []
  carry = 0
  for i in range(max(len(a), len(b))):
    tmp = carry # бит переноса всегда прибавляется по алгоритму
    if i < len(a): tmp += a[i]
    if i < len(b): tmp += b[i]
    ret.append(tmp & (2**BASE-1)) #добавляем в конец число, которое не больше (2**BASE-1)
    carry = tmp >> BASE # чтобы знать вылезли ли мы за рамки BASE
  if carry != 0:
    ret.append(carry)
  return ret

# разность
def sub(a, b):
  ret = []
  borrow = 0
  for i in range(max(len(a), len(b))):
    tmp = -borrow # одолженный бит всегда с нами
    if i < len(a): tmp += a[i]
    if i < len(b): tmp -= b[i]
    if tmp >= 0:
      ret.append(tmp) #добавляем в конец
      borrow = 0
    else:             # берем у соседей бит (2**BASE) если tmp<0
      ret.append(2**BASE + tmp)
      borrow = 1
  if borrow != 0:
    print('negative number')
  k = len(ret)-1
  while k >= 0 and ret[k] == 0: k -= 1
  return ret[0:k+1]

def cmp(a, b):
  la, lb = len(a), len(b)
  if la > lb: return 1
  elif lb > la: return -1
  for i in range(1, la+1): # проверка равенства
    if a[-i] != b[-i]:
      return 1 if a[-i] > b[-i] else -1
  return 0

#сдвигаем цифры массива вправо (в сторону старших цифр)
def rt_int_shift(a, n):
  return [0] * n + a  # дописываем просто нули слева, получается, что сдвигаем

#сдвигаем цифры массива влево (в сторону младших цифр)
def lt_int_shift(a, n):
  return [] if n >= len(a) else a[n:]

#сдвигаем биты массива цифр вправо (в сторону младших битов)
def rt_bit_shift(a, n):
  n_ints = n // BASE
  n_bits = n % BASE

  if n_ints >= len(a):                             # тогда будет пустой массив
    return []

  if n_bits == 0:
    return lt_int_shift(a, n_ints)                 # получаем сдвинутый массив в сторону младших цифр

  n_bits2 = BASE - n_bits
  hi_bits = a[-1] >> n_bits                                  # сдвигаем последний элемент на n_bits вправо
  c = [0] * (len(a) - n_ints - (0 if hi_bits != 0 else 1))   # создаем массив из нулей на то колличество элементов, которые будут ненулевыми после сдвига
  i = len(c)-1                                               # длина этого всего массива из нулей будет i+1

  if hi_bits != 0:
    c[i] = hi_bits
    i -=1

  j = len(a)-1
  while j > n_ints:
    t1 = (a[j] << n_bits2) & (2**BASE-1)
    t2 = a[j-1] >> n_bits
    c[i] = t1 | t2
    i -= 1
    j -= 1

  return c

#сдвигаем биты массива цифр влево (в сторону старших битов)
def lt_bit_shift(a, n):
  n_ints = n // BASE
  n_bits = n % BASE

  if n_bits == 0:
    return rt_int_shift(a, n_ints)

  n_bits2 = BASE - n_bits
  hi_bits = a[-1] >> n_bits2
  c = [0] * (len(a) + n_ints + (1 if hi_bits != 0 else 0))

  i = len(c)-1
  if hi_bits != 0:
    c[i] = hi_bits
    i -= 1

  j = len(a)-1
  while j > 0:
    t1 = (a[j] << n_bits) & (2**BASE-1)
    t2 = a[j-1] >> n_bits2
    c[i] = t1 | t2
    i -= 1
    j -= 1
  c[i] = (a[j] << n_bits) & (2**BASE-1)

  return c


def bit_length(a):
  return (len(a) - 1) * BASE + a[-1].bit_length()

# проверка того, что i-тый бит = 1
def test_bit_at(a, i):
  return (a[i // BASE] >> (i % BASE)) & 1 == 1

def mul(a, b):
  if len(a) < len(b): return mul(b, a)
  # иначе, иначе вот:
  ret = [] # нашего результата пока нет
  for i in range(len(a)):
    t1 = []   #
    carry = 0
    for j in range(len(a)):
      t2 = carry # бит переноса всегда с нами
      if i < len(b): t2 += a[j] * b[i] # проходимся по всем элементам b и умножаем их на число a[j]
      elif carry == 0: break
      t1.append(t2 & (2**BASE-1)) #добавляем в конец число, которое не больше (2**BASE-1)
      carry = t2 >> BASE     # чтобы знать вылезли ли мы за рамки BASE
    if carry != 0:
      t1.append(carry)
    ret = add(ret, rt_int_shift(t1, i))  # многоразрядное сложение со сдвигом
  return ret

def pow(a, b):
  c = [1]
  for i in range(bit_length(b)):
    if test_bit_at(b, i): #  i-тый бит в b = 1?
      c = mul(c, a)       #  Да
    a = karatsuba_pow2(a)
  return c

def div_mod(a, b):
  k = bit_length(b)  # битовая длина b
  r = a.copy()       # остаток
  q = []             # частное
  while cmp(r, b) >= 0: # многоразрядное сравнение
    t = bit_length(r)
    c = lt_bit_shift(b, t - k)
    if cmp(r, c) < 0:   # Вышло много?
      t = t - 1         # Возвращаемся на бит назад
      c = lt_bit_shift(b, t - k)
    r = sub(r, c)
    q = add(q, lt_bit_shift([1], t - k))  # частное имеет теперь бит с номером t - k
  return (q, r)


def karatsuba_mul(a, b):
  n = min(bit_length(a), bit_length(b))   # берем минимум длин
  if n < 512:
    return mul(a, b)

  n = n // 2 + n % 2

  hi1 = rt_bit_shift(a, n)            # верхняя часть а
  lo1 = sub(a, lt_bit_shift(hi1, n))  # нижняя часть а
  hi2 = rt_bit_shift(b, n)            # верхняя часть b
  lo2 = sub(b, lt_bit_shift(hi2, n))  # нижняя часть b

  z0 = karatsuba_mul(lo1, lo2)       # перемножаем нижние части
  z1 = karatsuba_mul(add(lo1, hi1), add(lo2, hi2))        #Вообще z1 = (lo1 + hi1)*(lo2 + hi2) - z2 - z1, но дальше все отнимем, ибо умножение одно лучше двух
  z2 = karatsuba_mul(hi1, hi2)       # перемножаем верхние части

  t1 = lt_bit_shift(z2, n * 2)       # сдвигаем результат умножения верхних чисел туда, где он должен быть (к старшим разрядам, вверх)
  t2 = lt_bit_shift(sub(sub(z1, z2), z0), n)  # теперь отнимем и сдвинем  (к старшим разрядам)

  return add(add(t1, t2), z0)


def karatsuba_pow2(a):
  n = bit_length(a)
  if n < 512:
    return mul(a, a)

  n = n // 2 + n % 2

  hi1 = rt_bit_shift(a, n)            # верхняя часть а
  lo1 = sub(a, lt_bit_shift(hi1, n))  # нижняя часть а

  z0 = karatsuba_pow2(lo1)            # умножаем нижнюю часть на себя (возводим в квадрат)
  z1 = karatsuba_pow2(add(lo1, hi1))  # умножение одно лучше двух
  z2 = karatsuba_pow2(hi1)            # умножаем верхнюю часть на себя (возводим в квадрат)

  t1 = lt_bit_shift(z2, n * 2)        # сдвигаем результат умножения z2 туда, где он должен быть (к старшим разрядам, вверх)
  t2 = lt_bit_shift(sub(sub(z1, z2), z0), n)   # теперь отнимем и сдвинем  (к старшим разрядам)


  return add(add(t1, t2), z0)


def gcd(a, b):
  d = [1]

  while a[0] % 2 == 0 and b[0] % 2 == 0:  # если а и b парные
    a = rt_bit_shift(a, 1)    # делим на 2, эквивалентно битовому сдвигу вправо
    b = rt_bit_shift(b, 1)    # делим на 2, эквивалентно битовому сдвигу вправо
    d = lt_bit_shift(d, 1)    # умножаем на 2, эквивалентно битовому сдвигу влево

  while a[0] % 2 == 0:
    a = rt_bit_shift(a, 1)

  while len(b) > 0:
    while b[0] % 2 == 0:      # если b парное
      b = rt_bit_shift(b, 1)  # делим на 2, эквивалентно битовому сдвигу вправо
    if cmp(a, b) >= 0:
      a, b = b, sub(a, b)     # теперь а = b, а b разнице
    else:
      a, b = a, sub(b, a)     # теперь а = а, а b разнице

  d = mul(d, a)

  return d


def lcm(a, b):
  return div_mod(mul(a, b), gcd(a, b))[0] # берем только частное


def calculate_mu(m):
  return div_mod(rt_int_shift([1], len(m) * 2), m)[0]   # берем только частное


def barret_reduction(x, m, mu = None):
  if mu == None: mu = calculate_mu(m) #находим мю

  q = lt_int_shift(x, len(m)-1)       #выкидываем последние (len(m)-1) цифр
  q = mul(q, mu)
  q = lt_int_shift(q, len(m)+1)       #выкидываем последние (len(m)+1) цифр

  ret = sub(x, mul(q, m))             #полагаем, что результат = х - q*m

  while cmp(ret, m) >= 0:             #делаем, пока ret не станет меньше m
    ret = sub(ret, m)

  return ret


def barret_add(a, b, m, mu = None):
  if mu == None: mu = calculate_mu(m)
  return barret_reduction(add(a, b), m, mu)


def barret_sub(a, b, m, mu = None):
  if mu == None: mu = calculate_mu(m)
  return barret_reduction(sub(a, b), m, mu)


def barret_mul(a, b, m, mu = None):
  if mu == None: mu = calculate_mu(m)
  return barret_reduction(mul(a, b), m, mu)


def barret_pow(a, b, m, mu = None):
  if mu == None: mu = calculate_mu(m)

  c = [1]
  for i in range(bit_length(b)):
    if test_bit_at(b, i):
      c = barret_mul(c, a, m, mu)
    a = barret_mul(a, a, m, mu)
  return c


def barret_karatsuba_mul(a, b, m, mu = None):
  if mu == None: mu = calculate_mu(m)

  n = min(bit_length(a), bit_length(b))
  if n < 512:
    return barret_mul(a, b, m, mu)                      # теперь тоже умножение, но по Баррету

  n = n // 2 + n % 2

  hi1 = rt_bit_shift(a, n)
  lo1 = sub(a, lt_bit_shift(hi1, n))
  hi2 = rt_bit_shift(b, n)
  lo2 = sub(b, lt_bit_shift(hi2, n))

  z0 = karatsuba_mul(lo1, lo2)
  z1 = karatsuba_mul(add(lo1, hi1), add(lo2, hi2))
  z2 = karatsuba_mul(hi1, hi2)

  t1 = lt_bit_shift(z2, n * 2)
  t2 = lt_bit_shift(sub(sub(z1, z2), z0), n)

  return barret_add(barret_add(t1, t2, m, mu), z0, m, mu)   # теперь тоже сумма, но по Баррету


def barret_karatsuba_pow2(a, b, m, mu = None):
  if mu == None: mu = calculate_mu(m)

  n = bit_length(a)
  if n < 512:
    return barret_mul(a, a, m, mu)                          # теперь тоже умножение, но по Баррету

  n = n // 2 + n % 2

  hi1 = rt_bit_shift(a, n)
  lo1 = sub(a, lt_bit_shift(hi1, n))

  z0 = karatsuba_pow2(lo1)
  z1 = karatsuba_pow2(add(lo1, hi1))
  z2 = karatsuba_pow2(hi1)

  t1 = lt_bit_shift(z2, n * 2)
  t2 = lt_bit_shift(sub(sub(z1, z2), z0), n)

  return barret_add(barret_add(t1, t2, m, mu), z0, m, mu)     # теперь тоже сумма, но по Баррету