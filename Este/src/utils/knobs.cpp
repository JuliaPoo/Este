#include "Este\knobs.hpp"
#include "Este\errors.hpp"
#include "Este\utils.hpp"

#include <iostream>
#include <toml.h>
#include <pin.H>

using namespace Knobs;

static bool is_init = false;

KNOB<std::string> knobs_config_file(KNOB_MODE_WRITEONCE, "pintool",
    "config-file", "este-config.toml",
    "Config file for Este."
);

#define OUTPUT_DIR_DOCUMENTATION                                             \
"         output-dir @type str:\n"                                           \
"             Directory where Este will dump logs and the taintdatabase.\n"

#define BINARY_WHITELIST_DOCUMENTATION                                       \
"         binary-whitelist @type List[str]:\n"                               \
"             Este will analyse only binaries in this list\n"             
 

std::vector<std::string> whitelist_binaries;
std::string output_dir;
std::string output_prefix;

void _parse_output_table(toml_table_t* conf)
{
    toml_table_t* taint_database = toml_table_in(conf, "OUTPUT");
    if (!taint_database) {
        RAISE_EXCEPTION("[CONFIG] Missing config [OUTPUT]. Cannot perform analysis without output config specified");
    }
    else {
        toml_datum_t v = toml_string_in(taint_database, "output-dir");
        if (!v.ok) {
            RAISE_EXCEPTION(
                "[CONFIG] Missing config `output-dir`.\n\n"
                OUTPUT_DIR_DOCUMENTATION"\n\n"
                "Missing option `output-dir` in table [OUTPUT] in config file!");
        }
        output_dir = v.u.s;
    }
}

void _init_logging()
{
    OS_MkDir(Knobs::getOutputDir().c_str(), 511);

    std::stringstream fileprefix;
    NATIVE_PID pid; OS_GetPid(&pid);
    fileprefix << Knobs::getOutputDir() << "/pid" << pid;
    output_prefix = fileprefix.str();

    // Initialise logging
    Log::Init(output_prefix + ".log");
}

void _parse_analysis_scope_table(toml_table_t* conf)
{
    toml_table_t* taint_policy = toml_table_in(conf, "ANALYSIS-SCOPE");
    if (!taint_policy) {
        LOGGING("[CONFIG] Missing config table [ANALYSIS-SCOPE]!\n");
        LOGGING(BINARY_WHITELIST_DOCUMENTATION "\n");
        RAISE_EXCEPTION("Missing config table [ANALYSIS-SCOPE] in config file!");
    }
    else {

        toml_array_t* t = toml_array_in(taint_policy, "binary-whitelist");
        if (!t) {
            LOGGING("[CONFIG] Missing config `binary-whitelist`.\n");
            LOGGING(BINARY_WHITELIST_DOCUMENTATION "\n");
            RAISE_EXCEPTION("Missing option `binary-whitelist` in table [ANALYSIS-SCOPE] in config file!");
        }
        else {
            for (uint i = 0;; ++i) {
                toml_datum_t fn = toml_string_at(t, i);
                if (!fn.ok) break;
                whitelist_binaries.push_back(fn.u.s);
            }
        }
    }
}

void parse_config_file(const std::string configfile)
{
    FILE* fp;
    char errbuf[200];

    fp = fopen(configfile.c_str(), "r");
    if (!fp) RAISE_EXCEPTION("Failed to open config file: `%s`", configfile.c_str());

    toml_table_t* conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
    if (!conf) RAISE_EXCEPTION("Failed to parse config file: `%s`, %s", configfile.c_str(), errbuf);
    fclose(fp);

    _parse_output_table(conf);
    _init_logging();
    LOGGING("[CONFIG] Using output directory `%s`", output_dir.c_str());

    toml_datum_t title = toml_string_in(conf, "title");
    if (title.ok)
        LOGGING("[CONFIG] Using config file `%s: %s`", configfile.c_str(), title.u.s);

    _parse_analysis_scope_table(conf);

    std::stringstream _taint_whitelist_str;
    for (auto& i : whitelist_binaries)
        _taint_whitelist_str << "`" << i << "`" << " ";

    LOGGING("[CONFIG] Final config: output-dir = %s", output_dir.c_str());
    LOGGING("[CONFIG] Final config: binary-whitelist = %s", _taint_whitelist_str.str().c_str());
}

void Knobs::Init()
{
    is_init = true;
    auto config_file = knobs_config_file.Value();
    parse_config_file(config_file);
}

std::vector<std::string> Knobs::getWhitelistBinaries()
{
    if (!is_init)
        RAISE_EXCEPTION("Knobs not initialised before calling `getWhitelistBinaries`! Call Knobs::Init()");
    return whitelist_binaries;
}

std::string Knobs::getOutputDir()
{
    if (!is_init)
        RAISE_EXCEPTION("Knobs not initialised before calling `getOutputDir`! Call Knobs::Init()");
    return output_dir;
}

std::string Knobs::getOutputPrefix()
{
    if (!is_init)
        RAISE_EXCEPTION("Knobs not initialised before calling `getOutputPrefix`! Call Knobs::Init()");
    return output_prefix;
}

bool Knobs::isBinaryWhitelisted(std::string path)
{
    if (!is_init)
        RAISE_EXCEPTION("Knobs not initialised before calling `isBinaryWhitelisted`! Call Knobs::Init()");

    path = EsteUtils::toLower(path);
    for (auto& p : whitelist_binaries)
        if (EsteUtils::hasSuffix(path, EsteUtils::toLower(p)))
            return true;

    return false;
}