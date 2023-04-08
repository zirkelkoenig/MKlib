#include <assert.h>
#include "MkString.h"

MkStringW * MkStringW_Copy_Cw(const wchar_t * cwString) {
    assert(MkString_MemAlloc);
    assert(MkString_MemRealloc);
    assert(MkString_MemFree);
    assert(MkString_MemCopy);

    assert(cwString);

    MkStringW * string = MkString_MemAlloc(sizeof(MkStringW));
    if (!string) {
        return NULL;
    }

    string->length = wcslen(cwString);
    string->chars = MkString_MemAlloc((string->length + 1) * sizeof(wchar_t));
    if (!string->chars) {
        MkString_MemFree(string);
        return NULL;
    }

    MkString_MemCopy(string->chars, cwString, string->length * sizeof(wchar_t));
    string->chars[string->length] = L'\0';
    return string;
}

MkStringW * MkStringW_Copy(const MkStringW * string) {
    assert(MkString_MemAlloc);
    assert(MkString_MemRealloc);
    assert(MkString_MemFree);
    assert(MkString_MemCopy);

    assert(string);

    MkStringW * copy = MkString_MemAlloc(sizeof(MkStringW));
    if (!copy) {
        return NULL;
    }

    copy->chars = MkString_MemAlloc((string->length + 1) * sizeof(wchar_t));
    if (!copy->chars) {
        MkString_MemFree(copy);
        return NULL;
    }
    copy->length = string->length;

    MkString_MemCopy(copy->chars, string->chars, string->length * sizeof(wchar_t));
    copy->chars[copy->length] = L'\0';
    return copy;
}

MkStringW * MkStringW_Concat_Cw(const wchar_t * cwStringA, const wchar_t * cwStringB) {
    assert(MkString_MemAlloc);
    assert(MkString_MemRealloc);
    assert(MkString_MemFree);
    assert(MkString_MemCopy);

    assert(cwStringA);
    assert(cwStringB);

    const size_t lengthA = wcslen(cwStringA);
    const size_t lengthB = wcslen(cwStringB);

    MkStringW * string = MkString_MemAlloc(sizeof(MkStringW));
    if (!string) {
        return NULL;
    }

    string->length = lengthA + lengthB;
    string->chars = MkString_MemAlloc((string->length + 1) * sizeof(wchar_t));
    if (!string->chars) {
        MkString_MemFree(string);
        return NULL;
    }

    MkString_MemCopy(string->chars, cwStringA, lengthA * sizeof(wchar_t));
    MkString_MemCopy(string->chars + lengthA, cwStringB, lengthB * sizeof(wchar_t));
    string->chars[string->length] = L'\0';
    return string;
}

void MkStringW_Destroy(MkStringW * string) {
    assert(MkString_MemAlloc);
    assert(MkString_MemRealloc);
    assert(MkString_MemFree);
    assert(MkString_MemCopy);

    assert(string);
    assert(string->chars);

    MkString_MemFree(string->chars);
    MkString_MemFree(string);
}

int MkStringW_Append_Cw(MkStringW * dest, const wchar_t * source) {
    assert(MkString_MemAlloc);
    assert(MkString_MemRealloc);
    assert(MkString_MemFree);
    assert(MkString_MemCopy);

    assert(dest);
    assert(source);

    const size_t sourceLength = wcslen(source);
    const size_t newLength = dest->length + sourceLength;

    wchar_t * newChars = MkString_MemRealloc(dest->chars, (newLength + 1) * sizeof(wchar_t));
    if (!newChars) {
        return 0;
    }
    MkString_MemCopy(newChars + dest->length, source, sourceLength * sizeof(wchar_t));
    newChars[newLength] = L'\0';

    dest->chars = newChars;
    dest->length = newLength;
    return 1;
}

