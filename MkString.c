#include <assert.h>
#include <stdint.h>
#include "MkString.h"
#include "MkDynArray.h"

byte * CwStringToUtf8_Impl(const wchar_t * string, const size_t stringLength, size_t * utf8Length);
void CwStringFromUtf8_Impl(const byte * utf8, const size_t utf8Length, wchar_t * string, size_t * stringLength);

byte * MkLib_CwStringToUtf8(const wchar_t * string, size_t * utf8Length) {
    assert(MkLib_MemAlloc);
    assert(MkLib_MemRealloc);
    assert(MkLib_MemFree);
    assert(MkLib_MemCopy);
    assert(MkLib_MemMove);

    assert(string);
    assert(utf8Length);

    return CwStringToUtf8_Impl(string, wcslen(string), utf8Length);
}

wchar_t * MkLib_CwStringFromUtf8(const byte * utf8, const size_t utf8Length) {
    assert(MkLib_MemAlloc);
    assert(MkLib_MemRealloc);
    assert(MkLib_MemFree);
    assert(MkLib_MemCopy);
    assert(MkLib_MemMove);

    assert(utf8Length == 0 || utf8);

    wchar_t * string = MkLib_MemAlloc((utf8Length + 1) * sizeof(wchar_t));
    if (!string) {
        return NULL;
    }
    size_t stringLength;
    CwStringFromUtf8_Impl(utf8, utf8Length, string, &stringLength);
    string = MkLib_MemRealloc(string, (stringLength + 1) * sizeof(wchar_t));
    return string;
}

MkLib_StringW * MkLib_StringW_FromCwString(const wchar_t * cwString) {
    assert(MkLib_MemAlloc);
    assert(MkLib_MemRealloc);
    assert(MkLib_MemFree);
    assert(MkLib_MemCopy);
    assert(MkLib_MemMove);

    assert(cwString);

    size_t length = wcslen(cwString);
    MkLib_StringW * string = MkLib_MemAlloc(sizeof(MkLib_StringW) + length * sizeof(wchar_t));
    if (!string) {
        return NULL;
    }
    MkLib_MemCopy(string->chars, cwString, length);
    string->chars[length] = L'\0';
    string->length = length;
    return string;
}

MkLib_StringW * MkLib_StringW_Copy(const MkLib_StringW * string) {
    assert(MkLib_MemAlloc);
    assert(MkLib_MemRealloc);
    assert(MkLib_MemFree);
    assert(MkLib_MemCopy);
    assert(MkLib_MemMove);

    assert(string);

    size_t size = sizeof(MkLib_StringW) + string->length * sizeof(wchar_t);
    MkLib_StringW * copy = MkLib_MemAlloc(size);
    if (!copy) {
        return NULL;
    }
    MkLib_MemCopy(copy, string, size);
    return copy;
}

MkLib_StringW * MkLib_StringW_FromConcatCwStrings(const wchar_t * cwStringA, const wchar_t * cwStringB) {
    assert(MkLib_MemAlloc);
    assert(MkLib_MemRealloc);
    assert(MkLib_MemFree);
    assert(MkLib_MemCopy);
    assert(MkLib_MemMove);

    assert(cwStringA);
    assert(cwStringB);

    size_t lengthA = wcslen(cwStringA);
    size_t lengthB = wcslen(cwStringB);
    size_t concatLength = lengthA + lengthB;
    MkLib_StringW * concat = MkLib_MemAlloc(sizeof(MkLib_StringW) + concatLength * sizeof(wchar_t));
    if (!concat) {
        return NULL;
    }
    MkLib_MemCopy(concat->chars, cwStringA, lengthA);
    MkLib_MemCopy(concat->chars + lengthA, cwStringB, lengthB);
    concat->chars[concatLength] = L'\0';
    concat->length = concatLength;
    return concat;
}

void MkLib_StringW_Destroy(MkLib_StringW * string) {
    assert(MkLib_MemAlloc);
    assert(MkLib_MemRealloc);
    assert(MkLib_MemFree);
    assert(MkLib_MemCopy);
    assert(MkLib_MemMove);

    assert(string);
    MkLib_MemFree(string);
}

int MkLib_StringW_AppendCwString(MkLib_StringW ** dest, const wchar_t * source) {
    assert(MkLib_MemAlloc);
    assert(MkLib_MemRealloc);
    assert(MkLib_MemFree);
    assert(MkLib_MemCopy);
    assert(MkLib_MemMove);

    assert(dest);
    assert(*dest);
    assert(source);

    size_t sourceLength = wcslen(source);
    size_t newDestLength = (*dest)->length + sourceLength;
    MkLib_StringW * newDest = MkLib_MemRealloc(*dest, sizeof(MkLib_StringW) + newDestLength * sizeof(wchar_t));
    if (!newDest) {
        return 0;
    }
    MkLib_MemCopy(newDest->chars + newDest->length, source, sourceLength);
    newDest->chars[newDestLength] = L'\0';
    newDest->length = newDestLength;
    *dest = newDest;
    return 1;
}

