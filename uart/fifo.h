#ifndef FIFO_DEF_H_
#define FIFO_DEF_H_


typedef struct
{
	volatile unsigned char * const data;
	const volatile unsigned char * const data_end;
	volatile unsigned char* volatile wrptr;
	volatile unsigned char* volatile rdptr;
}fifo_t;

#define DEFINE_FIFO(name,size) \
	unsigned char name ## _data[size]; \
	fifo_t name = {name ## _data, name ## _data + size, name ## _data, name ## _data}

#define DEFINE_STATIC_FIFO(name,size) \
		static unsigned char name ## _data[size]; \
		static fifo_t name = {name ## _data, name ## _data + size, name ## _data, name ## _data}

static inline void fifo_clear(fifo_t* fifo)
{
	fifo->wrptr = fifo->data;
	fifo->rdptr = fifo->data;
}

static inline long fifo_put(fifo_t* fifo,unsigned char byte)
{
	volatile unsigned char *tmwr = fifo->wrptr + 1;
	if(tmwr == fifo->data_end)
		tmwr = fifo->data;
	if (tmwr == fifo->rdptr)
			return 0;
	*tmwr = byte;
	fifo->wrptr = tmwr;
	return 1;
}

static inline long fifo_get(fifo_t* fifo,unsigned char *byte)
{
	if (fifo->wrptr == fifo->rdptr)
		return 0;
	volatile unsigned char *tmrd = fifo->rdptr + 1;
	if(tmrd == fifo->data_end)
		tmrd = fifo->data;
	*byte = *tmrd;
	fifo->rdptr = tmrd;
	return 1;
}

static inline long fifo_check(fifo_t* fifo)
{
	int ret = (fifo->wrptr - fifo->rdptr);
	if (ret < 0) ret +=(fifo->data_end - fifo->data);
	return ret;
}

#endif /* FIFO_H_ */
