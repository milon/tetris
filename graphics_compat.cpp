#include "graphics_compat.h"

#include <climits>
#include <cstring>

/*************************************************************************/
/*                                                                       */
/*                      BGI -> SDL2 compatibility                        */
/*                                                                       */
/*  Every primitive draws into a 640x480 software framebuffer which is    */
/*  uploaded to the window once per frame.  The game uses getimage() /    */
/*  putimage() as its sprite cache, so pixels read back must be exactly   */
/*  the pixels that were drawn; a GPU backbuffer does not guarantee that. */
/*                                                                       */
/*************************************************************************/

static const int SCREEN_W = 640;
static const int SCREEN_H = 480;

static SDL_Window *window = nullptr;
static SDL_Renderer *renderer = nullptr;
static SDL_Texture *screen_tex = nullptr;
static uint32_t *fb = nullptr;
static bool fb_dirty = true;

/* Graphics state */
static int current_color = WHITE;
static int current_bk_color = BLACK;
static int fill_color = WHITE;
static int line_thickness = 1;
static int text_direction = 0;
static int text_size = 1;
static int text_justify_horiz = LEFT_TEXT;
static int text_justify_vert = TOP_TEXT;

/* Active palette, as 0xAARRGGBB. Entries are remapped by setpalette(). */
static uint32_t palette[16] = {
    0xFF000000, // BLACK
    0xFF0000AA, // BLUE
    0xFF00AA00, // GREEN
    0xFF00AAAA, // CYAN
    0xFFAA0000, // RED
    0xFFAA00AA, // MAGENTA
    0xFFAA5500, // BROWN
    0xFFAAAAAA, // LIGHTGRAY
    0xFF555555, // DARKGRAY
    0xFF5555FF, // LIGHTBLUE
    0xFF55FF55, // LIGHTGREEN
    0xFF55FFFF, // LIGHTCYAN
    0xFFFF5555, // LIGHTRED
    0xFFFF55FF, // LIGHTMAGENTA
    0xFFFFFF55, // YELLOW
    0xFFFFFFFF  // WHITE
};

/*------------------------------ 5x7 font -------------------------------*/

struct GlyphArt {
  char ch;
  const char *rows; // 7 rows of 5 columns, '.' = off
};

