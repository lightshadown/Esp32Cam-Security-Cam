#include "esp_http_server.h"
#include "esp_camera.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
//#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "otros.h"

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
#define MAXIMUM_RETRY      5
#define PASSWORD "YOURPASSWORD"
#define SSID "YOUROWNSSID"

//static const char *PASSWORD = "3WTc7PVPV9";  // set your own password and ssid
//static const char *SSID = "INFINITUM5533_5";
static EventGroupHandle_t s_wifi_event_group;  /* FreeRTOS event group to signal when we are connected*/


//esp_err_t get_handler(httpd_req_t *reqt);
//esp_err_t set_handler(httpd_req_t *reqt);
//httpd_handle_t start_server(void);
//void kill_server(httpd_handle_t kill_it);

//static void event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data);
bool start_wifi(void);
