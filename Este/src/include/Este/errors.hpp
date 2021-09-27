#pragma once

#define __PATH_SEPERATOR__ '\\'
constexpr const char* __get_filename_from_fullpath(const char* fullpath) {
	return (strrchr(fullpath, __PATH_SEPERATOR__) ? strrchr(fullpath, __PATH_SEPERATOR__) + 1 : fullpath);
};
#define __FILENAME__ __get_filename_from_fullpath(__FILE__)

// TODO: Log to a file instead of printf
#define RAISE_EXCEPTION(format, ...)	{printf("[ERROR] %s:%d " format "\n", __FILENAME__, __LINE__, __VA_ARGS__); __debugbreak();}
#define LOGGING(format, ...) printf(format "\n", __VA_ARGS__);