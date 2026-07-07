#include "podoae.h"
#include <stdio.h>

PomodoroState pomodoro;
float tmp_work_dur = 25.0f;
float tmp_short_dur = 5.0f;
float tmp_long_dur = 15.0f;
int show_alarm_popup = 0;
char alarm_message[128] = "";

void pomodoro_init(void) {
  pomodoro.mode = MODE_WORK;
  pomodoro.work_duration = 25;
  pomodoro.short_break_duration = 5;
  pomodoro.long_break_duration = 15;
  pomodoro.time_left = 25 * 60;
  pomodoro.is_running = 0;
  pomodoro.completed_work_sessions = 0;
}

static void on_timer_complete(void (*log_callback)(const char*)) {
  if (pomodoro.mode == MODE_WORK) {
    pomodoro.completed_work_sessions++;
    sprintf(alarm_message, "Sesi kerja selesai! Waktunya istirahat.");
    if (log_callback) {
      log_callback("Sesi kerja selesai! Waktunya istirahat.");
    }
    if (pomodoro.completed_work_sessions % 4 == 0) {
      pomodoro.mode = MODE_LONG_BREAK;
      pomodoro.time_left = pomodoro.long_break_duration * 60;
      if (log_callback) {
        log_callback("Memulai Istirahat Panjang.");
      }
    } else {
      pomodoro.mode = MODE_SHORT_BREAK;
      pomodoro.time_left = pomodoro.short_break_duration * 60;
      if (log_callback) {
        log_callback("Memulai Istirahat Pendek.");
      }
    }
  } else {
    sprintf(alarm_message, "Waktu istirahat selesai! Kembali bekerja.");
    if (log_callback) {
      log_callback("Waktu istirahat selesai! Kembali bekerja.");
    }
    pomodoro.mode = MODE_WORK;
    pomodoro.time_left = pomodoro.work_duration * 60;
  }
  show_alarm_popup = 1;
}

void pomodoro_tick(Uint32 elapsed, void (*log_callback)(const char*)) {
  static Uint32 ms_accumulator = 0;
  if (pomodoro.is_running) {
    ms_accumulator += elapsed;
    if (ms_accumulator >= 1000) {
      int seconds_elapsed = ms_accumulator / 1000;
      ms_accumulator %= 1000;

      pomodoro.time_left -= seconds_elapsed;
      if (pomodoro.time_left <= 0) {
        pomodoro.time_left = 0;
        pomodoro.is_running = 0;
        on_timer_complete(log_callback);
      }
    }
  } else {
    ms_accumulator = 0;
  }
}

void pomodoro_set_mode(PomodoroMode mode, void (*log_callback)(const char*)) {
  pomodoro.mode = mode;
  pomodoro.is_running = 0;
  if (mode == MODE_WORK) {
    pomodoro.time_left = pomodoro.work_duration * 60;
    if (log_callback) log_callback("Mode diubah ke Bekerja.");
  } else if (mode == MODE_SHORT_BREAK) {
    pomodoro.time_left = pomodoro.short_break_duration * 60;
    if (log_callback) log_callback("Mode diubah ke Istirahat Pendek.");
  } else if (mode == MODE_LONG_BREAK) {
    pomodoro.time_left = pomodoro.long_break_duration * 60;
    if (log_callback) log_callback("Mode diubah ke Istirahat Panjang.");
  }
}

void pomodoro_skip(void (*log_callback)(const char*)) {
  if (log_callback) log_callback("Timer dilewati.");
  pomodoro.is_running = 0;
  on_timer_complete(log_callback);
}

void pomodoro_reset(void (*log_callback)(const char*)) {
  pomodoro.is_running = 0;
  if (pomodoro.mode == MODE_WORK) pomodoro.time_left = pomodoro.work_duration * 60;
  else if (pomodoro.mode == MODE_SHORT_BREAK) pomodoro.time_left = pomodoro.short_break_duration * 60;
  else pomodoro.time_left = pomodoro.long_break_duration * 60;
  if (log_callback) log_callback("Timer di-reset.");
}

void pomodoro_toggle_start(void (*log_callback)(const char*)) {
  pomodoro.is_running = !pomodoro.is_running;
  if (log_callback) {
    if (pomodoro.is_running) {
      log_callback("Timer dimulai.");
    } else {
      log_callback("Timer dijeda.");
    }
  }
}

void pomodoro_update_settings(void) {
  pomodoro.work_duration = (int)tmp_work_dur;
  pomodoro.short_break_duration = (int)tmp_short_dur;
  pomodoro.long_break_duration = (int)tmp_long_dur;
  if (!pomodoro.is_running) {
    if (pomodoro.mode == MODE_WORK) pomodoro.time_left = pomodoro.work_duration * 60;
    else if (pomodoro.mode == MODE_SHORT_BREAK) pomodoro.time_left = pomodoro.short_break_duration * 60;
    else pomodoro.time_left = pomodoro.long_break_duration * 60;
  }
}
