#pragma once

//#include <cstdint>
#include "select_size.hpp"


template<int SIZE, class DATA_T = uint8_t>
class RingBuffer
{
public:
    typedef typename SelectSizeForLength<SIZE>::Result INDEX_T;

private:
    static_assert((SIZE & (SIZE-1)) == 0, "The ring buffer size must be a power of 2");
    DATA_T _data[SIZE];
    volatile INDEX_T _readCount;
    volatile INDEX_T _writeCount;
    static const INDEX_T _mask = SIZE - 1;

public:
    volatile uint32_t IsTransmitting = 0;

    inline bool Write(DATA_T value) {
        if (IsFull())
            return false;
        _data[_writeCount++ & _mask] = value;
        return true;
    }

    inline bool Read(DATA_T &value) {
        if (IsEmpty())
            return false;
        value = _data[_readCount++ & _mask];
        return true;
    }

    inline bool IsEmpty() const {
        return _writeCount == _readCount;
    }

    inline bool IsFull() const {
        return ((_writeCount - _readCount) & (INDEX_T)~(_mask)) != 0;
    }

    INDEX_T Count() const {
        return (_writeCount - _readCount) & _mask;
    }

    inline void Clear() {
        _readCount = 0;
        _writeCount = 0;
    }

    inline unsigned Size() {
        return SIZE;
    }
};