static const GlyphArt glyph_art[] = {
    {' ', "....."
          "....."
          "....."
          "....."
          "....."
          "....."
          "....."},
    {'A', ".###."
          "#...#"
          "#...#"
          "#####"
          "#...#"
          "#...#"
          "#...#"},
    {'B', "####."
          "#...#"
          "#...#"
          "####."
          "#...#"
          "#...#"
          "####."},
    {'C', ".###."
          "#...#"
          "#...."
          "#...."
          "#...."
          "#...#"
          ".###."},
    {'D', "####."
          "#...#"
          "#...#"
          "#...#"
          "#...#"
          "#...#"
          "####."},
    {'E', "#####"
          "#...."
          "#...."
          "####."
          "#...."
          "#...."
          "#####"},
    {'F', "#####"
          "#...."
          "#...."
          "####."
          "#...."
          "#...."
          "#...."},
    {'G', ".###."
          "#...#"
          "#...."
          "#.###"
          "#...#"
          "#...#"
          ".###."},
    {'H', "#...#"
          "#...#"
          "#...#"
          "#####"
          "#...#"
          "#...#"
          "#...#"},
    {'I', "#####"
          "..#.."
          "..#.."
          "..#.."
          "..#.."
          "..#.."
          "#####"},
    {'J', "..###"
          "...#."
          "...#."
          "...#."
          "...#."
          "#..#."
          ".##.."},
    {'K', "#...#"
          "#..#."
          "#.#.."
          "##..."
          "#.#.."
          "#..#."
          "#...#"},
    {'L', "#...."
          "#...."
          "#...."
          "#...."
          "#...."
          "#...."
          "#####"},
    {'M', "#...#"
          "##.##"
          "#.#.#"
          "#...#"
          "#...#"
          "#...#"
          "#...#"},
    {'N', "#...#"
          "##..#"
          "#.#.#"
          "#..##"
          "#...#"
          "#...#"
          "#...#"},
    {'O', ".###."
          "#...#"
          "#...#"
          "#...#"
          "#...#"
          "#...#"
          ".###."},
    {'P', "####."
          "#...#"
          "#...#"
          "####."
          "#...."
          "#...."
          "#...."},
    {'Q', ".###."
          "#...#"
          "#...#"
          "#...#"
          "#.#.#"
          "#..#."
          ".##.#"},
    {'R', "####."
          "#...#"
          "#...#"
          "####."
          "#.#.."
          "#..#."
          "#...#"},
    {'S', ".####"
          "#...."
          "#...."
          ".###."
          "....#"
          "....#"
          "####."},
    {'T', "#####"
          "..#.."
          "..#.."
          "..#.."
          "..#.."
          "..#.."
          "..#.."},
    {'U', "#...#"
          "#...#"
          "#...#"
          "#...#"
          "#...#"
          "#...#"
          ".###."},
    {'V', "#...#"
          "#...#"
          "#...#"
          "#...#"
          "#...#"
          ".#.#."
          "..#.."},
    {'W', "#...#"
          "#...#"
          "#...#"
          "#.#.#"
          "#.#.#"
          "##.##"
          "#...#"},
    {'X', "#...#"
          "#...#"
          ".#.#."
          "..#.."
          ".#.#."
          "#...#"
          "#...#"},
    {'Y', "#...#"
          "#...#"
          ".#.#."
          "..#.."
          "..#.."
          "..#.."
          "..#.."},
    {'Z', "#####"
          "....#"
          "...#."
          "..#.."
          ".#..."
          "#...."
          "#####"},
    {'a', "....."
          "....."
          ".###."
          "....#"
          ".####"
          "#...#"
          ".####"},
    {'b', "#...."
          "#...."
          "####."
          "#...#"
          "#...#"
          "#...#"
          "####."},
    {'c', "....."
          "....."
          ".###."
          "#...#"
          "#...."
          "#...#"
          ".###."},
    {'d', "....#"
          "....#"
          ".####"
          "#...#"
          "#...#"
          "#...#"
          ".####"},
    {'e', "....."
          "....."
          ".###."
          "#...#"
          "#####"
          "#...."
          ".###."},
    {'f', "..##."
          ".#..."
          ".#..."
          "####."
          ".#..."
          ".#..."
          ".#..."},
    {'g', "....."
          ".####"
          "#...#"
          "#...#"
          ".####"
          "....#"
          ".###."},
    {'h', "#...."
          "#...."
          "####."
          "#...#"
          "#...#"
          "#...#"
          "#...#"},
    {'i', "..#.."
          "....."
          "..#.."
          "..#.."
          "..#.."
          "..#.."
          "..#.."},
    {'j', "...#."
          "....."
          "...#."
          "...#."
          "...#."
          "#..#."
          ".##.."},
    {'k', "#...."
          "#...."
          "#..#."
          "#.#.."
          "##..."
          "#.#.."
          "#..#."},
    {'l', ".##.."
          "..#.."
          "..#.."
          "..#.."
          "..#.."
          "..#.."
          ".###."},
    {'m', "....."
          "....."
          "##.#."
          "#.#.#"
          "#.#.#"
          "#.#.#"
          "#...#"},
    {'n', "....."
          "....."
          "####."
          "#...#"
          "#...#"
          "#...#"
          "#...#"},
    {'o', "....."
          "....."
          ".###."
          "#...#"
          "#...#"
          "#...#"
          ".###."},
    {'p', "....."
          "....."
          "####."
          "#...#"
          "#...#"
          "####."
          "#...."},
    {'q', "....."
          "....."
          ".####"
          "#...#"
          "#...#"
          ".####"
          "....#"},
    {'r', "....."
          "....."
          "#.##."
          "##..#"
          "#...."
          "#...."
          "#...."},
    {'s', "....."
          "....."
          ".####"
          "#...."
          ".###."
          "....#"
          "####."},
    {'t', ".#..."
          ".#..."
          "####."
          ".#..."
          ".#..."
          ".#..."
          "..###"},
    {'u', "....."
          "....."
          "#...#"
          "#...#"
          "#...#"
          "#...#"
          ".####"},
    {'v', "....."
          "....."
          "#...#"
          "#...#"
          "#...#"
          ".#.#."
          "..#.."},
    {'w', "....."
          "....."
          "#...#"
          "#.#.#"
          "#.#.#"
          "#.#.#"
          ".#.#."},
    {'x', "....."
          "....."
          "#...#"
          ".#.#."
          "..#.."
          ".#.#."
          "#...#"},
    {'y', "....."
          "#...#"
          "#...#"
          "#...#"
          ".####"
          "....#"
          ".###."},
    {'z', "....."
          "....."
          "#####"
          "...#."
          "..#.."
          ".#..."
          "#####"},
    {'0', ".###."
          "#...#"
          "#..##"
          "#.#.#"
          "##..#"
          "#...#"
          ".###."},
    {'1', "..#.."
          ".##.."
          "..#.."
          "..#.."
          "..#.."
          "..#.."
          ".###."},
    {'2', ".###."
          "#...#"
          "....#"
          "...#."
          "..#.."
          ".#..."
          "#####"},
    {'3', "####."
          "....#"
          "....#"
          ".###."
          "....#"
          "....#"
          "####."},
    {'4', "...#."
          "..##."
          ".#.#."
          "#..#."
          "#####"
          "...#."
          "...#."},
    {'5', "#####"
          "#...."
          "####."
          "....#"
          "....#"
          "#...#"
          ".###."},
    {'6', "..##."
          ".#..."
          "#...."
          "####."
          "#...#"
          "#...#"
          ".###."},
    {'7', "#####"
          "....#"
          "...#."
          "..#.."
          "..#.."
          "..#.."
          "..#.."},
    {'8', ".###."
          "#...#"
          "#...#"
          ".###."
          "#...#"
          "#...#"
          ".###."},
    {'9', ".###."
          "#...#"
          "#...#"
          ".####"
          "....#"
          "...#."
          ".##.."},
    {':', "....."
          "..#.."
          "..#.."
          "....."
          "..#.."
          "..#.."
          "....."},
    {',', "....."
          "....."
          "....."
          "....."
          "..#.."
          "..#.."
          ".#..."},
    {'.', "....."
          "....."
          "....."
          "....."
          "....."
          "..#.."
          "..#.."},
    {'?', ".###."
          "#...#"
          "....#"
          "...#."
          "..#.."
          "....."
          "..#.."},
    {'!', "..#.."
          "..#.."
          "..#.."
          "..#.."
          "..#.."
          "....."
          "..#.."},
    {'-', "....."
          "....."
          "....."
          "#####"
          "....."
          "....."
          "....."},
    {'^', "..#.."
          ".#.#."
          "#...#"
          "....."
          "....."
          "....."
          "....."},
};

