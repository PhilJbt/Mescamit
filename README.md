# PRESENTATION
**Mescamit** is a variable obfuscator written in C++20.\
It is useful as memory scanner mitigation.

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

```cpp

int main(void) {
    unsigned long seed(static_cast<unsigned long>(::clock()) * static_cast<unsigned long>(::time(NULL)) * static_cast<unsigned long>(::_getpid()));
    ::srand(seed);


    unitaryTest();


    {
        struct struct_test2 {
            int   i = 0;
            float f = .0f;
            char  str[10] { '\0' };
            int   arrI[3] { 0 };
        };

        CvarObfuscated<struct_test2> obfvarTest;

        struct_test2 structTest;

        structTest.i = INT_MIN;
        structTest.f = FLT_MIN;
        ::strcpy_s(structTest.str, "xINSF1Lv");
        structTest.arrI[0] = INT_MAX;
        structTest.arrI[1] = 0;
        structTest.arrI[2] = INT_MIN;

        obfvarTest = structTest;

        struct_test2 ret1 = obfvarTest;
    }

    {
        CvarObfuscated<int32_t> ovVariable, ovTemp;


        ovVariable = 496;
        int32_t iRet01(ovVariable); // = 496


        ovVariable += 4;
        int32_t iRet02(ovVariable); // = 500

        ovVariable *= 2;
        int32_t iRet03(ovVariable); // = 1000


        ovVariable ^= 0x00000011;
        int32_t iRet04(ovVariable); // = 0x1111111001 = 1017

        ovVariable &= 0x00100010;
        int32_t iRet05(ovVariable); // = 0x00000010 = 16


        ovTemp = -2;
        int32_t iRet06(ovVariable * ovTemp); // = -32

        int32_t iRet07(ovVariable + 4); // = 20
    }

    {
        CvarObfuscated<std::string> ovVariable;

        ovVariable = "5VRqw3slHk";
        std::string strRet01(ovVariable); // = "5VRqw3slHk"

        ovVariable += "!?";
        std::string strRet02(ovVariable); // = "5VRqw3slHk!?"
    }

    {
        CvarObfuscated<bool> ovVariable;

        ovVariable = true;
        bool bRet01(ovVariable); // = true

        ovVariable = false;
        bool bRet02(ovVariable); // = false
    }

    {
        CvarObfuscated<float> ovVariable;

        ovVariable = 123.5800f;
        float fRet01(ovVariable); // = ~123.58000f

        ovVariable = -987.68400f;
        float fRet02(ovVariable); // = ~-987.6840f
    }

    {
        CvarObfuscated<uint8_t> ovVariable;

        ovVariable = 8;
        uint8_t ui8Ret01(ovVariable >> 1); // = 4
        uint8_t ui8Ret02(ovVariable << 2); // = 32
    }

    {
        CvarObfuscated<int> ovA, ovB;

        ovA = 8;
        ovB = 8;
        bool bRet01(ovA == ovB); // = true

        ovB = 5;
        bool bRet02(ovA != ovB); // = true
    }

    {
        CvarObfuscated<std::vector<int64_t>> ovA;

        ovA = std::vector<int64_t> { INT64_MAX, 0x00000101, INT64_MIN };

        std::vector<int64_t> vecA1(ovA);
    }

    {
        CvarObfuscated<std::map<uint8_t, int64_t>> ovA;

        ovA = std::map<uint8_t, int64_t> { {0, INT64_MIN}, {2, 0x00001100}, {1, INT64_MAX} };

        std::map<uint8_t, int64_t> vecA1(ovA);
    }

    return 0;
}
```
