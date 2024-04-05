
# Calculations in the PB of char 2

The file *PolyNormGF.py* is an efficient implementation of operations in a Galois field of characteristic 2 in a polynomial basis. It considers $GF(2^{191})$ with generator $g(x) = x^{191} + x^9 + 1$. An example of its operation can be seen below.

## Interaction Example

```
Enter the first element (a): 224F24F26FE2B05C24D0B6DC7042DB225A71848E6D9D5403
Enter the second element (b): 58839D93424B2022C1490F16DC01933FB45295B08927C5C0

The first element in binary form: 1000100100111100100100111100100110111111100010101100000101110000100100110100001011011011011100011100000100001011011011001000100101101001110001100001001000111001101101100111010101010000000011
The second element in binary form: 10110001000001110011101100100110100001001001011001000000010001011000001010010010000111100010110110111000000000110010011001111111011010001010010100101011011000010001001001001111100010111000000

Enter the number H from 1 to (2^191 - 1): 2B4FBDB81C59124BEA6D91B4D7C4E9B53DC32F3BE81EB45E

The sum of the first and second: 7ACCB9612DA9907EFFFFFFFFE599B9CAFFFFFFFFAC43481DFFFFFFFFEE23113EFFFFFFFFE4BA91C3
Addition operation execution time: 1 μs
The sum of the first element with itself: 0

Multiplication of the first element by the second: 181272EB0BC64A0EFFFFFFFFA9572E3CFFFFFFFFD69CF309FFFFFFFFE42C976E1D44911F
Multiplication operation execution time: 324 μs

Squaring the first element: D4964BE1831BE5D6CBEAC1DFFFFFFFFCA5522E45D35E245FFFFFFFFCA7DA285
Squaring operation execution time: 79 μs

Inverse element (aa) to the first element by multiplication: 7B3D296039BD758B72402844FFFFFFFFB4F111D57DEEEC7DFFFFFFFFE0FEC89A
Inversion operation execution time: 1346 ms
Multiplication of a by aa: 1

The result of raising number a to the power of H: 14A818F3FFFFFFFF9E4DCD01FFFFFFFFA542335F393B8AD27B0C8AE7FFFFFFFF80BE57DF
Raising to the power of H operation execution time: 41 ms

The result of computing Tr (trace) of the first element: 1
The time taken to compute Tr (trace) of the first element: 1305 ms

Test for correctness (checking distributivity): 
Our first element (a): 224F24F26FE2B05C24D0B6DC7042DB225A71848E6D9D5403
Our second element (b): 58839D93424B2022FFFFFFFFC1490F16FFFFFFFFDC01933FFFFFFFFFB45295B0FFFFFFFF8927C5C0
Enter the third element (c): 1234567
The result of (a+b)*c: 58719EE0FFFFFFFFB57ED92CFFFFFFFFC51F1262FFFFFFFF8769385FFFFFFFFF872E63886FAF9E7A
The result of bc+ca: 58719EE0FFFFFFFFB57ED92CFFFFFFFFC51F1262FFFFFFFF8769385FFFFFFFFF872E63886FAF9E7A

Test for correctness (checking properties of finite field elements): 
The result of computing a * (2^191) - a: 0
```