int MkStringW_AppendWin32Path_Cw(MkStringW * win32Path, const wchar_t * component) {
    assert(MkString_MemAlloc);
    assert(MkString_MemRealloc);
    assert(MkString_MemFree);
    assert(MkString_MemCopy);

    assert(win32Path);
    assert(component);

    int separatorAdded;
    if (win32Path->length == 0 || win32Path->chars[win32Path->length - 1] != L'\\') {
        win32Path->chars[win32Path->length++] = L'\\';
        separatorAdded = 1;
    } else {
        separatorAdded = 0;
    }

    if (MkStringW_Append_Cw(win32Path, component)) {
        return 1;
    } else {
        if (separatorAdded) {
            win32Path->chars[--win32Path->length] = L'\0';
        }
        return 0;
    }
}

byte * MkStringW_ToUtf8(const MkStringW * string, size_t * streamLength) {
    assert(MkString_MemAlloc);
    assert(MkString_MemRealloc);
    assert(MkString_MemFree);
    assert(MkString_MemCopy);

    assert(string);
    assert(streamLength);

    byte * utf8String = MkString_MemAlloc(4 * string->length);
    if (!utf8String) {
        return NULL;
    }

    size_t streamLengthImpl = 0;
    for (size_t i = 0; i != string->length; i++) {
        wchar_t c = string->chars[i];
        if (c <= 0x007f) {
            utf8String[streamLengthImpl++] = c;
        } else if (c <= 0x07ff) {
            utf8String[streamLengthImpl++] = (c >> 6) + 0b11000000;
            utf8String[streamLengthImpl++] = (c & 0b00111111) + 0b10000000;
        } else if (c <= 0xffff) {
            utf8String[streamLengthImpl++] = (c >> 12) + 0b11100000;
            utf8String[streamLengthImpl++] = (c >> 6 & 0b00111111) + 0b10000000;
            utf8String[streamLengthImpl++] = (c & 0b00111111) + 0b10000000;
        } else if (c <= 0x0010ffff) {
            utf8String[streamLengthImpl++] = (c >> 18) + 0b11110000;
            utf8String[streamLengthImpl++] = (c >> 12 & 0b00111111) + 0b10000000;
            utf8String[streamLengthImpl++] = (c >> 6 & 0b00111111) + 0b10000000;
            utf8String[streamLengthImpl++] = (c & 0b00111111) + 0b10000000;
        } else {
            utf8String[streamLengthImpl++] = 0b11101111;
            utf8String[streamLengthImpl++] = 0b10111111;
            utf8String[streamLengthImpl++] = 0b10111101;
        }
    }

    utf8String = MkString_MemRealloc(utf8String, streamLengthImpl);
    *streamLength = streamLengthImpl;
    return utf8String;
}

typedef enum {
    UTF8_START,
    UTF8_END_OK,
    UTF8_END_ERROR,
    UTF8_READ_1,
    UTF8_READ_2,
    UTF8_READ_3,
    UTF8_READ_ERROR,
} Utf8State;

