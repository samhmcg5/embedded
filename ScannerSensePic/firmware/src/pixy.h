#ifndef PIXY_H
#define	PIXY_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "system_config.h"
#include "system_definitions.h"
#include "driver/usart/drv_usart.h"

#define PIXY_BLOCKSIZE              14
#define PIXY_START_WORD             0xaa55
#define PIXY_START_WORD_CC          0xaa56
#define PIXY_START_WORDX            0x55aa    

uint8_t pixyBuffer[PIXY_BLOCKSIZE];
unsigned int pixyBufferIdx = 0;
uint8_t prevData;
bool syncFlag = false;

uint8_t getByte();

void getBlock();

#endif	/* PIXY_H */

