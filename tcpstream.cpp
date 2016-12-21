#include "tcpstream.h"

//------------------------------------------------------------------------------
bool TTcpStream::Connect(const TSocketAddr &addr, unsigned timeout)
{
    if( !sock.Connect(addr, timeout) ) return false;
    sock.SetBlockMode();

    return true;
}
//------------------------------------------------------------------------------
void TTcpStream::Close()
{
    sock.Close();
}
//------------------------------------------------------------------------------
int TTcpStream::Send(const void *data, size_t size)
{
    return sock.Send(data, size);
}
//------------------------------------------------------------------------------
int TTcpStream::Recv(void *buffer, size_t size)
{
    return sock.Receive(buffer, size);
}
//------------------------------------------------------------------------------
