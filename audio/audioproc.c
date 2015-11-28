#include "audioproc.h"

#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include "../eeprom.h"
#include "../display.h"
#include "../pins.h"

static const sndGrid grid[] PROGMEM = {
	{  0,  0, 0.00 * 8},	/* 0: Not implemented */
	{-79,  0, 1.00 * 8},	/* 1: -79..0dB with 1dB step */
	{ -7,  7, 2.00 * 8},	/* 2: -14..14dB with 2dB step */
	{-47,  0, 1.00 * 8},	/* 3: -47..0dB with 1dB step */
	{-15, 15, 1.00 * 8},	/* 4: -21..21dB with 1dB step */
	{  0, 15, 2.00 * 8},	/* 5: 0..30dB with 2dB step */
	{-63,  0, 1.25 * 8},	/* 6: -78.75..0dB with 1.25dB step*/
	{-15, 15, 1.25 * 8},	/* 7: -18.75..18.75dB with 1.25dB step */
	{  0,  3, 3.75 * 8},	/* 8: 0..11.25dB with 3.75dB step */
	{  0,  3, 6.25 * 8},	/* 9: 0..18.75dB with 6.25dB step */
	{-15,  0, 1.00 * 8},	/*10: -15..0dB with 1dB step */
	{-96, 31, 1.00 * 8},	/*11: -96..31dB with 1dB step */
	{ -7,  7, 1.00 * 8},	/*12: -7..7dB with 1dB step */
	{  0,  1, 6.00 * 8},	/*13: 0..6dB with 6dB step */
};

static sndParam sndPar[MODE_SND_END];
static audioProc _aproc;

static uint8_t _inCnt;
static uint8_t _input;
static uint8_t _mute;
static uint8_t _loudness;
static uint8_t _surround;
static uint8_t _effect3d;
static uint8_t _toneDefeat;

static void setNothing(int8_t val)
{
	return;
}

