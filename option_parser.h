#ifndef OPTION_PARSER_H
#define OPTION_PARSER_H
#include <string>
#include <vector>
struct OptionParser;
typedef void (* callback_t)(struct OptionParser *self);

typedef struct
{
    int optional_arg;
    int required_arg;
    char *large;
    const char *small;
    const char *large_with_arg;
    const char *description;
    const char *argvalue;
    callback_t cb;
} option_t;

class OptionParser
{
public:
    OptionParser();
    ~OptionParser();
    void help();
    void option(const char *small, const char *large, const char *desc, callback_t cb = NULL);
    void parse(int argc, char **argv);
    void normalizeArgs(int argc, char **argv);
    void parseArgs();
    const option_t* cur_option; //current argument
    std::string normalize() const;
private:
    static const int COMMANDER_MAX_ARGS = 32;
    static const int COMMANDER_MAX_OPTIONS = 32;
    void parseArgname(option_t* option);
    option_t options[COMMANDER_MAX_OPTIONS];
    int option_count;
    std::vector<std::string> margv;
};
#endif 
