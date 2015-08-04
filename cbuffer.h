#ifndef __CBUFFER_H__
#define __CBUFFER_H__

#ifdef _DMALLOC_
#include <dmalloc.h>
#endif

typedef struct CBUFFERst
{
	char	*buf;
	unsigned long	size;
	unsigned long	write_index;
	unsigned long	read_index;
	unsigned long	item_count;
} CBUFFER;

#define BUFFER_EMPTY	3
#define BUFFER_FULL	2
extern int	cbuffer_init(CBUFFER *buffer, unsigned long size);
extern void			cbuffer_destroy(CBUFFER *buffer);
extern int	cbuffer_extract(CBUFFER *buffer, char *items, unsigned long count);
extern int	cbuffer_peek(CBUFFER *buffer, char *items, unsigned long count);
extern int	cbuffer_insert(CBUFFER *buffer, const char *items, unsigned long count);
extern unsigned long		cbuffer_get_free(CBUFFER *buffer);
extern unsigned long 		cbuffer_get_used(CBUFFER *buffer);
extern unsigned long		cbuffer_get_size(CBUFFER *buffer);

#endif //__CBUFFER_H__