void sndInit(uint8_t extFunc)
{
	uint8_t i;

	uint8_t **txtLabels = getTxtLabels();

	/* Load audio parameters stored in eeprom */
	for (i = 0; i < MODE_SND_END; i++) {
		sndPar[i].value = eeprom_read_byte((uint8_t*)EEPROM_VOLUME + i);
		sndPar[i].label = txtLabels[MODE_SND_VOLUME + i];
	}
	_loudness = eeprom_read_byte((uint8_t*)EEPROM_LOUDNESS);
	_surround = eeprom_read_byte((uint8_t*)EEPROM_SURROUND);
	_effect3d = eeprom_read_byte((uint8_t*)EEPROM_EFFECT3D);
	_toneDefeat = eeprom_read_byte((uint8_t*)EEPROM_TONE_DEFEAT);
	_aproc = eeprom_read_byte((uint8_t*)EEPROM_AUDIOPROC);
	_input = eeprom_read_byte((uint8_t*)EEPROM_INPUT);
	if (_aproc >= AUDIOPROC_END)
		_aproc = AUDIOPROC_TDA7439;

	switch (_aproc) {
	case AUDIOPROC_TDA7439:
		tda7439Init(sndPar);
		break;
	case AUDIOPROC_TDA7312:
	case AUDIOPROC_TDA7313:
	case AUDIOPROC_TDA7314:
	case AUDIOPROC_TDA7315:
	case AUDIOPROC_TDA7318:
	case AUDIOPROC_PT2314:
		tda731xInit(sndPar);
		break;
	case AUDIOPROC_TDA7448:
		tda7448Init(sndPar);
		break;
	case AUDIOPROC_PT232X:
		pt232xInit(sndPar);
		break;
	case AUDIOPROC_PGA2310:
		if (extFunc == USE_PGA2310)
			pga2310Init(sndPar);
		break;
	default:
		break;
	}

	/* Init grid and functions with empty values */
	for (i = 0; i < MODE_SND_END; i++) {
		sndPar[i].grid = &grid[0];
		sndPar[i].set = setNothing;
	}

#ifndef KS0066
	uint8_t ic;

	/* Setup icons */
	for (i = 0; i < MODE_SND_END; i++)
		sndPar[i].icon = i;
	/* Update input icons */
	for (i = 0; i < MODE_SND_END - MODE_SND_GAIN0; i++) {
		ic = eeprom_read_byte((uint8_t*)(EEPROM_INPUT_ICONS + i));
		if (ic < ICON24_END)
			sndPar[MODE_SND_GAIN0 + i].icon = ic;
	}
#endif

	/* Setup audio parameters grid and functions */
	switch (_aproc) {
	case AUDIOPROC_TDA7439:
		sndPar[MODE_SND_VOLUME].grid = &grid[1];
		sndPar[MODE_SND_BASS].grid = &grid[2];
		sndPar[MODE_SND_MIDDLE].grid = &grid[2];
		sndPar[MODE_SND_TREBLE].grid = &grid[2];
		sndPar[MODE_SND_PREAMP].grid = &grid[3];
		sndPar[MODE_SND_BALANCE].grid = &grid[4];
		sndPar[MODE_SND_GAIN0].grid = &grid[5];
		sndPar[MODE_SND_GAIN1].grid = &grid[5];
		sndPar[MODE_SND_GAIN2].grid = &grid[5];
		sndPar[MODE_SND_GAIN3].grid = &grid[5];
		_inCnt = TDA7439_IN_CNT;
		sndPar[MODE_SND_VOLUME].set = tda7439SetVolume;
		sndPar[MODE_SND_BASS].set = tda7439SetBass;
		sndPar[MODE_SND_MIDDLE].set = tda7439SetMiddle;
		sndPar[MODE_SND_TREBLE].set = tda7439SetTreble;
		sndPar[MODE_SND_PREAMP].set = tda7439SetPreamp;
		sndPar[MODE_SND_BALANCE].set= tda7439SetBalance;
		sndPar[MODE_SND_GAIN0].set = tda7439SetGain;
		sndPar[MODE_SND_GAIN1].set = tda7439SetGain;
		sndPar[MODE_SND_GAIN2].set = tda7439SetGain;
		sndPar[MODE_SND_GAIN3].set = tda7439SetGain;
		break;
	case AUDIOPROC_TDA7312:
		sndPar[MODE_SND_VOLUME].grid = &grid[6];
		sndPar[MODE_SND_BASS].grid = &grid[2];
		sndPar[MODE_SND_TREBLE].grid = &grid[2];
		sndPar[MODE_SND_BALANCE].grid = &grid[7];
		_inCnt = TDA7312_IN_CNT;
		sndPar[MODE_SND_VOLUME].set = tda731xSetVolume;
		sndPar[MODE_SND_BASS].set = tda731xSetBass;
		sndPar[MODE_SND_TREBLE].set = tda731xSetTreble;
		sndPar[MODE_SND_BALANCE].set = tda731xSetBalance;
		break;
	case AUDIOPROC_TDA7313:
		sndPar[MODE_SND_VOLUME].grid = &grid[6];
		sndPar[MODE_SND_BASS].grid = &grid[2];
		sndPar[MODE_SND_TREBLE].grid = &grid[2];
		sndPar[MODE_SND_FRONTREAR].grid = &grid[7];
		sndPar[MODE_SND_BALANCE].grid = &grid[7];
		sndPar[MODE_SND_GAIN0].grid = &grid[8];
		sndPar[MODE_SND_GAIN1].grid = &grid[8];
		sndPar[MODE_SND_GAIN2].grid = &grid[8];
		_inCnt = TDA7313_IN_CNT;
		sndPar[MODE_SND_VOLUME].set = tda731xSetVolume;
		sndPar[MODE_SND_BASS].set = tda731xSetBass;
		sndPar[MODE_SND_TREBLE].set = tda731xSetTreble;
		sndPar[MODE_SND_FRONTREAR].set = tda731xSetBalance;
		sndPar[MODE_SND_BALANCE].set = tda731xSetBalance;
		sndPar[MODE_SND_GAIN0].set = tda731xSetGain;
		sndPar[MODE_SND_GAIN1].set = tda731xSetGain;
		sndPar[MODE_SND_GAIN2].set = tda731xSetGain;
		break;
	case AUDIOPROC_TDA7314:
		sndPar[MODE_SND_VOLUME].grid = &grid[6];
		sndPar[MODE_SND_BASS].grid = &grid[2];
		sndPar[MODE_SND_TREBLE].grid = &grid[2];
		sndPar[MODE_SND_FRONTREAR].grid = &grid[7];
		sndPar[MODE_SND_BALANCE].grid = &grid[7];
		sndPar[MODE_SND_GAIN0].grid = &grid[9];
		_inCnt = TDA7314_IN_CNT;
		break;
		sndPar[MODE_SND_VOLUME].set = tda731xSetVolume;
		sndPar[MODE_SND_BASS].set = tda731xSetBass;
		sndPar[MODE_SND_TREBLE].set = tda731xSetTreble;
		sndPar[MODE_SND_FRONTREAR].set = tda731xSetBalance;
		sndPar[MODE_SND_BALANCE].set = tda731xSetBalance;
		sndPar[MODE_SND_GAIN0].set = tda731xSetGain;
	case AUDIOPROC_TDA7315:
		sndPar[MODE_SND_VOLUME].grid = &grid[6];
		sndPar[MODE_SND_BASS].grid = &grid[2];
		sndPar[MODE_SND_TREBLE].grid = &grid[2];
		sndPar[MODE_SND_BALANCE].grid = &grid[7];
		_inCnt = TDA7315_IN_CNT;
		break;
		sndPar[MODE_SND_VOLUME].set = tda731xSetVolume;
		sndPar[MODE_SND_BASS].set = tda731xSetBass;
		sndPar[MODE_SND_TREBLE].set = tda731xSetTreble;
		sndPar[MODE_SND_BALANCE].set = tda731xSetBalance;
	case AUDIOPROC_TDA7318:
		sndPar[MODE_SND_VOLUME].grid = &grid[6];
		sndPar[MODE_SND_BASS].grid = &grid[2];
		sndPar[MODE_SND_TREBLE].grid = &grid[2];
		sndPar[MODE_SND_FRONTREAR].grid = &grid[7];
		sndPar[MODE_SND_BALANCE].grid = &grid[7];
		sndPar[MODE_SND_GAIN0].grid = &grid[9];
		sndPar[MODE_SND_GAIN1].grid = &grid[9];
		sndPar[MODE_SND_GAIN2].grid = &grid[9];
		sndPar[MODE_SND_GAIN3].grid = &grid[9];
		_inCnt = TDA7318_IN_CNT;
		sndPar[MODE_SND_VOLUME].set = tda731xSetVolume;
		sndPar[MODE_SND_BASS].set = tda731xSetBass;
		sndPar[MODE_SND_TREBLE].set = tda731xSetTreble;
		sndPar[MODE_SND_FRONTREAR].set = tda731xSetBalance;
		sndPar[MODE_SND_BALANCE].set = tda731xSetBalance;
		sndPar[MODE_SND_GAIN0].set = tda731xSetGain;
		sndPar[MODE_SND_GAIN1].set = tda731xSetGain;
		sndPar[MODE_SND_GAIN2].set = tda731xSetGain;
		sndPar[MODE_SND_GAIN3].set =tda731xSetGain ;
		break;
	case AUDIOPROC_PT2314:
		sndPar[MODE_SND_VOLUME].grid = &grid[6];
		sndPar[MODE_SND_BASS].grid = &grid[2];
		sndPar[MODE_SND_TREBLE].grid = &grid[2];
		sndPar[MODE_SND_FRONTREAR].grid = &grid[7];
		sndPar[MODE_SND_BALANCE].grid = &grid[7];
		sndPar[MODE_SND_GAIN0].grid = &grid[8];
		sndPar[MODE_SND_GAIN1].grid = &grid[8];
		sndPar[MODE_SND_GAIN2].grid = &grid[8];
		sndPar[MODE_SND_GAIN3].grid = &grid[8];
		_inCnt = PT2314_IN_CNT;
		sndPar[MODE_SND_VOLUME].set = tda731xSetVolume;
		sndPar[MODE_SND_BASS].set = tda731xSetBass;
		sndPar[MODE_SND_TREBLE].set = tda731xSetTreble;
		sndPar[MODE_SND_FRONTREAR].set = tda731xSetBalance;
		sndPar[MODE_SND_BALANCE].set = tda731xSetBalance;
		sndPar[MODE_SND_GAIN0].set = tda731xSetGain;
		sndPar[MODE_SND_GAIN1].set = tda731xSetGain;
		sndPar[MODE_SND_GAIN2].set = tda731xSetGain;
		sndPar[MODE_SND_GAIN3].set = tda731xSetGain;
		break;
	case AUDIOPROC_TDA7448:
		sndPar[MODE_SND_VOLUME].grid = &grid[1];
		sndPar[MODE_SND_FRONTREAR].grid = &grid[12];
		sndPar[MODE_SND_BALANCE].grid = &grid[12];
		sndPar[MODE_SND_CENTER].grid = &grid[10];
		sndPar[MODE_SND_SUBWOOFER].grid = &grid[10];
		_inCnt = TDA7448_IN_CNT;
		sndPar[MODE_SND_VOLUME].set = tda7448SetSpeakers;
		sndPar[MODE_SND_FRONTREAR].set = tda7448SetSpeakers;
		sndPar[MODE_SND_BALANCE].set= tda7448SetSpeakers;
		sndPar[MODE_SND_CENTER].set = tda7448SetSpeakers;
		sndPar[MODE_SND_SUBWOOFER].set = tda7448SetSpeakers;
		break;
	case AUDIOPROC_PT232X:
		sndPar[MODE_SND_VOLUME].grid = &grid[1];
		sndPar[MODE_SND_BASS].grid = &grid[2];
		sndPar[MODE_SND_MIDDLE].grid = &grid[2];
		sndPar[MODE_SND_TREBLE].grid = &grid[2];
		sndPar[MODE_SND_FRONTREAR].grid = &grid[12];
		sndPar[MODE_SND_BALANCE].grid = &grid[12];
		sndPar[MODE_SND_CENTER].grid = &grid[10];
		sndPar[MODE_SND_SUBWOOFER].grid = &grid[10];
		sndPar[MODE_SND_GAIN4].grid = &grid[13];
		_inCnt = PT2323_IN_CNT;
		sndPar[MODE_SND_VOLUME].set = pt2322SetVolume;
		sndPar[MODE_SND_BASS].set = pt2322SetBass;
		sndPar[MODE_SND_MIDDLE].set = pt2322SetMiddle;
		sndPar[MODE_SND_TREBLE].set = pt2322SetTreble;
		sndPar[MODE_SND_FRONTREAR].set = pt2322SetSpeakers;
		sndPar[MODE_SND_BALANCE].set = pt2322SetSpeakers;
		sndPar[MODE_SND_CENTER].set = pt2322SetSpeakers;
		sndPar[MODE_SND_SUBWOOFER].set = pt2322SetSpeakers;
		sndPar[MODE_SND_GAIN4].set = pt2322SetMux;
		break;
	case AUDIOPROC_PGA2310:
		sndPar[MODE_SND_VOLUME].grid = &grid[11];
		sndPar[MODE_SND_BALANCE].grid = &grid[4];
		_inCnt = PGA2310_IN_CNT;
		sndPar[MODE_SND_VOLUME].set = pga2310SetSpeakers;
		sndPar[MODE_SND_BALANCE].set = pga2310SetSpeakers;
		break;
	default:
		break;
	}

	return;
}

