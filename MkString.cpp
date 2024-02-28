#include <stdlib.h>

#include "MkString.h"

enum ReadResult {
    READ_NEWLINE,
    READ_EOF,
    READ_MEMORY_ERROR,
};

static ReadResult ReadUtf8StreamLine(const byte * input, ulong inputSize, MkList * output, bool * lastWasCr, ulong * bytesRead) {
    const bool smallWc = sizeof(wchar_t) < 4;

    auto TryAppendWc = [&output](wchar_t wc) {
        if (output->count == output->capacity) {
            if (!MkListSetCapacity(output, 2 * output->capacity)) {
                return false;
            }
        }
        wchar_t * wcElem = (wchar_t *)MkListInsert(output, ULONG_MAX, 1);
        *wcElem = wc;
        return true;
    };

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
    ulong uc;
    ulong i = 0;

    while (i != inputSize || state == STATE_END_SUCCESS || state == STATE_END_ERROR) {
        switch (state) {
            case STATE_BEGIN:
            {
                if ((input[i] & 0b10000000) == 0b00000000) {
                    uc = input[i] & 0b01111111;
                    state = STATE_END_SUCCESS;
                } else if ((input[i] & 0b11000000) == 0b10000000) {
                    i++;
                    state = STATE_READ_ERROR;
                } else if ((input[i] & 0b11100000) == 0b11000000) {
                    uc = (input[i] & 0b00011111) << 6;
                    i++;
                    state = STATE_READ_1;
                } else if ((input[i] & 0b11110000) == 0b11100000) {
                    uc = (input[i] & 0b00001111) << 12;
                    i++;
                    state = STATE_READ_2;
                } else if ((input[i] & 0b11111000) == 0b11110000) {
                    uc = (input[i] & 0b00000111) << 18;
                    i++;
                    state = STATE_READ_3;
                } else {
                    i++;
                    state = STATE_READ_ERROR;
                }
                break;
            }

            case STATE_READ_1:
            {
                if ((input[i] & 0b11000000) == 0b10000000) {
                    uc += input[i] & 0b00111111;
                    state = STATE_END_SUCCESS;
                } else {
                    state = STATE_END_ERROR;
                }
                break;
            }

            case STATE_READ_2:
            {
                if ((input[i] & 0b11000000) == 0b10000000) {
                    uc += (input[i] & 0b00111111) << 6;
                    i++;
                    state = STATE_READ_1;
                } else {
                    state = STATE_END_ERROR;
                }
                break;
            }

            case STATE_READ_3:
            {
                if ((input[i] & 0b11000000) == 0b10000000) {
                    uc += (input[i] & 0b00111111) << 12;
                    i++;
                    state = STATE_READ_2;
                } else {
                    state = STATE_END_ERROR;
                }
            }

            case STATE_READ_ERROR:
            {
                if ((input[i] & 0b11000000) == 0b10000000) {
                    i++;
                } else {
                    state = STATE_END_ERROR;
                }
                break;
            }

            case STATE_END_SUCCESS:
            {
                if (uc > 0xffff) {
                    *lastWasCr = false;
                    if (smallWc) {
                        if (!TryAppendWc(0xfffd)) return READ_MEMORY_ERROR;
                    } else {
                        if (!TryAppendWc((wchar_t)uc)) return READ_MEMORY_ERROR;
                    }
                    i++;
                } else {
                    wchar_t wc = (wchar_t)uc;
                    i++;

                    if (wc == L'\n') {
                        if (*lastWasCr) {
                            *lastWasCr = false;
                        } else {
                            if (!TryAppendWc(wc)) return READ_MEMORY_ERROR;
                            *bytesRead = i;
                            return READ_NEWLINE;
                        }
                    } else if (wc == L'\r') {
                        *lastWasCr = true;
                        if (!TryAppendWc(L'\n')) return READ_MEMORY_ERROR;
                        *bytesRead = i;
                        return READ_NEWLINE;
                    } else {
                        *lastWasCr = false;
                        if (!TryAppendWc(wc)) return READ_MEMORY_ERROR;
                    }
                }

                state = STATE_BEGIN;
                break;
            }

            case STATE_END_ERROR:
            {
                if (!TryAppendWc(0xfffd)) return READ_MEMORY_ERROR;
                i++;
                state = STATE_BEGIN;
                break;
            }
        }
    }

    if (state != STATE_BEGIN) {
        if (!TryAppendWc(0xfffd)) return READ_MEMORY_ERROR;
    }

    *bytesRead = i;
    return READ_EOF;
}

