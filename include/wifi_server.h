#include "esp_http_server.h"
#include "esp_camera.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
//#include "tcpip_adapter.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "otros.h"
#include "esp_http_client.h"
#include "lwip/inet.h"
#include "esp_tls.h"
#include "esp_heap_caps.h"

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
#define MAXIMUM_RETRY      5
#define PASSWORD "3WTc7PVPV9"
#define SSID "INFINITUM5533_2.4"
#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048

//static const char *PASSWORD = "3WTc7PVPV9";  // set your own password and ssid
//static const char *SSID = "INFINITUM5533_5";
static EventGroupHandle_t s_wifi_event_group;  /* FreeRTOS event group to signal when we are connected*/


//esp_err_t get_handler(httpd_req_t *reqt);
//esp_err_t set_handler(httpd_req_t *reqt);
//httpd_handle_t wifi_server(void);
//void kill_server(httpd_handle_t kill_it);
esp_err_t stream_handler(esp_http_client_event_t *);
esp_http_client_handle_t conection();
void close_conection(esp_http_client_handle_t);
void stream(camera_fb_t *, esp_http_client_handle_t);
void stream_EOL(camera_fb_t *, esp_http_client_handle_t);
int decode_utf8(char **);
bool start_wifi(void);
