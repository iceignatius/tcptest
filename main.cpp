#include <stdio.h>
#include <iostream>
#include <gen/systime.h>
#include <gen/net/urlpar.h>
#include <gen/net/socktcp.h>
#include "cmdopt.h"

using namespace std;

//------------------------------------------------------------------------------
void PrintHelp()
{
    cout << "Usage: tcptest [options] HOSTURL" << endl;
    cout << "Options:" << endl;
    cout << "  -h, --help          Print help message." << endl;
    cout << "      --send[=FILE]   Send data to the host after the connection be established." << endl;
    cout << "                      And the data will be read from the standard input" << endl;
    cout << "                      if no source file be specified." << endl;
    cout << "      --recv[=FILE]   Receive data from the host to the file." << endl;
    cout << "                      And the data will be printed to the standard output" << endl;
    cout << "                      if no destination file be specified." << endl;
}
//------------------------------------------------------------------------------
bool ExtractHostAddr(TSocketAddr &addr, const string &url)
{
    char hostname[512] = {0};
    if( !urlpar_extract_host(hostname, sizeof(hostname)-1, url.c_str()) )
    {
        cerr << "ERROR: Cannot extract host name!" << endl;
        return false;
    }

    ipv4_t ip = ipv4_from_str(hostname);
    if( !ip.val ) ip = socknet_get_ip_by_hostname(hostname);
    if( !ip.val )
    {
        cerr << "ERROR: Cannot get host IP!" << endl;
        return 1;
    }

    uint16_t port = urlpar_get_port(url.c_str());
    if( !port )
    {
        cerr << "ERROR: Cannot get host port!" << endl;
        return 1;
    }

    char ipstr[64] = {0};
    ipv4_to_str(ipstr, sizeof(ipstr), ip);
    cout << "Host IP: " << ipstr << ", port: " << port << endl;

    addr.SetIP(ip);
    addr.SetPort(port);

    return true;
}
//------------------------------------------------------------------------------
bool ConnectHost(TSocketTCP &sock, const TSocketAddr &addr)
{
    bool res = sock.Connect(addr);
    cout << "TCP connect: " << ( res ? "Succeed." : "Failed!" ) << endl;
    return res;
}
//------------------------------------------------------------------------------
bool SendData(TSocketTCP &sock, const string &filename)
{
    FILE *srcfile = NULL;

    bool res = true;
    try
    {
        if( !filename.empty() && !( srcfile = fopen(filename.c_str(), "rb") ) )
            throw runtime_error( "Cannot open source file: " + filename );

        FILE *src = srcfile ? srcfile : stdin;

        cout << "Sending data..." << endl;
        sock.SetBlockMode();

        size_t sentsz_total = 0;
        while( !feof(src) )
        {
            uint8_t data[512];
            size_t  datasz = fread(data, 1, sizeof(data), src);

            int sentsz = sock.Send(data, datasz);
            if( sentsz <= 0 ) throw runtime_error("Send data failed!");

            sentsz_total += sentsz;
        }

        cout << "Send finished, total " << sentsz_total << " bytes sent out." << endl;
    }
    catch(exception &e)
    {
        res = false;
        cerr << "ERROR: " << e.what() << endl;
    }

    if( srcfile ) fclose(srcfile);

    return res;
}
//------------------------------------------------------------------------------
bool ReceiveData(TSocketTCP &sock, const string &filename)
{
    FILE *destfile = NULL;

    bool res = true;
    try
    {
        if( !filename.empty() && !( destfile = fopen(filename.c_str(), "wb") ) )
            throw runtime_error( "Cannot open destination file: " + filename );

        FILE *dest = destfile ? destfile : stdout;

        cout << "Receiving data..." << endl;
        sock.SetNonblockMode();

        size_t recvsz_total = 0;
        while(true)
        {
            uint8_t buf[512];
            int recvsz = sock.Receive(buf, sizeof(buf));
            if( recvsz > 0 )
            {
                fwrite(buf, 1, recvsz, dest);
                recvsz_total += recvsz;
            }
            else if( recvsz < 0 )
            {
                break;
            }
            else
            {
                systime_sleep_awhile();
            }
        }

        cout << "Receive finished, total " << recvsz_total << " bytes received." << endl;
    }
    catch(exception &e)
    {
        res = false;
        cerr << "ERROR: " << e.what() << endl;
    }

    if( destfile ) fclose(destfile);

    return res;
}
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    TCmdOpt cmdopts;
    cmdopts.LoadArgs(argc, argv);

    if( cmdopts.needhelp )
    {
        PrintHelp();
        return 0;
    }

    TSocketAddr addr;
    if( !ExtractHostAddr(addr, cmdopts.hosturl) )
        return 1;

    TSocketTCP sock;
    if( !ConnectHost(sock, addr) )
        return 1;

    if( cmdopts.needsend && !SendData(sock, cmdopts.srcfile) )
        return 1;

    if( cmdopts.needrecv && !ReceiveData(sock, cmdopts.destfile) )
        return 1;

    return 0;
}
//------------------------------------------------------------------------------
