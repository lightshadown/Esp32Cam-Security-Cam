
// // https://github.com/espressif/esp32-camera   // esp32 repo must see for camera

// /**
//  * follow how-to inside https://github.com/espressif/esp32-camera
//  * 
//  *
//  * This example takes a picture every 5s and save the image into the sd card
//  */

// enable psram in the menu config for the camera to work
// set the long filename buffer in heap to yes, so you can use long filenames,  FATFS_LFN_STACK
//  pixel_format= set the compresion type of the jpg file
//  frame_size = set the resolution of the image taken

// This example uses SDMMC peripheral to communicate with SD card.

// takes photos and broadcast it via wifi, need to conecct to RPi to analice the image
// check if raw image can be analize using jplib
// make a log file that save on the sd card
// switch from photos to video and broadcast it via wifi

// DOWNLOAD esp32-cam repo from github and save it inside components folder, REQUIRED
// enable psram in the menu config for the camera to work,    ESP32_SPIRAM_SUPPORT=y
// set flash frequency to 80 Mhz, default is 40,              ESPTOOLPY_FLASHFREQ_80M=y
// set flash size to 4mb, default is 2 mb  CONFIG_ESPTOOLPY_FLASHSIZE_4MB  esptoolpy_flashsize_4mb
// set the long filename buffer in heap to yes, so you can use long filenames,  FATFS_LFN_STACK
//  set logging time from system   Log_timestamp_source_system
//  pixel_format= set the compresion type of the jpg file
//  frame_size = set the resolution of the image taken

// check https://www.survivingwithandroid.com/esp32-cam-platformio-video-streaming-face-recognition/
// check cameraWebServer example from https://github.com/espressif/arduino-esp32


#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <stdio.h>

#include "wifi_server.h"   // main server stuff
#include "otros.h"

//Defines constants
#ifdef BOARD_ESP32CAM_AITHINKER

 #define CAM_PIN_PWDN 32
 #define CAM_PIN_RESET -1 //software reset will be performed
 #define CAM_PIN_XCLK 0
 #define CAM_PIN_SIOD 26
 #define CAM_PIN_SIOC 27

 #define CAM_PIN_D7 35
 #define CAM_PIN_D6 34
 #define CAM_PIN_D5 39
 #define CAM_PIN_D4 36
 #define CAM_PIN_D3 21
 #define CAM_PIN_D2 19
 #define CAM_PIN_D1 18
 #define CAM_PIN_D0 5
 #define CAM_PIN_VSYNC 25
 #define CAM_PIN_HREF 23
 #define CAM_PIN_PCLK 22

 #endif

 static const char *TAG = "security_camera_farm";

 static camera_config_t camera_config = {
     .pin_pwdn = CAM_PIN_PWDN,
     .pin_reset = CAM_PIN_RESET,
     .pin_xclk = CAM_PIN_XCLK,
     .pin_sscb_sda = CAM_PIN_SIOD,
     .pin_sscb_scl = CAM_PIN_SIOC,

     .pin_d7 = CAM_PIN_D7,
     .pin_d6 = CAM_PIN_D6,
     .pin_d5 = CAM_PIN_D5,
     .pin_d4 = CAM_PIN_D4,
     .pin_d3 = CAM_PIN_D3,
     .pin_d2 = CAM_PIN_D2,
     .pin_d1 = CAM_PIN_D1,
     .pin_d0 = CAM_PIN_D0,
     .pin_vsync = CAM_PIN_VSYNC,
     .pin_href = CAM_PIN_HREF,
     .pin_pclk = CAM_PIN_PCLK,

     //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
     .xclk_freq_hz = 20000000,
     .ledc_timer = LEDC_TIMER_0,
     .ledc_channel = LEDC_CHANNEL_0,

     // check sensor.h for pixel formats = pixformat_t or framesize_t
     .pixel_format = PIXFORMAT_JPEG,//PIXFORMAT_RGB565, //YUV422,GRAYSCALE,RGB565,JPEG
     .frame_size = FRAMESIZE_VGA, //FRAMESIZE_UXGA, //FRAMESIZE_QVGA,    //QQVGA-UXGA Do not use sizes above QVGA when not JPEG

     .jpeg_quality = 12, //0-63 lower number means higher quality  // 12 normal
     .fb_count = 1,       //if more than one, i2s runs in continuous mode. Use only with JPEG
     .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
 };

static esp_err_t init_camera()
 {
     //initialize the camera
     esp_err_t err = esp_camera_init(&camera_config);
     if (err != ESP_OK)
     {
        sensor_t * s = esp_camera_sensor_get();
        s->set_vflip(s,1); // flip frame buffer
         ESP_LOGE(TAG, "Camera Init Failed");
         return err;
     }

     return ESP_OK;
 }

