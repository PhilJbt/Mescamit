/*
* MESCAMIT - VARIABLE MEMORY SCANNER MITIGATION
* Philippe Jaubert - 2022
* https://github.com/PhilJbt/mescamit
* MIT License
*/



/*

**
** HOW THE VALUE IS PROCESSED
*
* How a VALUE is paired with a random key.
* class CvarObfuscated
**

    The CvarObfuscated class stores a VALUE into an obfuscated array,
    and generated a randomly generated KEY of a random length for byte to byte XORing.
    
    Two random length sequences with data NOISE are added as prefix and postfix of the stored VALUE.
    
    Also, the address POINTER stored does not point directly to the VALUE memory buffer,
    but point to several other pointers (as a linked list, each node is called HOP). The last of the list is the VALUE or KEY array.
    
    Using the same process, a randomly generated KEY is used to reverse the obfuscation of the stored VALUE.

    The stored VALUE is never directly deobfuscated, only the returned value is.

    Four informations are stored for each VALUE and KEY (CvarMasked).
        1. The POINTER address casted to an integer (m_mvValPtr and m_mvKeyPtr)
        2. The number of HOP from the first pointer to the VALUE or KEY memory buffer (m_mvValHopNbr and m_mvKeyHopNbr)
        3. The OFFSET in bytes between the pointing POINTER  (m_mvValOffset and m_mvKeyOffset)
        4. The LENGTH in bytes of the array (m_mvValSize and m_mvKeySize)

        +--- 1. KEY or VALUE POINTER (m_mvKeyPtr or m_mvValPtr)
        |
        |                  +--- 2. HOP number (m_mvValHopNbr and m_mvKeyHopNbr)
        |                  |
        |                  |                    +--- 4. KEY or VALUE OFFSET (m_mvKeyOffset or m_mvValOffset)
        |                  |                    |
        |                  |                    v
        v                  v            +--...--+--------------+--...--+
        POINTER -> HOP -> ... -> HOP -> | NOISE | KEY or VALUE | NOISE |
                                        +--...--+--------------+--...--+
                                                <-+---------->
                                                  | 3. KEY or VALUE LENGTH (m_mvKeySize or m_mvValSize)


**
** HOW THE SPECIFICATIONS OF THE VALUE AND THE KEY ARE STORED
* 
* How the POINTER, the LENGTH, the HOP number and the OFFSET of a value or key are stored.
* class CvarMasked
**

    KEY is the same length of VALUE, both are builtin type (mostly uintptr_t or uint32_t).
    VALUE uses KEY and the XOR ^ logical operation to be obfuscated / deobfuscated.

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
* Obfuscate pointers addresses or specifications (i.e. length, offset, hop number) (int32_t or uinptr_t)
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

    // Getter
    operator T() {
        const std::lock_guard<std::mutex> lock(m_mtx);
        return _get();
    }


    /*
    ** Assignment Operators
    */

    // = Assignation
    T operator=(const T &_val) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        _set(_val);
        return _val;
    }


    /*
    ** Arithmetic Operators
    */

    // + Addition
    T operator + (const T &_val) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        return (_get() + _val);
    }

    // - Subtraction
    T operator - (const T &_val) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        return (_get() - _val);
    }

    // * Multiplication
    T operator * (const T &_val) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        return (_get() * _val);
    }

    // / Division
    T operator / (const T &_val) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        return (_get() / _val);
    }

    // % Modulo operation (Remainder after division)
    T operator % (const T &_val) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        return (_get() % _val);
    }


    /*
    ** Arithmetic Compound Assignment Operators
    */

    // += Addition
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

    // -= Subtraction
    T operator -= (const T &_val) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get() - _val);
        _set(val);
        return val;
    }

    // *= Division
    T operator *= (const T &_val) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get() * _val);
        _set(val);
        return val;
    }

    // /= Division
    T operator /= (const T &_val) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get() / _val);
        _set(val);
        return val;
    }


    /**
    ** Bitwise Operators
    */

    // & Bitwise AND
    T operator & (int _iMask) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        return (_get() & _iMask);
    }

    // | Bitwise OR
    T operator | (int _iMask) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        return (_get() | _iMask);
    }

    // ^ Bitwise XOR
    T operator ^ (int _iMask) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get() ^ _iMask);
        return val;
    }
    
    // << Bitwise shift left
    T operator << (int _i) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        return (_get() << _i);
    }

    // >> Bitwise shift right
    T operator >> (int _i) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        return (_get() >> _i);
    }


    /**
    ** Bitwise Compound Assignment Operators
    */

    // &= Bitwise Compound Assignment AND
    T operator &= (int _iMask) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get() & _iMask);
        _set(val);
        return val;
    }

    // ^= Bitwise Compound Assignment XOR
    T operator ^= (int _iMask) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get() ^ _iMask);
        _set(val);
        return val;
    }

    // |= Bitwise Compound Assignment OR
    T operator |= (int _iMask) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get() | _iMask);
        _set(val);
        return val;
    }


    /*
    ** Increment and Decrement Operators
    */

    // ++ Increment prefix
    T operator ++ () {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get() + 1);
        _set(val);
        return val;
    }

    // ++ Increment postfix
    T operator ++ (int) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get() + 1);
        _set(val);
        return val;
    }

    // -- Decrement prefix
    T operator -- () {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get() - 1 );
        _set(val);
        return val;
    }

    // -- Decrement postfix
    T operator -- (int) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get() - 1);
        _set(val);
        return val;
    }


    /*
    ** Relational Operators
    */

    template<class R> struct is_vector : public std::false_type {};
    template<class R> struct is_map : public std::false_type {};

    template<class R, class Alloc>
    struct is_vector<std::vector<R, Alloc>> : public std::true_type {};

    template<class R, class S, class Alloc>
    struct is_map<std::map<R, S, Alloc>> : public std::true_type {};

    // == Is equal to
    bool operator == (const T &_vr) {
        // If the right value is a std::vector
        if constexpr (is_vector<T>::value) {
            const std::lock_guard<std::mutex> lock(m_mtx);
            return (_get() == _vr);
        }
        // If the right value is a std::map
        else if constexpr (is_map<T>::value) {
            const std::lock_guard<std::mutex> lock(m_mtx);
            return(_get() == _vr);
        }
        // If the right value can be compared byte to byte
        else {
            const std::lock_guard<std::mutex> lock(m_mtx);
            int iSize(sizeof(T));
            T vl(_get());
            return (::memcmp(&vl, &_vr, iSize) == 0);
        }
    }

    // != Not equal to
    bool operator != (const T &_vr) {
        // If the right value is a std::vector
        if constexpr (is_vector<T>::value) {
            const std::lock_guard<std::mutex> lock(m_mtx);
            return (_get() != _vr);
        }
        // If the right value is a std::map
        else if constexpr (is_map<T>::value) {
            const std::lock_guard<std::mutex> lock(m_mtx);
            return(_get() != _vr);
        }
        // If the right value can be compared byte to byte
        else {
            const std::lock_guard<std::mutex> lock(m_mtx);
            int iSize(sizeof(T));
            T vl(_get());
            return (::memcmp(&vl, &_vr, iSize) != 0);
        }
    }

