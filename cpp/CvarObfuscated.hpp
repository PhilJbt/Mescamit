/*
* MESCAMIT - VARIABLE MEMORY SCANNER MITIGATION
* Philippe Jaubert - 2022
* https://github.com/PhilJbt/mescamit
* MIT License



**
** HOW THE VALUE IS PROCESSED
*
* How a VALUE is paired with a random key.
* class CvarObfuscated
**

    I. GENERAL
    
        The CvarObfuscated class stores a VALUE into an obfuscated array,
        and generated a random length KEY to byte XORing the VALUE.

        The stored VALUE is never directly deobfuscated, only the returned value is.

    
    II. VALUE and KEY specifications

        VALUE and KEY has both 4 specifications (i.e. a POINTER address, a number of HOPs, a LENGTH and an OFFSET).
        The 4 specifications are stored in a randomly ordered array of pointers, and are obfuscated with their unique XOR key.

        +--- A. KEY or VALUE POINTER (Especs_KeyPtr or Especs_ValPtr)
        |
        |                  +--- B. HOP number (Especs_ValHopNbr or Especs_KeyHopNbr)
        |                  |
        |                  |                    +--- C. OFFSET for the KEY or VALUE (Especs_ValOffset or Especs_KeyOffset)
        |                  |                    |
        |                  |                    v
        v                  v            +--...--+-----...------+--...--+
        POINTER -> HOP -> ... -> HOP -> | NOISE | KEY or VALUE | NOISE |
                                        +--...--+-----...------+--...--+
                                            ^    <--+--------->
                                            |       | D. LENGTH of the KEY or VALUE (Especs_ValSize or Especs_KeySize)
                                            |
                                            +--- E. NOISE (random data)

        A. The address POINTER, casted to an integer.

        B. The address POINTER stored does not point directly to the VALUE memory buffer,
           but point to several other pointers (as a linked list, each node is called HOP).
           The value of each pointer corresponds to the offset in bits to reach the next pointer address.
           The last of the list is the VALUE or KEY memory buffer.

        C. The OFFSET in bytes between the buffer POINTER and the start of the VALUE or KEY memory buffer

        D. The LENGTH in bytes of the VALUE or KEY memory buffer

        E. Two random length sequences with data NOISE are added as prefix and postfix of the stored VALUE.



**
** HOW THE SPECIFICATIONS OF THE VALUE AND THE KEY ARE STORED
* 
* How the POINTER, the LENGTH, the HOP number and the OFFSET of a value or key are stored.
* class CvarMasked
**

    KEY is the same length of VALUE, both are builtin type (mostly intptr_t or uint32_t).
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
        const std::lock_guard<std::mutex> lock(m_mtx);
        m_msk = rand() % std::numeric_limits<T>::max();
        m_val = _val ^ m_msk;
    }

    // Getter
    T get() {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(m_val);
        val ^= m_msk;
        return val;
    }
    
private:
    std::mutex m_mtx;
    T          m_val, // Stored value
               m_msk; // Stored mask
};


/*
** SspecsVal, SspecsKey
* 
*/

struct SspecsVal {
    CvarMasked<uint32_t> m_mvSize;
    CvarMasked<uint32_t> m_mvOffset;
    CvarMasked<uint8_t>  m_mvHopNbr;
    CvarMasked<intptr_t> m_mvPtr;
};

struct SspecsKey : public SspecsVal {
    CvarMasked<uint8_t> m_mvReadOfsset;
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
        const std::lock_guard<std::mutex> lock(m_mtx);
        _flush(true);
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
        const std::lock_guard<std::mutex> lock(m_mtx);
        if constexpr (std::is_same_v<T, std::string>) {
            std::string val(_get());
            val.append(_val);
            _set(val);
            return val;
        }
        else {
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
    T operator & (T _iMask) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        return (_get() & _iMask);
    }

