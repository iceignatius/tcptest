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
    bool        tlsmode;
    bool        needsend;
    bool        needrecv;
    std::string srcfile;
    std::string destfile;

public:
    TCmdOpt() { LoadDefaults(); }

public:
    void LoadDefaults();
    void LoadArgs(int argc, char *argv[]);

};

#endif
