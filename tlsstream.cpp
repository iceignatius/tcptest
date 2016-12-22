#include <stdio.h>
#include <stdexcept>
#include <iostream>
#include <mbedtls/error.h>
#include "tlsstream.h"

using namespace std;

//------------------------------------------------------------------------------
static
void on_debug_message(void *stream, int level, const char *file, int line, const char *msg)
{
    fprintf((FILE*)stream, "DEBUG (%d): %s\n", level, msg);
}
//------------------------------------------------------------------------------
static
int on_send(TSocketTCP *sock, const unsigned char *data, size_t size)
{
    int sentsz = sock->Send(data, size);
    return sentsz ? sentsz : MBEDTLS_ERR_SSL_WANT_WRITE;
}
//------------------------------------------------------------------------------
static
int on_recv(TSocketTCP *sock, unsigned char *buf, size_t size)
{
    int recvsz = sock->Receive(buf, size);
    return recvsz ? recvsz : MBEDTLS_ERR_SSL_WANT_READ;
}
//------------------------------------------------------------------------------
TTlsStream::TTlsStream()
{
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&rndg);
    mbedtls_ssl_config_init(&conf);
    mbedtls_ssl_init(&tls);

    const char pdata[] = "tls-stream";
    if( mbedtls_ctr_drbg_seed(&rndg, mbedtls_entropy_func, &entropy, (unsigned char*)pdata, sizeof(pdata)) )
        throw runtime_error("Initialise random number generator failed!");

    if( mbedtls_ssl_config_defaults(&conf,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT) )
    {
        throw runtime_error("Set up TLS configuration failed!");
    }

    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &rndg);
    mbedtls_ssl_conf_dbg(&conf, on_debug_message, stderr);
}
//------------------------------------------------------------------------------
TTlsStream::~TTlsStream()
{
    mbedtls_ssl_free(&tls);
    mbedtls_ssl_config_free(&conf);
    mbedtls_ctr_drbg_free(&rndg);
    mbedtls_entropy_free(&entropy);
}
//------------------------------------------------------------------------------
bool TTlsStream::Connect(const TSocketAddr &addr, unsigned timeout)
{
    try
    {
        // TCP connection.

        if( !TTcpStream::Connect(addr) ) throw runtime_error("TCP connect failed!");

        // Set up session.

        if( mbedtls_ssl_setup(&tls, &conf) ) throw runtime_error("Set up TLS session failed!");

        mbedtls_ssl_set_bio(&tls,
                            &sock,
                            (int(*)(void*,const unsigned char*,size_t)) on_send,
                            (int(*)(void*,unsigned char*,size_t)) on_recv,
                            NULL);

        // Handshake.

        int handshake_rescode;
        do
        {
            handshake_rescode = mbedtls_ssl_handshake(&tls);
        } while( handshake_rescode == MBEDTLS_ERR_SSL_WANT_READ ||
                 handshake_rescode == MBEDTLS_ERR_SSL_WANT_WRITE );

        if( handshake_rescode )
        {
            char msg[512];
            mbedtls_strerror(handshake_rescode, msg, sizeof(msg));
            printf("ERROR: Handshake: %s\n", msg);
            throw runtime_error(msg);
        }

        // Information display.

        cout << "Connection protocol:" << endl;
        cout << "  Version : %s" << mbedtls_ssl_get_version(&tls) << endl;
        cout << "  Cipher  : %s" << mbedtls_ssl_get_ciphersuite(&tls) << endl;

        const mbedtls_x509_crt *cert = mbedtls_ssl_get_peer_cert(&tls);
        if( cert )
        {
            char info[1024];
            if( mbedtls_x509_crt_info(info, sizeof(info), "  ", cert) )
            {
                cout << "Certification information:" << endl;
                cout << info << endl;
            }
        }

        return true;
    }
    catch(exception &e)
    {
        cerr << "ERROR: " << e.what() << endl;
        Close();
    }

    return false;
}
//------------------------------------------------------------------------------
void TTlsStream::Close()
{
    int close_rescode;
    do
    {
        close_rescode = mbedtls_ssl_close_notify(&tls);
    } while( close_rescode == MBEDTLS_ERR_SSL_WANT_READ ||
             close_rescode == MBEDTLS_ERR_SSL_WANT_WRITE );

    return TTcpStream::Close();
}
//------------------------------------------------------------------------------
int TTlsStream::Send(const void *data, size_t size)
{
    int sentsz = mbedtls_ssl_write(&tls, (const unsigned char*)data, size);

    if( sentsz == MBEDTLS_ERR_SSL_WANT_READ )
        sentsz = 0;
    else if( sentsz == MBEDTLS_ERR_SSL_WANT_WRITE )
        sentsz = 0;
    else if( sentsz < 0 )
        sentsz = -1;

    return sentsz;
}
//------------------------------------------------------------------------------
int TTlsStream::Recv(void *buffer, size_t size)
{
    int recvsz = mbedtls_ssl_read(&tls, (unsigned char*)buffer, size);

    if( recvsz == MBEDTLS_ERR_SSL_WANT_READ )
        recvsz = 0;
    else if( recvsz == MBEDTLS_ERR_SSL_WANT_WRITE )
        recvsz = 0;
    else if( recvsz < 0 )
        recvsz = -1;

    return recvsz;
}
//------------------------------------------------------------------------------