int app_main(void){
    //char olo[8] = "program";
    char ulo[50];
    sprintf(ulo, "----------------\nstarting program\nconfig2");
    Log(ulo);
    
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);         // red led

    esp_err_t i_cam = init_camera();

    if (i_cam == ESP_OK){
        //Log("Camera Working");
    }else{    
        char log_msa[70];
        const char *string_err = esp_err_to_name(i_cam);  // esp_err to char
        sprintf(log_msa, "Camera init not Ok: %s", string_err);
        Log(log_msa);
        return 0;
     }

    if (start_wifi()){
        esp_http_client_handle_t est_conn =  conection();
        while (true){
            camera_fb_t *pic = esp_camera_fb_get();
            if (!pic){
                Log("Unable to grab frame");
            } else{
                //Log("about to take frame");
                stream_EOL(pic, est_conn);
                //stream(pic, est_conn);    // start broadcasting stuff
                //Log("Frame taken");
                esp_camera_fb_return(pic);
            }
        }
    } else{
        return 0;
    }
}


// void app_main_oldversion(void)
// {
//     Log("Start of program");
//     esp_err_t i_cam;
//     sdmmc_card_t *card;
//     const char mount_point[] = MOUNT_POINT;
    
//     gpio_set_direction(LED, GPIO_MODE_OUTPUT);         // red led

//     ///// INIT CAMERA HERE  ////////
//     i_cam = init_camera();
//     if(ESP_OK != i_cam) {
//         // blink twice for unable to initialized camera
//         printf("Camera init not OK");
//         //morse_Code(2,100);
//         ESP_LOGI(TAG, "Unable to initialize the camera");
//          return;
//      }
//     if (ESP_OK == i_cam){
//         // blink twice longer for camera initialized
//         //morse_Code(2,400);
//         ESP_LOGI(TAG,"Camera working");
//     }

//     // start
//     //Initialize NVS
//     esp_err_t ret = nvs_flash_init();
//     if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
//       ESP_ERROR_CHECK(nvs_flash_erase());
//       ret = nvs_flash_init();
//     }
//     if (ret != ESP_OK){ //check if theres an error
//         const char *string_err = esp_err_to_name(ret);
//         Log( string_err);
//     }
//     //ESP_ERROR_CHECK(ret);
//     //  WIFI server start here
//     bool wifi = start_wifi(); 
//     if (wifi == true){

//     ////////  INIT CARD HERE   ////////////
//     card = Open_Card();
//     // First create a file.
//     char name_f[250];
//     int num = 0;
//     while (true){    // loop here
//         char *name_files = name_f;
//         sprintf(name_files, MOUNT_POINT"/image_%d.jpg", num);
//         num++;

//         // check if file exist
//         if (access(name_files,F_OK) == 0){
//             continue;
//         };
        
//         const char *file = name_files; 
//         FILE *f = fopen(file, "wx");  // use wx with fwrite
//         if (f == NULL) {
//             morse_Code(4,100, true);
//             Log("Unable to open file for writing");
//             //ESP_LOGE(TAG, "Failed to open file for writing");
//             esp_vfs_fat_sdcard_unmount(mount_point, card);  // unmounting for safety
//             return;
//         } 
//         if (f != NULL){
//             Log("Taking Picture");
//             ESP_LOGI(TAG, "Taking picture...");
//             camera_fb_t *pic = esp_camera_fb_get();  // this takes the picture
//             if (!pic){
//                 // blink 3 times for error camera failed to take picture
//                 morse_Code(2,100, true);
//                 ESP_LOGE(TAG, "Camera failed to take picture");
//             } else{
//                 // use pic->buf to access the image
//                 // use pic->len to the get size of the frame, sizeof wont work
//                 // create the picture file
//                 // frame2jpg(pic);
//                 //camera_fb_t *lscap_FB = rotate_frame(pic);
//                 //fwrite(lscap_FB->buf, 1, lscap_FB->len, f);

//                 // esta correcto NO INTENTAR GIRAR!!!
//                 fwrite(pic->buf, 1, pic->len, f);
                
//                 // all of the follow works for saving the file, but for simplicity i use fwrite
//                 //fwrite( (char*)pic->buf, 1,pic->len,f );
//                 //fwrite(pic->buf, pic->len, 1, f);
//                 //fwrite((char*)pic->buf, pic->len, 1, f );  
//                 //fprintf(f, (char*)pic->buf);   
//                 //blink once for taking picture
//                 morse_Code(1,200, false);
//                 esp_camera_fb_return(pic); 
//             }

//         }

//         fclose(f);
//         ESP_LOGI(TAG, "File written");
//         //vTaskDelay(2000/portTICK_RATE_MS);  //  total delay from morse_code and this one apro 4-5 secs
//     }
 
//     // All done, unmount partition and disable SDMMC peripheral
//     esp_vfs_fat_sdcard_unmount(mount_point, card);
//     ESP_LOGI(TAG, "Card unmounted");
// }
// }

