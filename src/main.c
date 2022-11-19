
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

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include <stdio.h>
#include "esp_camera.h"

//Defines constants
#define LED 33  // internal Led
#define BOARD_ESP32CAM_AITHINKER   // board
#define MOUNT_POINT "/sdcard"

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
     .frame_size = FRAMESIZE_UXGA, //FRAMESIZE_QVGA,    //QQVGA-UXGA Do not use sizes above QVGA when not JPEG

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
        printf("Camera init fail, init_camera");
         ESP_LOGE(TAG, "Camera Init Failed");
         return err;
     }

     return ESP_OK;
 }


 void morse_Code( int blink, int time){   // blink, number of times the led blinks; time, how fast the led blinks
    int i;
    if (blink == 1){
        gpio_set_level(LED, 0);
        vTaskDelay(time/portTICK_RATE_MS);
        gpio_set_level(LED, 1);
        vTaskDelay(time/portTICK_RATE_MS);
    }
    if (blink == 2){
        for (i=0;i<2; i++){
            gpio_set_level(LED, 0);
            vTaskDelay(time/portTICK_RATE_MS);
            gpio_set_level(LED, 1);
            vTaskDelay(time/portTICK_RATE_MS);
        }
    }
    if (blink == 3){
        for (i=0;i<3 ;i++){
            gpio_set_level(LED, 0);
            vTaskDelay(time/portTICK_RATE_MS);
            gpio_set_level(LED, 1);
            vTaskDelay(time/portTICK_RATE_MS);
        }
    }
    if (blink == 4){
        for (i=0;i<4 ;i++){
            gpio_set_level(LED, 0);
            vTaskDelay(time/portTICK_RATE_MS);
            gpio_set_level(LED, 1);
            vTaskDelay(time/portTICK_RATE_MS);
        }
    }
    if (blink == 5){
        for (i=0;i<5 ;i++){
            gpio_set_level(LED, 0);
            vTaskDelay(time/portTICK_RATE_MS);
            gpio_set_level(LED, 1);
            vTaskDelay(time/portTICK_RATE_MS);
        }
    }
    if (blink == 6){
        for (i=0;i<6 ;i++){
            gpio_set_level(LED, 0);
            vTaskDelay(time/portTICK_RATE_MS);
            gpio_set_level(LED, 1);
            vTaskDelay(time/portTICK_RATE_MS);
        }
    }
    if (blink == 7){
        for (i=0;i<7 ;i++){
            gpio_set_level(LED, 0);
            vTaskDelay(time/portTICK_RATE_MS);
            gpio_set_level(LED, 1);
            vTaskDelay(time/portTICK_RATE_MS);
        }
    }
    if (blink == 8){
        for (i=0;i<8 ;i++){
            gpio_set_level(LED, 0);
            vTaskDelay(time/portTICK_RATE_MS);
            gpio_set_level(LED, 1);
            vTaskDelay(time/portTICK_RATE_MS);
        }
    }
    if (blink == 9){
        for (i=0;i<9 ;i++){
            gpio_set_level(LED, 0);
            vTaskDelay(time/portTICK_RATE_MS);
            gpio_set_level(LED, 1);
            vTaskDelay(time/portTICK_RATE_MS);
        }
    }
    vTaskDelay(2000/portTICK_RATE_MS);
    return;
 }

void Log(){
       // const char *file_foo = MOUNT_POINT"/foo.txt";

    // // Check if destination file exists before renaming
    // struct stat st;
    // if (stat(file_foo, &st) == 0) {
    //     // Delete it if it exists
    //     unlink(file_foo);
    // }

    // // Rename original file
    // ESP_LOGI(TAG, "Renaming file %s to %s", file_hello, file_foo);
    // if (rename(file_hello, file_foo) != 0) {
    //     ESP_LOGE(TAG, "Rename failed");
    //     return;
    // }

    // // Open renamed file for reading
    // ESP_LOGI(TAG, "Reading file %s", file_foo);
    // f = fopen(file_foo, "r");
    // if (f == NULL) {
    //     ESP_LOGE(TAG, "Failed to open file for reading");
    //     return;
    // }

    // // Read a line from file
    // char line[64];
    // fgets(line, sizeof(line), f);
    // fclose(f);

    // // Strip newline
    // char *pos = strchr(line, '\n');
    // if (pos) {
    //     *pos = '\0';
    // }
    // ESP_LOGI(TAG, "Read from file: '%s'", line);
    
    ///    make a Log file function
    // for(int a =0;a<5;){
    //     morse_Code(3,100);
    //     fprintf(f,"Bounjour Messie ----->  %i",a);
    //     for(int i=0;i<5;){
    //         fprintf(f, "Camera Working %s!\n", card->cid.name);
    //         i++;
    //     }
    //     a++;
    // }
    
}

