#include <iostream>
#include <getopt.h>
#include "cmdopt.h"

using namespace std;

//------------------------------------------------------------------------------
void TCmdOpt::LoadDefaults()
{
    needhelp = false;
    hosturl  = "";
}
//------------------------------------------------------------------------------
void TCmdOpt::LoadArgs(int argc, char *argv[])
{
    struct option longopts[] =
    {
        { "help" , no_argument, nullptr, 'h' },
        { nullptr, 0          , nullptr,  0  },
    };

    int opt, index;
    while( 0 <= ( opt = getopt_long(argc, argv, "h", longopts, &index) ) )
    {
        switch( opt )
        {
        case 'h':
        case '?':
            needhelp = true;
            break;

        }
    }

    if( optind < argc )
    {
        hosturl = argv[optind++];
    }
    else
    {
        needhelp = true;
        cerr << "ERROR: No host specified!" << endl;
    }

    if( optind < argc )
    {
        needhelp = true;
        cerr << "ERROR: Unknown argument: " << argv[optind] << endl;
    }
}
//------------------------------------------------------------------------------
