/*
* MESCAMIT - VARIABLE MEMORY SCANNER MITIGATION
* Philippe Jaubert - 2022
* https://github.com/PhilJbt/mescamit
* MIT License
*/



/*
** HOW IT WORKS

    The CvarObfuscated class stores a VALUE into an obfuscated array using a randomly generated KEY.
    +--------------+
    | KEY or VALUE |
    +--------------+

    Two noise data sequences of random lengths are added as prefix and postfix of the stored KEY or VALUE.
    +--...--+--------------+--...--+
    | NOISE | KEY or VALUE | NOISE |
    +--...--+--------------+--...--+

    Also, the address pointer stored does not point directly to the stored array, but to multiple others pointers (as a linked list), the last of the list is the array.
    POINTER --> HOP --> HOP --> +--...--+--------------+--...--+
                                | NOISE | KEY or VALUE | NOISE |
                                +--...--+--------------+--...--+

    Its POINTER address, LENGTH, HOP number and OFFSET are stored as integers, also obfuscated with differents randomly generated keys (CvarMasked).
    Using the same process, a randomly generated KEY is used to reverse the obfuscation of the stored VALUE.
    

    CvarObfuscated, VALUE and KEY are stored this way:

    KEY and VALUE arrays are the same length.

    +--- KEY or VALUE POINTER (m_mvKeyPtr or m_mvValPtr)
    |
    |       +--- KEY or VALUE OFFSET (m_mvKeyOffset or m_mvValOffset)
    |       |
    v       v
    +--...--+--------------+--...--+
    | NOISE | KEY or VALUE | NOISE |
    +--...--+--------------+--...--+
             <-+---------->
               | 
               | KEY or VALUE LENGTH (m_mvKeySize or m_mvValSize)


    CvarMasked, VALUE and KEY specifications are stored this way:

    KEY is the same length of VALUE.
    VALUE uses KEY and the xor logical operation to be obfuscated.

    +----...----+
    |   VALUE   |
    +----...----+
    |    KEY    |
    +----...----+
*/


#pragma once

#include <iostream>
#include <mutex>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <memory>
#include <type_traits>
#include <map>


/*
** CvarMasked
* Obfuscate pointers addresses or CvarObfuscated's values (int32_t or uinptr_t)
*/
template <typename T>
class CvarMasked {
public:
    // Setter
    void set(T _val) {
        m_msk = rand() % std::numeric_limits<T>::max();
        m_val = _val ^ m_msk;
    }

    // Getter
    T get() {
        T val(m_val);
        val ^= m_msk;
        return val;
    }
    
private:
    T m_val, // Stored value
      m_msk; // Stored mask
};


/*
** CvarObfuscated
* Obfuscate variables or structs from memory scanners
*/
template <typename T>
class CvarObfuscated {
public:
    // Destructor
    ~CvarObfuscated() {
        _flush();
    }

