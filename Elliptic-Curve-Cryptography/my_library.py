import random as rn

class AffinePoint:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def CheckAffinePointOnTheCurve(self, a, b, p):
        if self.x is None and self.y is None:
            return 'This is POINT_AT_INFINITY.'
        left_part = pow(self.y, 2, p)
        right_part = (self.x ** 3 + a * self.x + b) % p
        return left_part == right_part

    # Transformation of the affine points into a projective ones (simply add Z=1)
    def AffineToProjective(self):
        return ProjectivePoint(self.x, self.y)

    def __str__(self):
        return f'AffinePoint({self.x}, {self.y})'

class ProjectivePoint:
    def __init__(self, x, y, z=1):
        self.x = x
        self.y = y
        self.z = z

    def __eq__(self, other):
        return self.x == other.x and self.y == other.y and self.z == other.z

    def ProjectiveToAffine(self, p):
        if self == POINT_AT_INFINITY:
            return AffinePoint(None, None)
        return AffinePoint((self.x * invert(self.z, p)) % p, (self.y * invert(self.z, p)) % p)

    def PointDouble(self, a, p):
        # doubling the point at infinity will give it again
        if self.y == 0 or self == POINT_AT_INFINITY:
            return POINT_AT_INFINITY
        W = (a * self.z ** 2 + 3 * self.x ** 2) % p
        S = (self.y * self.z) % p
        B = (self.x * self.y * S) % p
        H = (W ** 2 - 8 * B) % p
        X = H * S << 1 # the same as 2*H*S
        Y = W * (4 * B - H) - 8 * self.y ** 2 * S ** 2
        Z = 8 * S ** 3
        return ProjectivePoint(X % p, Y % p, Z % p)

    def PointAdd(self, other, a, p):
        # processing cases of points at infinity
        if self == POINT_AT_INFINITY:
            return other
        if other == POINT_AT_INFINITY:
            return self
        U1 = (other.y * self.z) % p
        U2 = (self.y * other.z) % p
        V1 = (other.x * self.z) % p
        V2 = (self.x * other.z) % p
        if V1 == V2:
            if U1 != U2:
                return POINT_AT_INFINITY
            else:
                return self.PointDouble(a, p)
        U = (U1 - U2) % p
        V = (V1 - V2) % p
        W = (self.z * other.z) % p
        A = (U**2*W - V**3 - 2*V**2*V2) % p
        X3 = V*A
        Y3 = U*(V**2*V2 - A) - V**3*U2
        Z3 = V**3*W
        return ProjectivePoint(X3 % p, Y3 % p, Z3 % p)

    def ScalarMultiplication(self, k, a, p):
        res = POINT_AT_INFINITY
        temp = self
        for bit in reversed(bin(k)[2:]):
            if bit == '1':
                res = res.PointAdd(temp, a, p)
            temp = temp.PointDouble(a, p)
        return ProjectivePoint(res.x % p, res.y % p, res.z % p)

    def ScalarMultiplicationMontgomery(self, k, a, p):
        R0 = POINT_AT_INFINITY
        R1 = self
        for bit in bin(k)[2:]:
            if bit == '0':
                R1 = R1.PointAdd(R0, a, p)
                R0 = R0.PointDouble(a, p)
            else:
                R0 = R0.PointAdd(R1, a, p)
                R1 = R1.PointDouble(a, p)
        return ProjectivePoint(R0.x % p, R0.y % p, R0.z % p)

    def __str__(self):
        return f'ProjectivePoint({self.x}, {self.y}, {self.z})'

POINT_AT_INFINITY = ProjectivePoint(0, 1, 0)

def IsQuadraticResidue(x, p):
    res = pow(x, (p - 1) >> 1, p) # (p-1)>>1 the same as (p-1)//2
    return res == 1

def sqrt_mod(x, p): # function to find sqrt(x)modp as a
    x = x % p
    Is_Quadr_Res = IsQuadraticResidue(x, p)
    if Is_Quadr_Res and p % 4 == 3: # if p is the Bloom number
        return pow(x, (p + 1) >> 2, p) # we only need one root (doesn't matter which one)
    elif Is_Quadr_Res and p % 8 == 5: # well-known case
        xx = pow(x, (p - 1) >> 2, p) # (p-1)>>2 the same as (p - 1)//4
        if xx == 1:
            return pow(xx, (p + 3) >> 3, p)  # we only need one root (doesn't matter which one)
        else:
             return (pow(xx, (p + 3) >> 3, p) * pow(2, (p - 1) >> 2, p)) # we only need one root (doesn't matter which one)
    else:
        # Tonelli Shanks algorithm
        s = 0 # we need to represent p-1 as 2^s*d; start from s=0
        d = p - 1
        while d & 1 == 0: # d&1 the same as d%2
            d = d >> 1 # the same as d//2
            s = s + 1
        b = 2 # There is a need to find quadratic non resudue b; start from b=2
        while IsQuadraticResidue(b, p):
            b = b + 1
        z = pow(b, d, p) # ord z = 2^s
        r = s
        a = pow(x, (d + 1) >> 1, p) # (d + 1)>>1 the same as (d + 1)//2
        w = pow(x, d, p) # ord w | 2^(s-1)
        # try to find min m : w^(2^m) = 1 mod p
        while w != 1:
            m = 0
            e = 2
            for m in range(1, r):
                if pow(w, e, p) == 1:
                    break
                e = e << 1
                # Hooray, we found m!
            print("r = ", r)
            print("m = ", m)
            z = pow(z, 1 << (r - m - 1), p)
            a = (a * z) % p
            z = (z * z) % p
            w = (w * z) % p
            r = m
        return a # Hooray, we found sqrt(x)modp !!!

def gcdex(a, m):
    if a == 0:
        return 0, 1, m # x = 0; y = 1; GCD = m
    x1, y1, g = gcdex(m % a, a)
    x = y1 - (m//a)*x1
    y = x1
    return x, y, g

def invert(a, m): # to find a^(-1) mod m
    x, y, g = gcdex(a, m)
    if g != 1: # in this case we cannot find the inverted element
        raise Exception("We cannot find the inverted element. GCD(", a, ", ", m, " != 1.")
    return x % m # for negative x also

def GetRandomAffinePoint(a, b, p):
    temp = AffinePoint(-1, -1)
    while not temp.CheckAffinePointOnTheCurve(a, b, p):
        X_point = rn.randint(1, p)
        # y^2 = x^3 + a*x + b; p - field char.
        Y_point = sqrt_mod((X_point ** 3 + a * X_point + b) % p, p)
        temp = AffinePoint(X_point, Y_point)
    return temp

def IsSmooth(a, b, p):
    if (4*b**3 + 27*a**2) % p == 0:
        raise Exception("Curve is not smooth!")
    print("Curve is smooth =D")