bool MkReadUtf8Stream(const byte * input, ulong inputSize, MkList * output) {
    assert(input || inputSize == 0);
    MkListAssert(output);
    assert(output->elemSize == sizeof(wchar_t));

    ulong targetCapacity = (((output->count + inputSize) / output->growCount) + 1) * output->growCount;
    if (output->capacity < targetCapacity) {
        if (!MkListSetCapacity(output, targetCapacity)) return false;
    }

    ReadResult readResult;
    bool lastWasCr = false;
    ulong bytesRead;
    ulong i = 0;
    do {
        readResult = ReadUtf8StreamLine(input + i, inputSize - i, output, &lastWasCr, &bytesRead);
        i += bytesRead;
    } while (readResult == READ_NEWLINE);

    return true;
}

bool MkReadUtf8StreamToLines(const byte * input, ulong inputSize, MkList * outputLines) {
    assert(input || inputSize == 0);
    MkListAssert(outputLines);
    assert(outputLines->elemSize == sizeof(MkList));

    ReadResult readResult;
    bool lastWasCr = false;
    ulong bytesRead;
    ulong i = 0;
    do {
        MkList * outputLinePtr = (MkList *)MkListInsert(outputLines, ULONG_MAX, 1);
        if (!outputLinePtr) {
            return false;
        }
        MkListInit(outputLinePtr, 16, sizeof(wchar_t));
        readResult = ReadUtf8StreamLine(input + i, inputSize - i, outputLinePtr, &lastWasCr, &bytesRead);
        if (readResult == READ_NEWLINE) {
            outputLinePtr->count--;
        }
        i += bytesRead;
    } while (readResult == READ_NEWLINE);

    if (readResult == READ_MEMORY_ERROR) {
        return false;
    } else {
        return true;
    }
}

bool MkWriteUtf8Stream(MkByteStreamWriteCallback writeCallback, void * stream, void * status, const wchar_t * str, ulong strLength, bool toCrlf) {
    assert(writeCallback);
    assert(stream);
    assert(status);
    assert(str || strLength == 0);

    byte crlf[2] = { 0x0d, 0x0a };
    byte * newlineOutput;
    ulong newlineCount;
    if (toCrlf) {
        newlineOutput = crlf;
        newlineCount = 2;
    } else {
        newlineOutput = crlf + 1;
        newlineCount = 1;
    }

    byte output[4];
    ulong outputCount;

    for (ulong i = 0; i != strLength; i++) {
        wchar_t wc = str[i];

        if (wc == L'\0') {
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

    for (ulong i = 0; i != a->length; i++) {
        if (a->wcs[i] != b->wcs[i]) {
            return false;
        }
    }
    return true;
}

wchar_t * MkWcsFindChar(wchar_t * wcs, ulong length, wchar_t wc) {
    assert(wcs);

    for (ulong i = 0; i != length; i++) {
        if (wcs[i] == wc) {
            return wcs + i;
        }
    }
    return nullptr;
}

ulong MkWcsFindCharsIndex(const wchar_t * wcs, ulong length, const wchar_t * chars, ulong count) {
    assert(wcs);
    assert(chars);

    for (ulong i = 0; i != length; i++) {
        for (ulong j = 0; j != count; j++) {
            if (wcs[i] == chars[j]) {
                return i;
            }
        }
    }
    return ULONG_MAX;
}

ulong MkWcsFindSubstringIndex(const wchar_t * wcs, ulong length, const wchar_t * substring) {
    assert(wcs);
    assert(substring);

    ulong substringLength = 0;
    while (substring[substringLength] != L'\0') {
        substringLength++;
    }
    if (substringLength > length) {
        return ULONG_MAX;
    }
    if (substringLength == 0) {
        return 0;
    }

    ulong end = length - substringLength;
    for (ulong i = 0; i != end; i++) {
        bool found = true;
        for (ulong j = 0; j != substringLength; j++) {
            if (wcs[i + j] != substring[j]) {
                found = false;
                break;
            }
        }
        if (found) {
            return i;
        }
    }
    return ULONG_MAX;
}

bool MkWcsIsPrefix(const wchar_t * wcs, ulong length, const wchar_t * prefix) {
    assert(wcs);
    assert(prefix);

    ulong i = 0;
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