static const int GLYPH_W = 5;    // glyph columns
static const int GLYPH_H = 7;    // glyph rows
static const int CELL_ADV = 6;   // horizontal advance, unscaled
static const int CELL_VADV = 8;  // vertical advance, unscaled

static uint8_t font_bits[128][GLYPH_H];

static void InitFont() {
  memset(font_bits, 0, sizeof(font_bits));
  const int count = (int)(sizeof(glyph_art) / sizeof(glyph_art[0]));
  for (int i = 0; i < count; i++) {
    unsigned char c = (unsigned char)glyph_art[i].ch;
    if (c >= 128)
      continue;
    for (int row = 0; row < GLYPH_H; row++) {
      uint8_t mask = 0;
      for (int col = 0; col < GLYPH_W; col++)
        if (glyph_art[i].rows[row * GLYPH_W + col] != '.')
          mask |= (uint8_t)(1 << col);
      font_bits[c][row] = mask;
    }
  }
}

/*--------------------------- pixel plumbing ---------------------------*/

static inline void PutPixel(int x, int y, uint32_t color) {
  if (x < 0 || y < 0 || x >= SCREEN_W || y >= SCREEN_H)
    return;
  fb[y * SCREEN_W + x] = color;
}

static void FillRect(int x, int y, int w, int h, uint32_t color) {
  if (w <= 0 || h <= 0)
    return;
  if (x < 0) {
    w += x;
    x = 0;
  }
  if (y < 0) {
    h += y;
    y = 0;
  }
  if (x + w > SCREEN_W)
    w = SCREEN_W - x;
  if (y + h > SCREEN_H)
    h = SCREEN_H - y;
  for (int row = 0; row < h; row++) {
    uint32_t *dst = fb + (y + row) * SCREEN_W + x;
    for (int col = 0; col < w; col++)
      dst[col] = color;
  }
  fb_dirty = true;
}

