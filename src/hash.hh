#pragma once

#include <tuple>
namespace vcsn
{
  // http://stackoverflow.com/questions/2590677
  inline void hash_combine_hash(std::size_t& seed, size_t h)
  {
    seed ^= h + 0x9e3779b9 + (seed<<6) + (seed>>2);
  }

  // http://stackoverflow.com/questions/2590677
  template <typename T>
  inline void hash_combine(std::size_t& seed, const T& v)
  {
    std::hash<T> hasher;
    hash_combine_hash(seed, hasher(v));
  }
  // These definitions come in handy every time we define variadic tuples.
  namespace detail
  {

    /*-----------------.
    | index_sequence.  |
    `-----------------*/

    // See "Pretty-print std::tuple"
    // <http://stackoverflow.com/questions/6245735>.

    // See O(log N) implementation of integer sequence
    // <http://stackoverflow.com/questions/17424477>

    template<std::size_t...> struct index_sequence
    { using type = index_sequence; };

    template<class S1, class S2> struct concat;

    template<std::size_t... I1, std::size_t... I2>
    struct concat<index_sequence<I1...>, index_sequence<I2...>>
      : index_sequence<I1..., (sizeof...(I1)+I2)...>{};

    template<class S1, class S2>
    using Concat = typename concat<S1, S2>::type;

    template<std::size_t N> struct make_index_sequence;
    template<std::size_t N> using GenSeq =
      typename make_index_sequence<N>::type;

    template<std::size_t N>
    struct make_index_sequence : Concat<GenSeq<N/2>, GenSeq<N - N/2>>{};

    template<> struct make_index_sequence<0> : index_sequence<>{};
    template<> struct make_index_sequence<1> : index_sequence<0>{};

    template<std::size_t off, class S2> struct int_range;

    template<std::size_t off, std::size_t... I>
    struct int_range<off, index_sequence<I...>>
      : index_sequence<I + off...>{};

    template<std::size_t S, std::size_t L>
    struct make_index_range
      : int_range<S, typename make_index_sequence<L>::type>{};

    template<std::size_t S>
    struct make_index_range<S, 0> : index_sequence<>{};
    template<std::size_t S>
    struct make_index_range<S, -1U> : index_sequence<>{};

    template<typename S1, typename S2>
    struct concat_index_sequence;

    template<std::size_t... I1, std::size_t... I2>
    struct concat_index_sequence<index_sequence<I1...>, index_sequence<I2...>>
      : index_sequence<I1..., I2...>{};

    template <typename S1, typename S2>
    using concat_sequence = typename concat_index_sequence<S1, S2>::type;

    // There is a bug in clang making this one useless...
    // The index sequence generated is always <0>
    // Bug report:
    // http://llvm.org/bugs/show_bug.cgi?id=14858
    //template<class... T>
    //using index_sequence_for = make_index_sequence<sizeof...(T)>;
  }
}

namespace std
{

  /*-------------------------.
  | std::hash(tuple<T...>).  |
  `-------------------------*/

  template <typename... Elements>
  struct hash<std::tuple<Elements...>>
  {
    using value_t = std::tuple<Elements...>;
    using indices_t = vcsn::detail::make_index_sequence<sizeof...(Elements)>;

    std::size_t operator()(const value_t& v) const
    {
      return hash_(v, indices_t{});
    }

  private:
    template <std::size_t... I>
    static std::size_t
    hash_(const value_t& v, vcsn::detail::index_sequence<I...>)
    {
      std::size_t res = 0;
      using swallow = int[];
      (void) swallow
        {
          (vcsn::hash_combine(res, std::get<I>(v)), 0)...
        };
      return res;
    }
  };

/*
  template <>
  struct hash <std::wstring>
  {
    size_t operator()(const wstring& _Keyval) const
    {   // hash _Keyval to size_t value by pseudorandomizing transform
      size_t _Val = 2166136261U;
      size_t _First = 0;
      size_t _Last = _Keyval.size();
      size_t _Stride = 1 + _Last / 10;

      if (_Stride < _Last)
        _Last -= _Stride;
      for(; _First < _Last; _First += _Stride)
        _Val = 16777619U * _Val ^ (size_t)_Keyval[_First];
      return (_Val);
    }
  };
*/
}
