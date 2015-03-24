#include <algorithm> //unicode
#include <cstring> //unicode
#include <cwctype> //unicode
#include <locale> //unicode
#include <iostream>
#include <fstream>
#include <sstream>

#include <stdlib.h>
#include <time.h>

#include "frequency.hh"

/// List of words in alphabetical order
std::vector<std::wstring> dict;

using word_t = typename frequency_list::word_t;

frequency_list frequency;
markov_chain<word_t> first_chain;
markov_chain<word_t, word_t> second_chain;

static bool is_end_punctuation(wchar_t c)
{
  return c == L'.' || c == L'?' || c == L'!';
}

static bool is_punctuation(wchar_t c)
{
  return is_end_punctuation(c) || c == L',' || c == L':' || c == L';';
}

static bool is_end_punctuation(word_t c)
{
  return is_end_punctuation(c[0]);
}

static bool is_punctuation(word_t c)
{
  return is_punctuation(c[0]);
}

/// Open the file as an UTF-8 stream.
static std::wifstream* get_input_stream(const char* file)
{
  std::wifstream* f = new std::wifstream(file, std::ios::binary);
  if (!f->is_open())
  {
    std::cerr << "Unable to open file " << file << "\n";
    abort();
  }
  std::locale l("en_US.UTF-8");
  f->imbue(l);
  return f;
}

/// Initialize the dictionary from the input file
static void init_dict(const char* file, std::vector<std::wstring>& storage)
{
  std::wifstream* f = get_input_stream(file);
  std::wstring word;
  while (*f >> word)
  {
    if (std::find_if(begin(word), end(word), iswdigit) != end(word))
      continue;
    if (is_punctuation(word[word.size() - 1]))
    {
      if (word.size() > 1)
        storage.push_back(word.substr(0, word.size() - 1));
      storage.push_back(word.substr(word.size() - 1, 1));
    }
    else
      storage.push_back(word);
  }
  delete f;
}


static void generate(int n)
{
  word_t second = frequency.get();
  word_t first = first_chain.get(second);
  std::cout << wstrToUtf8(second) << " " << wstrToUtf8(first) << " ";
  for (int i = 0; i < n; i++) {
    size_t count = 0;
    {
      word_t temp = second_chain.get(first, second);
      second = first;
      first = temp;
      std::cout << " " << wstrToUtf8(first);
    }
    while (!is_end_punctuation(first) && count < 100)
    {
      word_t temp = second_chain.get(first, second);
      second = first;
      first = temp;
      if (!is_punctuation(first))
        std::cout << " ";
      std::cout << wstrToUtf8(first);
    }
    if (count == 100)
      std::cout << ". ";
  }
  std::cout << std::endl;
}


int main(int argc, const char *argv[])
{
  srand (time(0));

  if (argc < 2)
  {
    std::cerr << "Usage: ./bullshit <file> [sentences] \n";
    exit(1);
  }

  int number_of_sentences = 0;
  if (argc > 2)
    std::stringstream(argv[2]) >> number_of_sentences;
  if (!number_of_sentences)
    number_of_sentences = 10;

  init_dict(argv[1], dict);
  size_t words = 0;
  word_t first;
  word_t second;
  for (auto w : dict)
  {
    if (!is_punctuation(w[0]) && iswupper(w[0]))
      frequency.insert(w);
    if (words > 0)
    {
      if (!is_end_punctuation(w[0]) && !iswupper(w[0]))
        first_chain.insert(w, first);
      if (words > 1)
        second_chain.insert(w, first, second);
    }
    second = first;
    first = w;
    words++;
  }
  generate(number_of_sentences);

  return 0;
}
