/**
 * @file cser_dbg.h
 * @author Michael Shaw
 * @date 2017-03-02
 * @brief Header for cser_dbg module, provides a printf like interface to serialise debug messages to be formatted on host
 *
 * Provision has been made for up to 4 arguments, this may be expanded in the future.
 *
 */

#ifndef CSER_DBG_H
#define CSER_DBG_H

#include <stdint.h>
#include <stddef.h>

//Uncomment to enable string hashing or use -DCSER_DBG_HASH_MODE
//#define CSER_DBG_HASH_MODE

//This macro should defined to a function that will output a single byte of data to the target
extern void cser_test_putc(uint8_t x);
#define cser_dbg_putc(x) cser_test_putc(x)

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define CSER_ECHO_STR(x) _Pragma( STR( message __FILE__  ":"  STR(__LINE__) ":" x  ) )

#define CSER_DBG_PKT_ID 10

#define CSER_DBG_PACKET_OVERHEAD 0

#define CSER_DBG_HEADER_VALUE 0xAA
#define CSER_DBG_PKT_LEN(x) (x + CSER_DBG_PACKET_OVERHEAD)

//Preprocessor string hashing
#ifdef CSER_DBG_HASH_MODE

// http://stackoverflow.com/questions/2826559/compile-time-preprocessor-hashing-of-string
#define CSER_H1(s,i,x)   ((x*65599)+(uint8_t)(((i)<sizeof(s))?s[sizeof(s)-(i+1)]:0))
#define CSER_H4(s,i,x)   CSER_H1(s,i,CSER_H1(s,i+1,CSER_H1(s,i+2,CSER_H1(s,i+3,x))))
#define CSER_H16(s,i,x)  CSER_H4(s,i,CSER_H4(s,i+4,CSER_H4(s,i+8,CSER_H4(s,i+12,x))))
#define CSER_H64(s,i,x)  CSER_H16(s,i,CSER_H16(s,i+16,CSER_H16(s,i+32,CSER_H16(s,i+48,x))))
#define CSER_H256(s,i,x) CSER_H64(s,i,CSER_H64(s,i+64,CSER_H64(s,i+128,CSER_H64(s,i+192,x))))
#define CSER_HASH(s)     ((uint32_t)(CSER_H256(s,0,0)))

#endif

//Implementation of variable arguments with size of each argument
//http://stackoverflow.com/questions/5365440/variadic-macro-trick
#define CSER_VA_NARGS_IMPL(_1, _2, _3, _4, N, ...) N
#define CSER_VA_NARGS(...) CSER_VA_NARGS_IMPL(__VA_ARGS__, 4, 3, 2, 1)

#define CSER__IMPL2(count, ...) CSER_DBG ## count (__VA_ARGS__)
#define CSER__IMPL(count, ...) CSER__IMPL2(count, __VA_ARGS__)

#ifdef CSER_DBG_HASH_MODE

#define CSER_DBG1(str) cser_dbg_1arg(CSER_HASH(str)); \
    CSER_ECHO_STR(str)
#define CSER_DBG2(str,a) cser_dbg_2arg(CSER_HASH(str), (uint8_t*)&(a), sizeof(a)); \
    CSER_ECHO_STR(str)
#define CSER_DBG3(str,a,b) cser_dbg_3arg(CSER_HASH(str), (uint8_t*)&(a), sizeof(a), (uint8_t*)&(b), sizeof(b)); \
    CSER_ECHO_STR(str)
#define CSER_DBG4(str,a,b,c) cser_dbg_4arg(CSER_HASH(str), (uint8_t*)&(a), sizeof(a), (uint8_t*)&(b), sizeof(b), (uint8_t*)&(c), sizeof(c)); \
    CSER_ECHO_STR(str)

size_t cser_dbg_1arg(uint32_t hash);
size_t cser_dbg_2arg(uint32_t hash, const uint8_t *restrict a, size_t a_sz);
size_t cser_dbg_3arg(uint32_t hash, const uint8_t *restrict a, size_t a_sz, const uint8_t *restrict b, size_t b_sz);
size_t cser_dbg_4arg(uint32_t hash, const uint8_t *restrict a, size_t a_sz, const uint8_t *restrict b, size_t b_sz, const uint8_t *restrict c, size_t c_sz);

#define cser_dbg_ar(str, ar, sz) cser_dbg_2arg(CSER_HASH(str), ar, sz); \
    CSER_ECHO_STR(str)

#else

#define CSER_DBG1(str) cser_dbg_1arg((uint8_t*)&(str), sizeof(str))
#define CSER_DBG2(str,a) cser_dbg_2arg((uint8_t*)&(str), sizeof(str), (uint8_t*)&(a), sizeof(a))
#define CSER_DBG3(str,a,b) cser_dbg_3arg((uint8_t*)&(str), sizeof(str), (uint8_t*)&(a), sizeof(a), (uint8_t*)&(b), sizeof(b))
#define CSER_DBG4(str,a,b,c) cser_dbg_4arg((uint8_t*)&(str), sizeof(str), (uint8_t*)&(a), sizeof(a), (uint8_t*)&(b), sizeof(b), (uint8_t*)&(c), sizeof(c))

size_t cser_dbg_1arg(const uint8_t *restrict s, size_t s_sz);
size_t cser_dbg_2arg(const uint8_t *restrict s, size_t s_sz, const uint8_t *restrict a, size_t a_sz);
size_t cser_dbg_3arg(const uint8_t *restrict s, size_t s_sz, const uint8_t *restrict a, size_t a_sz, const uint8_t *restrict b, size_t b_sz);
size_t cser_dbg_4arg(const uint8_t *restrict s, size_t s_sz, const uint8_t *restrict a, size_t a_sz, const uint8_t *restrict b, size_t b_sz, const uint8_t *restrict c, size_t c_sz);

#define cser_dbg_ar(str, ar, sz) cser_dbg_2arg(str, sizeof(str), ar, sz);

#endif

/**
 * Macro accepting variable number of arguments to be called by user.
 * 
 * The arguments may be anything which can have its pointer taken (LVALUE?).
 * a message will be sent to the host with the arguments serialised
 * and sent via a callback function.
 * 
 * The first argument must be a string which can have its size taken with
 * sizeof.
 * 
 * The tokens are parsed using cser_dbg_parser. Currently supported
 * formats are:
 * 
 *  udxX specifiers with hh,h,l,ll widths
 *  Padding, zero padding
 *  left justification
 *
 * Up to 4 arguments are supported (including the string), if you run out then 
 * you should quit and get a job as a gardener.
 */
#define cser_dbg(...) CSER__IMPL(CSER_VA_NARGS(__VA_ARGS__), __VA_ARGS__) 

/**
 * Send a string and an array with a dynamic length
 * 
 */
#define cser_dbg_array(str, ar, sz) cser_dbg_ar(str, ar, sz)

/**
 * Inserts a trace point which is made unique using file and line position
 * This message will just display line:lineno in the parser
 */
#define cser_trace() cser_dbg(__FILE__":"STR(__LINE__))

#endif
