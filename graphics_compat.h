#ifndef GRAPHICS_COMPAT_H
#define GRAPHICS_COMPAT_H

#include <SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Basic types
#define far

// Graphics drivers and modes
#define DETECT 0
#define VGA 9
#define VGAHI 2

// Colors
enum COLORS {
  BLACK,
  BLUE,
  GREEN,
  CYAN,
  RED,
  MAGENTA,
  BROWN,
  LIGHTGRAY,
  DARKGRAY,
  LIGHTBLUE,
  LIGHTGREEN,
  LIGHTCYAN,
  LIGHTRED,
  LIGHTMAGENTA,
  YELLOW,
  WHITE
};

// Line styles
#define SOLID_LINE 0
#define DOTTED_LINE 1
#define CENTER_LINE 2
#define DASHED_LINE 3
#define USERBIT_LINE 4

// Fill styles
#define EMPTY_FILL 0
#define SOLID_FILL 1

// Text justification
#define LEFT_TEXT 0
#define CENTER_TEXT 1
#define RIGHT_TEXT 2
#define BOTTOM_TEXT 0
#define TOP_TEXT 2

// Fonts
#define DEFAULT_FONT 0
#define TRIPLEX_FONT 1
#define SMALL_FONT 2
#define SANS_SERIF_FONT 3
#define GOTHIC_FONT 4

// Keys (DOS scan codes mapped to ASCII or custom)
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_ESC 27
#define KEY_ENTER 13
#define KEY_SPACE 32

// Function prototypes
void initgraph(int *graphdriver, int *graphmode, const char *pathtodriver);
void closegraph();
void cleardevice();
void restorecrtmode();

void setcolor(int color);
void setbkcolor(int color);
void setfillstyle(int pattern, int color);
void setlinestyle(int linestyle, unsigned upattern, int thickness);
void settextstyle(int font, int direction, int charsize);
void settextjustify(int horiz, int vert);
void setpalette(int colornum, int color);

void line(int x1, int y1, int x2, int y2);
void rectangle(int left, int top, int right, int bottom);
void bar(int left, int top, int right, int bottom);
void fillpoly(int numpoints, int *polypoints);
void outtextxy(int x, int y, const char *textstring);
void putimage(int left, int top, void *bitmap, int op);
void getimage(int left, int top, int right, int bottom, void *bitmap);
unsigned imagesize(int left, int top, int right, int bottom);

// CONIO / DOS compat
int kbhit();
int getch();
void delay(int ms);
void sound(int frequency);
void nosound();
void randomize();

#endif // GRAPHICS_COMPAT_H
