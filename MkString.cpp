#include <wchar.h>

#include "MkString.h"

#define TryAppendWc(wc) { wchar_t * wcElem = (wchar_t *)MkListInsert(listPtr, SIZE_MAX, 1); if (!wcElem) return false; *wcElem = (wc); }

bool MkReadUtf8Stream(MkReadByteStream readCallback, void * stream, void * status, MkUtf8Options options, MkList * listPtr) {
    assert(readCallback);
    assert(stream);
    assert(status);
    MkListAssert(listPtr);
    assert(listPtr->elemSize == sizeof(wchar_t));

    const bool smallWc = sizeof(wchar_t) < 4;
    bool stopAtNull = options & MKUTF8_STOP_AT_NULL;

    enum State {
        STATE_BEGIN,
        STATE_READ_1,
        STATE_READ_2,
        STATE_READ_3,
        STATE_READ_ERROR,
        STATE_END_SUCCESS,
        STATE_END_ERROR,
    };

    State state = STATE_BEGIN;
    byte input;
    bool readSuccess = readCallback(stream, 1, &input, status);
    ulong uc;
    bool lastWasCr = false;

    while (readSuccess) {
        switch (state) {
            case STATE_BEGIN:
            {
                if ((input & 0b10000000) == 0b00000000) {
                    uc = input & 0b01111111;
                    state = STATE_END_SUCCESS;
                } else if ((input & 0b11000000) == 0b10000000) {
                    readSuccess = readCallback(stream, 1, &input, status);
                    state = STATE_READ_ERROR;
                } else if ((input & 0b11100000) == 0b11000000) {
                    uc = (input & 0b00011111) << 6;
                    readSuccess = readCallback(stream, 1, &input, status);
                    state = STATE_READ_1;
                } else if ((input & 0b11110000) == 0b11100000) {
                    uc = (input & 0b00001111) << 12;
                    readSuccess = readCallback(stream, 1, &input, status);
                    state = STATE_READ_2;
                } else if ((input & 0b11111000) == 0b11110000) {
                    uc = (input & 0b00000111) << 18;
                    readSuccess = readCallback(stream, 1, &input, status);
                    state = STATE_READ_3;
                } else {
                    readSuccess = readCallback(stream, 1, &input, status);
                    state = STATE_READ_ERROR;
                }
                break;
            }

            case STATE_READ_1:
            {
                if ((input & 0b11000000) == 0b10000000) {
                    uc += input & 0b00111111;
                    state = STATE_END_SUCCESS;
                } else {
                    state = STATE_END_ERROR;
                }
                break;
            }

            case STATE_READ_2:
            {
                if ((input & 0b11000000) == 0b10000000) {
                    uc += (input & 0b00111111) << 6;
                    readSuccess = readCallback(stream, 1, &input, status);
                    state = STATE_READ_1;
                } else {
                    state = STATE_END_ERROR;
                }
                break;
            }

            case STATE_READ_3:
            {
                if ((input & 0b11000000) == 0b10000000) {
                    uc += (input & 0b00111111) << 12;
                    readSuccess = readCallback(stream, 1, &input, status);
                    state = STATE_READ_2;
                } else {
                    state = STATE_END_ERROR;
                }
            }

            case STATE_READ_ERROR:
            {
                if ((input & 0b11000000) == 0b10000000) {
                    readSuccess = readCallback(stream, 1, &input, status);
                } else {
                    state = STATE_END_ERROR;
                }
                break;
            }

            case STATE_END_SUCCESS:
            {
                if (uc > 0xffff) {
                    lastWasCr = false;
                    if (smallWc) {
                        TryAppendWc(0xfffd);
                    } else {
                        TryAppendWc((wchar_t)uc);
                    }
                    readSuccess = readCallback(stream, 1, &input, status);
                } else {
                    wchar_t wc = (wchar_t)uc;

                    if (wc == L'\0') {
                        lastWasCr = false;

                        if (stopAtNull) {
                            readSuccess = false;
                        } else {
                            TryAppendWc(wc);
                            readSuccess = readCallback(stream, 1, &input, status);
                        }
                    } else {
                        if (wc == L'\n') {
                            if (lastWasCr) {
                                lastWasCr = false;
                            } else {
                                TryAppendWc(wc);
                            }
                        } else if (wc == L'\r') {
                            lastWasCr = true;
                            TryAppendWc(L'\n');
                        } else {
                            lastWasCr = false;
                            TryAppendWc(wc);
                        }
                        readSuccess = readCallback(stream, 1, &input, status);
                    }
                }

                state = STATE_BEGIN;
                break;
            }

            case STATE_END_ERROR:
            {
                TryAppendWc(0xfffd);
                readSuccess = readCallback(stream, 1, &input, status);
                state = STATE_BEGIN;
                break;
            }
        }
    }

    if (state != STATE_BEGIN) {
        TryAppendWc(0xfffd);
    }

    return true;
}

