#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include <stdint.h>
#include <string.h>


//#define MY_UUID { 0x9B, 0x41, 0xF2, 0x7A, 0x3E, 0x80, 0x4C, 0x72, 0xB2, 0x56, 0xB3, 0xA6, 0xFC, 0xF7, 0x15, 0xCB }
// 42c86ea4-1c3e-4a07-b889-2cccca914198
#define MY_UUID {0x42, 0xc8, 0x6e, 0xa4, 0x1c, 0x3e, 0x4a, 0x7, 0xb8, 0x89, 0x2c, 0xcc, 0xca, 0x91, 0x41, 0x98}
PBL_APP_INFO(MY_UUID,
             "Stock Pebble Price", "TalkingQuickly",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window window;
TextLayer hello_layer;

enum {
  WEATHER_ICON_KEY = 0x0,         // TUPLE_INT
  WEATHER_TEMPERATURE_KEY = 0x1,  // TUPLE_CSTRING
};

static struct WeatherData {
  AppSync sync;
  uint8_t sync_buffer[32];
} s_data;

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {

  switch (key) {
  case WEATHER_TEMPERATURE_KEY:
    // App Sync keeps the new_tuple around, so we may use it directly
    text_layer_set_text(&hello_layer, new_tuple->value->cstring);
    break;
  default:
    return;
  }
}


void handle_init(AppContextRef ctx) {

  window_init(&window, "Window Name");
  window_set_fullscreen(&window, true);
  window_stack_push(&window, true /* Animated */);
  text_layer_init(&hello_layer, GRect(0, 65, 144, 30));
  text_layer_set_text_alignment(&hello_layer, GTextAlignmentCenter);
  text_layer_set_text(&hello_layer, "Hello Ben");
  /*text_layer_set_font(&hello_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));*/
  /*text_layer_set_font(&hello_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));*/
  text_layer_set_font(&hello_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  layer_add_child(&window.layer, &hello_layer.layer);

  Tuplet initial_values[] = {
    TupletInteger(WEATHER_ICON_KEY, (uint8_t) 1),
    TupletCString(WEATHER_TEMPERATURE_KEY, "Unknown"),
  };

  app_sync_init(&s_data.sync, s_data.sync_buffer, sizeof(s_data.sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
                sync_tuple_changed_callback, NULL, NULL);
}

static void handle_deinit(AppContextRef c) {
  app_sync_deinit(&s_data.sync);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit,
    .messaging_info = {
      .buffer_sizes = {
        .inbound = 64,
        .outbound = 16,
      }
    }
  };
  app_event_loop(params, &handlers);
}