wchar_t * CwFromUtf8Impl(const byte * utf8, const size_t utf8Length, size_t * cwStringLength) {
    wchar_t * chars = MkString_MemAlloc((utf8Length + 1) * sizeof(wchar_t));
    if (!chars) {
        return NULL;
    }
    size_t charsCount = 0;

    int limited = sizeof(wchar_t) < 4;

    Utf8State state = UTF8_START;
    size_t i = 0;
    u32 c = 0;
    size_t readCount = 0;
    while (i != utf8Length || state == UTF8_END_OK || state == UTF8_END_ERROR) {
        switch (state) {
            case UTF8_START:
            {
                if ((utf8[i] & 0b10000000) == 0b00000000) {
                    c = utf8[i++];
                    readCount = 0;
                    state = UTF8_END_OK;
                } else if ((utf8[i] & 0b11000000) == 0b10000000) {
                    i++;
                    readCount = 0;
                    state = UTF8_READ_ERROR;
                } else if ((utf8[i] & 0b11100000) == 0b11000000) {
                    c = (utf8[i++] & 0b00011111) << 6;
                    readCount = 1;
                    state = UTF8_READ_1;
                } else if ((utf8[i] & 0b11110000) == 0b11100000) {
                    c = (utf8[i++] & 0b00001111) << 12;
                    readCount = 2;
                    state = UTF8_READ_2;
                } else if ((utf8[i] & 0b11111000) == 0b11110000) {
                    c = (utf8[i++] & 0b00000111) << 18;
                    readCount = 3;
                    state = UTF8_READ_3;
                } else {
                    i++;
                    readCount = 0;
                    state = UTF8_READ_ERROR;
                }

                if (readCount > utf8Length - i) {
                    i = utf8Length;
                    state = UTF8_END_ERROR;
                }
                break;
            }

            case UTF8_END_OK:
            {
                if (c > 0xffff && limited) {
                    chars[charsCount++] = 0xfffd;
                } else {
                    chars[charsCount] = c;
                    if (c == L'\0') {
                        i = utf8Length;
                    } else {
                        charsCount++;
                    }
                }
                state = UTF8_START;
                break;
            }

            case UTF8_END_ERROR:
            {
                chars[charsCount++] = 0xfffd;
                state = UTF8_START;
                break;
            }

            case UTF8_READ_1:
            {
                if ((utf8[i] & 0b11000000) == 0b10000000) {
                    c += utf8[i++] & 0b00111111;
                    state = UTF8_END_OK;
                } else {
                    i++;
                    state = UTF8_END_ERROR;
                }
                break;
            }

            case UTF8_READ_2:
            {
                if ((utf8[i] & 0b11000000) == 0b10000000) {
                    c += (utf8[i++] & 0b00111111) << 6;
                    state = UTF8_READ_1;
                } else {
                    i++;
                    state = UTF8_END_ERROR;
                }
                break;
            }

            case UTF8_READ_3:
            {
                if ((utf8[i] & 0b11000000) == 0b10000000) {
                    c += (utf8[i++] & 0b00111111) << 12;
                    state = UTF8_READ_2;
                } else {
                    i++;
                    state = UTF8_END_ERROR;
                }
                break;
            }

            case UTF8_READ_ERROR:
            {
                if ((utf8[i++] & 0b11000000) != 0b10000000) {
                    state = UTF8_END_ERROR;
                }
                break;
            }
        }
    }

    chars[charsCount] = L'\0';
    chars = MkString_MemRealloc(chars, (charsCount + 1) * sizeof(wchar_t));

    *cwStringLength = charsCount;
    return chars;
}

wchar_t * MkString_CwFromUtf8(const byte * utf8, const size_t utf8Length) {
    size_t cwStringLength;
    return CwFromUtf8Impl(utf8, utf8Length, &cwStringLength);
}

MkStringW * MkStringW_FromUtf8(const byte * stream, const size_t streamLength) {
    assert(MkString_MemAlloc);
    assert(MkString_MemRealloc);
    assert(MkString_MemFree);
    assert(MkString_MemCopy);

    assert(stream);

    MkStringW * string = MkString_MemAlloc(sizeof(MkStringW));
    if (!string) {
        return NULL;
    }

    string->chars = CwFromUtf8Impl(stream, streamLength, &string->length);
    if (!string->chars) {
        MkString_MemFree(string);
        return NULL;
    }
    return string;
}

byte * MkString_CwToUtf8(const wchar_t * cwString, size_t * utf8Length) {
    assert(MkString_MemAlloc);
    assert(MkString_MemRealloc);
    assert(MkString_MemFree);
    assert(MkString_MemCopy);

    assert(cwString);
    assert(utf8Length);

    MkStringW string;
    string.chars = cwString;
    string.length = wcslen(cwString);
    return MkStringW_ToUtf8(&string, utf8Length);
}