static void HLine(int x1, int x2, int y, uint32_t color) {
  if (x1 > x2) {
    int t = x1;
    x1 = x2;
    x2 = t;
  }
  FillRect(x1, y, x2 - x1 + 1, 1, color);
}

static void DrawLine(int x1, int y1, int x2, int y2, uint32_t color,
                     int thickness) {
  int dx = abs(x2 - x1), dy = abs(y2 - y1);
  int sx = x1 < x2 ? 1 : -1, sy = y1 < y2 ? 1 : -1;
  int err = dx - dy;
  int t = thickness < 1 ? 1 : thickness;
  int half = t / 2;
  while (true) {
    if (t == 1)
      PutPixel(x1, y1, color);
    else
      FillRect(x1 - half, y1 - half, t, t, color);
    if (x1 == x2 && y1 == y2)
      break;
    int e2 = 2 * err;
    if (e2 > -dy) {
      err -= dy;
      x1 += sx;
    }
    if (e2 < dx) {
      err += dx;
      y1 += sy;
    }
  }
  fb_dirty = true;
}

/*----------------------------- presenting -----------------------------*/

static void Present() {
  if (!renderer || !fb_dirty)
    return;
  SDL_UpdateTexture(screen_tex, nullptr, fb, SCREEN_W * 4);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, screen_tex, nullptr, nullptr);
  SDL_RenderPresent(renderer);
  fb_dirty = false;
}

static void ShutdownGraphics() {
  if (screen_tex) {
    SDL_DestroyTexture(screen_tex);
    screen_tex = nullptr;
  }
  if (renderer) {
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
  }
  if (window) {
    SDL_DestroyWindow(window);
    window = nullptr;
  }
  free(fb);
  fb = nullptr;
  SDL_Quit();
}

/*------------------------------- input --------------------------------*/

static int key_queue[256];
static int key_head = 0;
static int key_tail = 0;

static void PushKey(int key) {
  int next = (key_tail + 1) % 256;
  if (next == key_head)
    return; // buffer full, drop like the BIOS would
  key_queue[key_tail] = key;
  key_tail = next;
}

static void PumpEvents() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      ShutdownGraphics();
      exit(0);
    }
    if (e.type != SDL_KEYDOWN)
      continue;
    SDL_Keycode sym = e.key.keysym.sym;
    switch (sym) {
    // Extended keys arrive as a 0 byte followed by the DOS scan code,
    // which is what the game's getch() pairs expect.
    case SDLK_UP:
      PushKey(0);
      PushKey(KEY_UP);
      continue;
    case SDLK_DOWN:
      PushKey(0);
      PushKey(KEY_DOWN);
      continue;
    case SDLK_LEFT:
      PushKey(0);
      PushKey(KEY_LEFT);
      continue;
    case SDLK_RIGHT:
      PushKey(0);
      PushKey(KEY_RIGHT);
      continue;
    case SDLK_ESCAPE:
      PushKey(KEY_ESC);
      continue;
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
      PushKey(KEY_ENTER);
      continue;
    case SDLK_SPACE:
      PushKey(KEY_SPACE);
      continue;
    default:
      break;
    }
    if (sym >= 32 && sym < 127) {
      int c = (int)sym;
      if ((e.key.keysym.mod & KMOD_SHIFT) && c >= 'a' && c <= 'z')
        c -= 32;
      PushKey(c);
    }
  }
}

