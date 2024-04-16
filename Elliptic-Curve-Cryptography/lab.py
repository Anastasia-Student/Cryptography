import my_library as lib
# Params from https://neuromancer.sk/std/nist/P-224
# y^2 = x^3 + a*x + b; p - field char; n - curve order
p_hex = "fffffffffffffffffffffffffffffffeffffffffffffffff"
a_hex = "fffffffffffffffffffffffffffffffefffffffffffffffc"
b_hex = "64210519e59c80e70fa7e9ab72243049feb8deecc146b9b1"
n_hex = "ffffffffffffffffffffffff99def836146bc9b1b4d22831"

print("LAB#1")
p_dec, a_dec, b_dec, n_dec = int(p_hex, 16), int(a_hex, 16), int(b_hex, 16), int(n_hex, 16)
print("Input data:\np_dec = ", p_dec, "\na_dec = ", a_dec, "\nb_dec = ", b_dec, "\nn_dec = ", n_dec, "\n")
# Check curve smoothing (4*b^3 + 27*a^2 != 0)
lib.IsSmooth(a_dec, b_dec, p_dec)

# Random affine points generation
A = lib.GetRandomAffinePoint(a_dec, b_dec, p_dec)
B = lib.GetRandomAffinePoint(a_dec, b_dec, p_dec)

print("\nA:\n", A)
print("B:\n", B)

# Transformation of the affine points into a projective ones (simply add Z=1)
print("\nWe are generating our points and transfering them into projective ones:")
A = A.AffineToProjective()
B = B.AffineToProjective()

print("A:\n", A)
print("B:\n", B)

print("\nLet us double points:")
A2 = A.PointDouble(a_dec, p_dec).ProjectiveToAffine(p_dec)
B2 = B.PointDouble(a_dec, p_dec).ProjectiveToAffine(p_dec)

print("2A:\n", A2)
print("2B:\n", B2)

print("\nLet us adding points:")
A_Plus_B = A.PointAdd(B, a_dec, p_dec).ProjectiveToAffine(p_dec)
B_Plus_A = B.PointAdd(A, a_dec, p_dec).ProjectiveToAffine(p_dec)

print("A+B:\n", A_Plus_B)
print("B+A:\n", B_Plus_A)

print("\nCheck if nA = O:")
O = A.ScalarMultiplication(n_dec, a_dec, p_dec)
print(O)

print("\nCheck if nA = O (Montgomery method):")
O = A.ScalarMultiplicationMontgomery(n_dec, a_dec, p_dec)
print(O)

print("\nCheck if nB = O:")
O = B.ScalarMultiplication(n_dec, a_dec, p_dec)
print(O)
