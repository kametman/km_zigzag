#include <pebble.h>

static Window *mainWindow;

static GColor colorHr;
static GColor colorMin;

static GBitmap *bmOverlay;

static Layer *layerTime;
static Layer *layerBatt;
static BitmapLayer *bLayerOverlay;

static int size = 24;

static void time_layer_update(Layer *layer, GContext *context) {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  int hour = tick_time->tm_hour;
  int min = tick_time->tm_min;
  
  int x, y, w, h;
  int offset = 5;
  
  graphics_context_set_fill_color(context, colorHr);  
  if (hour < 13) {
    int rows = hour / 2;
    int r = hour % 2;
    int d = 1;
    
    for (int i = 0; i < rows; i++) {
      x = i * size - offset;
      y = 0;
      w = size;
      h = 2 * size;
      graphics_fill_rect(context, GRect(x, y, w, h), 0, GCornerNone);
      d = d * -1;
    }
    if (r < 6) {
      x = rows * size -offset;
      y = d > 0 ? 0 : size * (2 - r);
      w = size;
      h = r * size;
      graphics_fill_rect(context, GRect(x, y, w, h), 0, GCornerNone);
    }
  }
  else {
    int row = hour / 2 - 5;
    int r = 2 - hour % 2;
    int d = 1;
    
    for (int i = row; i < 6; i++) {
      x = i * size + offset;
      y = 0;
      w = size;
      h = 2 * size;
      graphics_fill_rect(context, GRect(x, y, w, h), 0, GCornerNone);
      d = d * -1;
    }
    x = (row - 1) * size + offset;
    y = d > 0 ? 0 : size * (2 - r);
    w = size;
    h = r * size;
    graphics_fill_rect(context, GRect(x, y, w, h), 0, GCornerNone);
  }  
  graphics_context_set_fill_color(context, colorMin);
  if (min < 31) {
    int s = min / 10;
    int d = 1;
    
    for (int i = 0; i < s; i++) {
      x = 2 * i * size;
      y = 48;
      w = 48;
      h = 120;
      graphics_fill_rect(context, GRect(x, y, w, h), 0, GCornerNone);
      d = d * -1;
    }
      
    if (d > 0) {
      int rows = (min % 10) / 2;
      int r = (min % 10) % 2;

      for (int i = 0; i < rows; i++) {
        x = 2 * s * size;
        y = 48 + size * i - 5;
        w = 2 * size;
        h = size;
        graphics_fill_rect(context, GRect(x, y, w, h), 0, GCornerNone);
        d = d * -1;
      }
      x = 2 * s * size + (d > 0 ? 0 : size);
      y = 48 + size * rows - 5;
      w = size;
      h = r * size;
      graphics_fill_rect(context, GRect(x, y, w, h), 0, GCornerNone);    

    }
    else {
      int rows = 5 - (min % 10) / 2;
      int r = (min % 10) % 2;

      for (int i = rows; i < 5; i++) {
        x = 2 * s * size;
        y = 48 + size * i + 5;
        w = 2 * size;
        h = size;
        graphics_fill_rect(context, GRect(x, y, w, h), 0, GCornerNone);
        d = d * -1;
      }
      x = 2 * s * size + (d > 0 ? size : 0);
      y = 48 + size * (rows - 1) + 5;
      w = size;
      h = r * size;
      graphics_fill_rect(context, GRect(x, y, w, h), 0, GCornerNone);    
    }
  }
  else {
    int s = min / 10 - 2;
    int d = 1;
    
    for (int i = s; i < 3; i++) {
      x = 2 * i * size;
      y = 48;
      w = 48;
      h = 120;
      graphics_fill_rect(context, GRect(x, y, w, h), 0, GCornerNone);
      d = d * -1;
    }
    
    if (d > 0) {
      int rows = ((min % 10) + 1) / 2;
      int r = (min % 10) % 2;

      for (int i = rows; i < 5; i++) {
        x = 2 * (s - 1) * size;
        y = 48 + size * i + 5;
        w = 2 * size;
        h = size;
        graphics_fill_rect(context, GRect(x, y, w, h), 0, GCornerNone);
        d = d * -1;
      }
      x = 2 * (s - 1) * size + (d > 0 ? size : 0);
      y = 48 + size * (rows - 1) + 5;
      w = size;
      h = r * size;
      graphics_fill_rect(context, GRect(x, y, w, h), 0, GCornerNone);    
    }
    else {
      int rows = min == 40 ? 5 : 5 - ((min % 10 + 1) / 2);
      int r = (min % 10) % 2;

      for (int i = 0; i < rows; i++) {
        x = 2 * (s - 1) * size;
        y = 48 + size * i - 5;
        w = 2 * size;
        h = size;
        graphics_fill_rect(context, GRect(x, y, w, h), 0, GCornerNone);
        d = d * -1;
      }
      x = 2 * (s - 1) * size + (d > 0 ? 0 : size);
      y = 48 + size * rows - 5;
      w = size;
      h = r * size;
      graphics_fill_rect(context, GRect(x, y, w, h), 0, GCornerNone);    
    }
  }
}

