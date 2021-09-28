#pragma once

#include <type_traits>
#include <string>

#define __PATH_SEPERATOR__ '\\'
constexpr const char* __get_filename_from_fullpath(const char* fullpath) {
	return (strrchr(fullpath, __PATH_SEPERATOR__) ? strrchr(fullpath, __PATH_SEPERATOR__) + 1 : fullpath);
};
#define __FILENAME__ __get_filename_from_fullpath(__FILE__)

#define __CONDITIONAL_FPRINTF(format, ...) \
if (Log::__logfile_handle) {fprintf(Log::__logfile_handle, format, __VA_ARGS__); fflush(Log::__logfile_handle); } \
else {printf(format, __VA_ARGS__);}

#define RAISE_EXCEPTION(format, ...) { __CONDITIONAL_FPRINTF("[ERROR] %s:%d " format "\n", __FILENAME__, __LINE__, __VA_ARGS__); __debugbreak(); }
#define LOGGING(format, ...) __CONDITIONAL_FPRINTF(format "\n", __VA_ARGS__);

namespace Log
{
	extern FILE* __logfile_handle;

	// Initialise logging with log filename
	void Init(std::string logfile);
}