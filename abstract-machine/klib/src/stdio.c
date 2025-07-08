#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// 辅助函数：整数转字符串，支持10进制和16进制
static int itoa(int value, char *buf, int base, int is_unsigned) {
  char tmp[32];
  int i = 0, j = 0;
  unsigned int v = (is_unsigned ? (unsigned int)value : value);
  int is_negative = 0;
  if (base == 10 && !is_unsigned && value < 0) {
    is_negative = 1;
    v = -value;
  }
  do {
    int digit = v % base;
    tmp[i++] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
    v /= base;
  } while (v);
  if (is_negative) tmp[i++] = '-';
  while (i > 0) buf[j++] = tmp[--i];
  buf[j] = '\0';
  return j;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  size_t pos = 0;
  for (; *fmt; fmt++) {
    if (*fmt != '%') {
      if (pos + 1 < n) out[pos] = *fmt;
      pos++;
      continue;
    }
    fmt++;
    if (*fmt == 'd' || *fmt == 'u' || *fmt == 'x') {
      int base = (*fmt == 'x') ? 16 : 10;
      int is_unsigned = (*fmt == 'u' || *fmt == 'x');
      int val = va_arg(ap, int);
      char buf[32];
      int len = itoa(val, buf, base, is_unsigned);
      for (int i = 0; i < len; i++) {
        if (pos + 1 < n) out[pos] = buf[i];
        pos++;
      }
    } else if (*fmt == 's') {
      const char *s = va_arg(ap, const char *);
      if (!s) s = "(null)";
      while (*s) {
        if (pos + 1 < n) out[pos] = *s;
        pos++;
        s++;
      }
    } else if (*fmt == 'c') {
      char c = (char)va_arg(ap, int);
      if (pos + 1 < n) out[pos] = c;
      pos++;
    } else if (*fmt == '%') {
      if (pos + 1 < n) out[pos] = '%';
      pos++;
    } else {
      // 未知格式，原样输出
      if (pos + 1 < n) out[pos] = '%';
      pos++;
      if (pos + 1 < n) out[pos] = *fmt;
      pos++;
    }
  }
  if (n > 0) out[(pos < n) ? pos : n - 1] = '\0';
  return pos;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return vsnprintf(out, (size_t)-1, fmt, ap);
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vsnprintf(out, n, fmt, ap);
  va_end(ap);
  return ret;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vsnprintf(out, (size_t)-1, fmt, ap);
  va_end(ap);
  return ret;
}

int printf(const char *fmt, ...) {
  char buf[1024];
  va_list ap;
  va_start(ap, fmt);
  int ret = vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  for (int i = 0; buf[i]; i++) putch(buf[i]);
  return ret;
}

#endif
