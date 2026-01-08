#ifndef DISPLAY_H
#define DISPLAY_H

#include <SPI.h>

#include <FS.h>
#include <SD.h>

#include <TFT_eSPI.h>

#define HEIGHT 320
#define WIDTH 480


extern const char* ANIM_FILEPATH;

void initDisplay(bool SD_enable = true);
void rotateColors();


#endif