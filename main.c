#include "microui/microui.h"
#include "podoae.h"
#include "renderer/renderer.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char logbuf[64000];
static int logbuf_updated = 0;
static float bg[3] = {40, 44, 52};

static void write_log(const char *text) {
  if (logbuf[0]) {
    strcat(logbuf, "\n");
  }
  strcat(logbuf, text);
  logbuf_updated = 1;
}

static int uint8_slider(mu_Context *ctx, unsigned char *value, int low,
                        int high) {
  static float tmp;
  mu_push_id(ctx, &value, sizeof(value));
  tmp = *value;
  int res = mu_slider_ex(ctx, &tmp, low, high, 0, "%.0f", MU_OPT_ALIGNCENTER);
  *value = tmp;
  mu_pop_id(ctx);
  return res;
}

static void dashboard_window(mu_Context *ctx) {
  int opts =
      MU_OPT_NOTITLE | MU_OPT_NORESIZE | MU_OPT_NOFRAME | MU_OPT_NOSCROLL;
  char title_buf[128];
#ifdef PODOAE_VERSION
  snprintf(title_buf, sizeof(title_buf), "Podoae v%s - Pomodoro Dashboard", PODOAE_VERSION);
#else
  snprintf(title_buf, sizeof(title_buf), "Podoae - Pomodoro Dashboard");
#endif
  if (mu_begin_window_ex(ctx, title_buf,
                         mu_rect(0, 0, 800, 600), opts)) {
    mu_Container *win = mu_get_current_container(ctx);
    win->rect.x = 0;
    win->rect.y = 0;
    win->rect.w = 800;
    win->rect.h = 600;

    // Two column layout
    // Left column: Width 360
    // Right column: remaining width (-1)
    mu_layout_row(ctx, 2, (int[]){360, -1}, -1);

    /* --- LEFT COLUMN (Timer & Settings) --- */
    mu_layout_begin_column(ctx);

    if (mu_header_ex(ctx, "Timer Pomodoro", MU_OPT_EXPANDED)) {
      /* Mode Selector */
      mu_layout_row(ctx, 3, (int[]){-1, -1, -1}, 0);
      if (mu_button(ctx, "Kerja")) {
        pomodoro_set_mode(MODE_WORK, write_log);
      }
      if (mu_button(ctx, "Istirahat")) {
        pomodoro_set_mode(MODE_SHORT_BREAK, write_log);
      }
      if (mu_button(ctx, "Istirahat Pjg")) {
        pomodoro_set_mode(MODE_LONG_BREAK, write_log);
      }

      /* Spacer */
      mu_layout_row(ctx, 1, (int[]){-1}, 5);
      mu_label(ctx, "");

      /* Timer Panel */
      mu_layout_row(ctx, 1, (int[]){-1}, 50);
      mu_begin_panel(ctx, "Timer Panel");
      char timer_str[64];
      int mins = pomodoro.time_left / 60;
      int secs = pomodoro.time_left % 60;
      const char *mode_name = (pomodoro.mode == MODE_WORK) ? "KERJA"
                              : (pomodoro.mode == MODE_SHORT_BREAK)
                                  ? "ISTIRAHAT"
                                  : "ISTIRAHAT PJG";
      sprintf(timer_str, "  [%s]  %02d:%02d", mode_name, mins, secs);
      mu_layout_row(ctx, 1, (int[]){-1}, -1);
      mu_label(ctx, timer_str);
      mu_end_panel(ctx);

      /* Progress Bar */
      mu_layout_row(ctx, 1, (int[]){-1}, 12);
      mu_Rect r = mu_layout_next(ctx);
      mu_draw_rect(ctx, r, mu_color(30, 30, 30, 255)); // dark background
      int total_seconds = 0;
      if (pomodoro.mode == MODE_WORK)
        total_seconds = pomodoro.work_duration * 60;
      else if (pomodoro.mode == MODE_SHORT_BREAK)
        total_seconds = pomodoro.short_break_duration * 60;
      else
        total_seconds = pomodoro.long_break_duration * 60;
      float pct = (total_seconds > 0)
                      ? (float)pomodoro.time_left / total_seconds
                      : 0.0f;
      mu_Rect fill_r = r;
      fill_r.w = (int)(r.w * pct);
      mu_Color fill_color = (pomodoro.mode == MODE_WORK)
                                ? mu_color(220, 80, 80, 255)
                                : mu_color(80, 200, 120, 255);
      mu_draw_rect(ctx, fill_r, fill_color);

      /* Spacer */
      mu_layout_row(ctx, 1, (int[]){-1}, 5);
      mu_label(ctx, "");

      /* Controls */
      mu_layout_row(ctx, 3, (int[]){-1, -1, -1}, 0);
      if (mu_button(ctx, pomodoro.is_running ? "Pause" : "Mulai")) {
        pomodoro_toggle_start(write_log);
      }
      if (mu_button(ctx, "Lewati")) {
        pomodoro_skip(write_log);
      }
      if (mu_button(ctx, "Reset")) {
        pomodoro_reset(write_log);
      }

      /* Stats */
      mu_layout_row(ctx, 1, (int[]){-1}, 20);
      char stats_str[64];
      sprintf(stats_str, "Sesi Kerja Selesai: %d",
              pomodoro.completed_work_sessions);
      mu_label(ctx, stats_str);
    }

    if (mu_header_ex(ctx, "Pengaturan Durasi", MU_OPT_EXPANDED)) {
      mu_layout_row(ctx, 2, (int[]){130, -1}, 0);
      mu_label(ctx, "Kerja (menit):");
      if (mu_slider(ctx, &tmp_work_dur, 1, 60)) {
        pomodoro_update_settings();
      }
      mu_label(ctx, "Istirahat (menit):");
      if (mu_slider(ctx, &tmp_short_dur, 1, 30)) {
        pomodoro_update_settings();
      }
      mu_label(ctx, "Istirahat Pjg (menit):");
      if (mu_slider(ctx, &tmp_long_dur, 1, 45)) {
        pomodoro_update_settings();
      }
    }

    mu_layout_end_column(ctx);

    /* --- RIGHT COLUMN (Log & Style) --- */
    mu_layout_begin_column(ctx);

    if (mu_header_ex(ctx, "Riwayat Aktivitas", MU_OPT_EXPANDED)) {
      mu_layout_row(ctx, 1, (int[]){-1}, 140);
      mu_begin_panel(ctx, "Log Output Panel");
      mu_Container *panel = mu_get_current_container(ctx);
      mu_layout_row(ctx, 1, (int[]){-1}, -1);
      mu_text(ctx, logbuf);
      mu_end_panel(ctx);
      if (logbuf_updated) {
        panel->scroll.y = panel->content_size.y;
        logbuf_updated = 0;
      }

      // Input textbox + submit button
      static char buf[128];
      int submitted = 0;
      mu_layout_row(ctx, 2, (int[]){-70, -1}, 0);
      if (mu_textbox(ctx, buf, sizeof(buf)) & MU_RES_SUBMIT) {
        mu_set_focus(ctx, ctx->last_id);
        submitted = 1;
      }
      if (mu_button(ctx, "Kirim")) {
        submitted = 1;
      }
      if (submitted) {
        write_log(buf);
        buf[0] = '\0';
      }
    }

    if (mu_header(ctx, "Editor Tema (UI Style)")) {
      static struct {
        const char *label;
        int idx;
      } colors[] = {{"text:", MU_COLOR_TEXT},
                    {"border:", MU_COLOR_BORDER},
                    {"windowbg:", MU_COLOR_WINDOWBG},
                    {"titlebg:", MU_COLOR_TITLEBG},
                    {"titletext:", MU_COLOR_TITLETEXT},
                    {"panelbg:", MU_COLOR_PANELBG},
                    {"button:", MU_COLOR_BUTTON},
                    {"buttonhover:", MU_COLOR_BUTTONHOVER},
                    {"buttonfocus:", MU_COLOR_BUTTONFOCUS},
                    {"base:", MU_COLOR_BASE},
                    {"basehover:", MU_COLOR_BASEHOVER},
                    {"basefocus:", MU_COLOR_BASEFOCUS},
                    {"scrollbase:", MU_COLOR_SCROLLBASE},
                    {"scrollthumb:", MU_COLOR_SCROLLTHUMB},
                    {NULL}};

      int sw = mu_get_current_container(ctx)->body.w * 0.12;
      mu_layout_row(ctx, 6, (int[]){80, sw, sw, sw, sw, -1}, 0);
      for (int i = 0; colors[i].label; i++) {
        mu_label(ctx, colors[i].label);
        uint8_slider(ctx, &ctx->style->colors[i].r, 0, 255);
        uint8_slider(ctx, &ctx->style->colors[i].g, 0, 255);
        uint8_slider(ctx, &ctx->style->colors[i].b, 0, 255);
        uint8_slider(ctx, &ctx->style->colors[i].a, 0, 255);
        mu_draw_rect(ctx, mu_layout_next(ctx), ctx->style->colors[i]);
      }

      mu_layout_row(ctx, 2, (int[]){120, -1}, 0);
      mu_label(ctx, "Background R:");
      mu_slider(ctx, &bg[0], 0, 255);
      mu_label(ctx, "Background G:");
      mu_slider(ctx, &bg[1], 0, 255);
      mu_label(ctx, "Background B:");
      mu_slider(ctx, &bg[2], 0, 255);
    }

    mu_layout_end_column(ctx);

    mu_end_window(ctx);
  }
}

