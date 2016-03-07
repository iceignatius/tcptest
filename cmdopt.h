/*
 * Command options.
 */
#ifndef _CMDOPT_H_
#define _CMDOPT_H_

#include <string>

class TCmdOpt
{
public:
    bool        needhelp;
    std::string hosturl;

public:
    TCmdOpt() { LoadDefaults(); }

public:
    void LoadDefaults();
    void LoadArgs(int argc, char *argv[]);

};

#endif