void app_main(void)
{
    esp_err_t ret;
    esp_err_t i_cam;
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);         // red led

    ///// INIT CAMERA HERE  ////////
    i_cam = init_camera();
    if(ESP_OK != i_cam) {
        // blink twice for unable to initialized camera
        printf("Camera init not OK");
        //morse_Code(2,100);
        ESP_LOGI(TAG, "Unable to initialize the camera");
         return;
     }
    if (ESP_OK == i_cam){
        // blink twice longer for camera initialized
        //morse_Code(2,400);
        ESP_LOGI(TAG,"Camera working");
    }

    ////////  INIT CARD HERE   ////////////
    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
#ifdef CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif // EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    sdmmc_card_t *card;
    const char mount_point[] = MOUNT_POINT;
    ESP_LOGI(TAG, "Initializing SD card");

    // Use settings defined above to initialize SD card and mount FAT filesystem.
    // Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
    // Please check its source code and implement error recovery when developing
    // production applications.

    ESP_LOGI(TAG, "Using SDMMC peripheral");
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    // Set bus width to use:
#ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
    slot_config.width = 4;
#else
    slot_config.width = 1;
#endif

    // On chips where the GPIOs used for SD card can be configured, set them in
    // the slot_config structure:
#ifdef CONFIG_SOC_SDMMC_USE_GPIO_MATRIX
    slot_config.clk = CONFIG_EXAMPLE_PIN_CLK;
    slot_config.cmd = CONFIG_EXAMPLE_PIN_CMD;
    slot_config.d0 = CONFIG_EXAMPLE_PIN_D0;
#ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
    slot_config.d1 = CONFIG_EXAMPLE_PIN_D1;
    slot_config.d2 = CONFIG_EXAMPLE_PIN_D2;
    slot_config.d3 = CONFIG_EXAMPLE_PIN_D3;
#endif  // CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
#endif  // CONFIG_SOC_SDMMC_USE_GPIO_MATRIX

    // Enable internal pullups on enabled pins. The internal pullups
    // are insufficient however, please make sure 10k external pullups are
    // connected on the bus. This is for debug / example purpose only.
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                     "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return;
    }
    ESP_LOGI(TAG, "Filesystem mounted");

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);

    // First create a file.
    char name_f[250];
    int num = 0;
    while (true){    // loop here
        char *name_files = name_f;
        sprintf(name_files, MOUNT_POINT"/image_%d.jpg", num);
        num++;

        // check if file exist
        if (access(name_files,F_OK) == 0){
            continue;
        };
        
        const char *file = name_files; 
        FILE *f = fopen(file, "wx");  // use wx with fwrite
        if (f == NULL) {
            morse_Code(4,100);
            ESP_LOGE(TAG, "Failed to open file for writing");
            esp_vfs_fat_sdcard_unmount(mount_point, card);  // unmounting for safety
            return;
        } 
        if (f != NULL){
            ESP_LOGI(TAG, "Taking picture...");
            camera_fb_t *pic = esp_camera_fb_get();  // this takes the picture
            if (!pic){
                // blink 3 times for error camera failed to take picture
                morse_Code(2,100);
                ESP_LOGE(TAG, "Camera failed to take picture");
            } else{
                // use pic->buf to access the image
                // create the picture file
                fwrite(pic->buf, 1, pic->len, f);
                // all of the follow works for saving the file, but for simplicity i use fwrite
                //fwrite( (char*)pic->buf, 1,pic->len,f );
                //fwrite(pic->buf, pic->len, 1, f);
                //fwrite((char*)pic->buf, pic->len, 1, f );  
                //fprintf(f, (char*)pic->buf);   
                //blink once for taking picture
                morse_Code(1,200);
                esp_camera_fb_return(pic); 
            }

        }

        fclose(f);
        ESP_LOGI(TAG, "File written");
        vTaskDelay(2000/portTICK_RATE_MS);  //  total delay from morse_code and this one apro 4-5 secs
    }
 
    // All done, unmount partition and disable SDMMC peripheral
    esp_vfs_fat_sdcard_unmount(mount_point, card);
    ESP_LOGI(TAG, "Card unmounted");
}
