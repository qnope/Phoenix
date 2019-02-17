#pragma once

#include "../ltl/ltl.h"
#include <iostream>

#define _LAMBDA_WRITE_AUTO(x, y, ...)                                                    \
  auto x LPL_WHEN(LPL_IS_NOT_PARENTHESES(y))(                                            \
      LPL_CAT(, , ) LPL_DEFER_TWICE(_LAMBDA_WRITE_AUTO_I)()(y, __VA_ARGS__))

#define _LAMBDA_WRITE_AUTO_I() _LAMBDA_WRITE_AUTO
#define _(variables, expr)                                                               \
  [](LPL_EVAL(LPL_DEFER(_LAMBDA_WRITE_AUTO)(LPL_STRIP_PARENTHESES(variables), (),        \
                                            ()))) { return expr; }

namespace lol {
struct Ostream_Wrapper {
  Ostream_Wrapper(std::ostream &stream) : stream{stream} {}
  std::ostream &stream;
};

LTL_MAKE_IS_KIND(std::optional, is_optional);

template <typename T> Ostream_Wrapper &operator<<(Ostream_Wrapper &s, T &&t) {
  if_constexpr(ltl::is_iterable(t)) {
    for (auto &&e : FWD(t))
      s.stream << FWD(e) << " ";
    s.stream << std::endl;
  }

  else_if_constexpr(is_optional(t)) {
    if (FWD(t)) {
      std::cout << *t;
    }
  }

  else s.stream << FWD(t);

  return s;
}

Ostream_Wrapper cout(std::cout);

constexpr auto identityFunction = [](auto &&x) constexpr -> decltype(auto) {
  return FWD(x);
};

template <typename F, typename G, typename... Fs> auto compose(F f, G g, Fs... fs) {
  using namespace ltl::literals;
  auto ret = [f, g](auto &&x) { return g(f(FWD(x))); };
  if_constexpr(ltl::type_list_v<Fs...>.length > 0_n) return compose(ret, fs...);
  else return ret;
}

template <typename... R> struct Range_Wrapper : R... {
  constexpr Range_Wrapper(R... r) : R{std::move(r)}... {}

  template <typename T> constexpr auto operator()(T &&t) {
    return compose(identityFunction, static_cast<R &>(*this)...)(FWD(t));
  }
};

template <typename... R> Range_Wrapper(R...)->Range_Wrapper<R...>;

template <typename R, typename... Rs>
constexpr auto operator|(R &&c, Range_Wrapper<Rs...> r) {
  typed_static_assert_msg(ltl::is_iterable(c), "R must be an iterable");
  return r(FWD(c));
}

template <typename... R1, typename... R2>
constexpr auto operator|(Range_Wrapper<R1...> r1, Range_Wrapper<R2...> r2) {
  return Range_Wrapper{static_cast<R1 &>(r1)..., static_cast<R2 &>(r2)...};
}

template <typename F> constexpr auto filter(F &&f) {
  return Range_Wrapper{ltl::filter(FWD(f))};
}

template <typename F> constexpr auto map(F &&f) {
  return Range_Wrapper{ltl::map(FWD(f))};
}

template <typename... F> struct Fmap_Wrapper : F... {
  constexpr Fmap_Wrapper(F... f) : F{std::move(f)}... {}

  template <typename T> constexpr auto operator()(T &&t) {
    return compose(identityFunction, static_cast<F &>(*this)...)(FWD(t));
  }
};

template <typename... F> Fmap_Wrapper(F...)->Fmap_Wrapper<F...>;

template <typename Opt, typename... Fs>
constexpr auto operator|(Opt &&opt, Fmap_Wrapper<Fs...> f)
    -> decltype(std::optional{f(*FWD(opt))}) {
  typed_static_assert_msg(is_optional(opt), "Opt must be an optional");
  if (FWD(opt))
    return f(*FWD(opt));
  return std::nullopt;
}

template <typename... F1, typename... F2>
constexpr auto operator|(Fmap_Wrapper<F1...> f1, Fmap_Wrapper<F2...> f2) {
  return Fmap_Wrapper{static_cast<F1 &>(f1)..., static_cast<F2 &>(f2)...};
}

template <typename F> constexpr auto fmap(F &&f) { return Fmap_Wrapper{FWD(f)}; }

} // namespace lol
