// server file
#include "start_server.h"
#include <stdio.h>

static const char *TAG_wifi = "Wifi_error";
/*
esp_err_t get_handler(httpd_req_t *reqt){   // get
    const char respt[] = "URL GET ";
    esp_err_t error_code;
    error_code = httpd_resp_send(reqt, respt, HTTPD_RESP_USE_STRLEN);
    return error_code;
}
esp_err_t set_handler(httpd_req_t *reqt){   // post
    // check first if conecction is close or timeout
    char content[100];
    // truncate if recieved is grater than buffer
    size_t tam_recv = MIN(reqt->content_len, sizeof(content));
    int recv = httpd_req_recv(reqt, content, tam_recv);
    if(recv <= 0){ // 0 is conecction close
        if (recv == HTTPD_SOCK_ERR_TIMEOUT){
            httpd_resp_send_408(reqt); // request timeout
        }
        return ESP_FAIL;
    }

    const char respt[] = "Post sending";
    esp_err_t error_code;
    error_code = httpd_resp_send(reqt, respt, HTTPD_RESP_USE_STRLEN);
    return error_code;
}

// URL structure for GET/POST
httpd_uri_t uri_post = {
    .uri = "/uri",
    .method = HTTP_POST,
    .handler = set_handler,
    .user_ctx = NULL
};
httpd_uri_t uri_get = {
    .uri = "/uri",
    .method = HTTP_GET,
    .handler = get_handler,
    .user_ctx = NULL
};

httpd_handle_t start_server(void){    // start the server
    httpd_config_t configurar = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &configurar) == ESP_OK){
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_post);
    }
    return server;
}

void kill_server(httpd_handle_t kill_it){  // stop the server
    httpd_stop(kill_it);
}
*/

static int s_retry_num = 0;

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            //ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        //ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        //ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        //ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        char log_msa[100];
        sprintf(log_msa, "Got ip: %s", SSID);
        Log(log_msa);
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

bool start_wifi(void){   // check to initialize nvs  ESP_ERR_NVS_NOT_INITIALIZED

    s_wifi_event_group = xEventGroupCreate();

    esp_err_t wifi_err;
    wifi_init_config_t config_wifi = WIFI_INIT_CONFIG_DEFAULT();
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = SSID,
            .password = PASSWORD,
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
	     .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    if( esp_netif_init() != ESP_OK ){   //Initialization and deinitialization of underlying TCP/IP stack and esp-netif instances
        //Log((char*)"Error netif init");
        return false;
    } else {
            
        esp_event_loop_create_default();
        esp_netif_create_default_wifi_sta();
        wifi_err = esp_wifi_init(&config_wifi);

        // morse_Code(1,1000,false);
        morse_Code(1,200,false);

        if (wifi_err == ESP_OK){
            morse_Code(3,500,false);
            esp_event_handler_instance_t instance_any_id;
            esp_event_handler_instance_t instance_got_ip;
            esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler,
                                                        NULL, &instance_any_id);
            esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler,
                                                        NULL, &instance_got_ip);

            esp_wifi_set_mode(WIFI_MODE_STA);   // set station mode
            esp_wifi_set_config(WIFI_IF_STA, &wifi_config);  // set your own ssid and password from start_server.h
            esp_wifi_start();
        
            /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
             * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
            EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                    WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE,
                    pdFALSE, portMAX_DELAY);

            /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
            * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        //char* log_msa;
        //sprintf(log_msa, "Got ip: ", SSID);
        //Log(log_msa);
        ESP_LOGI(TAG_wifi, "connected to ap SSID:%s password:%s",
                 SSID, PASSWORD);
        Log("Wifi connected");
        return true;
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG_wifi, "Failed to connect to SSID:%s, password:%s",
                 SSID, PASSWORD);
        Log("Failed to connect");
        return false;
    } else {
        ESP_LOGE(TAG_wifi, "UNEXPECTED EVENT");
        
    }
        }
    morse_Code(7,200,false);
    const char *string_err = esp_err_to_name(wifi_err);
    Log(string_err);
    
   }
   return false;
}
