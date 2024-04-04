
# Big Integers

The file *big_numbers.py* is an efficient implementation of a Big Integers library. It takes and returns integers in the hex format. Calculations are performed by working with arrays of remainders upon division by $2^{32}$ as this efficiently utilizes resources and algorithms work quickly. 

It's important to note that the library is designed to work with unsigned fixed-length integers since it was developed for cryptographic purposes.



## Examples

```python
from big_numbers import *

A = from_string('D4D2110984907B5625309D956521BAB4157B8B1ECE04043249A3D379AC112E5B9AF44E721E148D88A942744CF56A06B92D28A0DB950FE4CED2B41A0BD38BCE7D0BE1055CF5DE38F2A588C2C9A79A75011058C320A7B661C6CE1C36C7D870758307E5D2CF07D9B6E8D529779B6B2910DD17B6766A7EFEE215A98CAC300F2827DB')

B = from_string('3A7EF2554E8940FA9B93B2A5E822CC7BB262F4A14159E4318CAE3ABF5AEB1022EC6D01DEFAB48B528868679D649B445A753684C13F6C3ADBAB059D635A2882090FC166EA9F0AAACD16A062149E4A0952F7FAAB14A0E9D3CB0BE9200DBD3B0342496421826919148E617AF1DB66978B1FCD28F8408506B79979CCBCC7F7E5FDE7')

N = from_string('269D7722EA018F2AC35C5A3517AA06EAA1949059AE8240428BBFD0A8BE6E2EBF91223991F80D7413D6B2EB213E7122710EDEC617460FA0191F3901604619972018EBEF22D81AED9C56424014CADCC2CCDEE67D36A54BFC500230CA6693ABA057B374746622341ED6D52FE5A79E6860F54F197791B3FEF49FD534CB2C675B6BDB')

print('A mod B =', to_string(barret_reduction(A, B)), '\n')
print('gcd(A, B) =', to_string(gcd(A, B)), '\n')
print('lcm(A, B) =', to_string(lcm(A, B)), '\n')
print('(A + B) mod N =', to_string(barret_add(A, B, N)), '\n')
print('(A - B) mod N =', to_string(barret_sub(A, B, N)), '\n')
print('(A * B) mod N =', to_string(barret_mul(A, B, N)), '\n')
print('(A ^ 2) mod N =', to_string(barret_karatsuba_pow2(A, B, N)), '\n')
print('(A ^ B) mod N =', to_string(barret_pow(A, B, N)), '\n')
```