#include <iostream>
#include <gen/net/urlpar.h>
#include <gen/net/socktcp.h>
#include "cmdopt.h"

using namespace std;

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

bool ConnectHost(TSocketTCP &sock, const TSocketAddr &addr)
{
    bool res = sock.Connect(addr);
    cout << "TCP connect: " << ( res ? "Succeed." : "Failed!" ) << endl;
    return res;
}

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

    return 0;
}
