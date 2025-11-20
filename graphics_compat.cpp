#include "graphics_compat.h"
#include <cstring>
#include <iostream>
#include <vector>

// Global SDL variables
static SDL_Window *window = nullptr;
static SDL_Renderer *renderer = nullptr;
static int width = 640;
static int height = 480;

// Graphics state
static int current_color = WHITE;
static int current_bk_color = BLACK;
static int fill_pattern = SOLID_FILL;
static int fill_color = WHITE;
static int line_style = SOLID_LINE;
static int line_thickness = 1;
static int text_font = DEFAULT_FONT;
static int text_direction = 0;
static int text_size = 1;
static int text_justify_horiz = LEFT_TEXT;
static int text_justify_vert = TOP_TEXT;

// Palette (Standard EGA/VGA colors)
static SDL_Color palette[16] = {
    {0, 0, 0, 255},       // BLACK
    {0, 0, 170, 255},     // BLUE
    {0, 170, 0, 255},     // GREEN
    {0, 170, 170, 255},   // CYAN
    {170, 0, 0, 255},     // RED
    {170, 0, 170, 255},   // MAGENTA
    {170, 85, 0, 255},    // BROWN
    {170, 170, 170, 255}, // LIGHTGRAY
    {85, 85, 85, 255},    // DARKGRAY
    {85, 85, 255, 255},   // LIGHTBLUE
    {85, 255, 85, 255},   // LIGHTGREEN
    {85, 255, 255, 255},  // LIGHTCYAN
    {255, 85, 85, 255},   // LIGHTRED
    {255, 85, 255, 255},  // LIGHTMAGENTA
    {255, 255, 85, 255},  // YELLOW
    {255, 255, 255, 255}  // WHITE
};

void initgraph(int *graphdriver, int *graphmode, const char *pathtodriver) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    exit(1);
  }

  window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, width, height,
                            SDL_WINDOW_SHOWN);
  if (window == NULL) {
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    exit(1);
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
    exit(1);
  }

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

void closegraph() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void cleardevice() {
  SDL_SetRenderDrawColor(renderer, palette[current_bk_color].r,
                         palette[current_bk_color].g,
                         palette[current_bk_color].b, 255);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);
}

void restorecrtmode() { closegraph(); }

void setcolor(int color) {
  if (color >= 0 && color < 16) {
    current_color = color;
  }
}

void setbkcolor(int color) {
  if (color >= 0 && color < 16) {
    current_bk_color = color;
  }
}

void setfillstyle(int pattern, int color) {
  fill_pattern = pattern;
  if (color >= 0 && color < 16) {
    fill_color = color;
  }
}

void setlinestyle(int linestyle, unsigned upattern, int thickness) {
  line_style = linestyle;
  line_thickness = thickness;
}

void settextstyle(int font, int direction, int charsize) {
  text_font = font;
  text_direction = direction;
  text_size = charsize;
}

void settextjustify(int horiz, int vert) {
  text_justify_horiz = horiz;
  text_justify_vert = vert;
}

void setpalette(int colornum, int color) {
  // Ignored for now
}

