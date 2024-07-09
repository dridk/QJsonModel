/* QUtf8.hpp
 * Copyright © 2024 Saul D. Beniquez
 * License:
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <QJsonValue>

namespace QUtf8Functions {
/// returns 0 on success; errors can only happen if \a u is a surrogate:
/// Error if \a u is a low surrogate;
/// if \a u is a high surrogate, Error if the next isn't a low one,
/// EndOfString if we run into the end of the string.
template <typename Traits, typename OutputPtr, typename InputPtr>
inline int toUtf8(ushort u, OutputPtr &dst, InputPtr &src, InputPtr end) {
  if (!Traits::skipAsciiHandling && u < 0x80) {
    // U+0000 to U+007F (US-ASCII) - one byte
    Traits::appendByte(dst, uchar(u));
    return 0;
  } else if (u < 0x0800) {
    // U+0080 to U+07FF - two bytes
    // first of two bytes
    Traits::appendByte(dst, 0xc0 | uchar(u >> 6));
  } else {
    if (!QChar::isSurrogate(u)) {
      // U+0800 to U+FFFF (except U+D800-U+DFFF) - three bytes
      if (!Traits::allowNonCharacters && QChar::isNonCharacter(u))
        return Traits::Error;
      // first of three bytes
      Traits::appendByte(dst, 0xe0 | uchar(u >> 12));
    } else {
      // U+10000 to U+10FFFF - four bytes
      // need to get one extra codepoint
      if (Traits::availableUtf16(src, end) == 0)
        return Traits::EndOfString;
      ushort low = Traits::peekUtf16(src);
      if (!QChar::isHighSurrogate(u))
        return Traits::Error;
      if (!QChar::isLowSurrogate(low))
        return Traits::Error;
      Traits::advanceUtf16(src);
      uint ucs4 = QChar::surrogateToUcs4(u, low);
      if (!Traits::allowNonCharacters && QChar::isNonCharacter(ucs4))
        return Traits::Error;
      // first byte
      Traits::appendByte(dst, 0xf0 | (uchar(ucs4 >> 18) & 0xf));
      // second of four bytes
      Traits::appendByte(dst, 0x80 | (uchar(ucs4 >> 12) & 0x3f));
      // for the rest of the bytes
      u = ushort(ucs4);
    }
    // second to last byte
    Traits::appendByte(dst, 0x80 | (uchar(u >> 6) & 0x3f));
  }
  // last byte
  Traits::appendByte(dst, 0x80 | (u & 0x3f));
  return 0;
}
inline bool isContinuationByte(uchar b) { return (b & 0xc0) == 0x80; }
/// returns the number of characters consumed (including \a b) in case of
/// success; returns negative in case of error: Traits::Error or
/// Traits::EndOfString
template <typename Traits, typename OutputPtr, typename InputPtr>
inline int fromUtf8(uchar b, OutputPtr &dst, InputPtr &src, InputPtr end) {
  int charsNeeded;
  uint min_uc;
  uint uc;
  if (!Traits::skipAsciiHandling && b < 0x80) {
    // US-ASCII
    Traits::appendUtf16(dst, b);
    return 1;
  }
  if (!Traits::isTrusted && Q_UNLIKELY(b <= 0xC1)) {
    // an UTF-8 first character must be at least 0xC0
    // however, all 0xC0 and 0xC1 first bytes can only produce overlong
    // sequences
    return Traits::Error;
  } else if (b < 0xe0) {
    charsNeeded = 2;
    min_uc = 0x80;
    uc = b & 0x1f;
  } else if (b < 0xf0) {
    charsNeeded = 3;
    min_uc = 0x800;
    uc = b & 0x0f;
  } else if (b < 0xf5) {
    charsNeeded = 4;
    min_uc = 0x10000;
    uc = b & 0x07;
  } else {
    // the last Unicode character is U+10FFFF
    // it's encoded in UTF-8 as "\xF4\x8F\xBF\xBF"
    // therefore, a byte higher than 0xF4 is not the UTF-8 first byte
    return Traits::Error;
  }
  int bytesAvailable = Traits::availableBytes(src, end);
  if (Q_UNLIKELY(bytesAvailable < charsNeeded - 1)) {
    // it's possible that we have an error instead of just unfinished bytes
    if (bytesAvailable > 0 && !isContinuationByte(Traits::peekByte(src, 0)))
      return Traits::Error;
    if (bytesAvailable > 1 && !isContinuationByte(Traits::peekByte(src, 1)))
      return Traits::Error;
    return Traits::EndOfString;
  }
  // first continuation character
  b = Traits::peekByte(src, 0);
  if (!isContinuationByte(b))
    return Traits::Error;
  uc <<= 6;
  uc |= b & 0x3f;
  if (charsNeeded > 2) {
    // second continuation character
    b = Traits::peekByte(src, 1);
    if (!isContinuationByte(b))
      return Traits::Error;
    uc <<= 6;
    uc |= b & 0x3f;
    if (charsNeeded > 3) {
      // third continuation character
      b = Traits::peekByte(src, 2);
      if (!isContinuationByte(b))
        return Traits::Error;
      uc <<= 6;
      uc |= b & 0x3f;
    }
  }
  // we've decoded something; safety-check it
  if (!Traits::isTrusted) {
    if (uc < min_uc)
      return Traits::Error;
    if (QChar::isSurrogate(uc) || uc > QChar::LastValidCodePoint)
      return Traits::Error;
    if (!Traits::allowNonCharacters && QChar::isNonCharacter(uc))
      return Traits::Error;
  }
  // write the UTF-16 sequence
  if (!QChar::requiresSurrogates(uc)) {
    // UTF-8 decoded and no surrogates are required
    // detach if necessary
    Traits::appendUtf16(dst, ushort(uc));
  } else {
    // UTF-8 decoded to something that requires a surrogate pair
    Traits::appendUcs4(dst, uc);
  }
  Traits::advanceByte(src, charsNeeded - 1);
  return charsNeeded;
}
} // namespace QUtf8Functions

struct QUtf8BaseTraits {
  static const bool isTrusted = false;
  static const bool allowNonCharacters = true;
  static const bool skipAsciiHandling = false;
  static const int Error = -1;
  static const int EndOfString = -2;
  static bool isValidCharacter(uint u) { return int(u) >= 0; }
  static void appendByte(uchar *&ptr, uchar b) { *ptr++ = b; }
  static uchar peekByte(const uchar *ptr, int n = 0) { return ptr[n]; }
  static qptrdiff availableBytes(const uchar *ptr, const uchar *end) {
    return end - ptr;
  }
  static void advanceByte(const uchar *&ptr, int n = 1) { ptr += n; }
  static void appendUtf16(ushort *&ptr, ushort uc) { *ptr++ = uc; }
  static void appendUcs4(ushort *&ptr, uint uc) {
    appendUtf16(ptr, QChar::highSurrogate(uc));
    appendUtf16(ptr, QChar::lowSurrogate(uc));
  }
  static ushort peekUtf16(const ushort *ptr, int n = 0) { return ptr[n]; }
  static qptrdiff availableUtf16(const ushort *ptr, const ushort *end) {
    return end - ptr;
  }
  static void advanceUtf16(const ushort *&ptr, int n = 1) { ptr += n; }
  // it's possible to output to UCS-4 too
  static void appendUtf16(uint *&ptr, ushort uc) { *ptr++ = uc; }
  static void appendUcs4(uint *&ptr, uint uc) { *ptr++ = uc; }
};
