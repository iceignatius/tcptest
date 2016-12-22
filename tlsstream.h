#ifndef _TLSSTREAM_H_
#define _TLSSTREAM_H_

#include "tcpstream.h"

class TTlsStream : public TTcpStream
{
public:
    virtual bool Connect(const TSocketAddr &addr, unsigned timeout=-1);
    virtual void Close();

    virtual int Send(const void *data, size_t size);
    virtual int Recv(void *buffer, size_t size);
};

#endif
