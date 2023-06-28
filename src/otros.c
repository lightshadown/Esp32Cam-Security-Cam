#include "otros.h"
#include "stdio.h"

 void morse_Code( int blink, int time, bool Lag){   // blink, number of times the led blinks; time, how fast the led blinks
    int i;
    for (i=0;i<blink;){
        gpio_set_level(LED, 0);
        vTaskDelay(time/portTICK_PERIOD_MS); ;
        gpio_set_level(LED, 1);
        vTaskDelay(time/portTICK_PERIOD_MS);
        i++;
    }
    if (Lag == true){
        vTaskDelay(2000/portTICK_PERIOD_MS);
    }
    return;
 }

sdmmc_card_t * Open_Card(){
      ////////  INIT CARD HERE   ////////////
    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    sdmmc_card_t *card;
    esp_err_t ret_error;
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
#ifdef CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif // EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    
    const char mount_point[] = MOUNT_POINT;
    //ESP_LOGI(TAG, "Initializing SD card");

    // Use settings defined above to initialize SD card and mount FAT filesystem.
    // Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
    // Please check its source code and implement error recovery when developing
    // production applications.

    //ESP_LOGI(TAG, "Using SDMMC peripheral");
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

    //ESP_LOGI(TAG, "Mounting filesystem");
    ret_error = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret_error != ESP_OK) {
        if (ret_error == ESP_FAIL) {
            //ESP_LOGE(TAG, "Failed to mount filesystem. "
            //         "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            //ESP_LOGE(TAG, "Failed to initialize the card (%s). "
            //         "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret_error));
        }
        return card;
    }
    //ESP_LOGI(TAG, "Filesystem mounted");

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);
    return card;
}

void Log(const char *info){
    sdmmc_card_t *card;
    FILE *f;
    const char mount_point[] = MOUNT_POINT;
    const char *file_foo = MOUNT_POINT"/Log.txt";

    // open cards
    card = Open_Card();

    // check if file exist, then append to file
    if (access(file_foo,F_OK) == 0){
        f = fopen(file_foo, "a");   //append mode
        //fgets(info,sizeof(info),f);
        //fwrite(&info , 1, sizeof(info), f);
        fprintf(f,"%s\n",info);
        fclose(f);
    }else{
        f = fopen(file_foo, "w");   //creates and write mode
        fprintf(f,"%s\n",info);
        //fwrite(&info, 1,sizeof(info),f);
        fclose(f);
    };
        // All done, unmount partition and disable SDMMC peripheral
    esp_vfs_fat_sdcard_unmount(mount_point, card);
    
}
