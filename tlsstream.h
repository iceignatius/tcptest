#ifndef _TLSSTREAM_H_
#define _TLSSTREAM_H_

#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/ssl.h>
#include "tcpstream.h"

class TTlsStream : public TTcpStream
{
protected:
    mbedtls_entropy_context  entropy;
    mbedtls_ctr_drbg_context rndg;
    mbedtls_ssl_config       conf;
    mbedtls_ssl_context      tls;

public:
    TTlsStream();
    virtual ~TTlsStream();

public:
    virtual bool Connect(const TSocketAddr &addr, unsigned timeout=-1);
    virtual void Close();

    virtual int Send(const void *data, size_t size);
    virtual int Recv(void *buffer, size_t size);

};

#endif