static void alarm_window(mu_Context *ctx) {
  if (show_alarm_popup) {
    if (mu_begin_window_ex(ctx, "Alarm!", mu_rect(260, 240, 280, 120),
                           MU_OPT_NORESIZE)) {
      mu_layout_row(ctx, 1, (int[]){-1}, 30);
      mu_label(ctx, alarm_message);
      mu_layout_row(ctx, 1, (int[]){-1}, 0);
      if (mu_button(ctx, "OK")) {
        show_alarm_popup = 0;
      }
      mu_end_window(ctx);
    }
  }
}

static void process_frame(mu_Context *ctx) {
  mu_begin(ctx);
  dashboard_window(ctx);
  alarm_window(ctx);
  mu_end(ctx);
}

static const char button_map[256] = {
    [SDL_BUTTON_LEFT & 0xff] = MU_MOUSE_LEFT,
    [SDL_BUTTON_RIGHT & 0xff] = MU_MOUSE_RIGHT,
    [SDL_BUTTON_MIDDLE & 0xff] = MU_MOUSE_MIDDLE,
};

static const char key_map[256] = {
    [SDLK_LSHIFT & 0xff] = MU_KEY_SHIFT,
    [SDLK_RSHIFT & 0xff] = MU_KEY_SHIFT,
    [SDLK_LCTRL & 0xff] = MU_KEY_CTRL,
    [SDLK_RCTRL & 0xff] = MU_KEY_CTRL,
    [SDLK_LALT & 0xff] = MU_KEY_ALT,
    [SDLK_RALT & 0xff] = MU_KEY_ALT,
    [SDLK_RETURN & 0xff] = MU_KEY_RETURN,
    [SDLK_BACKSPACE & 0xff] = MU_KEY_BACKSPACE,
};

