#ifndef TUNER_H
#define TUNER_H

#include <inttypes.h>

#ifdef _TEA5767
#include "tea5767.h"
#endif
#ifdef _RDA580X
#include "rda580x.h"
#endif
#ifdef _TUX032
#include "tux032.h"
#endif
#ifdef _LM7001
#include "lm7001.h"
#endif
#ifdef _LC72131
#include "lc72131.h"
#endif
#ifdef _SI470X
#include "si470x.h"
#endif
#ifdef _RDS
#include "rds.h"
#endif

typedef enum {
    TUNER_NO = 0,
    TUNER_TEA5767,
    TUNER_RDA5807,
    TUNER_TUX032,
    TUNER_LM7001,
    TUNER_RDA5802,
    TUNER_SI470X,
    TUNER_LC72131,

    TUNER_END
} tunerIC;

typedef struct {
    // EEPROM part
    tunerIC ic;
    uint8_t ctrl;
    uint8_t step1;
    uint8_t step2;
    uint16_t fMin;
    uint16_t fMax;
    uint16_t freq;
    uint8_t mono;
    uint8_t rds;
    int8_t volume;
    uint8_t bass;
    // LIVE part
    uint16_t rdFreq;
    uint8_t mute;
} Tuner_type;

extern Tuner_type tuner;

#define TUNER_DFREQ     0x01    // (1) Enable Direct Freq input (RDA5807)
#define TUNER_SNC       0x02    // (1) Enable Stereo Noise Cancelling (TEA5767)
#define TUNER_HCC       0x04    // (1) Enable High Cut Control enabled (TEA5767)
#define TUNER_SMUTE     0x08    // (1) Enable Soft Mute (TEA5767) (RDA580x) (SI470X, inverted)
#define TUNER_XTAL      0x10    // (1) Use 32K oscillator (TEA5767)
#define TUNER_BL        0x20    // (1) Japanese or (0) US/Europe band limit (TEA5767)
#define TUNER_DE        0x40    // (1) 75us or (0) 50us de-emphasis
#define TUNER_PLLREF    0x80    // (1) 6.5MHz PLL ref

// Select maximumum read buffer size
#if defined(_RDA580X)
#define TUNER_RDBUF_SIZE    RDA5807_RDBUF_SIZE  // 12
#elif defined(_SI470X)
#define TUNER_RDBUF_SIZE    SI470X_RDBUF_SIZE   // 12
#elif defined(_TEA5767)
#define TUNER_RDBUF_SIZE    TEA5767_RDBUF_SIZE  // 5
#elif defined(_TUX032)
#define TUNER_RDBUF_SIZE    TUX032_RDBUF_SIZE   // 4
#endif

// Select maximumum write buffer size
#if defined(_RDA580X)
#define TUNER_WRBUF_SIZE    RDA5807_WRBUF_SIZE  // 14
#elif defined(_SI470X)
#define TUNER_WRBUF_SIZE    SI470X_WRBUF_SIZE   // 12
#elif defined(_TUX032)
#define TUNER_WRBUF_SIZE    TUX032_WRBUF_SIZE   // 9
#elif defined(_TEA5767)
#define TUNER_WRBUF_SIZE    TEA5767_WRBUF_SIZE  // 5
#endif

#if defined(_RDA580X) || defined(_TEA5767) || defined(_TUX032) || defined(_SI470X)
extern uint8_t tunerRdBuf[];
extern uint8_t tunerWrBuf[];
#endif

#define FM_BAND_DIV_FREQ    7600

#define FM_COUNT            62
#define FM_FAV_COUNT        10

#define SEARCH_UP           1
#define SEARCH_DOWN         -1

void tunerInit();

void tunerSetFreq();

void tunerChangeFreq(int8_t mult);

void tunerReadStatus();
void tunerSetMono(uint8_t value);
#ifdef _RDS
void tunerSetRDS(uint8_t value);
#endif
uint8_t tunerStereo();
uint8_t tunerLevel();

uint8_t tunerStationNum();
void tunerNextStation(int8_t direction);
void tunerLoadStation(uint8_t num);
void tunerStoreStation();

uint8_t tunerFavStationNum();
void tunerLoadFavStation(uint8_t num);
void tunerStoreFavStation(uint8_t num);

void tunerSetVolume(int8_t value);
void tunerSetMute(uint8_t value);
void tunerSetBass(uint8_t value);

void tunerPowerOn();
void tunerPowerOff();

void tunerSeek(int8_t direction);

#endif // TUNER_H
