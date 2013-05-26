#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

enum {
	CMD_KEY = 0x0, // TUPLE_INTEGER
};

enum {
	CMD_UP = 0x01, CMD_DOWN = 0x02, CMD_SELECT = 0x03
};

enum {
	RESULT_KEY = 0x01,
};


static char result[50] = "";

#define MY_UUID {0x1b,0x01,0xa8,0x78,0xc1,0x5b,0x40,0x58,0xac,0x2e,0xe4,0xfd,0x44,0x8e,0x86,0x30}
PBL_APP_INFO(MY_UUID,
		"Remote Camera", "QubeCAD",
		3, 0, /* App version */
		DEFAULT_MENU_ICON,
		APP_INFO_STANDARD_APP);

Window window;
TextLayer timeLayer;
static bool callbacks_registered;
static AppMessageCallbacksNode app_callbacks;

static void app_send_failed(DictionaryIterator* failed, AppMessageResult reason,
		void* context) {
	// TODO: error handling
}

static void app_received_msg(DictionaryIterator* received, void* context) {
	vibes_short_pulse();

	Tuple* result_tuple = dict_find(received, RESULT_KEY);

	if (result_tuple == NULL) {
		// TODO: handle invalid message

	} else {

		strcpy(result, result_tuple->value->cstring);

	}

	text_layer_set_text(&timeLayer, result);

}

void my_in_drp_handler(void *context, AppMessageResult reason) {
	// incoming message dropped
}

static void send_cmd(uint8_t cmd) {
	Tuplet value = TupletInteger(CMD_KEY, cmd);

	DictionaryIterator *iter;
	app_message_out_get(&iter);

	if (iter == NULL)
		return;

	dict_write_tuplet(iter, &value);
	dict_write_end(iter);

	app_message_out_send();
	app_message_out_release();
}

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	(void) recognizer;
	(void) window;

	send_cmd(CMD_UP);


}

void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	(void) recognizer;
	(void) window;

	send_cmd(CMD_DOWN);
}

void select_single_click_handler(ClickRecognizerRef recognizer,
		Window *window) {
	(void) recognizer;
	(void) window;

	send_cmd(CMD_SELECT);
}

void click_config_provider(ClickConfig **config, Window *window) {
	(void) window;

	config[BUTTON_ID_UP]->click.handler =
			(ClickHandler) up_single_click_handler;
	config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;

	config[BUTTON_ID_DOWN]->click.handler =
			(ClickHandler) down_single_click_handler;
	config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;

	config[BUTTON_ID_SELECT]->click.handler =
			(ClickHandler) select_single_click_handler;
	config[BUTTON_ID_SELECT]->click.repeat_interval_ms = 100;

}

void handle_init(AppContextRef ctx) {
	(void) ctx;

	window_init(&window, "Remote Camera");
	window_stack_push(&window, true /* Animated */);
	window_set_background_color(&window, GColorBlack);

	text_layer_init(&timeLayer,
			GRect(15, 15, 144 - 4 /* width */, 168 - 8 /* height */));
	text_layer_set_text_color(&timeLayer, GColorWhite);
	text_layer_set_background_color(&timeLayer, GColorClear);
	text_layer_set_font(&timeLayer,
			fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));



	layer_add_child(&window.layer, &timeLayer.layer);

	window_set_click_config_provider(&window,
			(ClickConfigProvider) click_config_provider);
}

void pbl_main(void *params) {
	PebbleAppHandlers
	handlers = {
		.init_handler = &handle_init,
		
		.messaging_info = {
			.buffer_sizes = {
				.inbound = 256,
				.outbound = 256,
			},
			.default_callbacks.callbacks = {
				//.out_sent = app_received_msg,
				//.out_failed = app_send_failed,
				.in_received = app_received_msg,
				.in_dropped =my_in_drp_handler,

			},
		},
	};
	app_event_loop(params, &handlers);
}