sndParam *sndParAddr(uint8_t index)
{
	return &sndPar[index];
}

uint8_t sndInputCnt(void)
{
	return _inCnt;
}

void sndSetInput(uint8_t input)
{
	if (input >= _inCnt)
		input = 0;
	_input = input;

	switch (_aproc) {
	case AUDIOPROC_TDA7439:
		tda7439SetInput(_input);
		break;
	case AUDIOPROC_TDA7312:
	case AUDIOPROC_TDA7313:
	case AUDIOPROC_TDA7314:
	case AUDIOPROC_TDA7315:
	case AUDIOPROC_TDA7318:
	case AUDIOPROC_PT2314:
		tda731xSetInput(_input);
		break;
	case AUDIOPROC_PT232X:
		pt2323SetInput(_input);
		break;
	default:
		break;
	}

	return;
}

uint8_t sndGetInput(void)
{
	return _input;
}


void sndSetMute(uint8_t value)
{
	int8_t vol;

	if (value) {
		vol = pgm_read_byte(&sndPar[MODE_SND_VOLUME].grid->min);
		_mute = 1;
		PORT(STMU_MUTE) &= ~STMU_MUTE_LINE;
	} else {
		vol = sndPar[MODE_SND_VOLUME].value;
		_mute = 0;
		PORT(STMU_MUTE) |= STMU_MUTE_LINE;
	}

	sndPar[MODE_SND_VOLUME].set(vol);

	switch (_aproc) {
	case AUDIOPROC_TDA7439:
		tda7439SetMute(_mute);
		break;
	case AUDIOPROC_TDA7312:
	case AUDIOPROC_TDA7313:
	case AUDIOPROC_TDA7314:
	case AUDIOPROC_TDA7315:
	case AUDIOPROC_TDA7318:
	case AUDIOPROC_PT2314:
		tda731xSetMute(_mute);
		break;
	case AUDIOPROC_TDA7448:
		tda7448SetMute(_mute);
		break;
	case AUDIOPROC_PT232X:
		pt232xSetMute(_mute);
		break;
	case AUDIOPROC_PGA2310:
		pga2310SetMute(_mute);
		break;
	default:
		break;
	}

	return;
}

