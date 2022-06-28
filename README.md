# PRESENTATION
**Mescamit** is a thread safe variable obfuscator written in C++20.\
It is useful as memory scanner mitigation.

&nbsp;

# HOW IT WORKS
Check the [header file](cpp/CvarObfuscated.hpp#L10-L66).

&nbsp;

# USAGE
Remember to initialize 'srand' at startup.

```cpp
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
ovVec = std::vector<int64_t> { INT64_MAX, 0x00000101, INT64_MIN };
std::vector<int64_t> vecRet(ovVec);

// std::map
CvarObfuscated<std::map<uint8_t, int64_t>> ovMap;
ovMap = std::map<uint8_t, int64_t> { {0, INT64_MIN}, {2, 0x00001100}, {1, INT64_MAX} };
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
