// server file
#include "wifi_server.h"
#include <stdio.h>

static int s_retry_num = 0;

esp_err_t stream_handler(esp_http_client_event_t *stream_event){
    int tls_err = 0;
    switch (stream_event->event_id)
    {
    case HTTP_EVENT_ERROR:
        Log("http_error");
        esp_err_t err = esp_tls_get_and_clear_last_error( (esp_tls_error_handle_t)stream_event->data, &tls_err, NULL);
        if (err!=0){
            char evt_err[60];
            sprintf(evt_err ,"Http_event_error, 0x%X",err);
            Log(evt_err);
        }
        break;
    case HTTP_EVENT_ON_CONNECTED:
        Log("http_event_on_connected");
        break;
    case HTTP_EVENT_DISCONNECTED:
        Log("http_event_disconected");
        esp_err_t err2 = esp_tls_get_and_clear_last_error( (esp_tls_error_handle_t)stream_event->data, &tls_err, NULL);
        if (err2!=0){
            char evt_err2[60];
            sprintf(evt_err2 ,"last esp error code, 0x%X",err2);
            Log(evt_err2);
        }
        break;
    case HTTP_EVENT_HEADER_SENT:
        Log("http_event_header_sent");
        break;
    case HTTP_EVENT_ON_HEADER:
        Log("http_event_on_header");
        break;
    case HTTP_EVENT_ON_DATA:
        Log("http_event_on_data");
        break;
    case HTTP_EVENT_ON_FINISH:
        Log("http_event_on_finish");
        break;
    default:
        Log("Something went wrong");
    }
    return ESP_OK;
}

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            Log("Retry to connect to AP");
            //ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        //ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        char log_msa[100];
        char got_ip[10] = "Got ip: ";

        sprintf(log_msa, got_ip,IPSTR, IP2STR(&event->ip_info.ip) );
        Log(log_msa);
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

bool start_wifi(void){   
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK){ //check if theres an error
        const char *string_err = esp_err_to_name(ret);
        Log( string_err);
    }

    s_wifi_event_group = xEventGroupCreate();

    esp_err_t wifi_err;
    //tcpip_adapter_init();
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
    esp_err_t netif_err;
    netif_err = esp_netif_init(); 
    if( netif_err != ESP_OK ){   //Initialization and deinitialization of underlying TCP/IP stack and esp-netif instances
        const char *string_err = esp_err_to_name(netif_err);
        char err[70];
        sprintf(err,"Netif init error: %s", string_err);
        Log(err);
        return false;
    } else {
        esp_event_loop_create_default();
        esp_netif_create_default_wifi_sta();
        wifi_err = esp_wifi_init(&config_wifi);

        // morse_Code(1,1000,false);
        morse_Code(1,200,false);

        if (wifi_err == ESP_OK){
            Log("wifi function");
            esp_wifi_set_storage(WIFI_STORAGE_RAM);
            esp_event_handler_instance_t instance_any_id;
            esp_event_handler_instance_t instance_got_ip;
            esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler,
                                                        NULL, &instance_any_id);
            esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler,
                                                        NULL, &instance_got_ip);

            esp_wifi_set_mode(WIFI_MODE_STA);   // set station mode
            // CHECK ESP_IF_WIFI_STA
            esp_wifi_set_config(WIFI_IF_STA, &wifi_config);  // set your own ssid and password from wifi_server.h
            esp_wifi_start();
        
            /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
             * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
            EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                    WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE,
                    pdFALSE, portMAX_DELAY);

            /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
            * happened. */
            if (bits & WIFI_CONNECTED_BIT) {
                Log("Wifi connected");
                return true;
            } else if (bits & WIFI_FAIL_BIT) {
                Log("Failed to connect");
                return false;
            } else {
                Log("Unexpected Event");
            }
        }else{
    //morse_Code(7,200,false);
            const char *string_err = esp_err_to_name(wifi_err);  // esp_err to char
            char wifierr[50];
            sprintf(wifierr,"wifi init error: %s", string_err);
            Log(string_err);
            //free((char*)string_err); 
    }
    
   }
   return false;
}

esp_http_client_handle_t conection(){
    esp_http_client_config_t config = {
        .url = "http://192.168.1.208:7500",
    };
    esp_http_client_handle_t cliente = esp_http_client_init(&config);
    esp_err_t error = esp_http_client_open(cliente,0);
    if (error != ESP_OK){
        Log("Unable to establish connection");
        esp_http_client_cleanup(cliente);
        //exit(1);
    }
    return cliente;
}

void close_conection(esp_http_client_handle_t cliente){
    esp_http_client_cleanup(cliente);
}


void stream_EOL(camera_fb_t *fb_camera, esp_http_client_handle_t cliente){

    Log("stream con salto de linea");
    esp_http_client_write(cliente, (const char*)fb_camera->buf,fb_camera->len);
    Log("sending data");
    vTaskDelay(500/portTICK_PERIOD_MS);    
    esp_camera_fb_return(fb_camera);
    
}

// deprecate, unable to decode msg coming from server, utf-8
void stream(camera_fb_t *fb_camera, esp_http_client_handle_t cliente){

    Log("inside stream");
    esp_http_client_write(cliente, (const char*)fb_camera->buf,fb_camera->len);
    char confir[7];
    char send_ok[7] = "ESP_OK";
    char send_error[10] = "ESP_ERROR";
    char found[10];
    int i = 0;
    // check encoding type in here, set it to utf-8
    esp_http_client_read(cliente, confir, sizeof(confir));
    char *decoded = (char *) confir;

    while(*decoded){
        int code_point = decode_utf8(&decoded);
        found[i] = code_point;
    }

    // char *decoded = malloc(sizeof(confir));
    // memcpy(decoded, confir,sizeof(confir));
    // decoded[sizeof(confir)] = '\0';

    char *check = strstr(decoded, send_ok); //check if theres confirmation, return null if wont contain
     if (check !=NULL ){
    //     //Log("recieved");
    //     size_t send_size= strlen(send_ok);
    //     char found[send_size +1];
    //     strncpy(found, check, send_size);
        Log(found);
        esp_http_client_write(cliente,send_ok,sizeof(send_ok));
    } else {
        Log("error on confirmation");
        Log(found);
        esp_http_client_write(cliente, send_error,sizeof(send_error));
    }

    esp_camera_fb_return(fb_camera);
    
}
    
int decode_utf8(char **s){  // decoding func from utf8 to unicode, not tested properly
    int k = **s ? __builtin_clz(~(**s << 24)) : 0;   // count number of leading 1 bits
    int mask = (1 << (8 - k)) - 1;        // all 1's with k leading 0
    int value = **s & mask;
    for (++(*s), --k; k >0 && **s; --k, ++(*s)) {  // note that k = #total bytes or 0 
        value <<= 6;
        value += (**s & 0x3F);
    }
    return value;
}
