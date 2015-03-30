#pragma once

#include <stdlib.h>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "hash.hh"
#include "utf8.hh"

class no_word_error {};

class frequency_list
{
public:
  using word_t = std::wstring;
  using freq_t = double;

private:
  using word_freq_t = std::pair<word_t, size_t>;
  using container_t = std::vector<word_freq_t>;
  using map_t = std::unordered_map<word_t, size_t>;

public:

  frequency_list()
    : total_(0)
  {}

  void insert(const word_t& w)
  {
    auto it = index_map_.find(w);
    if (it == end(index_map_))
    {
      list_.push_back({w, 1});
      index_map_[w] = list_.size() - 1;
    }
    else
      ++(list_[index_map_[w]].second);
    ++total_;
  }

  word_t get()
  {
    if (!total_)
      throw no_word_error{};
    size_t seed = rand() % total_;
    size_t index = 0;
    for (auto p : list_)
    {
      index += p.second;
      if (index >= seed)
        return p.first;
    }
    return L"";
  }

private:
  size_t total_;
  map_t index_map_;
  container_t list_;
};

template <typename... Key>
class markov_chain
{
  using key_t = std::tuple<Key...>;
  using freq_list_t = frequency_list;
  using freq_t = typename freq_list_t::freq_t;
  using word_t = typename freq_list_t::word_t;
  using map_t = std::unordered_map<key_t, freq_list_t>;

public:

  markov_chain()
  {}

  void insert(const word_t& w, Key... k)
  {
    key_t key = key_t{k...};
    auto it = map_.find(key);
    if (it == end(map_))
    {
      freq_list_t l;
      l.insert(w);
      map_[key] = l;
    }
    else
      map_[key].insert(w);
  }

  word_t get(Key... k)
  {
    key_t key = key_t{k...};
    return map_[key].get();
  }

private:

  map_t map_;
};