/*-------------------- scripted demo / screenshots ---------------------*/

static bool autoplay = false;
static const char *shot_dir = nullptr;
static Uint32 shot_at[16];
static int shot_count = 0;
static int shot_done = 0;
static Uint32 start_ticks = 0;
static Uint32 last_inject = 0;
static bool title_dismissed = false;

static void SaveScreenshot(const char *path) {
  Present();
  SDL_Surface *surf = SDL_CreateRGBSurfaceWithFormatFrom(
      fb, SCREEN_W, SCREEN_H, 32, SCREEN_W * 4, SDL_PIXELFORMAT_ARGB8888);
  if (!surf)
    return;
  SDL_SaveBMP(surf, path);
  SDL_FreeSurface(surf);
}

static void InitScripting() {
  start_ticks = SDL_GetTicks();
  autoplay = getenv("TETRIS_AUTOPLAY") != nullptr;
  shot_dir = getenv("TETRIS_SHOT_DIR");
  if (!shot_dir)
    return;
  const char *times = getenv("TETRIS_SHOT_MS");
  if (times) {
    const char *p = times;
    while (*p && shot_count < 16) {
      shot_at[shot_count++] = (Uint32)strtoul(p, nullptr, 10);
      const char *comma = strchr(p, ',');
      if (!comma)
        break;
      p = comma + 1;
    }
  } else {
    shot_at[shot_count++] = 400;
    shot_at[shot_count++] = 3000;
    shot_at[shot_count++] = 7000;
    shot_at[shot_count++] = 12000;
  }
}

// Plays the game by itself and grabs frames, so screenshots can be taken
// without a human at the keyboard. Inactive unless the env vars are set.
static void RunScript() {
  if (!autoplay && !shot_dir)
    return;
  Uint32 now = SDL_GetTicks();
  Uint32 elapsed = now - start_ticks;

  if (autoplay) {
    if (!title_dismissed) {
      if (elapsed > 600) {
        PushKey(KEY_ENTER);
        title_dismissed = true;
        last_inject = now;
      }
    } else if (now - last_inject > 45) {
      last_inject = now;
      int roll = rand() % 100;
      if (roll < 50) {
        PushKey(0);
        PushKey(KEY_DOWN);
      } else if (roll < 70) {
        PushKey(0);
        PushKey(KEY_LEFT);
      } else if (roll < 90) {
        PushKey(0);
        PushKey(KEY_RIGHT);
      } else {
        PushKey(KEY_SPACE);
      }
    }
  }

  while (shot_done < shot_count && elapsed >= shot_at[shot_done]) {
    char path[512];
    snprintf(path, sizeof(path), "%s/shot-%02d.bmp", shot_dir, shot_done);
    SaveScreenshot(path);
    shot_done++;
  }

  if (autoplay && shot_count && shot_done >= shot_count &&
      elapsed > shot_at[shot_count - 1] + 500) {
    ShutdownGraphics();
    exit(0);
  }
}

/*------------------------------ BGI API -------------------------------*/

void initgraph(int *graphdriver, int *graphmode, const char *pathtodriver) {
  (void)graphdriver;
  (void)graphmode;
  (void)pathtodriver;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    exit(1);
  }

  window = SDL_CreateWindow("Aguntuk Bricks", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, SCREEN_W, SCREEN_H,
                            SDL_WINDOW_SHOWN);
  if (!window) {
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    exit(1);
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer)
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
  if (!renderer) {
    printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
    exit(1);
  }

  screen_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                 SDL_TEXTUREACCESS_STREAMING, SCREEN_W,
                                 SCREEN_H);
  fb = (uint32_t *)calloc((size_t)SCREEN_W * SCREEN_H, sizeof(uint32_t));
  if (!screen_tex || !fb) {
    printf("Out of memory creating the framebuffer\n");
    exit(1);
  }

  InitFont();
  InitScripting();
  fb_dirty = true;
  Present();
}

void closegraph() { ShutdownGraphics(); }

