#include "term.h"

#include <stdio.h>
#include <unistd.h>

#include "auto_val.h"

namespace term {

namespace color {

auto_val<int> use_color(isatty, STDOUT_FILENO);

char const *normal() { return use_color ? "\033[m" : ""; }

#define C(LIGHT, COLOR) ("\033[" #LIGHT ";" #COLOR "m")

namespace dark {
  char const *black()   { return use_color ? C(0,30) : ""; }
  char const *red()     { return use_color ? C(0,31) : ""; }
  char const *green()   { return use_color ? C(0,32) : ""; }
  char const *yellow()  { return use_color ? C(0,33) : ""; }
  char const *blue()    { return use_color ? C(0,34) : ""; }
  char const *magenta() { return use_color ? C(0,35) : ""; }
  char const *cyan()    { return use_color ? C(0,36) : ""; }
  char const *white()   { return use_color ? C(0,37) : ""; }
} // end namespace dark

namespace light {
  char const *black()   { return use_color ? C(1,30) : ""; }
  char const *red()     { return use_color ? C(1,31) : ""; }
  char const *green()   { return use_color ? C(1,32) : ""; }
  char const *yellow()  { return use_color ? C(1,33) : ""; }
  char const *blue()    { return use_color ? C(1,34) : ""; }
  char const *magenta() { return use_color ? C(1,35) : ""; }
  char const *cyan()    { return use_color ? C(1,36) : ""; }
  char const *white()   { return use_color ? C(1,37) : ""; }
} // end namespace light

} // end namespace color

} // end namespace term