    // | Bitwise OR
    T operator | (T _iMask) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        return (_get() | _iMask);
    }

    // ^ Bitwise XOR
    T operator ^ (T _iMask) {
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
    T operator &= (T _iMask) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get() & _iMask);
        _set(val);
        return val;
    }

    // ^= Bitwise Compound Assignment XOR
    T operator ^= (T _iMask) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        T val(_get() ^ _iMask);
        _set(val);
        return val;
    }

    // |= Bitwise Compound Assignment OR
    T operator |= (T _iMask) {
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
        const std::lock_guard<std::mutex> lock(m_mtx);
        // If the right value is a std::vector
        if constexpr (is_vector<T>::value) {
            return (_get() == _vr);
        }
        // If the right value is a std::map
        else if constexpr (is_map<T>::value) {
            return(_get() == _vr);
        }
        // If the right value can be compared byte to byte
        else {
            int iSize(sizeof(T));
            T vl(_get());
            return (::memcmp(&vl, &_vr, iSize) == 0);
        }
    }

    // != Not equal to
    bool operator != (const T &_vr) {
        const std::lock_guard<std::mutex> lock(m_mtx);
        // If the right value is a std::vector
        if constexpr (is_vector<T>::value) {
            return (_get() != _vr);
        }
        // If the right value is a std::map
        else if constexpr (is_map<T>::value) {
            return(_get() != _vr);
        }
        // If the right value can be compared byte to byte
        else {
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
        // If not empty, erase all data and dynamic arrays
        _flush();
        
        // Allocate the dynamic variables to store the value's and key's specifications
        _alloc();

        // Generate a key with the same byte size as the variable
        _genKey();

        // If first run, update the m_bEmpty boolean
        if (m_bEmpty) m_bEmpty = false;

        // Packageing the byte array
        _copyVal(_val);
    }

    // Getter
    T _get() {
        // Throws an error if the user tries to get a value before initialization
        if (m_bEmpty)
            _set(T());

        // Retrieve stored value and key specifications
        SspecsVal *ptrSpecsVal(reinterpret_cast<SspecsVal*>(_retrieveSpecs(Especs_::Especs_Val)));
        SspecsKey* ptrSpecsKey(reinterpret_cast<SspecsKey*>(_retrieveSpecs(Especs_::Especs_Key)));
        int iValSize(ptrSpecsVal->m_mvSize.get()),
            iValOffset(ptrSpecsVal->m_mvOffset.get()),
            iKeySize(ptrSpecsKey->m_mvSize.get()),
            iKeyOffset(ptrSpecsKey->m_mvOffset.get()),
            iKeyReadOffset(ptrSpecsKey->m_mvReadOfsset.get());

        // Cast the value and the key addresses integers to working pointers
        uint8_t *ptrValBuff(_ptrUnfold(&ptrSpecsVal->m_mvPtr, &ptrSpecsVal->m_mvHopNbr)),
                *ptrKeyBuff(_ptrUnfold(&ptrSpecsKey->m_mvPtr, &ptrSpecsKey->m_mvHopNbr));
        
        // Shift the pointer positions to their payloads
        ptrValBuff += iValOffset;
        ptrKeyBuff += iKeyOffset;

        // Retrieve the obfuscated value
        uint8_t *ui8ValBuff(new uint8_t[iValSize]);
        ::memset(ui8ValBuff, 0, iValSize);
        ::memcpy(ui8ValBuff, ptrValBuff, iValSize);

        // Proceed for each byte of the value to a xor logical operation with the key
        for (uint8_t i(0); i < iValSize; ++i)
            ui8ValBuff[i] ^= ptrKeyBuff[(iKeyReadOffset + i) % iKeySize];

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

    // Specifications enumerator, used to require a specific data
    // in the randomly sorted array of masked variables
    enum Especs_ : uint8_t {
        Especs_Fake1,
        Especs_Val,
        Especs_Fake2,
        Especs_Key
    };

    // Generate a key that will be used to obfuscate the stored value
    void _genKey() {
        // Generate a key if m_bPerfMode is disabled, or if it has not yet been populated
        if (!m_bPerfMode || m_bEmpty) {
            // Retrieve the offset between the pointer and position of the key
            // and the size of the key, and the allocated memory of the whole key package
            int iKeyOffset(::rand() % 24 + 8),
                iKeySize(::rand() % 32 + 32),
                iAllocSize(iKeySize + iKeyOffset + 8 + ::rand() % 24),
                iReadOffset(::rand() % iKeySize);
            // Define a random number of element of the linked list of pointers (hops)
            uint8_t ui8KeyHopNbr(::rand() % 7 + 1);

            // Store in obfuscated variables those defined or calculated specifications
            SspecsKey *ptrSpecsKey(reinterpret_cast<SspecsKey*>(_retrieveSpecs(Especs_::Especs_Key)));
            ptrSpecsKey->m_mvOffset.set(iKeyOffset);
            ptrSpecsKey->m_mvSize.set(iKeySize);
            ptrSpecsKey->m_mvHopNbr.set(ui8KeyHopNbr);
            ptrSpecsKey->m_mvReadOfsset.set(iReadOffset);

            // Declare and initialize a dynamic array of bytes to store the key
            uint8_t *ui8KeyBuff(new uint8_t[iAllocSize]);
            ::memset(ui8KeyBuff, 0, iAllocSize);

            // Populate the memory buffer with random values (whose a sequence will be used as a key)
            for (int i(0); i < iAllocSize; ++i)
                ui8KeyBuff[i] = ::rand() % 256;

            // Create a linked list of pointers, the last pointing to the array of bytes
            _ptrFold(ui8KeyHopNbr, &ptrSpecsKey->m_mvPtr, ui8KeyBuff);
        }
    }

    // Obfuscate a value in the format of an array of bytes
    void _obfuscateVal(uint8_t *_ptrBuff, int _iValSize) {
        // Retrieve the offset between the pointer and position of the key and the size of the key
        SspecsKey *ptrSpecsKey(reinterpret_cast<SspecsKey *>(_retrieveSpecs(Especs_::Especs_Key)));
        int iKeyOffset(ptrSpecsKey->m_mvOffset.get()),
            iKeySize(ptrSpecsKey->m_mvSize.get()),
            iKeyReadOffset(ptrSpecsKey->m_mvReadOfsset.get());

        // Get a working pointer pointing to the key
        uint8_t *ui8KeyBuff(_ptrUnfold(&ptrSpecsKey->m_mvPtr, &ptrSpecsKey->m_mvHopNbr));

        // Obfuscate the temporary byte array of the value
        for (int i(0); i < _iValSize; ++i)
            _ptrBuff[i] ^= ui8KeyBuff[iKeyOffset + ((i + iKeyReadOffset) % iKeySize)];
    }

    // Process a value in the format of an array of bytes,
    // calculate or define its specifications (i.e. value length, noise around, etc),
    // and XOR this array of bytes
    void _copyVal(T _val) {
        // Calculate the size of the bytes of the value
        int iSize(_sizeVal<T>(_val));

        // Define two random offset, and calculate the total size of the memory buffer
        int iValOffset(::rand() % 24 + 8),
            iValSize(iSize + iValOffset + 8 + ::rand() % 24);
        // Define a random number of element of the linked list of pointers (hops)
        uint8_t ui8ValHopNbr(::rand() % 7 + 1);

        // Store in obfuscated variables those defined or calculated specifications
        SspecsVal *ptrSpecsVal(reinterpret_cast<SspecsVal *>(_retrieveSpecs(Especs_::Especs_Val)));
        ptrSpecsVal->m_mvOffset.set(iValOffset);
        ptrSpecsVal->m_mvSize.set(iSize);
        ptrSpecsVal->m_mvHopNbr.set(ui8ValHopNbr);

        // Declare and initialize a dynamic array of bytes to store the obfuscated value
        uint8_t *ui8ValBuff(new uint8_t[iValSize]);
        ::memset(ui8ValBuff, 0, iValSize);

        // Populate the sequence before the value with random noise data
        _copyVal_noisePadding(0, iValOffset, ui8ValBuff);
        // Populate the sequence after the value with random noise data
        _copyVal_noisePadding(iValOffset + iValSize, iSize, ui8ValBuff);

        // Cast the variable and store it into a byte array
        _castVal<T>(_val, iSize, ui8ValBuff + iValOffset);

        // Obfuscate the byte array
        _obfuscateVal(ui8ValBuff + iValOffset, iSize);

        // Create a linked list of pointers, the last pointing to the array of bytes
        _ptrFold(ui8ValHopNbr, &ptrSpecsVal->m_mvPtr, ui8ValBuff);
    }

    // Populate the value buffer with padding noise data sequence
    void _copyVal_noisePadding(int _iBeg, int _iEng, uint8_t *_ui8Ptr) {
        for (int i(_iBeg); i < _iEng; ++i)
            _ui8Ptr[i] = ::rand() % 256;
    }

    // Create a linked list of pointers with several hops,
    // from the stored address to the memory buffer of the value or key
    void _ptrFold(uint8_t _ui8HopNbr, CvarMasked<intptr_t> *_mvVal, uint8_t *_ui8ptrBuff) {
        // Declare and initialize the keeper of the last element of the linked list
        intptr_t addHopLast(0);

        // Declare and initialize a working pointer of the last stored address,
        // actually the beginning of the linked list
        intptr_t *ptrHopLast(new intptr_t);

        // Retrieve and store the memory address of the first element of the linked list
        addHopLast = reinterpret_cast<intptr_t>(ptrHopLast);
        _mvVal->set(addHopLast);

        // For the number of hops defined before
        for (int i(0); i < _ui8HopNbr - 1; ++i) {
            // Declare and initialize a new pointer
            intptr_t *ptrTemp(new intptr_t);
            // Update the pointer address of the last element of the linked list with the new created
            ptrHopLast = reinterpret_cast<intptr_t *>(addHopLast);
            // Temporary store the new last element address
            intptr_t addHopLastNew(reinterpret_cast<intptr_t>(ptrTemp));
            // Store the difference between the two last pointers addresses
            *ptrHopLast = addHopLast - addHopLastNew;
            // Update the stored address of the last element of the linked list with the new created
            addHopLast = addHopLastNew;
        }

        // Get the last node
        intptr_t *ptrTempLast(reinterpret_cast<intptr_t *>(addHopLast));
        // Make it pointing to the array of bytes of the value or key
        *ptrTempLast = addHopLast - reinterpret_cast<intptr_t>(_ui8ptrBuff);
    }

    // Unravel the linked list of pointers with several hops,
    // from the stored address to the memory buffer of the value or key
    uint8_t *_ptrUnfold(CvarMasked<intptr_t> *_mvPtr, CvarMasked<uint8_t> *_mvHopNbr) {
        // Declare and initialize the pointer used to unfold the linked list of pointers
        intptr_t *uiPtr(reinterpret_cast<intptr_t *>(_mvPtr->get()));
        // Retrieve the number of hops
        uint8_t ui8HopNbr(_mvHopNbr->get());

        // Jump from a pointer to another, the memory buffer value is the jump length
        for (int i(0); i < ui8HopNbr; ++i)
            _ptrUnfold_Walker(&uiPtr);

        // Return the last pointer of the linked list
        return reinterpret_cast<uint8_t *>(uiPtr);
    }

    // Jump from a pointer address to another
    void _ptrUnfold_Walker(intptr_t **_uiPtr) {
        intptr_t iDiff(0);
        try {
            // Difference is in bits, divide by 8 to get the offset value in bytes
            iDiff = (**_uiPtr) / 8;
        }
        catch (std::logic_error e) {
            throw std::runtime_error(e.what());
        }
        catch (std::overflow_error &e) {
            throw std::runtime_error(e.what());
        }
        // The jump length is depending on the value on this address
        (*_uiPtr) -= iDiff;
    }
    
    // Retrieve a specific CvarMasked var in the randomly sorted array of masked variables
    intptr_t *_retrieveSpecs(Especs_ _eType) {
        int iIndex(_getSpecsVarID(_eType));
        return reinterpret_cast<intptr_t *>(m_arrVarAddr[iIndex]);
    }

    // Retrieve the index of a specific data in the randomly sorted array of masked variables
    int _getSpecsVarID(Especs_ _eType) {
        for (int i(0); i < 8; ++i)
            if (_eType == m_arrConvert[i])
                return i;
        throw std::runtime_error("This type of specification does not exist.");
    }

    // Allocate the dynamic variables for value's and key's specifications
    void _alloc() {
        // If performance mode is disabled, and has already been populated one time
        if (m_bPerfMode && !m_bEmpty)
            return;

        // Allocate the array containing all specifications masked vars memory addresses,
        // and the array to translate a type name into an index
        m_arrVarAddr = new intptr_t * [4];
        m_arrConvert = new uint8_t[4];
        ::memset(m_arrConvert, 255, sizeof(uint8_t) * 4);

        // Array used create a random of Especs_ specifications variables order
        uint8_t ui8IndexDone[4] { 0, 1, 2, 3 },
        // Declare temporary storing of the selected Especs_ type
                ui8IndexSelect;

        // For every specifications
        int iRand;
        for (uint8_t i(0); i < 4; ++i) {
            // Randomly select an array index
            iRand = rand() % (4 - i);
            // Temporary store that value, corresponding to the Especs_ type chosen
            ui8IndexSelect = ui8IndexDone[iRand];
            // Swap the chosen value to the unused end of the array
            ui8IndexDone[iRand] = ui8IndexDone[3 - i];
            // Swap the value at this position to the part still used of the array
            ui8IndexDone[3 - i] = ui8IndexSelect;

            // This selected specification type is now binded to the actual ID 'i'
            m_arrConvert[ui8IndexSelect] = i;

            // Depending on the type of the specification type
            switch (i) {
                case Especs_::Especs_Val:
                {
                    // Allocate a new set of masked specifications var for the value
                    SspecsVal *ptrNew(new SspecsVal());
                    // Store the address of this new masked var set
                    m_arrVarAddr[ui8IndexSelect] = reinterpret_cast<intptr_t *>(ptrNew);
                }
                break;

                case Especs_::Especs_Key:
                {
                    // Allocate a new set of masked specifications var for the key
                    SspecsKey *ptrNew(new SspecsKey());
                    // Store the address of this new masked var set
                    m_arrVarAddr[ui8IndexSelect] = reinterpret_cast<intptr_t *>(ptrNew);
                }
                break;

                case Especs_::Especs_Fake1:
                {
                    // Store fake address
                    m_arrVarAddr[ui8IndexSelect] = reinterpret_cast<intptr_t *>(&ui8IndexDone);
                }
                break;

                case Especs_::Especs_Fake2:
                {
                    // Store fake address
                    m_arrVarAddr[ui8IndexSelect] = reinterpret_cast<intptr_t *>(&iRand);
                }
                break;
            }
        }
    }

    // Reset all value's and key's specifications, and release their memory buffers
    void _flush(bool _bForce = false) {
        // If already populated
        if (!m_bEmpty || (_bForce && !m_bEmpty)) {
            // Unfold the linked list, release every hops, and release the value or key buffer
            SspecsVal *ptrSpecsVal(reinterpret_cast<SspecsVal *>(_retrieveSpecs(Especs_::Especs_Val)));
            _ptrFlush(&ptrSpecsVal->m_mvPtr, &ptrSpecsVal->m_mvHopNbr);
            if (!m_bPerfMode || _bForce) {
                SspecsKey *ptrSpecsKey(reinterpret_cast<SspecsKey *>(_retrieveSpecs(Especs_::Especs_Key)));
                _ptrFlush(&ptrSpecsKey->m_mvPtr, &ptrSpecsKey->m_mvHopNbr);

                // Release all specifications data buffers and remove fake addresses
                for (int i(0); i < 4; ++i)
                    if (m_arrConvert[i] == Especs_::Especs_Val
                        || m_arrConvert[i] == Especs_::Especs_Key)
                        delete m_arrVarAddr[i];
                    else
                        m_arrVarAddr[i] = nullptr;

                // Release the specification masked var address randomizer
                delete[] m_arrVarAddr;
                delete[] m_arrConvert;
            }
        }
    }

    // Release every hops of the linked list, and the memory buffer of the value
    void _ptrFlush(CvarMasked<intptr_t> *_mvPtr, CvarMasked<uint8_t> *_mvHopNbr) {
        // Declare and initialize the pointer used to unfold the linked list of pointers
        intptr_t *uiPtrCurr(reinterpret_cast<intptr_t *>(_mvPtr->get()));
        // Retrieve the number of hops
        uint8_t ui8HopNbr(_mvHopNbr->get());

        // Jump from a pointer to another
        for (uint8_t i(0); i < ui8HopNbr; ++i) {
            // Store the current address to a temporary pointer
            intptr_t *uiptrDel(uiPtrCurr);
            // Go to the next pointer
            _ptrUnfold_Walker(&uiPtrCurr);
            // Release the memory of the temporary pointer
            delete uiptrDel;
        }

        // Delete the first stored node of the linked list
        delete[] uiPtrCurr;
        uiPtrCurr = nullptr;
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
    void _castVal(T _val, int _iSize, uint8_t *_ui8Ptr) {
        // Populate the bytes array with the value
        ::memset(_ui8Ptr, 0, _iSize);
        ::memcpy(_ui8Ptr, &_val, _iSize);
    }

    // Cast a std::string to a byte array
    template <>
    void _castVal<std::string>(std::string _str, int _iSize, uint8_t *_ui8Ptr) {
        // Populate the bytes array with the value
        ::memset(_ui8Ptr, 0, _iSize);
        ::memcpy(_ui8Ptr, _str.data(), _iSize);
    }

    // Cast a const char* string of characters to a byte array
    template <>
    void _castVal<const char *>(const char *_str, int _iSize, uint8_t *_ui8Ptr) {
        // Populate the bytes array with the value
        ::memset(_ui8Ptr, 0, _iSize);
        ::memcpy(_ui8Ptr, _str, _iSize);
    }

    // Cast a std::vector to a byte array
    template<typename T, typename R>
    void _castVal(std::vector<R> _val, int _iSize, uint8_t *_ui8Ptr) {
        // Initialize the memory buffer to store the value
        ::memset(_ui8Ptr, 0, _iSize);

        // Get the size of the type stored in the std::vector
        int iSizeType(sizeof(R));

        // For each element of the std::vector
        for (int i(0); i < _val.size(); ++i)
            // Populate the bytes array with the element
            ::memcpy(_ui8Ptr + (i * iSizeType), &_val[i], iSizeType);
    }

    // Cast a std::map to a byte array
    template<typename T, typename R, typename S>
    void _castVal(std::map<R, S> _val, int _iSize, uint8_t *_ui8Ptr) {
        // Initialize the memory buffer to store the value
        ::memset(_ui8Ptr, 0, _iSize);

        // Get the size of the type of the key
        int iSizeTypeR(sizeof(R)),
        // Get the size of the type of the value
            iSizeTypeS(sizeof(S)),
        // Declare and initialize the current index of the byte array
            iIndex(0);

        // For each element of the std::map
        for (const auto &[key, value] : _val) {
            // Populate the bytes array with the next key
            ::memcpy(_ui8Ptr + iIndex, &key, iSizeTypeR);
            // Update the current index of the byte array with the current key size
            iIndex += iSizeTypeR;

            // Populate the bytes array with the next value
            ::memcpy(_ui8Ptr + iIndex, &value, iSizeTypeS);
            // Update the current index of the byte array with the current value size
            iIndex += iSizeTypeS;
        }
    }


    /*
    ** Member variables
    */

    std::mutex          m_mtx;
    std::atomic<bool>   m_bEmpty     = true;
    bool                m_bPerfMode  = false;
    intptr_t          **m_arrVarAddr = nullptr;
    uint8_t            *m_arrConvert = nullptr;
};

template <>
class CvarObfuscated<void> {
public:
    // Initialization
    static void init(bool _bSrandInit = false) {
        if (_bSrandInit) {
            unsigned long seed(static_cast<unsigned long>(::clock()) * static_cast<unsigned long>(::time(NULL)) * static_cast<unsigned long>(::_getpid()));
            ::srand(seed);
        }
    }
};
