
### CSER

CSER provides tools to simplify and automate some of the work involved in creating serial interfaces on embedded systems.

## CSER Debug

CSER debug is a _printf_ replacement for small systems. CSER allows the user to save memory and serial bandwidth on their system by carrying out formatting on the host. If HASH\_MODE is enabled then strings are stored in a descriptor on the host instead of on the target saving more memory and in most cases significantly reducing serial bandwidth required to send debug messages.

To use CSER Debug include cser\_dbg.c and cser\_dbg.h in your project and provide an implementation of the function cser\_dbg\_putc connected to a serial interface. Use the cser\_dbg\_target.py on the host, connect it to the other end of the serial interface and messages will be shown. 

If the HASH\_MODE define in cser\_dbg.h is enabled then a set of descriptors must be generated and passed to cser\_dbg\_target.py to allow the parser to create output. To generate a descriptor the target toolchain must support the #pragma message directive. A message will be emitted in the build log for each call to cser\_dbg. Pass the build log to cser\_dbg\_target.py with the --messages switch.  

### Emitting messages

To send messages from target code use the function cser\_dbg(...). All arguments must be lvalues (i.e. they must work with the & operator). 

This means that things like:
~~~~
int i = 12;
cser_dbg("OMG HAALP, %d\n", i);
~~~~
will work fine, whilst things like:
~~~~
int i = 12;
cser_dbg("OMG HAALP, %d\n", i + 2);
~~~~
will not.

Note that the first argument must be a string literal in order for HASH\_MODE to work. 

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

cser_dbg("Blarg? %s, %h, %h\n", blarg, blarg, blerg);
~~~~

With cser_dbg it's complicated (but possible!) to send variable length data. A specialised function for sending arrays has been provided to help.

~~~~
int is[10] = {0,1,2,3,4,5,6,7,8,9};
cser_dbg_array("DATAS %h", is, 3);
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

