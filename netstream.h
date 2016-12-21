#ifndef _NETSTREAM_H_
#define _NETSTREAM_H_

#include <gen/net/sockbase.h>

class TNetStream
{
public:
    TNetStream() {}
    virtual ~TNetStream() {}
private:
    TNetStream(const TNetStream &src);             // Not allowed to use
    TNetStream& operator=(const TNetStream &src);  // Not allowed to use

public:
    virtual bool Connect(const TSocketAddr &addr, unsigned timeout=-1) = 0;
    virtual void Close() = 0;

    // Send and receive must in blocking mode.
    virtual int Send(const void *data, size_t size) = 0;
    virtual int Recv(void *buffer, size_t size) = 0;

};

#endif