private:
    /*
    ** Setter and Getter
    */

    // Setter
    void _set(T _val) {
        // If it is the first initialization
        if (!m_bEmpty) _flush(); // Erase all data and dynamic arrays
        else m_bEmpty = false;

        // Generate a key with the same byte size as the variable
        _genKey();
        // Calculate the size of the bytes of the value
        int iSize(_sizeVal<T>(_val));
        // Cast the variable and store it into a byte array
        uint8_t *ui8ValBuff(_castVal<T>(_val, iSize));
        // Obfuscate the byte array
        _obfuscateVal(ui8ValBuff, iSize);
        // Packageing the byte array
        _copyVal(ui8ValBuff, iSize);
        // Release the byte array
        delete[] ui8ValBuff;
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

        // Return the deobfuscated value
        return val;
    }


    /*
    ** Core
    */

    // Obfuscate a value in the format of an array of bytes
    void _obfuscateVal(uint8_t *_ptrBuff, int _iValSize) {
        // Retrieve the offset between the pointer and position of the key
        // and the size of the key
        int iKeyOffset(m_mvKeyOffset.get()),
            iKeySize(m_mvKeySize.get());

        // Get a working pointer pointing to the key
        uint8_t *ui8KeyBuff(_ptrUnfold(m_mvKeyPtr, m_mvKeyHopNbr));

        // Obfuscate the temporary byte array of the value
        for (int i(0); i < _iValSize; ++i)
            _ptrBuff[i] ^= ui8KeyBuff[iKeyOffset + (i % iKeySize)];
    }

    // Process a value in the format of an array of bytes,
    // calculate or define its specifications (i.e. value length, noise around, etc),
    // and XOR this array of bytes
    void _copyVal(uint8_t *_ptrBuff, int _iValSize) {
        // Define two random offset, and calculate the total size of the memory buffer
        int iValOffset(::rand() % 24 + 8),
            iValSize(_iValSize + iValOffset + 8 + ::rand() % 24);
        // Define a random number of element of the linked list of pointers (hops)
        uint8_t ui8ValHopNbr(::rand() % 7 + 1);

        // Store in obfuscated variables those defined or calculated specifications
        m_mvValOffset.set(iValOffset);
        m_mvValSize.set(_iValSize);
        m_mvValHopNbr.set(ui8ValHopNbr);

        // Declare and initialize a dynamic array of bytes to store the obfuscated value
        uint8_t *ui8ValBuff(new uint8_t[iValSize]);
        ::memset(ui8ValBuff, 0, iValSize);

        // Populate the sequence before the value with random noise data
        for (int i(0); i < iValOffset; ++i)
            ui8ValBuff[i] = ::rand() % 256;

        // Populate the sequence after the value with random noise data
        for (int i(iValOffset + iValSize); i < _iValSize; ++i)
            ui8ValBuff[i] = ::rand() % 256;

        // Copy the obfuscated value to the array of bytes
        ::memcpy(ui8ValBuff + iValOffset, _ptrBuff, _iValSize);

        // Create a linked list of pointers, the last pointing to the array of bytes
        _ptrFold(ui8ValHopNbr, m_mvValPtr, ui8ValBuff);
    }

    // Create a linked list of pointers with several hops,
    // from the stored address to the memory buffer of the value or key
    void _ptrFold(uint8_t _ui8HopNbr, CvarMasked<uintptr_t> &_mvVal, uint8_t *_ui8ptrBuff) {
        // Declare and initialize the keeper of the last element of the linked list
        uintptr_t addHopLast(0);

        // Declare and initialize a working pointer of the last stored address,
        // actually the beginning of the linked list
        uintptr_t *ptrHopLast(new uintptr_t);

        // Retrieve and store the memory address of the first element of the linked list
        addHopLast = reinterpret_cast<uintptr_t>(ptrHopLast);
        _mvVal.set(addHopLast);

        // For the number of hops defined before
        for (int i(0); i < _ui8HopNbr - 1; ++i) {
            // Declare and initialize a new pointer
            uintptr_t *ptrTemp(new uintptr_t),
            // Update the address of the last element of the linked list with the new created
            *ptrHopLast = reinterpret_cast<uintptr_t *>(addHopLast);
            // Update the stored address of the last element of the linked list with the new created
            addHopLast = reinterpret_cast<uintptr_t>(ptrTemp);
            // Make pointing the last linked element of the linked list to the new created node
            *ptrHopLast = addHopLast;
        }

        // Get the last node
        uintptr_t *ptrTempLast(reinterpret_cast<uintptr_t *>(addHopLast));
        // Make it pointing to the array of bytes of the value or key
        *ptrTempLast = reinterpret_cast<uintptr_t>(_ui8ptrBuff);
    }

    // Unravel the linked list of pointers with several hops,
    // from the stored address to the memory buffer of the value or key
    uint8_t *_ptrUnfold(CvarMasked<uintptr_t> &_mvPtr, CvarMasked<uint8_t> &_mvHopNbr) {
        // Declare and initialize the pointer used to unfold the linked list of pointers
        uintptr_t *uiPtr(reinterpret_cast<uintptr_t *>(_mvPtr.get()));
        // Retrieve the number of hops
        uint8_t ui8HopNbr(_mvHopNbr.get());

        // Jump from a pointer to another
        for (uint8_t i(0); i < ui8HopNbr; ++i)
            uiPtr = reinterpret_cast<uintptr_t *>(*uiPtr);

        // Return the last pointer of the linked list
        return reinterpret_cast<uint8_t *>(uiPtr);
    }

    // Reset all value's and key's specifications, and release their memory buffers
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

    // Release every hops of the linked list, and the memory buffer of the value
    void _ptrFlush(CvarMasked<uintptr_t> &_mvPtr, CvarMasked<uint8_t> &_mvHopNbr) {
        // Declare and initialize the pointer used to unfold the linked list of pointers
        uintptr_t *uiPtrCurr(reinterpret_cast<uintptr_t *>(_mvPtr.get()));
        // Retrieve the number of hops
        uint8_t ui8HopNbr(_mvHopNbr.get());

        // Jump from a pointer to another
        for (uint8_t i(0); i < ui8HopNbr; ++i) {
            // Store the current address to a temporary pointer
            uintptr_t *uiptrDel(uiPtrCurr);
            // Go to the next pointer
            uiPtrCurr = reinterpret_cast<uintptr_t *>(*uiPtrCurr);
            // Release the memory of the temporary pointer
            delete uiptrDel;
        }

        // Delete the first stored node of the linked list
        delete[] uiPtrCurr;
        uiPtrCurr = nullptr;
    }

    // Generate a key that will be used to obfuscate the stored value
    void _genKey() {
        // Retrieve the offset between the pointer and position of the key
        // and the size of the key, and the allocated memory of the whole key package
        int iKeyOffset(::rand() % 24 + 8),
            iKeySize(::rand() % 32 + 32),
            iAllocSize(iKeySize + iKeyOffset + 8 + ::rand() % 24);
        // Define a random number of element of the linked list of pointers (hops)
        uint8_t ui8KeyHopNbr(::rand() % 7 + 1);

        // Store in obfuscated variables those defined or calculated specifications
        m_mvKeyOffset.set(iKeyOffset);
        m_mvKeySize.set(iKeySize);
        m_mvKeyHopNbr.set(ui8KeyHopNbr);

        // Declare and initialize a dynamic array of bytes to store the key
        uint8_t *ui8KeyBuff(new uint8_t[iAllocSize]);
        ::memset(ui8KeyBuff, 0, iAllocSize);

        // Populate the memory buffer with random values (whose a sequence will be used as a key)
        for (int i(0); i < iAllocSize; ++i)
            ui8KeyBuff[i] = ::rand() % 256;

        // Create a linked list of pointers, the last pointing to the array of bytes
        _ptrFold(ui8KeyHopNbr, m_mvKeyPtr, ui8KeyBuff);
    }
    

    /*
    ** Template specialization of the returning data type
    */

    // Returns the majority of builtin types
    template <typename T>
    void _return(T *_val, uint8_t *_ui8ValBuff, int _iValSize) {
        ::memcpy(_val, _ui8ValBuff, _iValSize);
    }

    // Returns a std::string
    template <>
    void _return<std::string>(std::string *_val, uint8_t *_ui8ValBuff, int _iValSize) {
        _val->assign(reinterpret_cast<char *>(_ui8ValBuff), _iValSize);
    }

    // Returns a std::vector
    template<typename T, typename R>
    void _return(std::vector<R> *_val, uint8_t *_ui8ValBuff, int _iValSize) {
        // Get the size of the type of the value
        int iTypeSize(sizeof(R)),
        // Declare and initialize the current index of the byte array
            iIndex(0);

        // For each elements of the std::vector
        while (iIndex < _iValSize) {
            // Declare a temporary value variable
            R var;
            // Retrieve the data from the memory buffer corresponding to the next value
            ::memcpy(&var, _ui8ValBuff + iIndex, iTypeSize);
            // Update the current index of the byte array with the current value size
            iIndex += iTypeSize;
            // Insert this value in the std::vector to return
            _val->push_back(var);
        }
    }

    // Returns a std::map
    template<typename T, typename R, typename S>
    void _return(std::map<R, S> *_val, uint8_t *_ui8ValBuff, int _iValSize) {
        // Get the size of the type of the key
        int iTypeSizeR(sizeof(R)),
        // Get the size of the type of the value
            iTypeSizeS(sizeof(S)),
        // Declare and initialize the current index of the byte array
            iIndex(0);

        // For each elements of the std::map
        while (iIndex < _iValSize) {
            // Declare a temporary key variable
            R key;
            // Retrieve the data from the memory buffer corresponding to the next key
            ::memcpy(&key, _ui8ValBuff + iIndex, iTypeSizeR);
            // Update the current index of the byte array with the current key size
            iIndex += iTypeSizeR;

            // Declare a temporary value variable
            S var;
            // Retrieve the data from the memory buffer corresponding to the next value
            ::memcpy(&var, _ui8ValBuff + iIndex, iTypeSizeS);
            // Update the current index of the byte array with the current value size
            iIndex += iTypeSizeS;

            // Insert this pair of key/value in the std::map to return
            _val->insert(std::pair<R, S>(key, var));
        }
    }


    /*
    ** Template specialization of the value size calculation
    */

    // Calculate the bytes size of the majority of the builtin types
    template<typename T>
    int _sizeVal(T _val) {
        return sizeof(_val);
    }

    // Calculate the bytes size of a std::string
    template<>
    int _sizeVal<std::string>(std::string _val) {
        return static_cast<int>(_val.size());
    }

    // Calculate the bytes size of a const char* string of characters
    template<>
    int _sizeVal<const char *>(const char *_val) {
        return static_cast<int>(::strlen(_val));
    }

    // Calculate the bytes size of a std::vector
    template<typename T, typename R>
    int _sizeVal(std::vector<R> _val) {
        return static_cast<int>(_val.size() * sizeof(R));
    }

    // Calculate the bytes size of a std::map
    template<typename T, typename R, typename S>
    int _sizeVal(std::map<R, S> _val) {
        return static_cast<int>(_val.size() * (sizeof(R) + sizeof(S)));
    }


    /*
    ** Template specialization of the casting
    ** from the value's type to a byte array
    */

    // Cast the majority of the builtin types to a byte array
    template<typename T>
    uint8_t *_castVal(T _val, int _iSize) {
        // Populate the bytes array with the value
        uint8_t *ui8ValBuff(new uint8_t[_iSize]);
        ::memset(ui8ValBuff, 0, _iSize);
        ::memcpy(ui8ValBuff, &_val, _iSize);

        // Return the casted builtin type in a bytes array format
        return ui8ValBuff;
    }

    // Cast a std::string to a byte array
    template <>
    uint8_t *_castVal<std::string>(std::string _str, int _iSize) {
        // Populate the bytes array with the value
        uint8_t *ui8ValBuff(new uint8_t[_iSize]);
        ::memset(ui8ValBuff, 0, _iSize);
        ::memcpy(ui8ValBuff, _str.data(), _iSize);

        // Return the casted std::string in a bytes array format
        return ui8ValBuff;
    }

    // Cast a const char* string of characters to a byte array
    template <>
    uint8_t *_castVal<const char *>(const char *_str, int _iSize) {
        // Populate the bytes array with the value
        uint8_t *ui8ValBuff(new uint8_t[_iSize]);
        ::memset(ui8ValBuff, 0, _iSize);
        ::memcpy(ui8ValBuff, _str, _iSize);

        // Return the casted const char * in a bytes array format
        return ui8ValBuff;
    }

    // Cast a std::vector to a byte array
    template<typename T, typename R>
    uint8_t *_castVal(std::vector<R> _val, int _iSize) {
        // Declare and initialize the memory buffer to store the value
        uint8_t *ui8ValBuff(new uint8_t[_iSize]);
        ::memset(ui8ValBuff, 0, _iSize);

        // Get the size of the type stored in the std::vector
        int iSizeType(sizeof(R));

        // For each element of the std::vector
        for (int i(0); i < _val.size(); ++i)
            ::memcpy(ui8ValBuff + (i * iSizeType), &_val[i], iSizeType);

        // Return the casted std::vector in a bytes array format
        return ui8ValBuff;
    }

    // Cast a std::map to a byte array
    template<typename T, typename R, typename S>
    uint8_t *_castVal(std::map<R, S> _val, int _iSize) {
        // Declare and initialize the memory buffer to store the value
        uint8_t *ui8ValBuff(new uint8_t[_iSize]);
        ::memset(ui8ValBuff, 0, _iSize);

        // Get the size of the type of the key
        int iSizeTypeR(sizeof(R)),
        // Get the size of the type of the value
            iSizeTypeS(sizeof(S)),
        // Declare and initialize the current index of the byte array
            iIndex(0);

        // For each element of the std::map
        for (const auto &[key, value] : _val) {
            // Populate the bytes array with the next key
            ::memcpy(ui8ValBuff + iIndex, &key, iSizeTypeR);
            // Update the current index of the byte array with the current key size
            iIndex += iSizeTypeR;

            // Populate the bytes array with the next value
            ::memcpy(ui8ValBuff + iIndex, &value, iSizeTypeS);
            // Update the current index of the byte array with the current value size
            iIndex += iSizeTypeS;
        }

        // Return the casted std::map in a bytes array format
        return ui8ValBuff;
    }


    /*
    ** Member variables
    */

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
