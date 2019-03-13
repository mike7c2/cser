# CSER

CSER provides tools to simplify and automate some of the work involved in creating serial interfaces on embedded systems.

## Installing

CSER can be installed as a python package with 
```
python setup.py install
```

## Running

`cser-dbg` can be run from the command line once the package is installed
```
$ cser-dbg --help
usage: cser-dbg [-h] [--messages MESSAGES]
                [--serial SERIAL | --ip IP | --file FILE]

Converts binary streams of cser_dbg data to meaningful log output

optional arguments:
  -h, --help           show this help message and exit
  --messages MESSAGES  Build log containing hash pragmas (required for hash
                       mode)
  --serial SERIAL      Serial port i.e. /dev/ttyUSB0
  --ip IP              [IP address]:[port] i.e. 127.0.0.1:1234
  --file FILE          File to stream from i.e. mydata.bin
```

`cser-dbg` can take a data stream from a serial port, a file or a tcp socket, the source is specified by choosing one of the `--serial`, `--ip`, `--file` arguments. 

```
cser-dbg --ip 127.0.0.1:1234
```

## CSER Debug

CSER debug is a _printf_ replacement for small systems. CSER allows the user to save memory, cpu time and serial bandwidth on their system by carrying out formatting on the host. If HASH\_MODE is enabled then strings are stored in a descriptor on the host instead of on the target saving more memory and in most cases significantly reducing serial bandwidth required to send debug messages.

To use CSER Debug include cser\_dbg.c and cser\_dbg.h in your project and provide an implementation of the function cser\_dbg\_putc connected to a serial interface. Use the cser\_dbg\_target.py on the host, connect it to the other end of the serial interface and messages will be shown. 

If the HASH\_MODE define in cser\_dbg.h is enabled then a set of descriptors must be generated and passed to cser\_dbg\_target.py to allow the parser to create output. To generate a descriptor the target toolchain must support the #pragma message directive. A message will be emitted in the build log for each call to cser\_dbg. Pass the build log to `cser-dbg` with the `--messages` switch.

### Emitting messages

To send messages from target code use the function cser\_dbg(...). All arguments must be lvalues (i.e. they must work with the & operator). 

This means that things like
~~~~
int i = 12;
cser_dbg("OMG HAALP, %d\n", i);
~~~~
will work fine
~~~~
INFO:root:Log Msg:OMG HAALP, 12
~~~~

whilst things like
~~~~
int i = 12;
cser_dbg("OMG HAALP, %d\n", i + 2);
~~~~
will not
~~~~
In file included from cser_dbg_test.c:5:0:
cser_dbg.h:61:66: error: lvalue required as unary ‘&’ operand
 #define CSER_DBG2(str,a) cser_dbg_2arg(CSER_HASH(str), (uint8_t*)&(a), sizeof(a)); \
                                                                  ^
cser_dbg.h:54:33: note: in expansion of macro ‘CSER_DBG2’
 #define CSER__IMPL2(count, ...) CSER_DBG ## count (__VA_ARGS__)
                                 ^~~~~~~~
cser_dbg.h:55:32: note: in expansion of macro ‘CSER__IMPL2’
 #define CSER__IMPL(count, ...) CSER__IMPL2(count, __VA_ARGS__)
                                ^~~~~~~~~~~
cser_dbg.h:112:23: note: in expansion of macro ‘CSER__IMPL’
 #define cser_dbg(...) CSER__IMPL(CSER_VA_NARGS(__VA_ARGS__), __VA_ARGS__)
                       ^~~~~~~~~~
cser_dbg_test.c:32:5: note: in expansion of macro ‘cser_dbg’
     cser_dbg("OMG HAALP, %d\n", i + 2);
     ^~~~~~~~
~~~~

Note that the first argument must be a string literal in order for HASH\_MODE to work. It also *must* not have a line end appended.

cser\_dbg internally uses sizeof to determine the amount of data to send for each argument. This means that structs and arrays may be sent if their size is known at compile time. cser\_dbg recognises non-standard format specifiers such as %H to print an array in hex. 

~~~~
uint8_t blarg[5] = {'b','l','a','r','g'};
struct blorg{
    uint8_t a;
    uint8_t b;
	uint8_t c;
	uint8_t d;
	uint8_t e;
} blerg = {'b','l','e','r','g'};

cser_dbg("Blarg? %s", blarg);
cser_dbg("Blarg? %H", blarg);
cser_dbg("Blerg? %H", blerg);
~~~~
~~~~
INFO:root:Log Msg:Blarg? blarg
INFO:root:Log Msg:Blarg? 0x62:0x6c:0x61:0x72:0x67
INFO:root:Log Msg:Blerg? 0x62:0x6c:0x65:0x72:0x67
~~~~

A specialised function for sending arrays with dynamic length has been provided.
~~~~
int is[10] = {0,1,2,3,4,5,6,7,8,9};
cser_dbg_array("DATAS %H", is, 3);
~~~~
~~~~
INFO:root:Log Msg:DATAS 0x0:0x0:0x0
~~~~

### Emitting messages more haxily

Some of the constraints of cser\_dbg can be avoided by directly calling the internal cser\_dbg\_#arg functions. This will not work with HASH\_MODE enabled. They take the form:

~~~~
size_t cser_dbg_2arg(const uint8_t *restrict s, size_t s_sz, const uint8_t *restrict a, size_t a_sz);
~~~~

This method could be used to send a mixture of standard values and variable length arrays in a single message i.e.

~~~~
const uint8_t str[] = "OOOoooo %d cats with data %H\n";
const uint8_t * str_ptr = str;

int no_of_cats = 10;
cser_dbg_3arg(str, sizeof(str), &no_of_cats, sizeof(no_of_cats), str_ptr, 4); 
~~~~