uint8_t sndGetMute(void)
{
	return _mute;
}

void sndSetLoudness(uint8_t value)
{
	_loudness = value;

	if (_aproc == AUDIOPROC_TDA7313 || _aproc == AUDIOPROC_TDA7314 ||
			_aproc == AUDIOPROC_TDA7315 || _aproc == AUDIOPROC_PT2314)
		tda731xSetLoudness(_loudness);

	return;
}

uint8_t sndGetLoudness(void)
{
	return _loudness;
}

void sndSetSurround(uint8_t value)
{
	_surround = value;

	if (_aproc == AUDIOPROC_PT232X)
		pt2323SetSurround(_surround);

	return;
}

uint8_t sndGetSurround(void)
{
	return _surround;
}

void sndSetEffect3d(uint8_t value)
{
	_effect3d = value;

	if (_aproc == AUDIOPROC_PT232X)
		pt2322SetEffect3d(_effect3d);

	return;
}

uint8_t sndGetEffect3d(void)
{
	return _effect3d;
}

void sndSetToneDefeat(uint8_t value)
{
	_toneDefeat = value;

	if (_aproc == AUDIOPROC_PT232X)
		pt2322SetToneDefeat(_toneDefeat);

	return;
}

uint8_t sndGetToneDefeat(void)
{
	return _toneDefeat;
}