int MkLib_StringW_AppendCwWin32Path(MkLib_StringW ** win32Path, const wchar_t * component) {
    assert(MkLib_MemAlloc);
    assert(MkLib_MemRealloc);
    assert(MkLib_MemFree);
    assert(MkLib_MemCopy);
    assert(MkLib_MemMove);

    assert(win32Path);
    assert(*win32Path);
    assert(component);

    int separatorAdded;
    if ((*win32Path)->chars[(*win32Path)->length - 1] != L'\\') {
        (*win32Path)->chars[(*win32Path)->length++] = L'\\';
        separatorAdded = 1;
    } else {
        separatorAdded = 0;
    }

    if (!MkLib_StringW_AppendCwString(win32Path, component) && separatorAdded) {
        (*win32Path)->chars[--(*win32Path)->length] = L'\0';
        return 0;
    }
    return 1;
}

byte * MkLib_StringW_ToUtf8(const MkLib_StringW * string, size_t * utf8Length) {
    assert(MkLib_MemAlloc);
    assert(MkLib_MemRealloc);
    assert(MkLib_MemFree);
    assert(MkLib_MemCopy);
    assert(MkLib_MemMove);

    assert(string);
    assert(utf8Length);

    return CwStringToUtf8_Impl(string->chars, string->length, utf8Length);
}

MkLib_StringW * MkLib_StringW_FromUtf8(const byte * utf8, const size_t utf8Length) {
    assert(MkLib_MemAlloc);
    assert(MkLib_MemRealloc);
    assert(MkLib_MemFree);
    assert(MkLib_MemCopy);
    assert(MkLib_MemMove);

    assert(utf8Length == 0 || utf8);

    MkLib_StringW * string = MkLib_MemAlloc(sizeof(MkLib_StringW) + utf8Length * sizeof(wchar_t));
    if (!string) {
        return NULL;
    }
    CwStringFromUtf8_Impl(utf8, utf8Length, string->chars, &string->length);
    string = MkLib_MemRealloc(string, sizeof(MkLib_StringW) + string->length * sizeof(wchar_t));
    return string;
}