    // Assign a value (CvarObfuscated = rval)
    T operator=(const T &_val) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        _set(_val);
        return _val;
    }

    // Get the value (lval = CvarObfuscated)
    operator T() {
        const std::lock_guard<std::mutex> lock(m_mtx);
        return _get();
    }

    // Compound operator addition +=
    T operator += (const T &_val) {
        if constexpr (std::is_same_v<T, std::string>) {
            const std::lock_guard<std::mutex> lock(m_mtx);
            std::string val(_get());
            val.append(_val);
            _set(val);
            return val;
        }
        else {
            const std::lock_guard<std::mutex> lock(m_mtx);
            T val(_get());
            val += _val;
            _set(val);
            return val;
        }
    }

    // Compound operator substract -=
    T operator -= (const T &_val) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get());
        val -= _val;
        _set(val);
        return val;
    }

    // Compound operator multiply *=
    T operator *= (const T &_val) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get());
        val *= _val;
        _set(val);
        return val;
    }

    // Compound operator divide /=
    T operator /= (const T &_val) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get());
        val /= _val;
        _set(val);
        return val;
    }

    // Compound bitwise operator |=
    T operator |= (int _iMask) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get());
        val |= _iMask;
        _set(val);
        return val;
    }

    // Compound bitwise operator &=
    T operator &= (int _iMask) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get());
        val &= _iMask;
        _set(val);
        return val;
    }

    // Compound bitwise operator ^=
    T operator ^= (int _iMask) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get());
        val ^= _iMask;
        _set(val);
        return val;
    }

    // Unary operator addition +
    T operator + (const T &_val) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get() + _val);
        return val;
    }

    // Unary operator substract -
    T operator - (const T &_val) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get() - _val);
        return val;
    }

    // Unary operator multiply *
    T operator * (const T &_val) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get() * _val);
        return val;
    }

    // Unary operator divide /
    T operator / (const T &_val) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get() / _val);
        return val;
    }

    // Bitwise operator |
    T operator | (int _iMask) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get() | _iMask);
        return val;
    }

    // Bitwise operator &
    T operator & (int _iMask) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get() & _iMask);
        return val;
    }

    // Bitwise operator ^
    T operator ^ (int _iMask) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get() ^ _iMask);
        return val;
    }

    // Unary prefix incrementation operator ++
    T operator ++ () {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get());
        ++val;
        _set(val);
        return val;
    }

    // Unary postfix incrementation operator ++
    T operator ++ (int) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get());
        val++;
        _set(val);
        return val;
    }

    // Unary prefix decrementation operator --
    T operator -- () {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get());
        --val;
        _set(val);
        return val;
    }

    // Unary postfix decrementation operator --
    T operator -- (int) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get());
        val--;
        _set(val);
        return val;
    }

    // Shift bitwise operator <<
    T operator << (int _i) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get() << _i);
        return val;
    }

    // Shift bitwise operator >>
    T operator >> (int _i) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get() >> _i);
        return val;
    }


    template<class R> struct is_vector : public std::false_type {};
    template<class R> struct is_map : public std::false_type {};

    template<class R, class Alloc>
    struct is_vector<std::vector<R, Alloc>> : public std::true_type {};

    template<class R, class S, class Alloc>
    struct is_map<std::map<R, S, Alloc>> : public std::true_type {};

    // Comparison operator ==
    bool operator == (const T &_vr) {
        if constexpr (is_vector<T>::value) {
            const std::lock_guard<std::mutex> lock(m_mtx);
            T vl(_get());
            T vr(_vr);
            return (vl == vr);
        }
        else if constexpr (is_map<T>::value) {
            const std::lock_guard<std::mutex> lock(m_mtx);
            T vl(_get());
            T vr(_vr);
            return(vl == vr);
        }
        else {
            const std::lock_guard<std::mutex> lock(m_mtx);
            int iSize(sizeof(T));
            T vl(_get());
            T vr(_vr);
            return (::memcmp(&vl, &vr, iSize) == 0);
        }
    }

    // Comparison operator !=
    bool operator != (const T &_vr) {
        if constexpr (is_vector<T>::value) {
            const std::lock_guard<std::mutex> lock(m_mtx);
            T vl(_get());
            T vr(_vr);
            return (vl != vr);
        }
        else if constexpr (is_map<T>::value) {
            const std::lock_guard<std::mutex> lock(m_mtx);
            T vl(_get());
            T vr(_vr);
            return(vl != vr);
        }
        else {
            const std::lock_guard<std::mutex> lock(m_mtx);
            int iSize(sizeof(T));
            T vl(_get());
            T vr(_vr);
            return (::memcmp(&vl, &vr, iSize) != 0);
        }
    }

