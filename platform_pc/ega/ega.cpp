/*
 * ega.cpp
 *
 *  Created on: Jan 12, 2021
 *      Author: Korneliusz Osmenda
 */
#include<display/bezbios_display_api.h>
#include<sched/bezbios_sched_api.h>

#include<io.h>

#include <bit>

const DisplayParameters display_parameters =
{
		.height = 480,
		.width = 640,
		.stride = 640,
		.colors = 16,
};

static BezBios::Sched::Mutex ega_mutex;

void init_display()
{
	ega_mutex.aquire();

	ByteIO<0x3C2,0x3CC> MISC;
	MISC = 0xe3;

	ByteIO<0x3DA> AC_RESET;
	unsigned char ac_reset=AC_RESET;
	ac_reset = ac_reset;
	ByteIO<0x3C0> AC_INDEX;
	ByteIO<0x3C0,0x3C1> AC_DATA;
	AC_INDEX = 0x00;

	static const unsigned char seq[] = {0x03, 0x01, 0x08, 0x00, 0x06};

	for(unsigned long i=0;i<sizeof(seq);i++)
	{
		ByteIO<0x3C4> SEQ_INDEX;
		ByteIO<0x3C5> SEQ_DATA;

		SEQ_INDEX = i;
		SEQ_DATA = seq[i];
	}

	ByteIO<0x3D4> CRTC_INDEX;
	ByteIO<0x3D5> CRTC_DATA;

	CRTC_INDEX = 0x03;
	CRTC_DATA |= 0x80;
	CRTC_INDEX = 0x11;
	CRTC_DATA &= (unsigned char)~0x80;

	static const unsigned char crtc[] = {
			0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0x0B, 0x3E,
				0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0xEA, 0x0C, 0xDF, 0x28, 0x00, 0xE7, 0x04, 0xE3,
				0xFF};

	for(unsigned long i=0;i<sizeof(crtc);i++)
	{
		CRTC_INDEX = i;
		CRTC_DATA = crtc[i];
	}

	static const unsigned char gc[] = {
			0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x0F,
				0xFF};

	for(unsigned long i=0;i<sizeof(gc);i++)
	{
		ByteIO<0x3CE> GC_INDEX;
		ByteIO<0x3CF> GC_DATA;

		GC_INDEX = i;
		GC_DATA = gc[i];
	}

	static const unsigned char ac[] = {
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
				0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
				0x01, 0x00, 0x0F, 0x00, 0x00
	};

	ac_reset=AC_RESET;
	for(unsigned long i=0;i<sizeof(ac);i++)
	{
		AC_INDEX = i;
		AC_DATA = ac[i];
	}
	AC_INDEX = 0x20;

	ega_mutex.release();
}

static unsigned char *VGA_MEM = std::bit_cast<unsigned char *>(0xA0000);

static void set_plane(char p)
{
    p &= 3;
    char pmask = 1 << p;
	ByteIO<0x3CE> GC_INDEX;
	ByteIO<0x3CF> GC_DATA;
	GC_INDEX=4;
	GC_DATA=p;
    ByteIO<0x3C4> SEQ_INDEX;
	ByteIO<0x3C5> SEQ_DATA;
	SEQ_INDEX=2;
	SEQ_DATA=pmask;
	io_wait(0x02);
}

void put_pixel(int x, int y, int color)
{
	ega_mutex.aquire();
	for(char plane=0;plane<4;plane++)
	{
		set_plane(plane);
		MEM_BARRIER();
		long nrpix = x+y*display_parameters.stride;
		long offset = nrpix>>3;
		char pixnr = 7-(nrpix&0x07);
		VGA_MEM[offset] = (VGA_MEM[offset] & ~(1<<pixnr)) |
				(color&(1<<plane))?(1<<pixnr):0;
		MEM_BARRIER();
	}
	ega_mutex.release();
}


template<typename pixel_t>
static void put_next_pixels_imp(int x, int y, int len, pixel_t color[])
{
	ega_mutex.aquire();
	for(char plane=0;plane<4;plane++)
	{
		set_plane(plane);
		MEM_BARRIER();
		const long nrpix = x+y*display_parameters.stride;
		for(int i=0;i<len;i++)
		{
			long offset = (nrpix+i)>>3;
			char pixnr = 7-((nrpix+i)&0x07);
			VGA_MEM[offset] = (VGA_MEM[offset] & ~(1<<pixnr)) |
				((color[i]&(1<<plane))?(1<<pixnr):0);
		}
		MEM_BARRIER();
	}
	ega_mutex.release();
}

void put_next_pixels(int x, int y, int len, int color[])
{
	put_next_pixels_imp(x,y,len,color);
}

void put_next_pixels(int x, int y, int len, unsigned char color[])
{
	put_next_pixels_imp(x,y,len,color);
}

void put_palette(int start, int len, unsigned char pal[])
{
	ega_mutex.aquire();
	ByteIO<0x3DA> AC_RESET;
	unsigned char ac_reset=AC_RESET;
	ac_reset = ac_reset;
	ByteIO<0x3C0> AC_INDEX;
	ByteIO<0x3C0,0x3C1> AC_DATA;
	for(int i=0;i<len;i++)
	{
		AC_INDEX=start+i;
		char r=pal[3*i+0]>>6;
		char g=pal[3*i+1]>>6;
		char b=pal[3*i+2]>>6;
		AC_DATA = (0 |
	            ((r&1)<<5) | ((r&2)<<1) |
	            ((g&1)<<4) | ((g&2)<<0) |
	            ((b&1)<<3) | ((b&2)>>1));
	}
	AC_INDEX = 0x20;
	ega_mutex.release();
}
