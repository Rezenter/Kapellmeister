//
// Created by nz on 22.08.2023.
//
#ifndef GTS_CORE_REQUESTHANDLER_H
#define GTS_CORE_REQUESTHANDLER_H

#include "json.hpp"
#include "Mirror.h"

std::basic_string<char> handleRequest(char* request_str);
inline Mirror mirror;

#endif //GTS_CORE_REQUESTHANDLER_H
