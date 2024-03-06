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

size_t MkWcsFindCharIndex(const wchar_t * wcs, size_t wcsMax, wchar_t c) {
    if (!wcs) {
        return SIZE_MAX;
    }

    for (size_t i = 0; i != wcsMax; i++) {
        if (wcs[i] == c) {
            return i;
        }
        if (wcs[i] == L'\0') {
            return SIZE_MAX;
        }
    }
    return SIZE_MAX;
}

size_t MkWcsFindCharsIndex(const wchar_t * wcs, size_t wcsMax, const wchar_t * chars, size_t count) {
    assert(chars || count == 0);

    if (!wcs) {
        return SIZE_MAX;
    }

    for (size_t i = 0; i != wcsMax; i++) {
        for (size_t j = 0; j != count; j++) {
            if (wcs[i] == chars[j]) {
                return i;
            }
        }
        if (wcs[i] == L'\0') {
            return SIZE_MAX;
        }
    }
    return SIZE_MAX;
}

size_t MkWcsFindSubstrIndex(const wchar_t * wcs, size_t wcsMax, const wchar_t * substr) {
    if (!wcs || wcsMax == 0) {
        return SIZE_MAX;
    }
    size_t actualLength = wcsnlen_s(wcs, wcsMax);
    if (actualLength == 0) {
        return SIZE_MAX;
    }

    if (!substr) {
        return 0;
    }
    size_t substrLength = wcslen(substr);
    if (substrLength == 0) {
        return 0;
    }
    if (substrLength > actualLength) {
        return SIZE_MAX;
    }

    size_t end = actualLength - substrLength + 1;
    for (size_t i = 0; i != end; i++) {
        bool match = true;
        for (size_t j = 0; j != substrLength; j++) {
            if (wcs[i + j] != substr[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            return i;
        }
    }
    return SIZE_MAX;
}

bool MkWcsIsPrefix(const wchar_t * wcs, size_t wcsMax, const wchar_t * prefix) {
    if (!prefix) {
        return true;
    }
    if (!wcs) {
        return prefix[0] == L'\0';
    }

    for (size_t i = 0; i != wcsMax; i++) {
        if (wcs[i] != prefix[i]) {
            return prefix[i] == L'\0';
        }
        if (wcs[i] == L'\0') {
            return true;
        }
    }
    return prefix[wcsMax] == L'\0';
}

bool MkWcsAreEqual(const wchar_t * a, size_t aMax, const wchar_t * b, size_t bMax) {
    if (aMax != bMax) {
        return false;
    }

    if (!a) {
        if (!b) {
            return true;
        } else {
            return false;
        }
    }
    if (!b) {
        return false;
    }

    for (size_t i = 0; i != aMax; i++) {
        if (a[i] != b[i]) {
            return false;
        }
        if (a[i] == L'\0') {
            return true;
        }
    }
    return true;
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

    byte curByte;
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
    const wchar_t * wcs, size_t wcsMax, bool toCrlf,
    MkStreamWrite writeBytesCallback, void * byteStream, void * writeStatus)
{
    assert(writeBytesCallback);
    assert(byteStream);

    if (!wcs) {
        return true;
    }

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

    for (size_t i = 0; i != wcsMax; i++) {
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