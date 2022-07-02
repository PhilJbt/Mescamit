# INDEX

| [PRESENSATION](#presentation) &#65293; [HOW IT WORKS](#how-it-works) &#65293; [USAGE](#usage) &#65293; [BENCHMARK](#benchmark) |
:----------------------------------------------------------: |

&nbsp;

# PRESENTATION
**Mescamit** is a thread safe variable obfuscator written in C++20.\
It is useful as memory scanner mitigation.

###### [Return to index](#index)

&nbsp;

# HOW IT WORKS
Each value stored is *XOR* obfuscated with a generated key, as its pointer and its specifications values (size, offset, etc). Also, the stored address is not pointing directly to the memory buffer of the value.\
Check the [header file](../cpp/CvarObfuscated.hpp#L09-L74) for explanations.\
\
There are also [some examples](../cpp/CvarObfuscated_unitaryTests.cpp#L568-L684) and [unitary tests](../cpp/CvarObfuscated_unitaryTests.cpp#L15-L549).

###### [Return to index](#index)

&nbsp;

# USAGE
```cpp
// Initialization (once)
CvarObfuscated<void>::init(true); // 'True' will initialize ::srand()
                                  // One time call before any use of CvarObfuscated

// Declaration
CvarObfuscated<std::string> ovStr;
CvarObfuscated<float> ovFlt;
CvarObfuscated<int> ovInt;
CvarObfuscated<uint64_t> ovUint64;
CvarObfuscated<bool> ovBool;

// Assignment
ovStr = "Hello";
ovFlt = .15f;
ovInt = 2^16;
ovUint64 = 1.8446744e+10;

// Retrieve value
std::string strRet(ovStr);
float fltRet(ovFlt);
uint64_t ui64Ret(ovUint64);

// Arithmetic Operators
ovInt++;
++ovInt;
ovInt + 5
ovInt - ovInt
ovInt / 2
10 * ovInt

// Assignment Operators
ovStr += " world !"
ovFlt *= -2.f;
ovFlt /= 5.f;
ovInt += 5;
ovInt -= 10;

// Relational Operators
ovBool == true
ovIntA != ovIntB

// Bitwise Operators
ovIntA | ovIntB
ovint32 & 0x00001111
ovUint64 ^ (2 << 2)
ovIntA |= ovIntB
ovint32 &= 0x00110011
ovUint64 ^= (3 >> 1)
ovInt << 2
ovint32 >> 2

// std::vector
CvarObfuscated<std::vector<int64_t>> ovVec;
ovVec = std::vector<int64_t> { INT64_MAX, 0x00000101, 123 };
std::vector<int64_t> vecRet(ovVec);

// std::map
CvarObfuscated<std::map<uint8_t, int64_t>> ovMap;
ovMap = std::map<uint8_t, int64_t> { {0, INT64_MIN}, {2, 321}, {1, 0x00001100} };
std::map<uint8_t, int64_t> retMap(ovMap);

// Custom classes and structs
// MUST have only fixed size members
struct struct_test2 {
    int   i = 0;
    float f = .0f;
    char  str[10] { '\0' };
    int   arrI[3] { 0 };
};

Stest myTest;
myTest.i = INT_MIN;
structTest.f = FLT_MIN;
::strcpy_s(myTest.str, "xINSF1Lv");
myTest.arrI[0] = INT_MAX;
myTest.arrI[1] = 0;
myTest.arrI[2] = INT_MIN;

CvarObfuscated<struct_test2> ovStruct;
ovStruct = myTest;

Stest sRet1(ovStruct);
```

###### [Return to index](#index)

&nbsp;

# BENCHMARK

```
2022-07-01T17:46:50+02:00
Running memscan.exe
DoNotOptimize enabled
Run on (8 X 3655.17 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 256 KiB (x8)
  L3 Unified 12288 KiB (x1)
-----------------------------------------------------------------------------
Benchmark                                   Time             CPU   Iterations
-----------------------------------------------------------------------------
CvarObfuscated<int> ovVariable;          21.7 ns         21.5 ns     32000000
~CvarObfuscated()                        2810 ns         2825 ns       248889
ovVariable = rand() % INT_MAX;           2884 ns         2888 ns       248889
iRet = ovVariable;                        219 ns          219 ns      3136000
ovVariable += rand() % INT_MAX;          3076 ns         3115 ns       235789
```

###### [Return to index](#index)
