#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "option_parser.h"
static void error(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}
static void helps(OptionParser* that)
{
    that->help();
}
void OptionParser::help()
{
    printf("\n");
    printf("  Usage: %s [options]\n", margv[0].c_str());
    printf("\n");
    printf("  Options:\n");
    printf("\n");
    for(int i = 0; i < this->option_count; ++i)
    {
        option_t *option = &this->options[i];
        printf("    %s, %-25s %s\n"
               , option->small
               , option->large_with_arg
               , option->description);
    }
    printf("\n");
    exit(0);
}

OptionParser::OptionParser()
{
    this->option_count = 0;
    //option("-V", "--version", "output program version", version);
    option("-h", "--help", "output help information", callback_t(helps));
}

OptionParser::~OptionParser()
{
    for(int i = 0; i < this->option_count; ++i)
    {
        option_t *option = &this->options[i];
        free(option->large);
    }
}
/*
 * Take "--required <arg>" and populate
 * Take "--optional [arg]" and optional
 */
void OptionParser::parseArgname(option_t* option)
{
    int buffer = 0;
    size_t flagpos = 0;
    const char* str = option->large_with_arg;
    size_t len = strlen(str);
    for(int i = 0; i < len; ++i)
    {
        if(buffer || '[' == str[i] || '<' == str[i])
        {
            buffer = 1;
            if('[' == str[i]) option->optional_arg = 1;
            if('<' == str[i]) option->required_arg = 1;
        }
        else
        {
            if(' ' == str[i]) continue;
            option->large[flagpos++] = str[i];
        }
    }
    option->large[flagpos] = '\0';
}
void OptionParser::normalizeArgs(int argc, char **argv)
{
    margv.push_back(argv[0]);
    for(int i = 1; i < argc; ++i)
    {
        const char *arg = argv[i];
        int len = strlen(arg);
        if(len > 2 && '-' == arg[0])
        {
            char* p = strchr(arg + 1, '=');//split '='
            if(p)
            {
                int pos = int(p - arg);
                std::string str(arg, arg + pos);
                this->margv.push_back(str);
                this->margv.push_back(p + 1);
            }
            else if(!strchr(arg + 1, '-'))//split '-abc'
            {
                for(int j = 1; j < len; ++j)
                {
                    this->margv.push_back(std::string("-") + arg[j]);
                }
            }
            else //for -ab-c
            {
                this->margv.push_back(arg);
            }
        }
        else
            this->margv.push_back(arg);
    }
    margv.push_back("");
}
void OptionParser::option(const char *small, const char *large, const char *desc, callback_t cb)
{
    int n = this->option_count++;
    if(n == COMMANDER_MAX_OPTIONS) error("Maximum option definitions exceeded");
    option_t *option = &this->options[n];
    option->cb = cb;
    option->small = small;
    option->description = desc;
    option->required_arg = option->optional_arg = 0;
    option->large_with_arg = large;
    option->large = (char*)malloc(strlen(large) + 1);
    parseArgname(option);
    option->argvalue = NULL;
}

void  OptionParser::parseArgs()
{
    int n = margv.size() -  1;
    for(int i = 1; i < n; ++i)
    {
        bool found = false;
        const std::string& argi = this->margv[i];
        for(int j = 0; j < this->option_count; ++j)
        {
            option_t* option = &this->options[j];
            // match flag
            if(argi == option->small || argi == option->large)
            {
                found = true;
                // required
                if(option->required_arg)
                {
                    const std::string& arg = this->margv[++i];
                    option->argvalue = this->margv[i].c_str();
                    if(arg.empty() || '-' == arg[0])
                    {
                        fprintf(stderr, "\"%s\" argument value required\n", option->large);
                        exit(1);
                    }
                }
                // optional
                if(option->optional_arg)
                {
                    if(!margv[i+1].empty() && '-' != margv[i + 1][0])
                    {
                        option->argvalue = this->margv[++i].c_str();
                    }
                }
                // invoke callback
                this->cur_option = option;
                if (option->cb) option->cb(this);
            }
        }
        if (!found)
        {
            fprintf(stderr, "unrecognized flag %s\n", argi.c_str());
            exit(1);
        }
    }
}
void OptionParser::parse(int argc, char **argv)
{
    normalizeArgs(argc, argv);
    parseArgs();
}
std::string OptionParser::normalize() const
{
    std::string r;
    for (int i = 0;i < margv.size(); ++i)
    {
        r += margv[i] + " ";
    }
    return r;
}
