# option_parser.cpp

  option parser 

## Automated --help


```

Usage: example [options]

Options:

  -h, --help                    output help information
  -v, --verbose                 enable verbose stuff
  -r, --required <arg>          required arg
  -o, --optional [arg]          optional arg

```

## Example

```cpp

#include <stdio.h>
#include "option_parser.h"
#include <iostream>
using namespace std;
static void verbose(OptionParser *self)
{
    printf("verbose: enabled\n");
}

static void required(OptionParser *self)
{
    const option_t* op = self->cur_option;
    printf("required: %s or %s\n", op->large, op->small);
}

static void optional(OptionParser *self)
{
    const option_t* op = self->cur_option;
    printf("optional: %s or %s\n", op->large, op->small);
}

int main(int argc, char **argv)
{
    OptionParser cmd;
    cmd.option("-v", "--verbose", "enable verbose stuff", verbose);
    cmd.option("-r", "--required <arg>", "required arg", required);
    cmd.option("-o", "--optional [arg]", "optional arg", optional);
    cmd.parse(argc, argv);
    std::cout << cmd.normalize() << endl;
    cmd.help();
    return 0;
}
```
## Usage

## Short flags

  Compound short flags are automatically expanded to their canonical form. For example `-vLO` would
  become `-v -L -O`.

