//===========================================================
// utils.h
//-----------------------------------------------------------
//  Created by William McCarthy on 21 Mar 2022.
//===========================================================

#ifndef __utils_h__
#define __utils_h__

#include <semaphore.h>
#include <cstdarg>


std::mutex& get_common_mutex() { 
    static std::mutex m;
    return m;
}


class Utils {
public:
    static char* strconvert(char* s, int (*fp)(int)) {   // strconvert(s, toupper);   // strconvert(s, tolower);
        char* p = s;
        while (*p != '\0') { *p = fp(*p);  ++p; }
        return s;
    }

  static char* strstrip(char* s) {   // "1, 2, 3, 4"     ==>  // "1 2 3 4"
      char* p = s;
      while (*p != '\0') {
          if ((isalnum(*p) || isspace(*p)) && !ispunct(*p)) { *s++ = *p++; }
          else { ++p; }
      }
      *s = '\0';
      return s;
  }
  
  static const char* yes_or_no(bool condition) { return (condition? "YES" : "no"); }

  static void print_locked(const char* fmt... ) {    // simple printf with locking 
      char c;                                //   does not work with field widths yet
      va_list args;
      va_start(args, fmt);

      std::ostream& os = std::cout;
      get_common_mutex().lock();

      while ((c = *fmt++) != '\0') {
          if ('%' == c) {
              c = *fmt++;
              if (c == '%') { os << '%';  } 
              else if (c == 'd') { int i = va_arg(args, int);        os << i;  }
              else if (c == 'c') { int c = va_arg(args, int);        os << (char)c; }
              else if (c == 'f') { double d = va_arg(args, double);  os << d; }
              else if (c == 's') { char* s = va_arg(args, char*);    os << s; }
          } else { std::cout << c; }
      }
      get_common_mutex().unlock();
  }
};

#endif

