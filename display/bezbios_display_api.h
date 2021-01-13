/*
 * bezbios_display_api.h
 *
 *  Created on: Jan 12, 2021
 *      Author: Korneliusz Osmenda
 */

#ifndef DISPLAY_BEZBIOS_DISPLAY_API_H_
#define DISPLAY_BEZBIOS_DISPLAY_API_H_

struct DisplayParameters {
	int height;
	int width;
	int stride;
	int colors;
};

extern const DisplayParameters display_parameters;

void init_display();
void put_pixel(int x, int y, int color);
void put_next_pixels(int x, int y, int len, int color[]);
void put_next_pixels(int x, int y, int len, unsigned char color[]);
//RGB
void put_palette(int start, int len, unsigned char pal[]);

#endif /* DISPLAY_BEZBIOS_DISPLAY_API_H_ */