void sndNextParam(uint8_t *mode)
{
	do {					/* Skip unused params (with step = 0) */
		(*mode)++;
		if (*mode >= MODE_SND_GAIN0)
			*mode = MODE_SND_VOLUME;
	} while((pgm_read_byte(&sndPar[*mode].grid->step) == 0) &&
			(*mode < MODE_SND_GAIN0) && (*mode != MODE_SND_VOLUME));

	return;
}

void sndChangeParam(uint8_t mode, int8_t diff)
{
	sndParam *param = sndParAddr(mode);
	param->value += diff;
	if (param->value > (int8_t)pgm_read_byte(&param->grid->max))
		param->value = (int8_t)pgm_read_byte(&param->grid->max);
	if (param->value < (int8_t)pgm_read_byte(&param->grid->min))
		param->value = (int8_t)pgm_read_byte(&param->grid->min);
	param->set(param->value);

	return;
}

void sndPowerOn(void)
{
	uint8_t i;

	if (_aproc == AUDIOPROC_PT232X)
		pt232xReset();

	sndSetMute(1);
	sndSetInput(_input);
	for (i = MODE_SND_VOLUME + 1; i < MODE_SND_GAIN0; i++)
		sndPar[i].set(sndPar[i].value);
	sndSetMute(0);
	sndSetLoudness(_loudness);
	sndSetSurround(_surround);
	sndSetEffect3d(_effect3d);
	sndSetToneDefeat(_toneDefeat);

	return;
}

void sndPowerOff(void)
{
	uint8_t i;

	for (i = 0; i < MODE_SND_END; i++)
		eeprom_update_byte((uint8_t*)EEPROM_VOLUME + i, sndPar[i].value);

	eeprom_update_byte((uint8_t*)EEPROM_LOUDNESS, _loudness);
	eeprom_update_byte((uint8_t*)EEPROM_SURROUND, _surround);
	eeprom_update_byte((uint8_t*)EEPROM_EFFECT3D, _effect3d);
	eeprom_update_byte((uint8_t*)EEPROM_TONE_DEFEAT, _toneDefeat);
	eeprom_update_byte((uint8_t*)EEPROM_AUDIOPROC, _aproc);
	eeprom_update_byte((uint8_t*)EEPROM_INPUT, _input);

	return;
}
