#include "CvarObfuscated.hpp"


#if !defined(SK_BENCHMARK)
    #define execute (main)
#endif



/*
** Unitary tests
* 
*/
void unitaryTest() {
    {
        CvarObfuscated<std::map<uint8_t, int64_t>> ovA;

        ovA = std::map<uint8_t, int64_t> { {0, INT64_MIN}, {2, 0x00001100}, {1, INT64_MAX} };

        std::map<uint8_t, int64_t> ret(ovA);

        if (ret != std::map<uint8_t, int64_t> { {0, INT64_MIN}, { 2, 0x00001100 }, { 1, INT64_MAX } }) throw std::runtime_error("TEST std::map #1 FAILED");
        if (ovA != ret) throw std::runtime_error("TEST std::map #2 FAILED");
    }
    {
        CvarObfuscated<std::vector<int64_t>> ovA;

        ovA = std::vector<int64_t> { INT64_MAX, 0, INT64_MIN };

        std::vector<int64_t> ret(ovA);

        if (ret != std::vector<int64_t>{ INT64_MAX, 0, INT64_MIN}) throw std::runtime_error("TEST std::vector #1 FAILED");
        if (ovA != ret) throw std::runtime_error("TEST std::vector #2 FAILED");
    }
    {
        CvarObfuscated<int32_t> ovTest;

        ovTest = INT32_MIN;
        int32_t ret1 = ovTest;
        if (ret1 != INT32_MIN) throw std::runtime_error("TEST int32_t #1 FAILED");

        ovTest = INT32_MAX;
        int32_t ret2 = ovTest;
        if (ret2 != INT32_MAX) throw std::runtime_error("TEST int32_t #2 FAILED");

        int32_t i32Rand(::rand() % INT32_MAX);
        ovTest = i32Rand;
        int32_t ret3 = ovTest;
        if (ret3 != i32Rand) throw std::runtime_error("TEST int32_t #3 FAILED");
    }
    {
        CvarObfuscated<uint64_t> ovTest;

        ovTest = 0;
        uint64_t ret1 = ovTest;
        if (ret1 != 0) throw std::runtime_error("TEST uint64_t #1 FAILED");

        ovTest = UINT64_MAX;
        uint64_t ret2 = ovTest;
        if (ret2 != UINT64_MAX) throw std::runtime_error("TEST uint64_t #2 FAILED");

        uint64_t ui64Rand(::rand() % UINT64_MAX);
        ovTest = ui64Rand;
        uint64_t ret3 = ovTest;
        if (ret3 != ui64Rand) throw std::runtime_error("TEST uint64_t #3 FAILED");
    }
    {
        CvarObfuscated<float_t> ovTest;

        ovTest = FLT_MIN;
        float_t ret1 = ovTest;
        if (ret1 != FLT_MIN) throw std::runtime_error("TEST float_t #1 FAILED");

        ovTest = FLT_MAX;
        float_t ret2 = ovTest;
        if (ret2 != FLT_MAX) throw std::runtime_error("TEST float_t #2 FAILED");

        float_t fRand(static_cast<float_t>(::rand()) / FLT_MAX);
        ovTest = fRand;
        float_t ret3 = ovTest;
        if (ret3 != fRand) throw std::runtime_error("TEST float_t #3 FAILED");
    }
    {
        CvarObfuscated<bool> ovTest;

        ovTest = true;
        bool ret1 = ovTest;
        if (ret1 != true) throw std::runtime_error("TEST bool #1 FAILED");

        ovTest = false;
        bool ret2 = ovTest;
        if (ret2 != false) throw std::runtime_error("TEST bool #2 FAILED");
    }
    {
        CvarObfuscated<std::string> ovTest;

        ovTest = "cED66";
        std::string ret1 = ovTest;
        if (::strcmp(ret1.c_str(), "cED66") != 0) throw std::runtime_error("TEST STD::STRING #1 FAILED");

        ovTest += "Q9jr7QWycx";
        std::string ret2 = ovTest;
        if (::strcmp(ret2.c_str(), "cED66Q9jr7QWycx") != 0) throw std::runtime_error("TEST STD::STRING #2 FAILED");

        ovTest = "1YESX9x";
        std::string ret3 = ovTest;
        if (::strcmp(ret3.c_str(), "1YESX9x") != 0) throw std::runtime_error("TEST STD::STRING #3 FAILED");
    }
    {
        struct struct_test1 {
            int   i       = 0;
            float f       = .0f;
            char  str[15] { '\0' };
            int   arrI[3] { 0 };
        };

        CvarObfuscated<struct_test1> ovTest;

        struct_test1 stTest;

        stTest.i = INT_MIN;
        stTest.f = FLT_MIN;
        ::strcpy_s(stTest.str, "KPpQk");
        stTest.arrI[0] = 1;
        stTest.arrI[1] = 2;
        stTest.arrI[2] = 3;
        ovTest = stTest;
        struct_test1 ret1 = ovTest;
        if (ret1.f != FLT_MIN) throw std::runtime_error("TEST struct_test1 #1:A FAILED");
        if (ret1.i != INT_MIN) throw std::runtime_error("TEST struct_test1 #1:B FAILED");
        if (::strcmp(ret1.str, "KPpQk") != 0) throw std::runtime_error("TEST struct_test1 #1:C FAILED");
        if (ret1.arrI[0] != 1 || ret1.arrI[1] != 2 || ret1.arrI[2] != 3) throw std::runtime_error("TEST struct_test1 #1:D FAILED");

        stTest.i = 0;
        stTest.f = .0f;
        ::strcpy_s(stTest.str, "tTl4f785e7");
        stTest.arrI[0] = INT_MIN;
        stTest.arrI[1] = INT_MAX;
        stTest.arrI[2] = 0;
        ovTest = stTest;
        struct_test1 ret2 = ovTest;
        if (ret2.f != 0) throw std::runtime_error("TEST struct_test1 #2:A FAILED");
        if (ret2.i != .0f) throw std::runtime_error("TEST struct_test1 #2:B FAILED");
        if (::strcmp(ret2.str, "tTl4f785e7") != 0) throw std::runtime_error("TEST struct_test1 #2:C FAILED");
        if (ret2.arrI[0] != INT_MIN || ret2.arrI[1] != INT_MAX || ret2.arrI[2] != 0) throw std::runtime_error("TEST struct_test1 #2:D FAILED");

        stTest.i = INT_MAX;
        stTest.f = FLT_MAX;
        ::strcpy_s(stTest.str, "sJhhMAp");
        stTest.arrI[0] = 0x00011100;
        stTest.arrI[1] = 2^3;
        stTest.arrI[2] = 8 << 1;
        ovTest = stTest;
        struct_test1 ret3 = ovTest;
        if (ret3.f != FLT_MAX) throw std::runtime_error("TEST struct_test1 #3:A FAILED");
        if (ret3.i != INT_MAX) throw std::runtime_error("TEST struct_test1 #3:B FAILED");
        if (::strcmp(ret3.str, "sJhhMAp") != 0) throw std::runtime_error("TEST struct_test1 #3:C FAILED");
        if (ret3.arrI[0] != 0x00011100 || ret3.arrI[1] != (2 ^ 3) || ret3.arrI[2] != (8 << 1)) throw std::runtime_error("TEST struct_test1 #3:D FAILED");
    }
    {
        CvarObfuscated<int32_t> ovA;


        ovA = 50;
        ++ovA;

        int32_t iRet1(ovA);

        if (iRet1 != 51) throw std::runtime_error("TEST ++var #1 FAILED");


        ovA = 60;
        ovA++;

        int32_t iRet2(ovA);

        if (iRet2 != 61) throw std::runtime_error("TEST var++ #2 FAILED");
    }
    {
        CvarObfuscated<int32_t> ovA;


        ovA = 50;
        --ovA;

        int32_t iRet1(ovA);

        if (iRet1 != 49) throw std::runtime_error("TEST --var #1 FAILED");


        ovA = 60;
        ovA--;

        int32_t iRet2(ovA);

        if (iRet2 != 59) throw std::runtime_error("TEST var-- #2 FAILED");
    }
    {
        CvarObfuscated<int32_t> ovA;


        ovA = 50;
        ovA += 5;

        int32_t iRet1(ovA);

        if (iRet1 != 55) throw std::runtime_error("TEST var += #1 FAILED");


        ovA = 60;
        ovA += 10;

        int32_t iRet2(ovA);

        if (iRet2 != 70) throw std::runtime_error("TEST var += #2 FAILED");
    }
    {
        CvarObfuscated<int32_t> ovA;


        ovA = 50;
        ovA -= 5;

        int32_t iRet1(ovA);

        if (iRet1 != 45) throw std::runtime_error("TEST var -= #1 FAILED");


        ovA = 60;
        ovA -= 10;

        int32_t iRet2(ovA);

        if (iRet2 != 50) throw std::runtime_error("TEST var -= #2 FAILED");
    }
    {
        CvarObfuscated<int32_t> ovA;


        ovA = 10;
        ovA *= 2;

        int32_t iRet1(ovA);

        if (iRet1 != 20) throw std::runtime_error("TEST var *= #1 FAILED");


        ovA = 50;
        ovA *= 10;

        int32_t iRet2(ovA);

        if (iRet2 != 500) throw std::runtime_error("TEST var *= #2 FAILED");
    }
    {
        CvarObfuscated<int32_t> ovA;


        ovA = 10;
        ovA /= 2;

        int32_t iRet1(ovA);

        if (iRet1 != 5) throw std::runtime_error("TEST var /= #1 FAILED");


        ovA = 50;
        ovA /= 10;

        int32_t iRet2(ovA);

        if (iRet2 != 5) throw std::runtime_error("TEST var /= #2 FAILED");
    }
    {
        CvarObfuscated<std::string> ovA;
        CvarObfuscated<int> ovB;


        ovA = "QAJj0agMXw";

        bool bRet1(ovA == "v8Y5psf973");

        if (bRet1 != false) throw std::runtime_error("TEST var == #1 FAILED");


        ovB = 123;

        bool bRet2(ovB == 123);

        if (bRet2 != true) throw std::runtime_error("TEST var == #2 FAILED");
    }
    {
        CvarObfuscated<int32_t> ovA;


        ovA = 16;

        int32_t iRet1(ovA << 2);

        if (iRet1 != 64) throw std::runtime_error("TEST var << #1 FAILED");


        int32_t iRet2(ovA >> 1);

        if (iRet2 != 8) throw std::runtime_error("TEST var >> #2 FAILED");
    }
    {
        CvarObfuscated<int32_t> ovA;


        ovA = 0x00000101;
        ovA |= 0x00001001;

        int32_t iRet1(ovA);

        if (iRet1 != 0x00001101) throw std::runtime_error("TEST var |= #1 FAILED");


        ovA = 0x00011100;
        ovA |= 0x00001110;

        int32_t iRet2(ovA);

        if (iRet2 != 0x00011110) throw std::runtime_error("TEST var |= #2 FAILED");
    }
    {
        CvarObfuscated<int32_t> ovA;


        ovA = 0x00000101;
        ovA &= 0x00001001;

        int32_t iRet1(ovA);

        if (iRet1 != 0x00000001) throw std::runtime_error("TEST var &= #1 FAILED");


        ovA = 0x00011100;
        ovA &= 0x00001110;

        int32_t iRet2(ovA);

        if (iRet2 != 0x00001100) throw std::runtime_error("TEST var &= #2 FAILED");
    }
    {
        CvarObfuscated<int32_t> ovA;

        ovA = 0x00000101;
        ovA ^= 0x00001001;

        int32_t iRet1(ovA);

        if (iRet1 != 0x00001100) throw std::runtime_error("TEST var ^= #1 FAILED");


        ovA = 0x00011100;
        ovA ^= 0x00001110;

        int32_t iRet2(ovA);

        if (iRet2 != 0x00010010) throw std::runtime_error("TEST var ^= #2 FAILED");
    }
    {
        CvarObfuscated<int32_t> ovA, ovB;


        ovA = 123;
        ovB = 456;

        int32_t iRet1(ovA + ovB);
        int32_t iA1(ovA);
        int32_t iB1(ovB);

        if (iA1 != 123) throw std::runtime_error("TEST var + var #1:A FAILED");
        if (iB1 != 456) throw std::runtime_error("TEST var + var #3:B FAILED");
        if (iRet1 != 579) throw std::runtime_error("TEST var + var #1:C FAILED");


        ovA = 789;
        ovB = 348;

        int32_t iRet2(ovA + ovB);
        int32_t iA2(ovA);
        int32_t iB2(ovB);

        if (iA2 != 789) throw std::runtime_error("TEST var + var #2:A FAILED");
        if (iB2 != 348) throw std::runtime_error("TEST var + var #2:B FAILED");
        if (iRet2 != 1137) throw std::runtime_error("TEST var + var #2:C FAILED");
    }
    {
        CvarObfuscated<int32_t> ovA, ovB;


        ovA = 123;
        ovB = 456;

        int32_t iRet1(ovA - ovB);
        int32_t iA1(ovA);
        int32_t iB1(ovB);

        if (iA1 != 123) throw std::runtime_error("TEST var - var #1:A FAILED");
        if (iB1 != 456) throw std::runtime_error("TEST var - var #3:B FAILED");
        if (iRet1 != -333) throw std::runtime_error("TEST var - var #1:C FAILED");


        ovA = 789;
        ovB = 348;

        int32_t iRet2(ovA - ovB);
        int32_t iA2(ovA);
        int32_t iB2(ovB);

        if (iA2 != 789) throw std::runtime_error("TEST var - var #2:A FAILED");
        if (iB2 != 348) throw std::runtime_error("TEST var - var #2:B FAILED");
        if (iRet2 != 441) throw std::runtime_error("TEST var - var #2:C FAILED");
    }
    {
        CvarObfuscated<int32_t> ovA, ovB;


        ovA = 123;
        ovB = 789;

        int32_t iRet1(ovA * ovB);
        int32_t iA1(ovA);
        int32_t iB1(ovB);

        if (iA1 != 123) throw std::runtime_error("TEST var * var #1:A FAILED");
        if (iB1 != 789) throw std::runtime_error("TEST var * var #3:B FAILED");
        if (iRet1 != 97047) throw std::runtime_error("TEST var * var #1:C FAILED");


        ovA = 684;
        ovB = 321;

        int32_t iRet2(ovA * ovB);
        int32_t iA2(ovA);
        int32_t iB2(ovB);

        if (iA2 != 684) throw std::runtime_error("TEST var * var #2:A FAILED");
        if (iB2 != 321) throw std::runtime_error("TEST var * var #2:B FAILED");
        if (iRet2 != 219564) throw std::runtime_error("TEST var * var #2:C FAILED");
    }
    {
        CvarObfuscated<int32_t> ovA, ovB;


        ovA = 500;
        ovB = 10;

        int32_t iRet1(ovA / ovB);
        int32_t iA1(ovA);
        int32_t iB1(ovB);

        if (iA1 != 500) throw std::runtime_error("TEST var / var #1:A FAILED");
        if (iB1 != 10) throw std::runtime_error("TEST var / var #3:B FAILED");
        if (iRet1 != 50) throw std::runtime_error("TEST var / var #1:C FAILED");


        ovA = 8;
        ovB = 2;

        int32_t iRet2(ovA / ovB);
        int32_t iA2(ovA);
        int32_t iB2(ovB);

        if (iA2 != 8) throw std::runtime_error("TEST var / var #2:A FAILED");
        if (iB2 != 2) throw std::runtime_error("TEST var / var #2:B FAILED");
        if (iRet2 != 4) throw std::runtime_error("TEST var / var #2:C FAILED");
    }
    {
        CvarObfuscated<int32_t> ovA, ovB;

        ovA = 0x00000101;
        ovB = 0x00001001;

        int32_t iRet1(ovA | ovB);
        int32_t iA1(ovA);
        int32_t iB1(ovB);

        if (iA1 != 0x00000101) throw std::runtime_error("TEST var | var #1:A FAILED");
        if (iB1 != 0x00001001) throw std::runtime_error("TEST var | var #3:B FAILED");
        if (iRet1 != 0x00001101) throw std::runtime_error("TEST var | var #1:C FAILED");


        ovA = 0x00010101;
        ovB = 0x01000100;

        int32_t iRet2(ovA | ovB);
        int32_t iA2(ovA);
        int32_t iB2(ovB);

        if (iA2 != 0x00010101) throw std::runtime_error("TEST var | var #2:A FAILED");
        if (iB2 != 0x01000100) throw std::runtime_error("TEST var | var #2:B FAILED");
        if (iRet2 != 0x01010101) throw std::runtime_error("TEST var | var #2:C FAILED");
    }
    {
        CvarObfuscated<int32_t> ovA, ovB;


        ovA = 0x00000101;
        ovB = 0x00001001;

        int32_t iRet1(ovA & ovB);
        int32_t iA1(ovA);
        int32_t iB1(ovB);

        if (iA1 != 0x00000101) throw std::runtime_error("TEST var & var #1:A FAILED");
        if (iB1 != 0x00001001) throw std::runtime_error("TEST var & var #3:B FAILED");
        if (iRet1 != 0x00000001) throw std::runtime_error("TEST var & var #1:C FAILED");

        ovA = 0x00010101;
        ovB = 0x01000100;

        int32_t iRet2(ovA & ovB);
        int32_t iA2(ovA);
        int32_t iB2(ovB);

        if (iA2 != 0x00010101) throw std::runtime_error("TEST var & var #2:A FAILED");
        if (iB2 != 0x01000100) throw std::runtime_error("TEST var & var #2:B FAILED");
        if (iRet2 != 0x00000100) throw std::runtime_error("TEST var & var #2:C FAILED");
    }
    {
        CvarObfuscated<int32_t> ovA, ovB;


        ovA = 0x00000101;
        ovB = 0x00001001;

        int32_t iRet1(ovA ^ ovB);
        int32_t iA1(ovA);
        int32_t iB1(ovB);

        if (iA1 != 0x00000101) throw std::runtime_error("TEST var ^ var #1:A FAILED");
        if (iB1 != 0x00001001) throw std::runtime_error("TEST var ^ var #3:B FAILED");
        if (iRet1 != 0x00001100) throw std::runtime_error("TEST var ^ var #1:C FAILED");


        ovA = 0x00010101;
        ovB = 0x01000100;

        int32_t iRet2(ovA ^ ovB);
        int32_t iA2(ovA);
        int32_t iB2(ovB);

        if (iA2 != 0x00010101) throw std::runtime_error("TEST var ^ var #2:A FAILED");
        if (iB2 != 0x01000100) throw std::runtime_error("TEST var ^ var #2:B FAILED");
        if (iRet2 != 0x01010001) throw std::runtime_error("TEST var ^ var #2:C FAILED");
    }
}



/*
** Entry point
*
*/
int execute(void) {
    // Initialize the CvarObfuscated class
    CvarObfuscated<void>::init(true);



    unitaryTest();



    // Examples
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