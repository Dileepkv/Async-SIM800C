#pragma once

#ifdef WINDOWS
#include <stdio.h>
#endif

#ifdef ARDUINO
#include <stdio.h>
#include "ardprintf.h"
//#include "ardprintf.cpp"
#define printf ardprintf
#endif
