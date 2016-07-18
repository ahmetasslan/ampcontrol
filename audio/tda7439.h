#ifndef TDA7439_H
#define TDA7439_H

#include <inttypes.h>
#include "audio.h"

#define TDA7439_ADDR			0b10001000

#define CHAN_CNT				4

/* TDA7439 I2C function selection */
#define TDA7439_INPUT_SELECT	0x00
#define TDA7439_INPUT_GAIN		0x01
#define TDA7439_PREAMP			0x02
#define TDA7439_BASS			0x03
#define TDA7439_MIDDLE			0x04
#define TDA7439_TREBLE			0x05
#define TDA7439_VOLUME_RIGHT	0x06
#define TDA7439_VOLUME_LEFT		0x07
/* I2c autoincrement flag */
#define TDA7439_AUTO_INC		0x10

#define MUTE_ON					1
#define MUTE_OFF				0

void sndInit(void);

uint8_t sndInputCnt(void);

void sndSetInput(uint8_t ch);
uint8_t sndGetInput(void);

void switchMute(void);
void muteVolume(void);
void unmuteVolume(void);
void sndSetMute(uint8_t value);
uint8_t sndGetMute(void);

void sndSetLoudness(uint8_t value);
uint8_t sndGetLoudness(void);

void sndNextParam(uint8_t *mode);
void sndChangeParam(uint8_t mode, int8_t diff);

void sndPowerOn(void);
void sndPowerOff(void);

#endif /* TDA7439_H */
