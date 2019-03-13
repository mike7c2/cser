#include <stdio.h>
#include <stdint.h>

#include "cser_dbg.h"

#define TEST_OUTPUT "dbg_test_output"

FILE * test_data;

void cser_test_putc(uint8_t x)
{
    fputc(x, test_data);
}

uint8_t blarg[5] = {'b','l','a','r','g'};
struct blorg{
    uint8_t a;
    uint8_t b;
	uint8_t c;
	uint8_t d;
	uint8_t e;
} blerg = {'b','l','e','r','g'};

void readme_examples(){
    cser_dbg("Blarg? %s", blarg);
    cser_dbg("Blarg? %H", blarg);
    cser_dbg("Blerg? %H", blerg);

    int i = 12;
    cser_dbg("OMG HAALP, %d", i);

    int is[10] = {0,1,2,3,4,5,6,7,8,9};
    cser_dbg_array("DATAS %H", is, 3);
}

int main(int argc, char * argv[])
{
    int64_t a64, b64, c64;
    int32_t a32, b32, c32;
    int16_t a16, b16, c16;
    int8_t a8, b8, c8;
    float af, bf, cf;
    
    uint8_t array[32] = {0};
    int i;
    
    for ( i = 0; i < 32; i++ )
    {
        array[i] = i;
    }
 
    test_data = fopen(TEST_OUTPUT, "w");
    
    printf("//n ord %d\n", '\n');
    
    a8 = 1, b8 = 2, c8 = 3;
    a16 = 4, b16 = 5, c16 = 6;
    a32 = 7, b32 = 8, c32 = 9;
    a64 = 10, b64 = 11, c64 = 12;
    af = 0.0, bf = 1.0, cf = 2.0;
    
    cser_trace();
    
    cser_dbg("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    cser_dbg("Blarg %d!", a32);
    cser_dbg("Blarg %d %d!", b32, c32);
    cser_dbg("Blarg %d %d %d!", a32, b32, c32);
    cser_trace();
    
    cser_dbg("unsigned8 %hhu", a8);
    cser_dbg("unsigned16 %hu", a16);
    cser_dbg("unsigned32 %u", a32);
    cser_dbg("unsigned64 %llu", a64);
    cser_trace();
    
    cser_dbg("signed8 %hhd", a8);
    cser_dbg("signed16 %hd", a16);
    cser_dbg("signed32 %d", a32);
    cser_dbg("signed64 %lld", a64);
    cser_trace();
    
    cser_dbg("hex8 %hhx", a8);
    cser_dbg("hex16 %hx", a16);
    cser_dbg("hex32 %x", a32);
    cser_dbg("hex64 %x", a64);
    cser_trace();

    cser_dbg("pad2 0x%2hhx", a8);
    cser_dbg("pad4 0x%4hhx", a8);
    cser_dbg("pad8 0x%8hhx", a8);
    cser_trace();
    
    cser_dbg("zpad2 0x%02hhx", a8);
    cser_dbg("zpad4 0x%04hhx", a8);
    cser_dbg("zpad8 0x%08hhx", a8);
    cser_trace();

    cser_dbg("lzpad2 0x%-02hhx", a8);
    cser_dbg("lzpad4 0x%-04hhx", a8);
    cser_dbg("lzpad8 0x%-08hhx", a8);
    cser_trace();

    cser_dbg(__FILE__);
    
    cser_dbg("Floats %f %f %f!", af, bf, cf);
    
    cser_trace();
    
    cser_dbg_array("An array %H", array, 12);
    
    cser_trace();

    readme_examples();

    fflush(test_data);
    fclose(test_data);
}






