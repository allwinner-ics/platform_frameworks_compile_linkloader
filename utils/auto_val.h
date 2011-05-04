#ifndef AUTO_VAL_H
#define AUTO_VAL_H

template <typename T>
class auto_val {
private:
  T ans;

public:
  template <typename Functor>
  auto_val(Functor f) : ans(f()) {
  }

  template <typename Functor, typename T1>
  auto_val(Functor f, T1 a1) : ans(f(a1)) {
  }

  template <typename Functor, typename T1, typename T2>
  auto_val(Functor f, T1 a1, T2 a2) : ans(f(a1, a2)) {
  }

  template <typename Functor, typename T1, typename T2, typename T3>
  auto_val(Functor f, T1 a1, T2 a2, T3 a3) : ans(f(a1, a2, a3)) {
  }

  template <typename Functor,
            typename T1, typename T2, typename T3, typename T4>
  auto_val(Functor f, T1 a1, T2 a2, T3 a3, T4 a4) : ans(f(a1, a2, a3, a4)) {
  }

  template <typename Functor,
            typename T1, typename T2, typename T3, typename T4, typename T5>
  auto_val(Functor f, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5)
  : ans(f(a1, a2, a3, a4, a5)) {
  }

  template <typename Functor,
            typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6>
  auto_val(Functor f, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6)
  : ans(f(a1, a2, a3, a4, a5, a6)) {
  }

  template <typename Functor,
            typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7>
  auto_val(Functor f, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7)
  : ans(f(a1, a2, a3, a4, a5, a6, a7)) {
  }

  template <typename Functor,
            typename T1, typename T2, typename T3, typename T4, typename T5,
            typename T6, typename T7, typename T8>
  auto_val(Functor f, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8)
  : ans(f(a1, a2, a3, a4, a5, a6, a7, a8)) {
  }

  inline T const &val() const {
    return ans;
  }

  inline operator T const & () const {
    return ans;
  }
};

#endif // AUTO_VAL_H
