#include "utf8.hh"
#ifdef UTF8TEST
#include <stdio.h>
#endif


void utf8toWStr(WStr& dest, const Str& src){
  dest.clear();
  wchar_t w = 0;
  int bytes = 0;
  wchar_t err = L'�';
  for (size_t i = 0; i < src.size(); i++){
    unsigned char c = (unsigned char)src[i];
    if (c <= 0x7f){//first byte
      if (bytes){
        dest.push_back(err);
        bytes = 0;
      }
      dest.push_back((wchar_t)c);
    }
    else if (c <= 0xbf){//second/third/etc byte
      if (bytes){
        w = ((w << 6)|(c & 0x3f));
        bytes--;
        if (bytes == 0)
          dest.push_back(w);
      }
      else
        dest.push_back(err);
    }
    else if (c <= 0xdf){//2byte sequence start
      bytes = 1;
      w = c & 0x1f;
    }
    else if (c <= 0xef){//3byte sequence start
      bytes = 2;
      w = c & 0x0f;
    }
    else if (c <= 0xf7){//3byte sequence start
      bytes = 3;
      w = c & 0x07;
    }
    else{
      dest.push_back(err);
      bytes = 0;
    }
  }
  if (bytes)
    dest.push_back(err);
}

void wstrToUtf8(Str& dest, const WStr& src){
  dest.clear();
  for (size_t i = 0; i < src.size(); i++){
    wchar_t w = src[i];
    if (w <= 0x7f)
      dest.push_back((char)w);
    else if (w <= 0x7ff){
      dest.push_back(0xc0 | ((w >> 6)& 0x1f));
      dest.push_back(0x80| (w & 0x3f));
    }
    else if (w <= 0xffff){
      dest.push_back(0xe0 | ((w >> 12)& 0x0f));
      dest.push_back(0x80| ((w >> 6) & 0x3f));
      dest.push_back(0x80| (w & 0x3f));
    }
    else if (w <= 0x10ffff){
      dest.push_back(0xf0 | ((w >> 18)& 0x07));
      dest.push_back(0x80| ((w >> 12) & 0x3f));
      dest.push_back(0x80| ((w >> 6) & 0x3f));
      dest.push_back(0x80| (w & 0x3f));
    }
    else
      dest.push_back('?');
  }
}

Str wstrToUtf8(const WStr& str){
  Str result;
  wstrToUtf8(result, str);
  return result;
}

WStr utf8toWStr(const Str& str){
  WStr result;
  utf8toWStr(result, str);
  return result;
}

std::ostream& operator<<(std::ostream& f, const WStr& s){
  Str s1;
  wstrToUtf8(s1, s);
  f << s1;
  return f;
}

std::istream& operator>>(std::istream& f, WStr& s){
  Str s1;
  f >> s1;
  utf8toWStr(s, s1);
  return f;
}

#ifdef UTF8TEST
bool utf8test(){
  WStr w1;
  //for (wchar_t c = 1; c <= 0x10ffff; c++){
  for (wchar_t c = 0x100000; c <= 0x100002; c++){
    w1 += c;
  }
  Str s = wstrToUtf8(w1);
  WStr w2 = utf8toWStr(s);
  bool result = true;
  if (w1.length() != w2.length()){
    printf("length differs\n");
    //std::cout << "length differs" << std::endl;
    result = false;
  }

  printf("w1: %S\ns: %s\nw2: %S\n", w1.c_str(), s.c_str(), w2.c_str());

  for (size_t i = 0; i < w1.size(); i++)
    if (w1[i] != w2[i]){
      result = false;
      printf("character at pos %x differs (expected %.8x got %.8x)\n", i, w1[i], w2[i]);
      //std::cout << "character at pos " << i  << " differs" << std::endl;
      break;
    }

  if (!result){
    printf("utf8 dump: \n");
    for (size_t i = 0; i < s.size(); i++)
      printf("%2x ", (unsigned char)s[i]);
  }

  return result;
}
#endif