void cleardevice() {
  FillRect(0, 0, SCREEN_W, SCREEN_H, palette[current_bk_color]);
  Present();
}

void restorecrtmode() { ShutdownGraphics(); }

void setcolor(int color) {
  if (color >= 0 && color < 16)
    current_color = color;
}

void setbkcolor(int color) {
  if (color >= 0 && color < 16)
    current_bk_color = color;
}

void setfillstyle(int pattern, int color) {
  (void)pattern; // patterns are drawn solid
  if (color >= 0 && color < 16)
    fill_color = color;
}

void setlinestyle(int linestyle, unsigned upattern, int thickness) {
  (void)linestyle;
  (void)upattern;
  line_thickness = thickness < 1 ? 1 : thickness;
}

void settextstyle(int font, int direction, int charsize) {
  (void)font; // one built-in bitmap font stands in for the BGI fonts
  text_direction = direction;
  text_size = charsize < 1 ? 1 : (charsize > 10 ? 10 : charsize);
}

void settextjustify(int horiz, int vert) {
  text_justify_horiz = horiz;
  text_justify_vert = vert;
}

// EGA palette values are 6-bit rgbRGB: the low three bits are the full
// intensity primaries, the high three the half intensity ones.
void setpalette(int colornum, int color) {
  if (colornum < 0 || colornum > 15)
    return;
  int r = ((color >> 2) & 1) * 170 + ((color >> 5) & 1) * 85;
  int g = ((color >> 1) & 1) * 170 + ((color >> 4) & 1) * 85;
  int b = ((color >> 0) & 1) * 170 + ((color >> 3) & 1) * 85;
  palette[colornum] = 0xFF000000u | ((uint32_t)r << 16) | ((uint32_t)g << 8) |
                      (uint32_t)b;
}

void line(int x1, int y1, int x2, int y2) {
  DrawLine(x1, y1, x2, y2, palette[current_color], line_thickness);
}

void rectangle(int left, int top, int right, int bottom) {
  uint32_t c = palette[current_color];
  DrawLine(left, top, right, top, c, line_thickness);
  DrawLine(right, top, right, bottom, c, line_thickness);
  DrawLine(right, bottom, left, bottom, c, line_thickness);
  DrawLine(left, bottom, left, top, c, line_thickness);
}

void bar(int left, int top, int right, int bottom) {
  FillRect(left, top, right - left + 1, bottom - top + 1,
           palette[fill_color]);
}

void fillpoly(int numpoints, int *polypoints) {
  if (numpoints < 2)
    return;

  int miny = INT_MAX, maxy = INT_MIN;
  for (int i = 0; i < numpoints; i++) {
    int y = polypoints[2 * i + 1];
    if (y < miny)
      miny = y;
    if (y > maxy)
      maxy = y;
  }
  if (miny < 0)
    miny = 0;
  if (maxy > SCREEN_H - 1)
    maxy = SCREEN_H - 1;

  int crossings[64];
  for (int y = miny; y <= maxy; y++) {
    int n = 0;
    for (int i = 0; i < numpoints && n < 64; i++) {
      int j = (i + 1) % numpoints;
      int y0 = polypoints[2 * i + 1], y1 = polypoints[2 * j + 1];
      if (y0 == y1)
        continue;
      if ((y >= y0 && y < y1) || (y >= y1 && y < y0)) {
        int x0 = polypoints[2 * i], x1 = polypoints[2 * j];
        crossings[n++] = x0 + (y - y0) * (x1 - x0) / (y1 - y0);
      }
    }
    for (int a = 1; a < n; a++) {
      int v = crossings[a], b = a - 1;
      while (b >= 0 && crossings[b] > v) {
        crossings[b + 1] = crossings[b];
        b--;
      }
      crossings[b + 1] = v;
    }
    for (int a = 0; a + 1 < n; a += 2)
      HLine(crossings[a], crossings[a + 1], y, palette[fill_color]);
  }

  uint32_t outline = palette[current_color];
  for (int i = 0; i < numpoints; i++) {
    int j = (i + 1) % numpoints;
    DrawLine(polypoints[2 * i], polypoints[2 * i + 1], polypoints[2 * j],
             polypoints[2 * j + 1], outline, 1);
  }
}

