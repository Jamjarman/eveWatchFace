#include <pebble.h>
#include <time.h>
#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_weather_layer;
static TextLayer *s_battery_layer;
static TextLayer *s_bluetooth_layer;
static TextLayer *s_date_layer;
static TextLayer *s_eve_layer;
static TextLayer *s_second_layer;
static TextLayer *s_ampm_layer;

static void battery_handler(BatteryChargeState newstate){
  static char s_battery_buffer[32]="Reading...";
  if(newstate.is_charging){
    snprintf(s_battery_buffer, sizeof(s_battery_buffer), "Charging");
  }
  else{
    snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d%%", newstate.charge_percent);
  }
  text_layer_set_text(s_battery_layer, s_battery_buffer);
}

static void bluetooth_handler(bool status){
   text_layer_set_text(s_bluetooth_layer, status ? "Connected" : "Disconnected");
//   if(status){
//     text_layer_set_text(s_bluetooth_layer, "Connected");
//   }
//   else{
//     text_layer_set_text(s_bluetooth_layer, "Disconnected");
//   }
}

static void update_time(){
  //get a tm structureb
  
	time_t evetemp=time(NULL)+14400;
  struct tm *eve_time=gmtime(&evetemp);
	time_t temp=time(NULL);
  struct tm *tick_time=localtime(&temp);
  
  //create a long lived buffer
  static char buffer[]="00:00";
  static char evebuffer[60]="EVE Time";
  static char datebuffer[60]="determining...";
  static char secondbuffer[]="00";
  static char ampmbuffer[]="XX";
  //write the current hours and minutes into the buffer
  if(clock_is_24h_style()==true){
    //use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  }
  else{
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
    strftime(ampmbuffer, sizeof("XX"), "%p", tick_time);
    text_layer_set_text(s_ampm_layer, ampmbuffer);
  }
	strftime(evebuffer, sizeof(evebuffer), "EST: %H:%M %D", eve_time);
  strftime(datebuffer, sizeof(datebuffer), "%A, %B %d", tick_time);
  strftime(secondbuffer, sizeof("00"), "%S", tick_time);
  text_layer_set_text(s_time_layer, buffer);
  text_layer_set_text(s_eve_layer, evebuffer);
  text_layer_set_text(s_date_layer, datebuffer);
  text_layer_set_text(s_second_layer, secondbuffer);
  
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
  update_time();
  //get weather update every 30 mnutes
  if(tick_time->tm_min%30==0){
    //begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    
    //add a key-value pair
    dict_write_uint8(iter, 0, 0);
    
    //send the message
    app_message_outbox_send();
  }
}

static void main_window_load(Window *window){
  //create time text layer
  s_time_layer=text_layer_create(GRect(0,0,144,50));
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);
  //text_layer_set_text(s_time_layer, "00:00");
  
  //create second layer
  s_second_layer=text_layer_create(GRect(120,25,24,25));
  text_layer_set_background_color(s_second_layer, GColorBlack);
  text_layer_set_text_color(s_second_layer, GColorWhite);
  text_layer_set_font(s_second_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_second_layer, GTextAlignmentCenter);
  
  //create am pm layer
  s_ampm_layer=text_layer_create(GRect(120,0,24,25));
  text_layer_set_background_color(s_ampm_layer, GColorBlack);
  text_layer_set_text_color(s_ampm_layer, GColorWhite);
  text_layer_set_font(s_ampm_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_ampm_layer, GTextAlignmentCenter);
  
  //create weather layer
  s_weather_layer=text_layer_create(GRect(0, 120, 144, 25));
  text_layer_set_background_color(s_weather_layer, GColorBlack);
  text_layer_set_text_color(s_weather_layer, GColorWhite);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  text_layer_set_text(s_weather_layer, "Loading...");
  text_layer_set_font(s_weather_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  
  //create battery layer
  s_battery_layer=text_layer_create(GRect(0,145,72,23));
  text_layer_set_background_color(s_battery_layer, GColorBlack);
  text_layer_set_text_color(s_battery_layer, GColorWhite);
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentCenter);
  text_layer_set_text(s_battery_layer, "Reading...");
  battery_handler(battery_state_service_peek());
  text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  
  //create bluetooth layer
  s_bluetooth_layer=text_layer_create(GRect(72,145,72,23));
  text_layer_set_background_color(s_bluetooth_layer, GColorBlack);
  text_layer_set_text_color(s_bluetooth_layer, GColorWhite);
  text_layer_set_text_alignment(s_bluetooth_layer, GTextAlignmentCenter);
  text_layer_set_text(s_bluetooth_layer, "Reading...");
  text_layer_set_font(s_bluetooth_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  bluetooth_handler(bluetooth_connection_service_peek());
  
  //create date layer
  s_date_layer=text_layer_create(GRect(0,50,144,35));
  text_layer_set_background_color(s_date_layer, GColorBlack);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  
  //create eve layer
  s_eve_layer=text_layer_create(GRect(0,85,144,35));
  text_layer_set_background_color(s_eve_layer, GColorBlack);
  text_layer_set_text_color(s_eve_layer, GColorWhite);
  text_layer_set_font(s_eve_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_eve_layer, GTextAlignmentCenter);
  
  //add it as a child layer to the windows layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_battery_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_bluetooth_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_eve_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_second_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_ampm_layer));
}
  
static void main_window_unload(Window *window){
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_weather_layer);
  text_layer_destroy(s_battery_layer);
  text_layer_destroy(s_bluetooth_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_eve_layer);
  text_layer_destroy(s_second_layer);
  text_layer_destroy(s_ampm_layer);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context){
  // Store incoming information
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];
  //read first item
  Tuple *t = dict_read_first(iterator);
  //for all items
  while(t!=NULL){
    //which key was received?
    switch(t->key){
      case KEY_TEMPERATURE:
        snprintf(temperature_buffer, sizeof(temperature_buffer), "%dF", (int)t->value->int32);
        break;
      case KEY_CONDITIONS:
        snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);
        break;
      default:
        APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
        break;
    }
    
    t=dict_read_next(iterator);
  }
  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s, %s", temperature_buffer, conditions_buffer);
  text_layer_set_text(s_weather_layer, weather_layer_buffer);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context){
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message Dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}



static void init(){
 //create main window and assign to pointer
  s_main_window=window_create();
  //set handlers to manage elements inside the window
  window_set_window_handlers(s_main_window, (WindowHandlers){
    .load=main_window_load, 
    .unload=main_window_unload
  });
  
  //show window on watch, with animation
  window_stack_push(s_main_window, true);
  
  update_time();
  
  //register with ticktimerservice, battery and bluetooth
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  battery_state_service_subscribe(battery_handler);
  bluetooth_connection_service_subscribe(bluetooth_handler);
  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit(){
  window_destroy(s_main_window);
}

int main(void){
  init();
  app_event_loop();
  deinit();
}