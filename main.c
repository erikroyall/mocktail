#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/mongoose.h"
#include "lib/jerry-api.h"
#include "lib/jerry-port.h"


struct mg_mgr mgr;
struct mg_connection *c;
struct mt_response_object {
  const char *content;
  int length;
  unsigned int status_code;
} mt_resp;

static void ev_handler(struct mg_connection *c, int ev, void *p) {
  if (ev == MG_EV_HTTP_REQUEST) {
    struct http_message *hm = (struct http_message *) p;

    mg_send_head(c, mt_resp.status_code, mt_resp.length, "Content-Type: 'text/plain'");
    mg_printf(c, "%.*s", mt_resp.length, mt_resp.content);
  }
}

static jerry_value_t handle (const jerry_value_t func_value, // function object
                              const jerry_value_t this_value, // this arg
                              const jerry_value_t *args_p,   // function args
                              const jerry_value_t args_cnt) { // nargs
  if (args_cnt < 3) {
    fprintf(stderr, "Insufficient number of arguments to handle()");
    exit(-1);
  }

  mt_resp.content = args_p[0];
  mt_resp.length = (int) strlen((const char *) args_p[0]);
  mt_resp.status_code = args_p[2];
  c = mg_bind(&mgr, atoi(args_p[1]), ev_handler);
  mg_set_protocol_http_websocket(c);
  return jerry_create_string((const jerry_char_t *) args_p[0]);
}

void create_modules(void) {
  jerry_value_t object = jerry_create_object();
  jerry_value_t prop_name = jerry_create_string((const jerry_char_t *) "handle");
  jerry_value_t func_obj = jerry_create_external_function(handle);
  jerry_set_property(object, prop_name, func_obj);
  jerry_release_value(prop_name);
  jerry_release_value(func_obj);

  jerry_value_t global_object = jerry_get_global_object();

  prop_name = jerry_create_string((const jerry_char_t *) "http");
  jerry_set_property(global_object, prop_name, object);
  jerry_release_value(prop_name);
  jerry_release_value(object);
  jerry_release_value(global_object);
}

int main(void) {
  jerry_init(JERRY_INIT_EMPTY);
  mg_mgr_init(&mgr, NULL);

  const jerry_char_t script[] = "var str = http.handle('Hello world', '8000', '200'); print(str);";
  size_t script_size = strlen((const char*) script);

  jerry_value_t eval_ret = jerry_eval(script, script_size, false);
  jerry_release_value(eval_ret);

  for (;;) {
    mg_mgr_poll(&mgr, 1000);
  }

  mg_mgr_free(&mgr);

  jerry_cleanup();

  return 0;
}
