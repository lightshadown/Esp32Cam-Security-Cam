#include "esp_camera.h"
#include "esp_vfs_fat.h"    
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "esp_log.h"
#include <unistd.h>
#include <string.h>

#define LED 33  // internal Led
#define BOARD_ESP32CAM_AITHINKER   // board
#define MOUNT_POINT "/sdcard"
#define F_OK 0
#define	R_OK 4
#define	W_OK 2
#define	X_OK 1

void morse_Code( int blink, int time, bool Lag);
void Log(const char *info);
sdmmc_card_t * Open_Card();
//camera_fb_t * rotate_frame(camera_fb_t *frame);  // deprecated