byte * CwStringToUtf8_Impl(const wchar_t * string, const size_t stringLength, size_t * utf8Length) {
    byte * utf8 = MkLib_MemAlloc(4 * stringLength);
    if (!utf8) {
        return NULL;
    }
    *utf8Length = 0;

    for (size_t i = 0; i != stringLength; i++) {
        u32 c = string[i];
        if (c <= 0x007f) {
            utf8[*utf8Length++] = (byte)c;
        } else if (c <= 0x07ff) {
            utf8[*utf8Length++] = (byte)((c >> 6) + 0b11000000);
            utf8[*utf8Length++] = (byte)((c & 0b00111111) + 0b10000000);
        } else if (c <= 0xffff) {
            utf8[*utf8Length++] = (byte)((c >> 12) + 0b11100000);
            utf8[*utf8Length++] = (byte)((c >> 6 & 0b00111111) + 0b10000000);
            utf8[*utf8Length++] = (byte)((c & 0b00111111) + 0b10000000);
        } else if (c <= 0x0010ffff) {
            utf8[*utf8Length++] = (byte)((c >> 18) + 0b11110000);
            utf8[*utf8Length++] = (byte)((c >> 12 & 0b00111111) + 0b10000000);
            utf8[*utf8Length++] = (byte)((c >> 6 & 0b00111111) + 0b10000000);
            utf8[*utf8Length++] = (byte)((c & 0b00111111) + 0b10000000);
        } else {
            // 0xFFFD
            utf8[*utf8Length++] = 0b11101111;
            utf8[*utf8Length++] = 0b10111111;
            utf8[*utf8Length++] = 0b10111101;
        }
    }

    utf8 = MkLib_MemRealloc(utf8, *utf8Length);
    return utf8;
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

void CwStringFromUtf8_Impl(const byte * utf8, const size_t utf8Length, wchar_t * string, size_t * stringLength) {
    assert(utf8Length == 0 || utf8);
    assert(string);
    assert(stringLength);

    int limited = sizeof(wchar_t) < 4;

    Utf8State state = UTF8_START;
    size_t i = 0;
    u32 c = 0;
    *stringLength = 0;
    while (i != utf8Length || state == UTF8_END_OK || state == UTF8_END_ERROR) {
        switch (state) {
            case UTF8_START:
            {
                size_t readCount;
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
                    string[(*stringLength)++] = 0xfffd;
                } else {
                    if (c == L'\0') {
                        i = utf8Length;
                    } else {
                        string[(*stringLength)++] = (wchar_t)c;
                    }
                }
                state = UTF8_START;
                break;
            }

            case UTF8_END_ERROR:
            {
                string[(*stringLength)++] = 0xfffd;
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
    string[*stringLength] = L'\0';
}

wchar_t ** MkLib_CwDynArrayLinesFromUtf8(const byte * utf8, const size_t utf8Length) {
    assert(MkLib_MemAlloc);
    assert(MkLib_MemRealloc);
    assert(MkLib_MemFree);
    assert(MkLib_MemCopy);
    assert(MkLib_MemMove);

    assert(utf8Length == 0 || utf8);

    wchar_t ** lines = MkLib_DynArray_Create(wchar_t *, 256, 256);
    if (!lines) {
        return NULL;
    }

    byte * lineStart = utf8;
    byte * lineEnd = utf8;
    const byte * eof = utf8 + utf8Length;
    while (lineEnd != eof) {
        if (*lineEnd == '\r' || *lineEnd == '\n' || *lineEnd == '\0' || lineEnd == eof) {
            size_t lineLength = lineEnd - lineStart;
            wchar_t * line = MkLib_DynArray_Create(wchar_t, (((lineLength + 1) / 8) + 1) * 8, 8);
            if (!line) {
                goto error;
            }

            size_t stringLength;
            CwStringFromUtf8_Impl(lineStart, lineLength, line, &stringLength);
            MkLib_DynArray_SetCount(&line, stringLength);

            if (!MkLib_DynArray_Push(&lines, line)) {
                MkLib_DynArray_Destroy(line);
                goto error;
            }

            if (*lineEnd == '\0') {
                lineEnd = eof;
            } else if (*lineEnd == '\r') {
                lineEnd++;
                if (lineEnd != eof && *lineEnd == '\n') {
                    lineEnd++;
                }
            } else {
                lineEnd++;
            }
            lineStart = lineEnd;
        } else {
            lineEnd++;
        }
    }

    return lines;

error:
    ;
    size_t destroyLineCount = MkLib_DynArray_Count(lines);
    for (size_t i = 0; i != destroyLineCount; i++) {
        MkLib_DynArray_Destroy(lines[i]);
    }
    MkLib_DynArray_Destroy(lines);
    return NULL;
}

MkLib_StringW ** MkLib_StringW_LinesFromUtf8(const byte * utf8, const size_t utf8Length, size_t * lineCount) {
    assert(MkLib_MemAlloc);
    assert(MkLib_MemRealloc);
    assert(MkLib_MemFree);
    assert(MkLib_MemCopy);
    assert(MkLib_MemMove);

    assert(utf8Length == 0 || utf8);
    assert(lineCount);

    MkLib_StringW ** lines = MkLib_MemAlloc(256 * sizeof(MkLib_StringW *));
    if (!lines) {
        return NULL;
    }
    *lineCount = 0;

    byte * lineStart = utf8;
    byte * lineEnd = utf8;
    const byte * eof = utf8 + utf8Length;
    while (lineEnd != eof) {
        if (*lineEnd == '\r' || *lineEnd == '\n' || *lineEnd == '\0' || lineEnd == eof) {
            if (*lineCount % 256 == 0 && *lineCount != 0) {
                MkLib_StringW ** newLines = MkLib_MemRealloc(lines, (*lineCount + 256) * sizeof(MkLib_StringW *));
                if (!newLines) {
                    goto error;
                }
                lines = newLines;
            }

            lines[*lineCount] = MkLib_StringW_FromUtf8(lineStart, lineEnd - lineStart);
            if (!lines[*lineCount]) {
                goto error;
            }
            (*lineCount)++;

            if (*lineEnd == '\0') {
                lineEnd = eof;
            } else if (*lineEnd == '\r') {
                lineEnd++;
                if (lineEnd != eof && *lineEnd == '\n') {
                    lineEnd++;
                }
            } else {
                lineEnd++;
            }
            lineStart = lineEnd;
        } else {
            lineEnd++;
        }
    }

    return lines;

error:
    for (size_t i = 0; i != *lineCount; i++) {
        MkLib_MemFree(lines[i]);
    }
    MkLib_MemFree(lines);
    return NULL;
}

size_t MkLib_CwStringFindChar(const wchar_t * string, const size_t length, const size_t start, const wchar_t c) {
    assert(string);
    assert(start <= length);
    for (size_t i = start; i != length; i++) {
        if (string[i] == c) {
            return i;
        }
    }
    return SIZE_MAX;
}