#pragma once

#include <string>
#include <iostream>

typedef std::string Str;
typedef std::wstring WStr;

std::ostream& operator<<(std::ostream& f, const WStr& s);
std::istream& operator>>(std::istream& f, WStr& s);
void utf8toWStr(WStr& dest, const Str& src);
void wstrToUtf8(Str& dest, const WStr& src);
Str wstrToUtf8(const WStr& str);
WStr utf8toWStr(const Str& str);
