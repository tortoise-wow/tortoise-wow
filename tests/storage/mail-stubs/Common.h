#pragma once
#include <cassert>
#include <cstdint>
#include <string>
#include <vector>
#include <ctime>
using uint8=uint8_t; using uint16=uint16_t; using uint32=uint32_t; using uint64=uint64_t;
enum LocaleConstant { LOCALE_enUS };
#define MANGOS_ASSERT(value) assert(value)
constexpr uint32 HOUR=3600, DAY=24*HOUR;
