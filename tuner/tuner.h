#ifndef TUNER_H
#define TUNER_H

#include <inttypes.h>

/* Tuner type selection */
#if !defined(TEA5767) && !defined(TUX032) && !defined(LM7001) && !defined(RDA5807)
#define TEA5767
#endif

#if defined(TEA5767)
#include "tea5767.h"
#elif defined(TUX032)
#include "tux032.h"
#elif defined(LM7001)
#include "lm7001.h"
#elif defined(RDA5807)
#include "rda5807.h"
#endif

#define FM_COUNT		50

#define SEARCH_DOWN		0
#define SEARCH_UP		1

#define FM_MONO			1
#define FM_STEREO		0

#define FM_FREQ_MIN		8750
#define FM_FREQ_MAX		10800

void tunerInit();
void tunerSetFreq(uint16_t freq);

void tunerReadStatus();
uint16_t tunerGetFreq();
void tunerSwitchMono();
uint8_t tunerStereo();
uint8_t tunerLevel();

uint8_t stationNum(uint16_t freq);
void scanStoredFreq(uint8_t direction);
void loadStation(uint8_t num);
void storeStation(void);

void loadTunerParams(void);
void setTunerParams(void);
void saveTunerParams(void);

#endif /* TUNER_H */
