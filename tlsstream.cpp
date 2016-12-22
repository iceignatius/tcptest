#include "tlsstream.h"

//------------------------------------------------------------------------------
bool TTlsStream::Connect(const TSocketAddr &addr, unsigned timeout)
{
    return TTcpStream::Connect(addr);
}
//------------------------------------------------------------------------------
void TTlsStream::Close()
{
    return TTcpStream::Close();
}
//------------------------------------------------------------------------------
int TTlsStream::Send(const void *data, size_t size)
{
    return TTcpStream::Send(data, size);
}
//------------------------------------------------------------------------------
int TTlsStream::Recv(void *buffer, size_t size)
{
    return TTcpStream::Recv(buffer, size);
}
//------------------------------------------------------------------------------