static int text_width(mu_Font font, const char *text, int len) {
  (void)font;
  if (len == -1) {
    len = strlen(text);
  }
  return r_get_text_width(text, len);
}

static int text_height(mu_Font font) {
  (void)font;
  return r_get_text_height();
}

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;
  /* init SDL and renderer */
  SDL_Init(SDL_INIT_EVERYTHING);
  r_init();

  /* init pomodoro */
  pomodoro_init();

  /* init microui */
  mu_Context *ctx = malloc(sizeof(mu_Context));
  mu_init(ctx);
  ctx->text_width = text_width;
  ctx->text_height = text_height;

  /* main loop */
  Uint32 last_tick = SDL_GetTicks();

  for (;;) {
    Uint32 current_tick = SDL_GetTicks();
    Uint32 elapsed = current_tick - last_tick;
    last_tick = current_tick;

    pomodoro_tick(elapsed, write_log);

    /* handle SDL events */
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
      case SDL_QUIT:
        exit(EXIT_SUCCESS);
        break;
      case SDL_MOUSEMOTION:
        mu_input_mousemove(ctx, e.motion.x, e.motion.y);
        break;
      case SDL_MOUSEWHEEL:
        mu_input_scroll(ctx, 0, e.wheel.y * -30);
        break;
      case SDL_TEXTINPUT:
        mu_input_text(ctx, e.text.text);
        break;

      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP: {
        int b = button_map[e.button.button & 0xff];
        if (b && e.type == SDL_MOUSEBUTTONDOWN) {
          mu_input_mousedown(ctx, e.button.x, e.button.y, b);
        }
        if (b && e.type == SDL_MOUSEBUTTONUP) {
          mu_input_mouseup(ctx, e.button.x, e.button.y, b);
        }
        break;
      }

      case SDL_KEYDOWN:
      case SDL_KEYUP: {
        int c = key_map[e.key.keysym.sym & 0xff];
        if (c && e.type == SDL_KEYDOWN) {
          mu_input_keydown(ctx, c);
        }
        if (c && e.type == SDL_KEYUP) {
          mu_input_keyup(ctx, c);
        }
        break;
      }
      }
    }

    /* process frame */
    process_frame(ctx);

    /* render */
    r_clear(mu_color(bg[0], bg[1], bg[2], 255));
    mu_Command *cmd = NULL;
    while (mu_next_command(ctx, &cmd)) {
      switch (cmd->type) {
      case MU_COMMAND_TEXT:
        r_draw_text(cmd->text.str, cmd->text.pos, cmd->text.color);
        break;
      case MU_COMMAND_RECT:
        r_draw_rect(cmd->rect.rect, cmd->rect.color);
        break;
      case MU_COMMAND_ICON:
        r_draw_icon(cmd->icon.id, cmd->icon.rect, cmd->icon.color);
        break;
      case MU_COMMAND_CLIP:
        r_set_clip_rect(cmd->clip.rect);
        break;
      }
    }
    r_present();
    SDL_Delay(16);
  }

  free(ctx);
  return 0;
}
