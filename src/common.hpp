#pragma once

#include "SDL.h"

#include <string>
#include <vector>

void splitString(std::vector<std::string> &container, std::string toSplit, std::string delim = " ");

std::string trimWhitespace(std::string s);

SDL_Rect stringToSDLRect(std::string s, std::string delim = " ");