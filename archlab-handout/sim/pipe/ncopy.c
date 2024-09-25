#include <stdio.h>

typedef word_t word_t;

word_t src[8], dst[8];

/* $begin ncopy */
/*
 * ncopy - copy src to dst, returning number of positive ints
 * contained in src array.
 */
word_t ncopy(word_t *src, word_t *dst, word_t len)
{
    word_t count = 0;
    word_t val;

    while (len > 0) {
	val = *src++;
	*dst++ = val;
	if (val > 0)
	    count++;
	len--;
    }
    return count;
}
word_t ncopy(word_t *src, word_t *dst, word_t len)
{
    word_t count = 0;
    word_t val_1, val_2;
    word_t limit = len - 1;

    while (limit > 0) {
        val_1 = *src;
        val_2 = *(src + 1);
        src = src + 2;
        *dst = val_1;
        *(dst + 1) = val_2;
        dst = dst + 2;
        if (val_1 > 0)
            count++;
        if (val_2 > 0)
            count++;
        limit = limit - 2;
    }
    len = limit + 1;
    while(len > 0){
        val_1 = *src++;
        *dst++ = val_1;
        if (val > 0)
            count++;
        len--;
    }
    return count;
}
/* $end ncopy */

int main()
{
    word_t i, count;

    for (i=0; i<8; i++)
	src[i]= i+1;
    count = ncopy(src, dst, 8);
    printf ("count=%d\n", count);
    exit(0);
}


