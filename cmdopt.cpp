#include <iostream>
#include <getopt.h>
#include "cmdopt.h"

using namespace std;

//------------------------------------------------------------------------------
void TCmdOpt::LoadDefaults()
{
    needhelp = false;
    hosturl  = "";
    needsend = false;
    needrecv = false;
    srcfile  = "";
    destfile = "";
}
//------------------------------------------------------------------------------
void TCmdOpt::LoadArgs(int argc, char *argv[])
{
    struct option longopts[] =
    {
        { "help", no_argument      , NULL, 'h' },
        { "send", optional_argument, NULL, 's' },
        { "recv", optional_argument, NULL, 'r' },
        { NULL  , 0                , NULL,  0  },
    };

    int opt, index;
    while( 0 <= ( opt = getopt_long(argc, argv, "h", longopts, &index) ) )
    {
        switch( opt )
        {
        case 's':
            needsend = true;
            srcfile  = optarg ? optarg : "";
            break;

        case 'r':
            needrecv = true;
            destfile = optarg ? optarg : "";
            break;

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
    else if( !needhelp )
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
