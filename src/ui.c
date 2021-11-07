/*============================================================================
	MiSTer test harness OS - User interface functions

	Author: Jim Gregory - https://github.com/JimmyStones/
	Version: 1.0
	Date: 2021-07-03

	This program is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by the Free
	Software Foundation; either version 3 of the License, or (at your option)
	any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along
	with this program. If not, see <http://www.gnu.org/licenses/>.
===========================================================================*/

#pragma once
#include "sys.c"

char asc_0 = 48;
char asc_1 = 49;

// Set all character RAM to specified character
void clear_chars(char c)
{
	unsigned int p;
	
	for (p = 0; p < chram_size; p++)
	{
		chram[p] = c;
	}
}

// Write string to character RAM
void write_string(const unsigned char *string, unsigned char color, unsigned int x, unsigned int y)
{
	unsigned int p = (y * chram_cols) + x;
	unsigned char l = strlen(string);
	unsigned char c;
	
	for (c = 0; c < l; c++)
	{
		chram[p] = string[c];
		colram[p] = color;
		p++;
	}
}

// Write formatted string to character RAM (signed char data)
void write_stringfs(const unsigned char *format, unsigned char color, unsigned int x, unsigned int y, signed char data)
{
	unsigned int p = (y * chram_cols) + x;
	char temp[30];
	unsigned char l;
	unsigned char c;
	
	sprintf(temp, format, data);
	l = strlen(temp);
		
	for (c = 0; c < l; c++)
	{
		if (temp[c] == 0)
		{
			return;
		}
		chram[p] = temp[c];
		colram[p] = color;
		p++;
	}
}

// Write formatted string to character RAM (unsigned char data)
void write_stringf(const char *format, char color, unsigned int x, unsigned int y, char data)
{
	unsigned int p = (y * chram_cols) + x;
	char temp[30];
	unsigned char l;
	unsigned char c;
	
	sprintf(temp, format, data);
	l = strlen(temp);
	
	for (c = 0; c < l; c++)
	{
		if (temp[c] == 0)
		{
			return;
		}
		chram[p] = temp[c];
		colram[p] = color;
		p++;
	}
}

// Write single char to character RAM and colour RAM
void write_char(unsigned char c, unsigned char color, unsigned int x, unsigned int y)
{
	unsigned int p = (y * chram_cols) + x;
	chram[p] = c;
	colram[p] = color;
}

// Set colour of single char
void set_colour(unsigned char color, unsigned int x, unsigned int y)
{
	unsigned int p = (y * chram_cols) + x;
	colram[p] = color;
}

// Write grouped bits to character RAM
void write_bits(char bits[], char multi, unsigned char first, unsigned char length, char color, unsigned int x, unsigned int y)
{
	char b, m, i;
	
	for (b = first; b < first + length; b++)
	{
		write_char((b) ? asc_1 : asc_0, color, x, y - 1);
		m = 0b00000001;
		for (i = 0; i < 8; i++)
		{
			write_char((bits[b * multi] & m) ? asc_1 : asc_0, color, x, y);
			x++;
			m <<= 1;
		}
		x++;
	}
}

// Draw box outline with specified character
void box(unsigned int tx, unsigned int ty, unsigned int bx, unsigned int by, unsigned char c, unsigned char color)
{
	unsigned int x, y;
	
	for (x = tx; x <= bx; x++)
	{
		write_char(c, color, x, ty);
		write_char(c, color, x, by);
	}
	for (y = ty + 1; y < by; y++)
	{
		write_char(c, color, tx, y);
		write_char(c, color, bx, y);
	}
}

// Draw UI panel
void panel(unsigned char tx, unsigned char ty, unsigned char bx, unsigned char by, unsigned char color)
{
	unsigned char x, y;
	
	write_char(128, color, tx, ty);
	write_char(130, color, bx, ty);
	write_char(133, color, tx, by);
	write_char(132, color, bx, by);
	for (x = tx + 1; x < bx; x++)
	{
		write_char(129, color, x, ty);
		write_char(129, color, x, by);
	}
	for (y = ty + 1; y < by; y++)
	{
		write_char(131, color, tx, y);
		write_char(131, color, bx, y);
	}
}

void fill(char tx, char ty, char bx, char by, char c, char color)
{
	char x, y;
	
	for (x = tx; x <= bx; x++)
	{
		for (y = ty; y <= by; y++)
		{
			write_char(c, color, x, y);
		}
	}
}

// Draw page border
void page_border(char color)
{
	panel(0, 0, 39, 29, color);
}

unsigned char color_pad_outline = 0xFE;

// Draw game pad outline
void draw_pad(char xo, char yo)
{
	char x, y;
	
	// Outline
	write_char(134, color_pad_outline, xo, yo + 1);				// Top-left corner.
	for (x = 1; x < 27; x++)
	{
		write_char(135, color_pad_outline, xo + x, yo + 1);		// Top line.
	}
	write_char(136, color_pad_outline, xo + 27, yo + 1);		// Top-right corner.
	
	for (y = 2; y < 5; y++)
	{
		write_char(137, color_pad_outline, xo, yo + y);
		write_char(137, color_pad_outline, xo + 27, yo + y);	// Right-hand vert line.
	}
	write_char(139, color_pad_outline, xo, yo + 5);
	write_char(138, color_pad_outline, xo + 27, yo + 5);

	write_char(138, color_pad_outline, xo + 8, yo + 5);		// Middle-left corner bit.
	write_char(139, color_pad_outline, xo + 19, yo + 5);	// Middle-right corner bit.
	write_char(134, color_pad_outline, xo + 8, yo + 4);
	write_char(136, color_pad_outline, xo + 19, yo + 4);
	for (x = 1; x < 8; x++)
	{
		write_char(135, color_pad_outline, xo + x, yo + 5);	// Lower-left horiz line.
	}
	for (x = 9; x < 19; x++)
	{
		write_char(135, color_pad_outline, xo + x, yo + 4);	// Lower-mid horiz line.
	}
	for (x = 20; x < 27; x++)								
	{
		write_char(135, color_pad_outline, xo + x, yo + 5);	// Lower-right horiz line.
	}
	// Shoulders
	//write_char(134, color_pad_outline, xo + 1, yo);
	//write_char(136, color_pad_outline, xo + 5, yo);
	//write_char(134, color_pad_outline, xo + 21, yo);
	//write_char(136, color_pad_outline, xo + 25, yo);
}

char color_analog_grid = 0x23;

// Draw game pad outline
void draw_analog(char xo, char yo, char xs, char ys)
{
	char mx, my, x, y;
	
	panel(xo, yo, xo + xs, yo + ys, 0xFF);
	fill(xo + 1, yo + 1, xo + xs - 1, yo + ys - 1, 27, color_analog_grid);
	mx = xo + (xs / 2);
	my = yo + (ys / 2);
	for (x = xo + 1; x < xo + xs; x++)
	{
		write_char(129, color_analog_grid, x, my);
	}
	for (y = yo + 1; y < yo + ys; y++)
	{
		write_char(131, color_analog_grid, mx, y);
	}
	write_char('+', color_analog_grid, mx, my);
}