private:
    // Setter
    void _set(T _val) {
        // If it is the first initialization
        if (!m_bEmpty)
            // Erase all data and dynamic arrays
            _flush();
        else
            m_bEmpty = false;

        // Generate a key with the same byte size as the variable
        _genKey();
        // Calculate the size of the bytes of the value
        int iSize(_sizeVal<T>(_val));
        // Cast the variable and store it into a byte array
        _castVal<T>(_val, iSize);
    }

    // Getter
    T _get() {
        // Throws an error if the user tries to get a value before initialization
        if (m_bEmpty) throw("The obfuscated variable has not yet been initialized.");

        // Retrieve stored value and key specifications
        int iValSize(m_mvValSize.get()),
            iValOffset(m_mvValOffset.get()),
            iKeySize(m_mvKeySize.get()),
            iKeyOffset(m_mvKeyOffset.get());

        // Cast the value and the key addresses integers to working pointers
        uint8_t *ptrValBuff(_ptrUnfold(m_mvValPtr, m_mvValHopNbr)),
                *ptrKeyBuff(_ptrUnfold(m_mvKeyPtr, m_mvKeyHopNbr));

        // Shift the pointer positions to their payloads
        ptrValBuff += iValOffset;
        ptrKeyBuff += iKeyOffset;

        // Retrieve the obfuscated value
        uint8_t *ui8ValBuff(new uint8_t[iValSize]);
        ::memset(ui8ValBuff, 0, iValSize);
        ::memcpy(ui8ValBuff, ptrValBuff, iValSize);

        // Proceed for each byte of the value to a xor logical operation with the key
        for (int i(0); i < iValSize; ++i)
            ui8ValBuff[i] ^= ptrKeyBuff[i % iKeySize];

        // Cast the deobfuscated array to a variable of the expected type
        T val;
        _return<T>(&val, ui8ValBuff, iValSize);

        // Release
        delete[] ui8ValBuff;

        // 
        return val;
    }

    template <typename T>
    void _return(T *_val, uint8_t *_ui8ValBuff, int _iValSize) {
        ::memcpy(_val, _ui8ValBuff, _iValSize);
    }

    template <>
    void _return<std::string>(std::string *_val, uint8_t *_ui8ValBuff, int _iValSize) {
        _val->assign(reinterpret_cast<char *>(_ui8ValBuff), _iValSize);
    }

    template<typename T, typename R>
    void _return(std::vector<R> *_val, uint8_t *_ui8ValBuff, int _iValSize) {
        int iIndex(0),
            iTypeSize(sizeof(R));

        while (iIndex < _iValSize) {
            R var;
            ::memcpy(&var, _ui8ValBuff + iIndex, iTypeSize);
            _val->push_back(var);
            iIndex += iTypeSize;
        }
    }

    template<typename T, typename R, typename S>
    void _return(std::map<R, S> *_val, uint8_t *_ui8ValBuff, int _iValSize) {
        int iTypeSizeR(sizeof(R)),
            iTypeSizeS(sizeof(S)),
            iIndex(0);

        while (iIndex < _iValSize) {
            R key;
            ::memcpy(&key, _ui8ValBuff + iIndex, iTypeSizeR);
            iIndex += iTypeSizeR;

            S var;
            ::memcpy(&var, _ui8ValBuff + iIndex, iTypeSizeS);
            iIndex += iTypeSizeS;

            _val->insert(std::pair<R, S>(key, var));
        }
    }
    
    void _flush() {
        _ptrFlush(m_mvValPtr, m_mvValHopNbr);
        _ptrFlush(m_mvKeyPtr, m_mvKeyHopNbr);
        m_mvKeySize.set(0);
        m_mvKeyOffset.set(0);
        m_mvValSize.set(0);
        m_mvValOffset.set(0);
        m_mvKeyHopNbr.set(0);
        m_mvValHopNbr.set(0);
        m_mvKeyPtr.set(0);
        m_mvValPtr.set(0);
    }

    template<typename T>
    int _sizeVal(T _val) {
        return sizeof(_val);
    }

    template<>
    int _sizeVal<std::string>(std::string _val) {
        return static_cast<int>(_val.size());
    }

    template<>
    int _sizeVal<const char *>(const char *_val) {
        return static_cast<int>(::strlen(_val));
    }

    template<typename T, typename R>
    int _sizeVal(std::vector<R> _val) {
        return static_cast<int>(_val.size() * sizeof(R));
    }

    template<typename T, typename R, typename S>
    int _sizeVal(std::map<R, S> _val) {
        return static_cast<int>(_val.size() * (sizeof(R) + sizeof(S)));
    }

    void _genKey() {
        int iKeySize(::rand() % 32 + 32),
            iKeyOffset(::rand() % 24 + 8),
            iAllocSize(iKeySize + iKeyOffset + 8 + ::rand() % 24);
        uint8_t ui8KeyHopNbr(::rand() % 7 + 1);

        m_mvKeyOffset.set(iKeyOffset);
        m_mvKeySize.set(iKeySize);
        m_mvKeyHopNbr.set(ui8KeyHopNbr);
        
        uint8_t *ui8KeyBuff(new uint8_t[iAllocSize]);
        ::memset(ui8KeyBuff, 0, iAllocSize);

        for (int i(0); i < iAllocSize; ++i)
            ui8KeyBuff[i] = ::rand() % 256;
        
        _ptrFold(ui8KeyHopNbr, m_mvKeyPtr, ui8KeyBuff);
    }

    template<typename T>
    void _castVal(T _val, int _iSize) {
        uint8_t *ui8ValBuff(new uint8_t[_iSize]);
        ::memset(ui8ValBuff, 0, _iSize);
        ::memcpy(ui8ValBuff, &_val, _iSize);

        _copyVal(ui8ValBuff, _iSize);

        delete[] ui8ValBuff;
    }
    
    template <>
        void _castVal<std::string>(std::string _str, int _iSize) {
        uint8_t *ui8ValBuff(new uint8_t[_iSize]);
        ::memset(ui8ValBuff, 0, _iSize);
        ::memcpy(ui8ValBuff, _str.data(), _iSize);

        _copyVal(ui8ValBuff, _iSize);

        delete[] ui8ValBuff;
    }

    template <>
    void _castVal<const char *>(const char *_str, int _iSize) {
        uint8_t *ui8ValBuff(new uint8_t[_iSize]);
        ::memset(ui8ValBuff, 0, _iSize);
        ::memcpy(ui8ValBuff, _str, _iSize);

        _copyVal(ui8ValBuff, _iSize);

        delete[] ui8ValBuff;
    }

    template<typename T, typename R>
    void _castVal(std::vector<R> _val, int _iSize) {
        uint8_t *ui8ValBuff(new uint8_t[_iSize]);
        ::memset(ui8ValBuff, 0, _iSize);

        int iSizeType(sizeof(R));
        for (int i(0); i < _val.size(); ++i)
            ::memcpy(ui8ValBuff + (i * iSizeType), &_val[i], iSizeType);

        _copyVal(ui8ValBuff, _iSize);

        delete[] ui8ValBuff;
    }

    template<typename T, typename R, typename S>
    void _castVal(std::map<R, S> _val, int _iSize) {
        uint8_t *ui8ValBuff(new uint8_t[_iSize]);
        ::memset(ui8ValBuff, 0, _iSize);

        int iSizeTypeR(sizeof(R)),
            iSizeTypeS(sizeof(S)),
            iIndex(0);
        
        for (const auto &[key, value] : _val) {
            ::memcpy(ui8ValBuff + iIndex, &key, iSizeTypeR);
            iIndex += iSizeTypeR;
            ::memcpy(ui8ValBuff + iIndex, &value, iSizeTypeS);
            iIndex += iSizeTypeS;
        }

        _copyVal(ui8ValBuff, _iSize);

        delete[] ui8ValBuff;
    }

    void _copyVal(uint8_t *_ptrBuff, int _iValSize) {
        int iValOffset(::rand() % 24 + 8),
            iValSize(_iValSize + iValOffset + 8 + ::rand() % 24);
        uint8_t ui8ValHopNbr(::rand() % 7 + 1);
        
        m_mvValOffset.set(iValOffset);
        m_mvValSize.set(_iValSize);
        m_mvValHopNbr.set(ui8ValHopNbr);

        uint8_t *ui8ValBuff(new uint8_t[iValSize]);
        ::memset(ui8ValBuff, 0, iValSize);

        for (int i(0); i < iValOffset; ++i)
            ui8ValBuff[i] = ::rand() % 256;

        for (int i(iValOffset + iValSize); i < _iValSize; ++i)
            ui8ValBuff[i] = ::rand() % 256;

        ::memcpy(ui8ValBuff + iValOffset, _ptrBuff, _iValSize);

        int iKeyOffset(m_mvKeyOffset.get()),
            iKeySize(m_mvKeySize.get());

        uint8_t *ui8KeyBuff(_ptrUnfold(m_mvKeyPtr, m_mvKeyHopNbr));

        for (int i(0); i < _iValSize; ++i)
            ui8ValBuff[iValOffset + i] ^= ui8KeyBuff[iKeyOffset + (i % iKeySize)];

        _ptrFold(ui8ValHopNbr, m_mvValPtr, ui8ValBuff);
    }

    void _ptrFlush(CvarMasked<uintptr_t> &_mvPtr, CvarMasked<uint8_t> &_mvHopNbr) {
        uintptr_t *uiPtrCurr(reinterpret_cast<uintptr_t *>(_mvPtr.get()));
        uint8_t ui8HopNbr(_mvHopNbr.get());

        for (uint8_t i(0); i < ui8HopNbr; ++i) {
            uintptr_t *uiptrDel(uiPtrCurr);
            uiPtrCurr = reinterpret_cast<uintptr_t *>(*uiPtrCurr);
            delete uiptrDel;
        }

        delete[] uiPtrCurr;
        uiPtrCurr = nullptr;
    }

    void _ptrFold(uint8_t _ui8HopNbr, CvarMasked<uintptr_t> &_mvVal, uint8_t *_ui8ptrBuff) {
        uintptr_t addHopLast(0);
        
        uintptr_t *ptrTempInit(new uintptr_t);
        addHopLast = reinterpret_cast<uintptr_t>(ptrTempInit);
        _mvVal.set(addHopLast);

        for (int i(0); i < _ui8HopNbr - 1; ++i) {
            uintptr_t *ptrTemp(new uintptr_t),
                      *ptrHopLast(reinterpret_cast<uintptr_t *>(addHopLast));
            addHopLast = reinterpret_cast<uintptr_t>(ptrTemp);
            *ptrHopLast = addHopLast;
        }

        uintptr_t *ptrTempLast(reinterpret_cast<uintptr_t*>(addHopLast));
        *ptrTempLast = reinterpret_cast<uintptr_t>(_ui8ptrBuff);
    }

    uint8_t *_ptrUnfold(CvarMasked<uintptr_t> &_mvPtr, CvarMasked<uint8_t> &_mvHopNbr) {
        uintptr_t *uiPtr(reinterpret_cast<uintptr_t*>(_mvPtr.get()));
        uint8_t ui8HopNbr(_mvHopNbr.get());

        for (uint8_t i(0); i < ui8HopNbr; ++i)
            uiPtr = reinterpret_cast<uintptr_t*>(*uiPtr);

        return reinterpret_cast<uint8_t*>(uiPtr);
    }

    std::mutex                 m_mtx;
    CvarMasked<uint32_t>       m_mvKeySize,
                               m_mvKeyOffset,
                               m_mvValSize,
                               m_mvValOffset;
    CvarMasked<uint8_t>        m_mvKeyHopNbr,
                               m_mvValHopNbr;
    CvarMasked<uintptr_t>      m_mvKeyPtr,
                               m_mvValPtr;
    std::atomic<bool>          m_bEmpty = true;
};
