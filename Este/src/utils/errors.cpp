#include "Este\errors.hpp"

using namespace Log;

FILE* Log::__logfile_handle = NULL;

void Log::Init(std::string logfile) { __logfile_handle = fopen(logfile.c_str(), "w"); }