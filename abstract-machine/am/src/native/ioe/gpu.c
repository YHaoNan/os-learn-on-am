#include <am.h>
#include <SDL.h>
#include <fenv.h>

//#define MODE_800x600
#define WINDOW_W 800
#define WINDOW_H 600
#ifdef MODE_800x600
const int disp_w = WINDOW_W, disp_h = WINDOW_H;
#else
const int disp_w = 400, disp_h = 300;
#endif

#define FPS   60

#define RMASK 0x00ff0000
#define GMASK 0x0000ff00
#define BMASK 0x000000ff
#define AMASK 0x00000000

static SDL_Window *window = NULL;
static SDL_Surface *surface = NULL;

static Uint32 texture_sync(Uint32 interval, void *param) {
  SDL_BlitScaled(surface, NULL, SDL_GetWindowSurface(window), NULL);
  SDL_UpdateWindowSurface(window);
  return interval;
}

void __am_gpu_init() {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
  window = SDL_CreateWindow("Native Application",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      WINDOW_W, WINDOW_H, SDL_WINDOW_OPENGL);
  surface = SDL_CreateRGBSurface(SDL_SWSURFACE, disp_w, disp_h, 32,
      RMASK, GMASK, BMASK, AMASK);
  SDL_AddTimer(1000 / FPS, texture_sync, NULL);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = disp_w, .height = disp_h,
    .vmemsz = 0
  };
}

void __am_gpu_status(AM_GPU_STATUS_T *stat) {
  stat->ready = true;
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;
  if (w == 0 || h == 0) return;
  feclearexcept(-1);
  SDL_Surface *s = SDL_CreateRGBSurfaceFrom(ctl->pixels, w, h, 32, w * sizeof(uint32_t),
      RMASK, GMASK, BMASK, AMASK);
  SDL_Rect rect = { .x = x, .y = y };
  SDL_BlitSurface(s, NULL, surface, &rect);
  SDL_FreeSurface(s);
}