void line(int x1, int y1, int x2, int y2) {
  SDL_SetRenderDrawColor(renderer, palette[current_color].r,
                         palette[current_color].g, palette[current_color].b,
                         255);
  SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void rectangle(int left, int top, int right, int bottom) {
  SDL_SetRenderDrawColor(renderer, palette[current_color].r,
                         palette[current_color].g, palette[current_color].b,
                         255);
  SDL_Rect rect = {left, top, right - left, bottom - top};
  SDL_RenderDrawRect(renderer, &rect);
}

void bar(int left, int top, int right, int bottom) {
  SDL_SetRenderDrawColor(renderer, palette[fill_color].r, palette[fill_color].g,
                         palette[fill_color].b, 255);
  SDL_Rect rect = {left, top, right - left + 1, bottom - top + 1};
  SDL_RenderFillRect(renderer, &rect);
}

void fillpoly(int numpoints, int *polypoints) {
  SDL_SetRenderDrawColor(renderer, palette[fill_color].r, palette[fill_color].g,
                         palette[fill_color].b, 255);
  // Just draw outline for now
  for (int i = 0; i < numpoints - 1; i++) {
    SDL_RenderDrawLine(renderer, polypoints[2 * i], polypoints[2 * i + 1],
                       polypoints[2 * (i + 1)], polypoints[2 * (i + 1) + 1]);
  }
  SDL_RenderDrawLine(renderer, polypoints[2 * (numpoints - 1)],
                     polypoints[2 * (numpoints - 1) + 1], polypoints[0],
                     polypoints[1]);
}

void outtextxy(int x, int y, const char *textstring) {
  // Placeholder text box
  SDL_SetRenderDrawColor(renderer, palette[current_color].r,
                         palette[current_color].g, palette[current_color].b,
                         255);
  SDL_Rect rect = {x, y, (int)strlen(textstring) * 8 * text_size,
                   8 * text_size};
  SDL_RenderDrawRect(renderer, &rect);
}

unsigned imagesize(int left, int top, int right, int bottom) {
  return (right - left + 1) * (bottom - top + 1) * 4 +
         4; // +4 for width/height header
}

void getimage(int left, int top, int right, int bottom, void *bitmap) {
  uint16_t w = right - left + 1;
  uint16_t h = bottom - top + 1;
  uint16_t *header = (uint16_t *)bitmap;
  header[0] = w;
  header[1] = h;

  uint32_t *pixels = (uint32_t *)(header + 2);
  SDL_Rect rect = {left, top, w, h};
  SDL_RenderReadPixels(renderer, &rect, SDL_PIXELFORMAT_RGBA8888, pixels,
                       w * 4);
}

void putimage(int left, int top, void *bitmap, int op) {
  uint16_t *header = (uint16_t *)bitmap;
  uint16_t w = header[0];
  uint16_t h = header[1];
  uint32_t *pixels = (uint32_t *)(header + 2);

  SDL_Surface *surf = SDL_CreateRGBSurfaceFrom(
      pixels, w, h, 32, w * 4, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
  SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);

  SDL_Rect dst = {left, top, w, h};
  SDL_RenderCopy(renderer, tex, NULL, &dst);

  SDL_DestroyTexture(tex);
  SDL_FreeSurface(surf);
  SDL_RenderPresent(renderer);
}

int kbhit() {
  SDL_PumpEvents();
  SDL_Event e;
  if (SDL_PeepEvents(&e, 1, SDL_PEEKEVENT, SDL_KEYDOWN, SDL_KEYDOWN) > 0) {
    return 1;
  }
  return 0;
}

int getch() {
  SDL_Event e;
  while (true) {
    if (SDL_WaitEvent(&e)) {
      if (e.type == SDL_QUIT) {
        exit(0);
      }
      if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_UP:
          return KEY_UP;
        case SDLK_DOWN:
          return KEY_DOWN;
        case SDLK_LEFT:
          return KEY_LEFT;
        case SDLK_RIGHT:
          return KEY_RIGHT;
        case SDLK_ESCAPE:
          return KEY_ESC;
        case SDLK_RETURN:
          return KEY_ENTER;
        case SDLK_SPACE:
          return KEY_SPACE;
        case SDLK_p:
          return 'p';
        case SDLK_s:
          return 's';
        case SDLK_a:
          return 'a';
        case SDLK_y:
          return 'y';
        default:
          return 0;
        }
      }
    }
  }
  return 0;
}

void delay(int ms) {
  SDL_RenderPresent(renderer); // Ensure screen is updated
  SDL_Delay(ms);
  SDL_PumpEvents();
}

void sound(int frequency) {}
void nosound() {}
void randomize() { srand(time(NULL)); }