static void batt_layer_update(Layer *layer, GContext *context) {
  bool connected = bluetooth_connection_service_peek();
  BatteryChargeState charge = battery_state_service_peek();
  int battLvl = charge.charge_percent / 10;
  
  graphics_context_set_fill_color(context, GColorBlack);
  graphics_context_set_stroke_color(context, GColorBlack);
  int offset = 2 + (10 - battLvl) * 7;
  
  if (charge.is_charging) {
      int x = offset;
      int y = 1;
      int w = 14 * battLvl;
      int h = 2;
      if (connected) {
        graphics_fill_rect(context, GRect(x, y, w, h), 0, GCornerNone);
      }
      else {
        graphics_draw_rect(context, GRect(x, y, w, h));
      }
  }
  else {
    for (int i = 0; i < battLvl; i++){
      int x = offset + i * 14 + (connected ? 1 : 4);
      int y = 1;
      int w = connected ? 12 : 6;
      int h = 2;
      if (connected) {
        graphics_fill_rect(context, GRect(x, y, w, h), 0, GCornerNone);
      }
      else {
        graphics_draw_rect(context, GRect(x, y, w, h));
      }
    }
  }
}

static void time_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(layerTime);
}

static void bluetooth_handler(bool btConn) {
  if (btConn) vibes_short_pulse();
  else vibes_long_pulse();
}

static void battery_handler(BatteryChargeState charge) {
  layer_mark_dirty(layerBatt);
}

static void main_window_load_handler(Window *window) {
  window_set_background_color(mainWindow, GColorBlack);
  
  bmOverlay = gbitmap_create_with_resource(RESOURCE_ID_IMG_OVERLAY);
  
  layerTime = layer_create(GRect(0, 0, 144, 168));
  layer_set_update_proc(layerTime, time_layer_update);
  
  layerBatt = layer_create(GRect(0, 46, 144, 4));
  layer_set_update_proc(layerBatt, batt_layer_update);
  
  bLayerOverlay = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_background_color(bLayerOverlay, GColorClear);
  bitmap_layer_set_compositing_mode(bLayerOverlay, GCompOpSet);
  bitmap_layer_set_bitmap(bLayerOverlay, bmOverlay);
  
  layer_add_child(window_get_root_layer(mainWindow), layerTime);
  layer_add_child(window_get_root_layer(mainWindow), bitmap_layer_get_layer(bLayerOverlay));
  layer_add_child(window_get_root_layer(mainWindow), layerBatt);
}

static void main_window_unload_handler(Window *window) {
  gbitmap_destroy(bmOverlay);
  layer_destroy(layerTime);
  layer_destroy(layerBatt);
  bitmap_layer_destroy(bLayerOverlay);
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple* hrColor = dict_find(iter, MESSAGE_KEY_HOUR);
  if (hrColor) {
    colorHr = GColorFromHEX(hrColor->value->int32);
    persist_write_int(0, hrColor->value->int32);
  }
  Tuple* minColor = dict_find(iter, MESSAGE_KEY_MINUTE);
  if (minColor) {
    colorMin = GColorFromHEX(minColor->value->int32);
    persist_write_int(1, minColor->value->int32);
  }
  
  layer_mark_dirty(layerTime);
}

void init(void) {
  colorHr = persist_exists(0) ? GColorFromHEX(persist_read_int(0)) : GColorWhite;
  colorMin = persist_exists(1) ? GColorFromHEX(persist_read_int(1)) : GColorWhite;
  
  mainWindow = window_create();
  
  window_set_window_handlers(mainWindow, (WindowHandlers) {
    .load = main_window_load_handler,
    .unload = main_window_unload_handler
  });
  
  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(128, 128);
  
  tick_timer_service_subscribe(SECOND_UNIT, time_handler);
  bluetooth_connection_service_subscribe(bluetooth_handler);
  battery_state_service_subscribe(battery_handler);
  
  window_stack_push(mainWindow, true);  
}

void deinit(void) {
  window_destroy(mainWindow);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}