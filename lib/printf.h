#pragma once

#ifdef _WINDOWS_
#include "stdio.h"
#endif

#ifdef _ARDUINO_
#include "ardprintf.h"
#include "ardprintf.c"
#define printf ardprintf
#endif