static void DrawGlyph(int x, int y, char ch, int scale, uint32_t color) {
  unsigned char c = (unsigned char)ch;
  if (c >= 128)
    return;
  const uint8_t *g = font_bits[c];
  for (int row = 0; row < GLYPH_H; row++) {
    for (int col = 0; col < GLYPH_W; col++) {
      if (!(g[row] & (1 << col)))
        continue;
      if (scale == 1)
        PutPixel(x + col, y + row, color);
      else
        FillRect(x + col * scale, y + row * scale, scale, scale, color);
    }
  }
  fb_dirty = true;
}

void outtextxy(int x, int y, const char *textstring) {
  if (!textstring || !*textstring)
    return;
  int len = (int)strlen(textstring);
  int scale = text_size;
  uint32_t color = palette[current_color];

  if (text_direction == 0) {
    int w = len * CELL_ADV * scale - scale;
    int h = GLYPH_H * scale;
    if (text_justify_horiz == CENTER_TEXT)
      x -= w / 2;
    else if (text_justify_horiz == RIGHT_TEXT)
      x -= w;
    if (text_justify_vert == BOTTOM_TEXT)
      y -= h;
    for (int i = 0; i < len; i++)
      DrawGlyph(x + i * CELL_ADV * scale, y, textstring[i], scale, color);
  } else {
    // Vertical text: characters stay upright and stack downwards.
    int h = len * CELL_VADV * scale - scale;
    if (text_justify_vert == BOTTOM_TEXT)
      y -= h;
    for (int i = 0; i < len; i++)
      DrawGlyph(x, y + i * CELL_VADV * scale, textstring[i], scale, color);
  }
}

unsigned imagesize(int left, int top, int right, int bottom) {
  return (unsigned)((right - left + 1) * (bottom - top + 1) * 4 + 4);
}

void getimage(int left, int top, int right, int bottom, void *bitmap) {
  uint16_t w = (uint16_t)(right - left + 1);
  uint16_t h = (uint16_t)(bottom - top + 1);
  uint16_t *header = (uint16_t *)bitmap;
  header[0] = w;
  header[1] = h;
  uint32_t *pixels = (uint32_t *)(header + 2);

  for (int row = 0; row < h; row++) {
    for (int col = 0; col < w; col++) {
      int sx = left + col, sy = top + row;
      uint32_t px = 0xFF000000u;
      if (sx >= 0 && sy >= 0 && sx < SCREEN_W && sy < SCREEN_H)
        px = fb[sy * SCREEN_W + sx];
      pixels[row * w + col] = px;
    }
  }
}

void putimage(int left, int top, void *bitmap, int op) {
  (void)op; // only COPY_PUT is used by the game
  uint16_t *header = (uint16_t *)bitmap;
  uint16_t w = header[0];
  uint16_t h = header[1];
  uint32_t *pixels = (uint32_t *)(header + 2);

  for (int row = 0; row < h; row++) {
    int dy = top + row;
    if (dy < 0 || dy >= SCREEN_H)
      continue;
    for (int col = 0; col < w; col++) {
      int dx = left + col;
      if (dx < 0 || dx >= SCREEN_W)
        continue;
      fb[dy * SCREEN_W + dx] = pixels[row * w + col];
    }
  }
  fb_dirty = true;
}

int kbhit() {
  PumpEvents();
  RunScript();
  Present();
  return key_head != key_tail;
}

int getch() {
  while (key_head == key_tail) {
    PumpEvents();
    RunScript();
    Present();
    if (key_head == key_tail)
      SDL_Delay(2);
  }
  int key = key_queue[key_head];
  key_head = (key_head + 1) % 256;
  return key;
}

void delay(int ms) {
  Present();
  PumpEvents();
  RunScript();
  if (ms > 0)
    SDL_Delay((Uint32)ms);
}

void sound(int frequency) { (void)frequency; }
void nosound() {}
void randomize() { srand((unsigned)time(NULL)); }
