/*
MIT License

Copyright (c) 2024 Marvin Kipping

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "MkString.h"

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

const wchar_t * MkWcsFindCharConst(const wchar_t * wcs, ulong length, wchar_t wc) {
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

bool MkUtf8Read(
    MkStreamRead readBytesCallback, void * byteStream, void * readStatus,
    MkStreamWrite writeCharsCallback, void * charStream, void * writeStatus)
{
    assert(readBytesCallback);
    assert(byteStream);
    assert(writeCharsCallback);
    assert(charStream);

    enum State {
        STATE_BEGIN,
        STATE_READ_1,
        STATE_READ_2,
        STATE_READ_3,
        STATE_READ_ERROR,
        STATE_END_SUCCESS,
        STATE_END_ERROR,
    };

    bool smallChars = sizeof(wchar_t) < 4;

    unsigned char curByte;
    ulong uc;
    State state = STATE_BEGIN;
    bool readSuccess = readBytesCallback(byteStream, &curByte, 1, readStatus);
    bool lastWasCr = false;

    while (readSuccess || state == STATE_END_SUCCESS || state == STATE_END_ERROR) {
        switch (state) {
            case STATE_BEGIN:
            {
                if ((curByte & 0b10000000) == 0b00000000) {
                    uc = curByte & 0b01111111;
                    state = STATE_END_SUCCESS;
                } else if ((curByte & 0b11000000) == 0b10000000) {
                    readSuccess = readBytesCallback(byteStream, &curByte, 1, readStatus);
                    state = STATE_READ_ERROR;
                } else if ((curByte & 0b11100000) == 0b11000000) {
                    uc = (curByte & 0b00011111) << 6;
                    readSuccess = readBytesCallback(byteStream, &curByte, 1, readStatus);
                    state = STATE_READ_1;
                } else if ((curByte & 0b11110000) == 0b11100000) {
                    uc = (curByte & 0b00001111) << 12;
                    readSuccess = readBytesCallback(byteStream, &curByte, 1, readStatus);
                    state = STATE_READ_2;
                } else if ((curByte & 0b11111000) == 0b11110000) {
                    uc = (curByte & 0b00000111) << 18;
                    readSuccess = readBytesCallback(byteStream, &curByte, 1, readStatus);
                    state = STATE_READ_3;
                } else {
                    readSuccess = readBytesCallback(byteStream, &curByte, 1, readStatus);
                    state = STATE_READ_ERROR;
                }
                break;
            }

            case STATE_READ_1:
            {
                if ((curByte & 0b11000000) == 0b10000000) {
                    uc += curByte & 0b00111111;
                    state = STATE_END_SUCCESS;
                } else {
                    state = STATE_END_ERROR;
                }
                break;
            }

            case STATE_READ_2:
            {
                if ((curByte & 0b11000000) == 0b10000000) {
                    uc += (curByte & 0b00111111) << 6;
                    readSuccess = readBytesCallback(byteStream, &curByte, 1, readStatus);
                    state = STATE_READ_1;
                } else {
                    state = STATE_END_ERROR;
                }
                break;
            }

            case STATE_READ_3:
            {
                if ((curByte & 0b11000000) == 0b10000000) {
                    uc += (curByte & 0b00111111) << 12;
                    readSuccess = readBytesCallback(byteStream, &curByte, 1, readStatus);
                    state = STATE_READ_2;
                } else {
                    state = STATE_END_ERROR;
                }
            }

            case STATE_READ_ERROR:
            {
                if ((curByte & 0b11000000) == 0b10000000) {
                    readSuccess = readBytesCallback(byteStream, &curByte, 1, readStatus);
                } else {
                    state = STATE_END_ERROR;
                }
                break;
            }

            case STATE_END_SUCCESS:
            {
                readSuccess = readBytesCallback(byteStream, &curByte, 1, readStatus);
                state = STATE_BEGIN;

                wchar_t c;
                if (uc > 0xffff && smallChars) {
                    c = 0xfffd;
                } else {
                    c = static_cast<wchar_t>(uc);
                }

                if (c == L'\r') {
                    lastWasCr = true;
                    c = L'\n';
                } else if (c == L'\n') {
                    if (lastWasCr) {
                        lastWasCr = false;
                        break;
                    }
                } else {
                    lastWasCr = false;
                }

                if (!writeCharsCallback(charStream, &c, 1, writeStatus)) {
                    return false;
                }
                break;
            }

            case STATE_END_ERROR:
            {
                readSuccess = readBytesCallback(byteStream, &curByte, 1, readStatus);
                state = STATE_BEGIN;

                wchar_t c = 0xfffd;
                if (!writeCharsCallback(charStream, &c, 1, writeStatus)) {
                    return false;
                }
                break;
            }
        }
    }

    if (state != STATE_BEGIN) {
        wchar_t c = 0xfffd;
        if (!writeCharsCallback(charStream, &c, 1, writeStatus)) {
            return false;
        }
    }

    return true;
}

bool MkUtf8WriteWcs(
    const wchar_t * wcs, ulong count, bool toCrlf,
    MkStreamWrite writeBytesCallback, void * byteStream, void * writeStatus)
{
    assert(wcs || count == 0);
    assert(writeBytesCallback);
    assert(byteStream);

    byte newlineBytes[] = { 0x0d, 0x0a };
    byte * newlineBuffer;
    ulong newlineCount;
    if (toCrlf) {
        newlineBuffer = newlineBytes;
        newlineCount = 2;
    } else {
        newlineBuffer = &newlineBytes[1];
        newlineCount = 1;
    }

    for (ulong i = 0; i != count; i++) {
        if (wcs[i] == L'\0') {
            break;
        }

        if (wcs[i] == L'\n') {
            if (!writeBytesCallback(byteStream, newlineBuffer, newlineCount, writeStatus)) {
                return false;
            }
        } else {
            ulong uc = static_cast<ulong>(wcs[i]);
            byte output[4];
            ulong outputCount;

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

            if (!writeBytesCallback(byteStream, output, outputCount, writeStatus)) {
                return false;
            }
        }
    }

    return true;
}