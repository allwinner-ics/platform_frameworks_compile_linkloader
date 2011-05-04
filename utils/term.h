#ifndef TERM_H
#define TERM_H

namespace term {

namespace color {

extern char const *normal();

namespace dark {
  extern char const *black();
  extern char const *red();
  extern char const *green();
  extern char const *yellow();
  extern char const *blue();
  extern char const *magenta();
  extern char const *cyan();
  extern char const *white();
} // end namespace dark

namespace light {
  extern char const *black();
  extern char const *red();
  extern char const *green();
  extern char const *yellow();
  extern char const *blue();
  extern char const *magenta();
  extern char const *cyan();
  extern char const *white();
} // end namespace light

} // end namespace color

} // end namespace term

#endif // TERM_H
