#include <iostream>
#include <gen/net/urlpar.h>
#include <gen/net/socktcp.h>

using namespace std;

int main(int argc, char *argv[])
{
    if( argc < 2 )
    {
        cout << "Usage: tcptest HOSTURL" << endl;
        return 1;
    }

    char *url = argv[1];

    char hostname[512] = {0};
    if( !urlpar_extract_host(hostname, sizeof(hostname)-1, url) )
    {
        cerr << "ERROR: Cannot extract host name!" << endl;
        return 1;
    }

    ipv4_t hostip = ipv4_from_str(hostname);
    if( !hostip.val ) hostip = socknet_get_ip_by_hostname(hostname);
    if( !hostip.val )
    {
        cerr << "ERROR: Cannot get host IP!" << endl;
        return 1;
    }

    uint16_t hostport = urlpar_get_port(url);
    if( !hostport )
    {
        cerr << "ERROR: Cannot get host port!" << endl;
        return 1;
    }

    char ipstr[64] = {0};
    ipv4_to_str(ipstr, sizeof(ipstr), hostip);
    cout << "Host IP: " << ipstr << ", port: " << hostport << endl;

    TSocketTCP sock;
    bool connres = sock.Connect(TSocketAddr(hostip, hostport));
    cout << "TCP connect: " << ( connres ? "Succeed." : "Failed!" ) << endl;

    return connres ? 0 : 1;
}
