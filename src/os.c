/*============================================================================
	MiSTer test harness OS - Main application

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

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "sys.c"
#include "ui.c"
#include "ps2.c"

bool hsync;
bool hsync_last;
bool vsync;
bool vsync_last;

// Application state
#define STATE_START_INPUTTESTER 0
#define STATE_INPUTTESTER 1

#define STATE_START_INPUTTESTERADVANCED 2
#define STATE_INPUTTESTERADVANCED 3

#define STATE_START_INPUTTESTERANALOG 4
#define STATE_INPUTTESTERANALOG 5

#define STATE_FADEOUT 8
#define STATE_START_FADEIN 9
#define STATE_FADEIN 10

#define STATE_START_ATTRACT 16
#define STATE_ATTRACT 17

#define STATE_START_GAME 24
#define STATE_GAME 25

unsigned long sd_lba = 0x00000000;	// Remember - each unit here is a 512-BYTE sector!
unsigned long flash_addr;

unsigned long img_size;

unsigned char mystring [32];
unsigned char text_line = 0;
unsigned char rxd_byte;

unsigned char nbr=0, bt=0, get=0, ver=0, id=0, rd=0, go=0, wr=0, er=0, wp=0, wup=0, rp=0, rup=0, ck=0;

char state = STATE_START_INPUTTESTER;
char nextstate = 0;

// SNEK variables
unsigned char movefreqinit = 14;
unsigned char movefreqdecfreq = 200;
unsigned char movefreqdectimer = 0;
unsigned char movefreq = 0;
unsigned char movetimer = 0;
signed int x = 20;
signed int y = 15;
signed char xd = 0;
signed char yd = 1;
signed char nxd = 0;
signed char nyd = 1;
unsigned int length = 0;
unsigned char playerchar = 83;

// Fade in/out variables
unsigned char fade = 0;
unsigned char fadetimer = 0;
unsigned char fadefreq = 4;

// Attract mode variables
unsigned char attractstate = 0;

// Input tester variables
unsigned char joystick_last[12];
signed char ax_last[6];
signed char ay_last[6];
unsigned char px_last[6];
signed char sx_toggle_last[6];
signed char sx_last[6];
unsigned long sx_pos[6];

unsigned char con_x;	  // Console cursor X position
unsigned char con_y;	  // Console cursor X position
unsigned char con_l = 2;  // Console left edge X
unsigned char con_t = 20; // Console top edge Y
unsigned char con_r = 37; // Console right edge X
unsigned char con_b = 37; // Console bottom edge Y
bool con_cursor;
unsigned char con_cursortimer = 1;
unsigned char con_cursorfreq = 30;

char modeswitchtimer_select = 0;
char modeswitchtimer_start = 0;

// DPAD tracker
bool bdown_left = 0;
bool bdown_left_last = 0;
bool bdown_right = 0;
bool bdown_right_last = 0;
bool bdown_up = 0;
bool bdown_up_last = 0;
bool bdown_down = 0;
bool bdown_down_last = 0;

#define HISTORY_LENGTH 6
char history[HISTORY_LENGTH];

#define PAD_COUNT 4
#define BUTTON_COUNT 12

char pad_offset_x[PAD_COUNT] = {7, 7, 7, 7};
char pad_offset_y[PAD_COUNT] = {2, 7, 12, 17};

char button_name[BUTTON_COUNT][6] = {"R", "L", "D", "U", "1", "2", "3", "4", "5", "6", "Coin", "Start"};

char button_x[BUTTON_COUNT] 	  = { 6,   2,   4,   4,   21,  23,  25,  21,  23,  25,   9,      14};
char button_y[BUTTON_COUNT] 	  = { 3,   3,   4,   2,   2,   2,   2,   4,   4,   4,    3,      3};

char analog_offset_x[PAD_COUNT] = {1, 20};
char analog_offset_y[PAD_COUNT] = {5, 5};
char analog_size = 18;
signed char analog_x[PAD_COUNT];
signed char analog_y[PAD_COUNT];
char analog_ratio = 256 / 17;

	
bool p1_pass [BUTTON_COUNT];
bool p2_pass [BUTTON_COUNT];
bool p3_pass [BUTTON_COUNT];
bool p4_pass [BUTTON_COUNT];


// Draw static elements for digital input test page
void page_inputtester_digital()
{
	unsigned char j;
	
	clear_chars(0);
	page_border(0b00000111);

	write_string("- JAMMIX Test Jig v1.0 -", 0b11100011, 9, 1);
	write_string("Hold: Select=analog Start=advanced", 0b11100011, 3, 29);

	// Draw pads
	for (j = 0; j < PAD_COUNT; j++)
	{
		write_stringf("JOY %d", 0xFF, pad_offset_x[j] - 5, pad_offset_y[j] + 3, j + 1);
		draw_pad(pad_offset_x[j], pad_offset_y[j]);
	}
}

// Draw static elements for analog input test page
void page_inputtester_analog()
{
	unsigned char j;
	
	clear_chars(0);
	page_border(0b00000111);

	write_string("- JAMMIX Test Jig v1.0 -", 0b11100011, 9, 1);
	write_string("Hold: Select=digital Start=advanced", 0b11100011, 3, 29);

	for (j = 0; j < PAD_COUNT; j++)
	{
		write_stringf("ANALOG %d", 0xFF, analog_offset_x[j] + 5, analog_offset_y[j] - 1, j + 1);
		draw_analog(analog_offset_x[j], analog_offset_y[j], analog_size, analog_size);
	}
}

// Draw static elements for advanced input test page
void page_inputtester_advanced()
{
	unsigned char j;
	char label[5];
	
	clear_chars(0);
	page_border(0b00000111);

	write_string("- JAMMIX Test Jig v1.0 -", 0b11100011, 9, 1);
	write_string("Hold: Select=digital Start=analog", 0b11100011, 3, 29);

	write_string("RLDUABXYLRsS", 0xFF, 7, 3);
	write_string("AX", 0xFF, 22, 3);
	write_string("AY", 0xFF, 27, 3);

	write_string("POS", 0xFF, 7, 11);
	write_string("SPD  POS", 0xFF, 18, 11);
	
	for (j = 0; j < 6; j++)
	{
		sprintf(label, "JOY%d", j + 1);
		write_string(label, 0xFF - (j * 2), 2, 4 + j);

		sprintf(label, "PAD%d", j + 1);
		write_string(label, 0xFF - (j * 2), 2, 12 + j);

		sprintf(label, "SPN%d", j + 1);
		write_string(label, 0xFF - (j * 2), 14, 12 + j);
	}
	write_string("CON", 0xFF, 2, 19);
}

void reset_inputstates()
{
	char i;
	
	modeswitchtimer_select = 0;
	modeswitchtimer_start = 0;
	for (i = 0; i < 12; i++)
	{
		joystick_last[i] = 1;
	}
	for (i = 0; i < 6; i++)
	{
		ax_last[i] = 1;
		ay_last[i] = 1;
		px_last[i] = 1;
		sx_toggle_last[i] = 1;
		sx_last[i] = 1;
		sx_pos[i] = 0;
	}
}

// Initialise digital inputtester state and draw static elements
void start_inputtester_digital()
{
	state = STATE_INPUTTESTER;

	// Draw page
	page_inputtester_digital();

	// Reset last states for inputs
	reset_inputstates();
}

// Initialise analog inputtester state and draw static elements
void start_inputtester_analog()
{
	state = STATE_INPUTTESTERANALOG;

	// Draw page
	page_inputtester_analog();

	// Reset last states for inputs
	reset_inputstates();
}

// Initialise advanced inputtester state and draw static elements
void start_inputtester_advanced()
{
	state = STATE_INPUTTESTERADVANCED;

	// Draw page
	page_inputtester_advanced();

	// Reset console cursor
	con_x = con_l;
	con_y = con_t;

	// Reset last states for inputs
	reset_inputstates();
}

// Initialise fadeout state
void start_fadeout()
{
	state = STATE_FADEOUT;
	fadetimer = fadefreq;
	fade = 0;
}

// Initialise fadein state
void start_fadein()
{
	state = STATE_FADEIN;
	fadetimer = fadefreq;
	fade = 15;
}

// Initialise attract state and draw static elements
void start_attract()
{
	state = STATE_ATTRACT;
	attractstate = 0;
	clear_chars(0);
	page_border(0b00000111);
	write_string("SNEK", 0b00000111, 18, 0);
	movefreq = 5;
	movetimer = 1;
}

// Initialise attract state and draw static elements
void start_gameplay()
{
	state = STATE_GAME;
	length = 0;
	x = 20;
	y = 15;
	xd = 0;
	yd = 1;
	nxd = 0;
	nyd = 1;
	clear_chars(0);
	page_border(0b00000111);
	write_string("SNEK", 0b00000111, 18, 0);
	write_char(playerchar, 0xFF, x, y);

	movefreq = movefreqinit;
	movefreqdectimer = movefreqdecfreq;
	movetimer = movefreq;
}

// Fade out state
void fadeout()
{
	if (vsync && !vsync_last)
	{
		fadetimer--;
		if (fadetimer == 0)
		{
			box(fade, fade, 39 - fade, 29 - fade, 127, 0b0000111);
			fadetimer = fadefreq;
			fade++;
			if (fade == 16)
			{
				start_fadein();
			}
		}
	}
}

// Fade in state
void fadein()
{
	if (vsync && !vsync_last)
	{
		fadetimer--;
		if (fadetimer == 0)
		{
			box(fade, fade, 39 - fade, 29 - fade, 0, 0b0000000);
			fadetimer = fadefreq;
			fade--;
			if (fade == 0)
			{
				state = nextstate;
			}
		}
	}
}

// Rotate DPAD direction history and push new entry
void pushhistory(char new)
{
	char h;
	
	for (h = 1; h < HISTORY_LENGTH; h++)
	{
		history[h - 1] = history[h];
	}
	history[HISTORY_LENGTH - 1] = new;
}

// Track input history of P1 DPAD for secret codes!
void handle_codes()
{
	bdown_up_last = bdown_up;
	bdown_down_last = bdown_down;
	bdown_left_last = bdown_left;
	bdown_right_last = bdown_right;
	bdown_up = CHECK_BIT(joystick[0], 3);
	bdown_down = CHECK_BIT(joystick[0], 2);
	bdown_left = CHECK_BIT(joystick[0], 1);
	bdown_right = CHECK_BIT(joystick[0], 0);
	if (!bdown_up && bdown_up_last)
	{
		pushhistory(1);
	}
	if (!bdown_down && bdown_down_last)
	{
		pushhistory(2);
	}
	if (!bdown_left && bdown_left_last)
	{
		pushhistory(3);
	}
	if (!bdown_right && bdown_right_last)
	{
		pushhistory(4);
	}
	// Check for SNEK code
	if (history[0] == 1 && history[1] == 1 && history[2] == 2 && history[3] == 2 && history[4] == 3 && history[5] == 4)
	{
		nextstate = STATE_START_ATTRACT;
		pushhistory(0);
		start_fadeout();
		return;
	}
}

bool modeswitcher()
{
	// Switch to advanced mode if start is held for 1 second
	if (CHECK_BIT(joystick[8], 3))
	{
		modeswitchtimer_start++;
		if (modeswitchtimer_start == 60)
		{
			if (state == STATE_INPUTTESTERADVANCED)
			{
				start_inputtester_analog();
			}
			else
			{
				start_inputtester_advanced();
			}
			return 1;
		}
	}
	else
	{
		modeswitchtimer_start = 0;
	}
	// Switch between digital/analog mode if select is held for 1 second
	if (CHECK_BIT(joystick[8], 2))
	{
		modeswitchtimer_select++;
		if (modeswitchtimer_select == 60)
		{
			if (state == STATE_INPUTTESTER)
			{
				start_inputtester_analog();
			}
			else
			{
				start_inputtester_digital();
			}
			return 1;
		}
	}
	else
	{
		modeswitchtimer_select = 0;
	}
	return 0;
}

// Digital input tester state
void inputtester_digital()
{
	char joy, index, button, color;
	
	char pressed = 0;
	
	// Handle PS/2 inputs whenever possible to improve latency
	handle_ps2();

	// Handle secret code detection (joypad 1 directions)
	if (hsync && !hsync_last)
	{
		handle_codes();
	}

	// As soon as vsync is detected start drawing screen updates
	if (vsync && !vsync_last)
	{

		// Handle test mode switch
		if (modeswitcher())
		{
			return;
		}

		// Draw control pad buttons
		for (joy = 0; joy < PAD_COUNT; joy++)
		{
			index = joy * 32;
			for (button = 0; button < BUTTON_COUNT; button++)
			{
				pressed = (button < 8 ? CHECK_BIT(joystick[index], button) : CHECK_BIT(joystick[index + 8], button - 8));	// (bits are split across more than one BYTE.)
				color = 0b10010010; // Default button color when NOT pressed. (or not set to Green/Red for Pass/Fail yet.)
				
				switch (joy) {
					case 0: if (pressed) {p1_pass[button]=1; color=0xFF;} else if (p1_pass[button]) color = 0x38; break;
					case 1: if (pressed) {p2_pass[button]=1; color=0xFF;} else if (p2_pass[button]) color = 0x38; break;
					case 2: if (pressed) {p3_pass[button]=1; color=0xFF;} else if (p3_pass[button]) color = 0x38; break;
					case 3: if (pressed) {p4_pass[button]=1; color=0xFF;} else if (p4_pass[button]) color = 0x38; break;
				}
				
				if ( (joy==2 || joy==3) && (button==8 || button==9) ) {}
				else write_string(button_name[button], color, pad_offset_x[joy] + button_x[button], pad_offset_y[joy] + button_y[button]);
			}
		}
	}
}

// Analog input tester state
void inputtester_analog()
{
	char j, mx, my;
	signed char ax, ay;

	// Handle PS/2 inputs whenever possible to improve latency
	handle_ps2();

	// Handle secret code detection (joypad 1 directions)
	if (hsync && !hsync_last)
	{
		handle_codes();
	}

	// As soon as vsync is detected start drawing screen updates
	if (vsync && !vsync_last)
	{

		// Handle test mode switch
		if (modeswitcher())
		{
			return;
		}

		// Draw analog point
		for (j = 0; j < PAD_COUNT; j++)
		{

			mx = analog_offset_x[j] + (analog_size / 2);
			my = analog_offset_y[j] + (analog_size / 2);

			// Reset previous color
			set_colour(color_analog_grid, analog_x[j] + mx, analog_y[j] + my);

			ax = analog[(j * 16)];
			ay = analog[(j * 16) + 8];

			analog_x[j] = ax / analog_ratio;
			analog_y[j] = ay / analog_ratio;

			// Set new color
			set_colour(0xFF, analog_x[j] + mx, analog_y[j] + my);

			write_stringfs("%4d", 0xFF, analog_offset_x[j], analog_offset_y[j] + analog_size + 1, ax);
			write_stringfs("%4d", 0xFF, analog_offset_x[j] + 5, analog_offset_y[j] + analog_size + 1, ay);
		}
	}
}

// Advanced input tester state
void inputtester_advanced()
{
	unsigned char inputindex, m, x, y, inputoffset, lastoffset, b, index, lastindex, joy, bytes, i;
	signed char ax, ay;
	char stra[11];
	unsigned char px;
	char strp[6];
	bool sx_toggle;
	signed char sx;
	char k;

	// Handle PS/2 inputs whenever possible to improve latency
	handle_ps2();

	// Handle secret code detection (joypad 1 directions)
	if (hsync && !hsync_last)
	{
		handle_codes();
	}

	// As soon as vsync is detected start drawing screen updates
	if (vsync && !vsync_last)
	{

		// Handle test mode switch
		if (modeswitcher())
		{
			return;
		}

		// Draw joystick inputs (only update each byte if value has changed)
		for (inputindex = 0; inputindex < 6; inputindex++)
		{
			m = 0b00000001;
			x = 6;
			y = 4 + inputindex;
			inputoffset = (inputindex * 32);
			lastoffset = (inputindex * 2);
			for (b = 0; b < 2; b++)
			{
				index = (b * 8) + inputoffset;
				lastindex = b + lastoffset;
				joy = joystick[index];
				if (joy != joystick_last[lastindex])
				{
					m = 0b00000001;
					bytes = (b == 0 ? 8 : 4);
					for (i = 0; i < bytes; i++)
					{
						x++;
						write_char((joy & m) ? asc_1 : asc_0, 0xFF, x, y);
						m <<= 1;
					}
				}
				else
				{
					x += 8;
				}
				joystick_last[lastindex] = joy;
			}

			// Draw analog inputs (only update if value has changed)
			ax = analog[(inputindex * 16)];
			ay = analog[(inputindex * 16) + 8];
			if (ax != ax_last[inputindex] || ay != ay_last[inputindex])
			{
				stra[10];
				sprintf(stra, "%4d %4d", ax, ay);
				write_string(stra, 0xFF, 20, 4 + inputindex);
			}
			ax_last[inputindex] = ax;
			ay_last[inputindex] = ay;

			// Draw paddle inputs (only update if value has changed)
			px = paddle[(inputindex * 8)];
			if (px != px_last[inputindex])
			{
				strp[5];
				sprintf(strp, "%4d", px);
				write_string(strp, 0xFF, 6, 12 + inputindex);
			}
			px_last[inputindex] = px;

			// Draw spinner inputs (only update when update clock changes)
			sx_toggle = CHECK_BIT(spinner[(inputindex * 16) + 8], 0);
			sx = spinner[(inputindex * 16)];
			if (sx_toggle != sx_toggle_last[inputindex])
			{
				sx_pos[inputindex] += sx;
				write_stringf("%4d", 0xFF, 22, 12 + inputindex, sx_pos[inputindex] / 8);
			}
			else
			{
				if (sx == 1 || sx == -1)
				{
					sx = 0;
				}
			}
			if (sx_last[inputindex] != sx)
			{
				write_stringfs("%4d", 0xFF, 17, 12 + inputindex, sx);
			}
			sx_last[inputindex] = sx;
			sx_toggle_last[inputindex] = sx_toggle;
		}

		// Keyboard test console
		if (kbd_buffer_len > 0)
		{
			// Clear existing cursor if visible
			if (con_cursor)
			{
				write_char(' ', 0xFF, con_x, con_y);
			}
			// Write characters in buffer
			for (k = 0; k < kbd_buffer_len; k++)
			{
				if (kbd_buffer[k] == '\n')
				{
					// New line
					con_x = con_l;
					con_y++;
					if (con_y > con_b)
					{
						// Wrap to top
						con_y = con_t;
					}
				}
				else if (kbd_buffer[k] == '\b')
				{
					// Backspace - only if not at beginning of line
					if (con_x > con_l)
					{
						con_x--;
						// Clear existing character
						write_char(' ', 0xFF, con_x, con_y);
					}
				}
				else
				{
					// Write character
					write_char(kbd_buffer[k], 0xFF, con_x, con_y);
					// Move cursor right
					con_x++;
					if (con_x > con_r)
					{
						// New line
						con_x = con_l;
						con_y++;
						if (con_y > con_b)
						{
							// Wrap to top
							con_y = con_t;
						}
					}
				}
			}
			// Clear buffer and enable cursor
			kbd_buffer_len = 0;
			con_cursor = 0;
			con_cursortimer = 1;
		}

		// Cursor blink timer
		con_cursortimer--;
		if (con_cursortimer <= 0)
		{
			con_cursor = !con_cursor;
			con_cursortimer = con_cursorfreq;
			write_char(con_cursor ? '|' : ' ', 0xFF, con_x, con_y);
		}
	}
}

// SNEK - gameplay state
void gameplay()
{
	unsigned int p;
	char score[5];
	char str_movefreq[3];

	if (hsync && !hsync_last)
	{
		if (yd != 1 && joystick[0] & 0b00001000) // up
		{
			nyd = -1;
			nxd = 0;
		}
		if (yd != -1 && joystick[0] & 0b00000100) // down
		{
			nyd = 1;
			nxd = 0;
		}
		if (xd != 1 && joystick[0] & 0b00000010) // left
		{
			nxd = -1;
			nyd = 0;
		}
		if (xd != -1 && joystick[0] & 0b00000001) //right
		{
			nxd = 1;
			nyd = 0;
		}

		if (CHECK_BIT(joystick[8], 2)) // select to quit
		{
			start_inputtester_advanced();
			return;
		}
	}

	if (vsync && !vsync_last)
	{
		movetimer--;
		if (movetimer == 0)
		{
			write_char(127, 0x66, x, y);
			xd = nxd;
			yd = nyd;
			x += xd;
			y += yd;
			p = (y * chram_cols) + x;
			if (chram[p] > 0)
			{
				nextstate = STATE_START_ATTRACT;
				start_fadeout();
				return;
			}
			length++;
			write_char(playerchar, 0xFF, x, y);
			movetimer = movefreq;
			sprintf(score, "%4d", length);
			write_string(score, 0xFF, 35, 0);
		}

		movefreqdectimer--;
		if (movefreqdectimer == 0)
		{
			movefreqdectimer = movefreqdecfreq;
			if (movefreq > 3)
			{
				movefreq--;
			}
			sprintf(str_movefreq, "%4d", movefreq);
			write_string(str_movefreq, 0xFF, 35, 29);
		}
	}
}

// SNEK - attract state
void attract()
{

	if (hsync && !hsync_last)
	{
		if (CHECK_BIT(joystick[8], 3)) // start to start
		{
			start_gameplay();
			return;
		}
		if (CHECK_BIT(joystick[8], 2)) // select to quit
		{
			start_inputtester_advanced();
			return;
		}
	}

	if (vsync && !vsync_last)
	{
		movetimer--;
		if (movetimer == 0)
		{
			attractstate = !attractstate;
			write_string("PRESS START", attractstate == 0 ? 0x00 : 0xFF, 16, 15);
			movetimer = movefreq;
		}
	}
}

#define sd_rd sd_flags[4]
#define txd_data sd_flags[16]
#define txd_start sd_flags[17]
#define rxd_data sd_flags[18]
#define img_just_loaded sd_flags[4]&0x01
#define img_readonly sd_flags[4]&0x02
#define sd_ack sd_flags[4]&0x04
#define load_fw sd_flags[4]&0x08
#define txd_busy sd_flags[4]&0x10
#define rxd_ready_flag sd_flags[4]&0x20

void uart_send(unsigned char data) {
	while (txd_busy) {}			// Loop if TXD busy. TODO: Add a timeout maybe?
	
	txd_data = data;				// UART TXD write data.
	txd_start = 1;					// Trigger UART TXD to start!
}

unsigned char wait_uart_recv() {
	int timeout = 5000000;
	
	while ( !(rxd_ready_flag) ) {	// Loop if RXD not ready.
		//if (timeout==0) break; else timeout--;
	}
	
	return rxd_data;				// Grab the received byte.
}

void write_sd_lba() {
	sd_flags[0] = (sd_lba>>24)&0xFF;	// sd_lba[31:24].
	sd_flags[1] = (sd_lba>>16)&0xFF;	// sd_lba[23:16].
	sd_flags[2] = (sd_lba>>8 )&0xFF;	// sd_lba[15:08].
	sd_flags[3] = (sd_lba    )&0xFF;	// sd_lba[07:00].
}

void send_address(unsigned long addr) {
	unsigned char checksum = 0x00;
	
	checksum ^= (addr>>24)&0xFF;
	checksum ^= (addr>>16)&0xFF;
	checksum ^= (addr>>8 )&0xFF;
	checksum ^= (addr>>0 )&0xFF;
	
	uart_send( (addr>>24)&0xFF ); uart_send( (addr>>16)&0xFF ); uart_send( (addr>>8)&0xFF ); uart_send( (addr>>0)&0xFF );
	uart_send(checksum);
}

void small_delay() {
	unsigned long delay = 10000;
	
	while (delay>0) delay--;
}

char check_ack() {
	if ( wait_uart_recv()!=0x79 ) {
		sprintf(mystring, "NACK Error! rxd_data: %02X", rxd_data); write_string(mystring, 0b11100011, 0, 18);
		return 1;
	}
	else {
		return 0;
	}
}

char send_get_command() {
	write_string("Sending GET command...", 0b11100011, 0, text_line); text_line++;
	uart_send(0x00); uart_send(0xFF);
	if (check_ack()) return 1;
	
	nbr = wait_uart_recv() + 1;				// Get number of bytes that follow...
	
	if (nbr>0) bt = wait_uart_recv();		// Bootloader version. (eg. 0x10 = version 1.0).
	if (nbr>1) get = wait_uart_recv();		// "Get" command. (usually 0x00).
	if (nbr>2) ver = wait_uart_recv();		// Version and Read Protection Status. (usually 0x01).
	if (nbr>3) id = wait_uart_recv();		// ID command. (usually 0x02).
	if (nbr>4) rd = wait_uart_recv();		// Read Memory command. (usually 0x11).
	if (nbr>5) go = wait_uart_recv();		// Go command. (usually 0x21).	
	if (nbr>6) wr = wait_uart_recv();		// Write Memory command. (usually 0x31).
	if (nbr>7) er = wait_uart_recv();		// Erase command or Extended Erase command. (either 0x43 or 0x44, never both!)
	if (nbr>8) wp = wait_uart_recv();		// Write Protect command. (usually 0x63).
	if (nbr>9) wup = wait_uart_recv();		// Write Unprotect command. (usually 0x73).
	if (nbr>10) rp = wait_uart_recv();		// Readout Protect command. (usually 0x82).
	if (nbr>11) rup = wait_uart_recv();		// Readout Unprotect command. (usually 0x92).
	if (nbr>12) ck = wait_uart_recv();		// Get Checksum command. (usually 0xA1. Only for bootloader V3.3).
	sprintf(mystring, "nb:%02X b:%02X gt:%02X vr:%02X id:%02X rd:%02X go:%02X", nbr, bt, get, ver, id, rd, go); write_string(mystring, 0b11100011, 0, text_line); text_line++;
	sprintf(mystring, "wr:%02X e:%02X wp:%02X wu:%02X rp:%02X ru:%02X ck:%02X", wr, er, wp, wup, rp, rup, ck); write_string(mystring, 0b11100011, 0, text_line); text_line++;
	
	return 0;
}

char write_unprot() {
	write_string("Sending Write Unprot command...", 0b11100011, 0, text_line); text_line++;
	uart_send(0x73); uart_send(0x8C);	// Send Write Unprotect command, plus checksum.
	//small_delay();
	if (check_ack()) return 2;
		
	return 0;
}

char erase_chip() {	
	if (er==0x43) {
		write_string("Sending ERASE command...", 0b11100011, 0, text_line); text_line++;
		uart_send(0x43); uart_send(0xBC);	// Send ERASE command, plus checksum.
		//small_delay();
		if (check_ack()) return 3;
	}
	else {
		write_string("Sending Extended ERASE command...", 0b11100011, 0, text_line); text_line++;
		uart_send(0x44); uart_send(0xBB);	// Send Extended ERASE command, plus checksum.
		//small_delay();
		if (check_ack()) return 4;
		
		write_string("Waiting for Mass ERASE to finish...", 0b11100011, 0, text_line); text_line++;
		uart_send(0xFF); uart_send(0xFF); uart_send(0x00);	// Mass erase request. 0xFFFF, plus checksum (0x00).
		//small_delay();
		if (check_ack()) return 5;
	}
	
	return 0;
}

char stm32_send_sector() {
	unsigned int i;
	unsigned char checksum;	
	unsigned char addr_byte_1=0x00, addr_byte_2=0x00, addr_byte_3=0x00, addr_byte_4=0x00;
	
	//write_string("Sending WRITE command (1)...", 0b11100011, 0, text_line); text_line++;
	uart_send(0x31); uart_send(0xCE); // Send WRITE command, plus checksum.
	if (check_ack()) return 6;
	
	addr_byte_1 = (flash_addr>>24)&0xff;
	addr_byte_2 = (flash_addr>>16)&0xff;
	addr_byte_3 = (flash_addr>>8 )&0xff;
	addr_byte_4 = (flash_addr>>0 )&0xff;
	//write_string("Sending ADDRESS (1)...", 0b11100011, 0, text_line); text_line++;
	send_address(flash_addr);	// Send the start address, plus checksum.
	if (check_ack()) return 7;

	//write_string("Sending nbr of bytes to write (1)...", 0b11100011, 0, text_line); text_line++;
	uart_send(0xFF);		// Number of BYTES to be written == 255+1 == 256. (no checksum!)
	
	sprintf(mystring, "Write 256 bytes (1) addr: %02X%02X%02X%02X", addr_byte_1, addr_byte_2, addr_byte_3, addr_byte_4);
	write_string(mystring, 0b11100011, 0, 14);
	
	checksum = 0x00;
	for (i=0; i<=255; i++) {
		uart_send(sd_buf[i]);
		checksum ^= sd_buf[i];
	}
	checksum ^= 0xFF;		// The checksum needs to include the "number of bytes"-1 value!
	uart_send(checksum);
	
	if (check_ack()) return 8;
	//small_delay();
	
	
	flash_addr += 256;

	//write_string("Sending WRITE command (2)...", 0b11100011, 0, text_line); text_line++;
	uart_send(0x31); uart_send(0xCE); // Send WRITE command, plus checksum.
	if (check_ack()) return 9;

	addr_byte_1 = (flash_addr>>24)&0xff;
	addr_byte_2 = (flash_addr>>16)&0xff;
	addr_byte_3 = (flash_addr>>8 )&0xff;
	addr_byte_4 = (flash_addr>>0 )&0xff;	
	//write_string("Sending ADDRESS (2)...", 0b11100011, 0, text_line); text_line++;
	send_address(flash_addr);	// Send the start address, plus checksum.
	if (check_ack()) return 10;

	//write_string("Sending nbr of bytes to write (2)...", 0b11100011, 0, text_line); text_line++;
	uart_send(0xFF);		// Number of BYTES to be written == 255+1 == 256. (no checksum!)
	
	sprintf(mystring, "Write 256 bytes (2) addr: %02X%02X%02X%02X", addr_byte_1, addr_byte_2, addr_byte_3, addr_byte_4);
	write_string(mystring, 0b11100011, 0, 14);
	
	checksum = 0x00;	
	for (; i<=511; i++) {		// Carry on where we left off, at i-255.
		uart_send(sd_buf[i]);
		checksum ^= sd_buf[i];
	}
	checksum ^= 0xFF;		// The checksum needs to include the "number of bytes"-1 value!
	uart_send(checksum);
	
	if (check_ack()) return 11;
	//small_delay();
	
	return 0;
}

void request_sector() {
	sd_rd = 1;	// Trigger an SD sector read! (if image is mounted in the MiSTer OSD).
	while (sd_ack) {}
	while (!sd_ack) {}
}

char update_fw() {
	unsigned char temp;
	unsigned char nack_byte;
	
	clear_chars(0);	// Clear the screen first. (The old/default os.bin might have loaded with the core - wipe out anything that's already in Char RAM.)

	text_line = 3;
	write_string("     JAMMIX Firmware Updater v1.0      ", 0b11100011, 0, text_line); text_line++;
	text_line++;
	write_string("1. Set DIP switches 7, 8, 9 to ON", 0b11100011, 0, text_line); text_line++;
	write_string("2. Use 'Mount fw file' to Load STM32_FW", 0b11100011, 0, text_line); text_line++;
	write_string("3. Briefly press the STM32 Reset button", 0b11100011, 0, text_line); text_line++;
	write_string("4. Use 'Flash Firmware!' option", 0b11100011, 0, text_line); text_line++;
	
	while (1) {
		if (load_fw) break;
	}
	
	clear_chars(0);	// Clear the screen.
	
	//temp = sd_flags[4];	// Clear any old flags by reading first.
	
	text_line = 3;
	
	write_string("Sending INIT command...", 0b11100011, 0, text_line); text_line++;
	uart_send(0x7F);
	if (check_ack()) return 12;
	
	send_get_command();
	
	//write_unprot();
	//small_delay();
	//write_string("Sending INIT command...", 0b11100011, 0, text_line); text_line++;
	//uart_send(0x7F);
	//check_ack();	
	
	temp = sd_flags[4];	// Clear any old flags by reading first.
	text_line++;
	write_string("(if Erasing takes more than 20 seconds,", 0b11100011, 0, text_line); text_line++;
	write_string("please use Reset core option on OSD,", 0b11100011, 0, text_line); text_line++;
	write_string("then retry the steps)", 0b11100011, 0, text_line); text_line++;
	text_line++;
	nack_byte = erase_chip();
	if (nack_byte) {
		sprintf(mystring, "Erase ERROR: %d", nack_byte);
		write_string(mystring, 0b11100011, 0, text_line); text_line++;
		return 13;
	}
	
	text_line++; text_line++;
	sprintf(mystring, "img_size: %08X", img_size);
	write_string(mystring, 0b11100011, 0, text_line); text_line++; text_line++;
	
	sd_lba = 0x00000000; write_sd_lba();	// First sector of FW file.
	flash_addr = 0x08000000;				// Start of Flash on STM32F4.
	
	while ( (sd_lba*512) < img_size ) {
		//sprintf(mystring, "  sd_lba: %02X%02X%02X%02X  sd_flags: %02X", sd_flags[0],sd_flags[1],sd_flags[2],sd_flags[3],  sd_flags[4]);
		//write_string(mystring, 0b11100011, 5, 23);
		request_sector();
		
		nack_byte = stm32_send_sector();
		if (nack_byte>0) {
			sprintf(mystring, "check_ack() ERROR: %d      ", nack_byte);
			write_string(mystring, 0b11100011, 5, 23);
			return 14;
		}
		small_delay();
		sd_lba++; write_sd_lba();
		flash_addr += 256;
	}
	
	write_string("DONE!                     ", 0b11100011, 0, 19);
	write_string("Set DIP switches 7, 8, 9 to OFF.", 0b11100011, 0, 23);
	write_string("Disconnect and reconnect power to JAMMIX.", 0b11100011, 0, 24);
	
	//return 0;
	while (1) {}
}


// Main entry and state machine
void main()
{
	char temp = sd_flags[4];	// Clear any old flags by reading first.
	int i;
	
	for (i=0; i<=BUTTON_COUNT; i++) {
		p1_pass[i]=0;
		p2_pass[i]=0;
		p3_pass[i]=0;
		p4_pass[i]=0;
	}
	
	//chram_size = chram_cols * chram_rows;
	while (1)
	{
		if (img_just_loaded) {
			// Only using the lower four bytes for this atm (32 bits is plenty).
			img_size = (sd_flags[12]<<24) | (sd_flags[13]<<16) | (sd_flags[14]<<8) | (sd_flags[15]<<0);
			if (img_size==0) {
				write_string("STM32_FW image size is Zero!", 0b11100011, 0, 12);
				write_string("Use 'Mount fw file' to Load STM32_FW", 0b11100011, 0, 13);
			}
			else update_fw();
		}
		
		//sprintf(mystring, "img_size: %02X%02X%02X%02X%02X%02X%02X%02X", sd_flags[8],sd_flags[9],sd_flags[10],sd_flags[11],sd_flags[12],sd_flags[13],sd_flags[14],sd_flags[15]);
		//write_string(mystring, 0b11100011, 5, 22);
		
		//sprintf(mystring, "  sd_lba: %02X%02X%02X%02X  sd_flags: %02X", sd_flags[0],sd_flags[1],sd_flags[2],sd_flags[3],  sd_flags[4]);
		//write_string(mystring, 0b11100011, 5, 23);
		
		//sprintf(mystring, "  sd_buf: %02X%02X%02X%02X", sd_buf[0],sd_buf[1],sd_buf[2],sd_buf[3]);
		//write_string(mystring, 0b11100011, 5, 24);
		
		//sprintf(mystring, "txd_data: %02X  rxd_data: %02X", sd_flags[16], sd_flags[18]);
		//write_string(mystring, 0b11100011, 5, 25);
		
		hsync = input0 & 0x80;
		vsync = input0 & 0x40;
		switch (state)
		{
		case STATE_START_INPUTTESTER:
			start_inputtester_digital();
			break;
		case STATE_INPUTTESTER:
			inputtester_digital();
			break;

		case STATE_START_INPUTTESTERADVANCED:
			start_inputtester_advanced();
			break;
		case STATE_INPUTTESTERADVANCED:
			inputtester_advanced();
			break;

		case STATE_START_INPUTTESTERANALOG:
			start_inputtester_analog();
			break;
		case STATE_INPUTTESTERANALOG:
			inputtester_analog();
			break;

		case STATE_FADEOUT:
			fadeout();
			break;
		case STATE_FADEIN:
			fadein();
			break;

		case STATE_START_ATTRACT:
			start_attract();
			break;
		case STATE_ATTRACT:
			attract();
			break;

		case STATE_START_GAME:
			start_gameplay();
			break;
		case STATE_GAME:
			gameplay();
			break;

		default:
			break;
		}
		hsync_last = hsync;
		vsync_last = vsync;
	}
}
