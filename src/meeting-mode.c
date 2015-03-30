#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static GFont *font;


static void update_time(struct tm *tick_time) {
  static char buffer[] = "00";

  strftime(buffer, sizeof("00"), "%M", tick_time);
  text_layer_set_text(text_layer, buffer);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ROBOTO_70));

  text_layer = text_layer_create((GRect) { .origin = { 0, 38 }, .size = { bounds.size.w, 100 } });
  text_layer_set_text(text_layer, "..");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_font(text_layer, font);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  update_time(tick_time);
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
  fonts_unload_custom_font(font);
}

static void tick_handler(struct tm *tick_time, TimeUnits time_changed) {
  static const uint32_t const double_segments[] = { 200, 250, 200 };
  VibePattern double_pattern = {
    .durations = double_segments,
    .num_segments = ARRAY_LENGTH(double_segments),
  };

  if ((tick_time->tm_min % 15) == 0) {
    vibes_enqueue_custom_pattern(double_pattern);
  } else if ((tick_time->tm_min % 5) == 0) {
    vibes_short_pulse();
  }
  update_time(tick_time);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  const bool animated = true;
  window_stack_push(window, animated);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
