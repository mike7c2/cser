/**
 * @file cser_dbg.c
 * @author Mike
 * @date 2017-03-02
 * @brief Implementation of cser_dbg functionality
 *
 * The cser_dbg call is implemented by having a single function 
 * for each possible number of arguments. Currently up to 4 arguments
 * are supported.
 *
 * Note that each of the function takes both a pointer argument and a size
 * argument to allow the argument to be serialised. 
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "cser_dbg.h"

#ifdef CSER_DBG_HASH_MODE
size_t cser_dbg_1arg(uint32_t hash)
#else
size_t cser_dbg_1arg(const uint8_t *restrict s, size_t s_sz)
#endif
{
#ifdef CSER_DBG_HASH_MODE
    int len = CSER_DBG_PKT_LEN(sizeof(hash));
#else
    int len = CSER_DBG_PKT_LEN(s_sz);
#endif
    
    uint8_t checksum = 0;
    uint8_t i = 0;

    cser_dbg_putc(CSER_DBG_HEADER_VALUE);
    cser_dbg_putc(len);
    checksum += len;
    
#ifdef CSER_HEADER
    cser_dbg_putc(CSER_DBG_PKT_ID);
    checksum += CSER_DBG_PKT_ID;
#endif

#ifdef CSER_DBG_HASH_MODE
    for ( i = 0; i < sizeof(hash); i++)
    {
        cser_dbg_putc(((uint8_t*)&hash)[i]);
        checksum += ((uint8_t*)&hash)[i];
    }
#else
    for ( i = 0; i < s_sz; i++)
    {
        cser_dbg_putc(((uint8_t*)s)[i]);
        checksum += ((uint8_t*)s)[i];
    }
#endif

    cser_dbg_putc(checksum);

    return 0;
}

#ifdef CSER_DBG_HASH_MODE
size_t cser_dbg_2arg(uint32_t hash, const uint8_t *restrict a, size_t a_sz)
#else
size_t cser_dbg_2arg(const uint8_t *restrict s, size_t s_sz, const uint8_t *restrict a, size_t a_sz)
#endif
{
#ifdef CSER_DBG_HASH_MODE
    int len = CSER_DBG_PKT_LEN(sizeof(hash) + a_sz);
#else
    int len = CSER_DBG_PKT_LEN(s_sz + a_sz);
#endif
    uint8_t checksum = 0;
    uint8_t i = 0;
    uint8_t t;

    cser_dbg_putc(CSER_DBG_HEADER_VALUE);
    cser_dbg_putc(len);
    checksum += len;

#ifdef CSER_HEADER
    cser_dbg_putc(CSER_DBG_PKT_ID);
    checksum += CSER_DBG_PKT_ID;
#endif
    
#ifdef CSER_DBG_HASH_MODE
    for ( i = 0; i < sizeof(hash); i++)
    {
        cser_dbg_putc(((uint8_t*)&hash)[i]);
        checksum += ((uint8_t*)&hash)[i];
    }
#else
    for ( i = 0; i < s_sz; i++)
    {
        cser_dbg_putc(((uint8_t*)s)[i]);
        checksum += ((uint8_t*)s)[i];
    }
#endif
    
    for ( i = 0; i < a_sz; i++)
    {
        t = a[i];
        cser_dbg_putc(t);
        checksum += t;
    }

    cser_dbg_putc(checksum);

    return 0;
}


#ifdef CSER_DBG_HASH_MODE
size_t cser_dbg_3arg(uint32_t hash, const uint8_t *restrict a, size_t a_sz, const uint8_t *restrict b, size_t b_sz)
#else
size_t cser_dbg_3arg(const uint8_t *restrict s, size_t s_sz, const uint8_t *restrict a, size_t a_sz, const uint8_t *restrict b, size_t b_sz)
#endif
{
#ifdef CSER_DBG_HASH_MODE
    int len = CSER_DBG_PKT_LEN(sizeof(hash) + a_sz + b_sz);
#else
    int len = CSER_DBG_PKT_LEN(s_sz + a_sz + b_sz);
#endif
    uint8_t checksum = 0;
    uint8_t i = 0;
    uint8_t t;

    cser_dbg_putc(CSER_DBG_HEADER_VALUE);
    cser_dbg_putc(len);
    checksum += len;

#ifdef CSER_HEADER
    cser_dbg_putc(CSER_DBG_PKT_ID);
    checksum += CSER_DBG_PKT_ID;
#endif
    
#ifdef CSER_DBG_HASH_MODE
    for ( i = 0; i < sizeof(hash); i++)
    {
        cser_dbg_putc(((uint8_t*)&hash)[i]);
        checksum += ((uint8_t*)&hash)[i];
    }
#else
    for ( i = 0; i < s_sz; i++)
    {
        cser_dbg_putc(((uint8_t*)s)[i]);
        checksum += ((uint8_t*)s)[i];
    }
#endif
    
    for ( i = 0; i < a_sz; i++)
    {
        t = a[i];
        cser_dbg_putc(t);
        checksum += t;
    }
    
    for ( i = 0; i < b_sz; i++)
    {
        t = b[i];
        cser_dbg_putc(t);
        checksum += t;
    }

    cser_dbg_putc(checksum);

    return 0;
}


#ifdef CSER_DBG_HASH_MODE
size_t cser_dbg_4arg(uint32_t hash, const uint8_t *restrict a, size_t a_sz, const uint8_t *restrict b, size_t b_sz, const uint8_t *restrict c, size_t c_sz)
#else
size_t cser_dbg_4arg(const uint8_t *restrict s, size_t s_sz, const uint8_t *restrict a, size_t a_sz, const uint8_t *restrict b, size_t b_sz, const uint8_t *restrict c, size_t c_sz)
#endif
{
#ifdef CSER_DBG_HASH_MODE
    int len = CSER_DBG_PKT_LEN(sizeof(hash) + a_sz + b_sz + c_sz);
#else
    int len = CSER_DBG_PKT_LEN(s_sz + a_sz + b_sz + c_sz);
#endif
    uint8_t checksum = 0;
    uint8_t i = 0;
    uint8_t t;

    cser_dbg_putc(CSER_DBG_HEADER_VALUE);
    cser_dbg_putc(len);
    checksum += len;

#ifdef CSER_HEADER
    cser_dbg_putc(CSER_DBG_PKT_ID);
    checksum += CSER_DBG_PKT_ID;
#endif
    
#ifdef CSER_DBG_HASH_MODE
    for ( i = 0; i < sizeof(hash); i++)
    {
        cser_dbg_putc(((uint8_t*)&hash)[i]);
        checksum += ((uint8_t*)&hash)[i];
    }
#else
    for ( i = 0; i < s_sz; i++)
    {
        cser_dbg_putc(((uint8_t*)s)[i]);
        checksum += ((uint8_t*)s)[i];
    }
#endif
    
    for ( i = 0; i < a_sz; i++)
    {
        t = a[i];
        cser_dbg_putc(t);
        checksum += t;
    }
    
    for ( i = 0; i < b_sz; i++)
    {
        t = b[i];
        cser_dbg_putc(t);
        checksum += t;
    }
    
    for ( i = 0; i < c_sz; i++)
    {
        t = c[i];
        cser_dbg_putc(t);
        checksum += t;
    }

    cser_dbg_putc(checksum);

    return 0;
}