bool MkWriteUtf8Stream(MkWriteByteStream writeCallback, void * stream, void * status, MkUtf8Options options, const wchar_t * wcs, size_t count) {
    assert(writeCallback);
    assert(stream);
    assert(status);
    assert(wcs || count == 0);

    bool stopAtNull = options & MKUTF8_STOP_AT_NULL;

    byte crlf[2] = { 0x0d, 0x0a };
    byte * newlineOutput;
    ulong newlineCount;
    if (options & MKUTF8_CRLF) {
        newlineOutput = crlf;
        newlineCount = 2;
    } else {
        newlineOutput = crlf + 1;
        newlineCount = 1;
    }

    byte output[4];
    ulong outputCount;

    for (size_t i = 0; i != count; i++) {
        wchar_t wc = wcs[i];

        if (wc == L'\0' && stopAtNull) {
            return true;
        }

        if (wc == L'\n') {
            if (!writeCallback(stream, newlineOutput, newlineCount, status)) return false;
        } else {
            ulong uc = (ulong)wc;

            if (uc <= 0x007f) {
                output[0] = (byte)uc;
                outputCount = 1;
            } else if (uc <= 0x07ff) {
                output[0] = (byte)((uc >> 6) + 0b11000000);
                output[1] = (byte)((uc & 0b00111111) + 0b10000000);
                outputCount = 2;
            } else if (uc <= 0xffff) {
                output[0] = (byte)((uc >> 12) + 0b11100000);
                output[1] = (byte)((uc >> 6 & 0b00111111) + 0b10000000);
                output[2] = (byte)((uc & 0b00111111) + 0b10000000);
                outputCount = 3;
            } else if (uc <= 0x0010ffff) {
                output[0] = (byte)((uc >> 18) + 0b11110000);
                output[1] = (byte)((uc >> 12 & 0b00111111) + 0b10000000);
                output[2] = (byte)((uc >> 6 & 0b00111111) + 0b10000000);
                output[3] = (byte)((uc & 0b00111111) + 0b10000000);
                outputCount = 4;
            } else {
                // 0xFFFD
                output[0] = 0b11101111;
                output[1] = 0b10111111;
                output[2] = 0b10111101;
                outputCount = 3;
            }

            if (!writeCallback(stream, output, outputCount, status)) return false;
        }
    }

    return true;
}

bool MkWstrsAreEqual(const MkWstr * a, const MkWstr * b) {
    assert(a);
    assert(a->wcs);
    assert(b);
    assert(b->wcs);

    if (a->length != b->length) {
        return false;
    }

    for (size_t i = 0; i != a->length; i++) {
        if (a->wcs[i] != b->wcs[i]) {
            return false;
        }
    }
    return true;
}

wchar_t * MkWcsFindChar(wchar_t * wcs, size_t length, wchar_t wc) {
    assert(wcs);

    for (size_t i = 0; i != length; i++) {
        if (wcs[i] == wc) {
            return wcs + i;
        }
    }
    return nullptr;
}

size_t MkWcsFindCharsIndex(const wchar_t * wcs, size_t length, const wchar_t * chars, size_t count) {
    assert(wcs);
    assert(chars);

    for (size_t i = 0; i != length; i++) {
        for (size_t j = 0; j != count; j++) {
            if (wcs[i] == chars[j]) {
                return i;
            }
        }
    }
    return SIZE_MAX;
}

size_t MkWcsFindSubstringIndex(const wchar_t * wcs, size_t length, const wchar_t * substring) {
    assert(wcs);
    assert(substring);

    size_t substringLength = wcslen(substring);
    if (substringLength > length) {
        return SIZE_MAX;
    }
    if (substringLength == 0) {
        return 0;
    }

    size_t end = length - substringLength;
    for (size_t i = 0; i != end; i++) {
        bool found = true;
        for (size_t j = 0; j != substringLength; j++) {
            if (wcs[i + j] != substring[j]) {
                found = false;
                break;
            }
        }
        if (found) {
            return i;
        }
    }
    return SIZE_MAX;
}

bool MkWcsIsPrefix(const wchar_t * wcs, size_t length, const wchar_t * prefix) {
    assert(wcs);
    assert(prefix);

    size_t i = 0;
    while (i != length) {
        if (prefix[i] == L'\0') {
            return true;
        }
        if (wcs[i] != prefix[i]) {
            return false;
        }
        i++;
    }
    if (prefix[i] == L'\0') {
        return true;
    } else {
        return false;
    }
}