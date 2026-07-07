#ifndef POMODORO_H
#define POMODORO_H

#include <SDL2/SDL.h>

typedef enum {
  MODE_WORK,
  MODE_SHORT_BREAK,
  MODE_LONG_BREAK
} PomodoroMode;

typedef struct {
  PomodoroMode mode;
  int work_duration;          // in minutes
  int short_break_duration;   // in minutes
  int long_break_duration;    // in minutes
  int time_left;              // in seconds
  int is_running;
  int completed_work_sessions;
} PomodoroState;

extern PomodoroState pomodoro;
extern float tmp_work_dur;
extern float tmp_short_dur;
extern float tmp_long_dur;
extern int show_alarm_popup;
extern char alarm_message[128];

void pomodoro_init(void);
void pomodoro_tick(Uint32 elapsed, void (*log_callback)(const char*));
void pomodoro_set_mode(PomodoroMode mode, void (*log_callback)(const char*));
void pomodoro_skip(void (*log_callback)(const char*));
void pomodoro_reset(void (*log_callback)(const char*));
void pomodoro_toggle_start(void (*log_callback)(const char*));
void pomodoro_update_settings(void);

#endif // POMODORO_H
