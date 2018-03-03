#include "nikitin.h"

#include <avr/pgmspace.h>
#include "audio.h"
#include "../spisw.h"
#include "../pins.h"

static void nikitinSendGainLevels(uint8_t right, uint8_t left)
{
    // TODO: Send right and left levels via SPI
}

void nikitinInit()
{
    SPIswInit(SPISW_DORD_MSB_FIRST);    // or SPISW_DORD_LSB_FIRST

    SET(SPISW_CE);                      // or CLR(SPISW_CE)
}

void nikitinSetSpeakers()
{
    int8_t spLeft = sndPar[MODE_SND_VOLUME].value;
    int8_t spRight = sndPar[MODE_SND_VOLUME].value;
    int8_t volMin = pgm_read_byte(&sndPar[MODE_SND_VOLUME].grid->min);

    if (sndPar[MODE_SND_BALANCE].value > 0) {
        spLeft -= sndPar[MODE_SND_BALANCE].value;
        if (spLeft < volMin)
            spLeft = volMin;
    } else {
        spRight += sndPar[MODE_SND_BALANCE].value;
        if (spRight < volMin)
            spRight = volMin;
    }

    nikitinSendGainLevels(spRight, spLeft);
}

void nikitinSetMute()
{

}
