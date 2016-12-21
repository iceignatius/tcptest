#ifndef _TCPSTREAM_H_
#define _TCPSTREAM_H_

#include <gen/net/socktcp.h>
#include "netstream.h"

class TTcpStream : public TNetStream
{
protected:
    TSocketTCP sock;

public:
    virtual bool Connect(const TSocketAddr &addr, unsigned timeout=-1);
    virtual void Close();

    virtual int Send(const void *data, size_t size);
    virtual int Recv(void *buffer, size_t size);
};

#endif
