#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <Arduino.h>
#include <TFT_eSPI.h>

#include <SPI.h>
#include <SD.h>
#include <FS.h>

void initDisplay();
void printDirectory(File dir, int numTabs);

#endif