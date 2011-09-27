#include <stdio.h>
#include <stdlib.h>
#include "compress.h"

/*************************************************************************
// This program shows the basic usage of the LZO library.
// We will compress a block of data and decompress again.
//
// For more information, documentation, example programs and other support
// files (like Makefiles and build scripts) please download the full LZO
// package from
//    http://www.oberhumer.com/opensource/lzo/
**************************************************************************/

/* First let's include "minizo.h". */

#include "minilzo.h"


/* We want to compress the data block at 'in' with length 'IN_LEN' to
 * the block at 'out'. Because the input block may be incompressible,
 * we must provide a little more output space in case that compression
 * is not possible.
 */

#if defined(__LZO_STRICT_16BIT)
#define IN_LEN      (8*1024u)
#elif defined(LZO_ARCH_I086) && !defined(LZO_HAVE_MM_HUGE_ARRAY)
#define IN_LEN      (60*1024u)
#else
#define IN_LEN      (128*1024ul)
#endif
#define OUT_LEN     (IN_LEN + IN_LEN / 16 + 64 + 3)

//static unsigned char __LZO_MMODEL in  [ IN_LEN ];
//static unsigned char __LZO_MMODEL out [ OUT_LEN ];


/* Work-memory needed for compression. Allocate memory in units
 * of 'lzo_align_t' (instead of 'char') to make sure it is properly aligned.
 */

#define HEAP_ALLOC(var,size) \
    lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]

static HEAP_ALLOC(wrkmem, LZO1X_1_MEM_COMPRESS);


/*************************************************************************
//
**************************************************************************/


void initializeCompression(){

/*
 * Step 1: initialize the LZO library
 */
    if (lzo_init() != LZO_E_OK)
    {
        printf("internal error - lzo_init() failed !!!\n");
        printf("(this usually indicates a compiler bug - try recompiling\nwithout optimizations, and enable '-DLZO_DEBUG' for diagnostics)\n");
        exit(0);
    }
}


unsigned char* compressingBuffer(unsigned char in_buffer[], uint32_t *buf_len){
    
    lzo_uint in_len;
    lzo_uint out_len;
/*
 * Step 2: prepare the input block that will get compressed.
 *         We just fill it with zeros in this example program,
 *         but you would use your real-world data here.
 */
    in_len = *buf_len;
    unsigned char *out_buffer = (unsigned char *)malloc(MAXDATASIZE);
    lzo_memset(out_buffer,'\0',MAXDATASIZE);
    //lzo_memcpy(in, "Manu Bhadoria is from USC", sizeof("Manu bhadoria is from usc"));

/*
 * Step 3: compress from 'in' to 'out' with LZO1X-1
 */
    int r = lzo1x_1_compress(in_buffer,in_len,out_buffer,&out_len,wrkmem);
    if (r == LZO_E_OK)
        printf("compressed %lu bytes into %lu bytes\n",
            (unsigned long) in_len, (unsigned long) out_len);
    else
    {
        /* this should NEVER happen */
        printf("internal error - compression failed: %d\n", r);
        exit(1);
    }
    /* check for an incompressible block */
    if (out_len >= in_len)
    {
        printf("This block contains incompressible data.\n");
        //exit(1);
    }
    
    *buf_len = out_len;
    return out_buffer;
}

unsigned char* decompressingBuffer(unsigned char out_buffer[], uint32_t *buf_len){

/*
 * Step 4: decompress again, now going from 'out' to 'in'
 */
    lzo_uint new_len ;
    unsigned char *in_buffer = (unsigned char *)malloc(MAXDATASIZE);
    lzo_memset(in_buffer, '\0', MAXDATASIZE);

    int r = lzo1x_decompress(out_buffer,*buf_len,in_buffer,&new_len,NULL);
    if (r == LZO_E_OK )
        printf("decompressed %lu bytes back into %lu bytes\n",
            (unsigned long) buf_len, (unsigned long) new_len);
    else
    {
        /* this should NEVER happen */
        printf("internal error - decompression failed: %d\n", r);
        exit(0);
    }
    *buf_len = new_len;
    return in_buffer;
}
