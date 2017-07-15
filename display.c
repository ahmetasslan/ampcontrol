#include "display.h"

#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include "eeprom.h"
#include "input.h"
#include "tuner/tuner.h"
#include "adc.h"
#include "alarm.h"
#ifdef _RDS
#include "tuner/rds.h"
#endif

static int8_t brStby;						/* Brightness in standby mode */
static int8_t brWork;						/* Brightness in working mode */

static uint8_t spMode;						/* Spectrum mode */
static uint8_t fallSpeed;					/* Spectrum fall speed */

static uint8_t rcIndex = CMD_RC_STBY;

static uint8_t defDisplay;					/* Default display mode */

char strbuf[STR_BUFSIZE + 1];				/* String buffer */
uint8_t *txtLabels[LABEL_END];				/* Array with text label pointers */

const char STR_RC_STBY[]		PROGMEM = "Standby mode";
const char STR_RC_MUTE[]		PROGMEM = "Mute sound";
const char STR_RC_NEXT_SNDPAR[] PROGMEM = "Sound menu";
const char STR_RC_VOL_UP[]		PROGMEM = "Volume +";
const char STR_RC_VOL_DOWN[]	PROGMEM = "Volume -";
const char STR_RC_IN_0[]		PROGMEM = "Input 1";
const char STR_RC_IN_1[]		PROGMEM = "Input 2";
const char STR_RC_IN_2[]		PROGMEM = "Input 3";
const char STR_RC_IN_3[]		PROGMEM = "Input 4";
const char STR_RC_IN_4[]		PROGMEM = "Input 5";
const char STR_RC_IN_PREV[]		PROGMEM = "Prev input";
const char STR_RC_IN_NEXT[]		PROGMEM = "Next input";
const char STR_RC_LOUDNESS[]	PROGMEM = "Loudness";
const char STR_RC_SURROUND[]	PROGMEM = "Surround";
const char STR_RC_EFFECT_3D[]	PROGMEM = "3D effect";
const char STR_RC_TONE_DEFEAT[]	PROGMEM = "Tone defeat";

const char STR_RC_FM_RDS[]		PROGMEM = "RDS enable";
const char STR_RC_FM_INC[]		PROGMEM = "Channel +";
const char STR_RC_FM_DEC[]		PROGMEM = "Channel -";
const char STR_RC_FM_MODE[]		PROGMEM = "FM tune";
const char STR_RC_FM_MONO[]		PROGMEM = "FM mono";
const char STR_RC_FM_STORE[]	PROGMEM = "Store freq";
const char STR_RC_FM_0[]		PROGMEM = "Button 0";
const char STR_RC_FM_1[]		PROGMEM = "Button 1";
const char STR_RC_FM_2[]		PROGMEM = "Button 2";
const char STR_RC_FM_3[]		PROGMEM = "Button 3";
const char STR_RC_FM_4[]		PROGMEM = "Button 4";
const char STR_RC_FM_5[]		PROGMEM = "Button 5";
const char STR_RC_FM_6[]		PROGMEM = "Button 6";
const char STR_RC_FM_7[]		PROGMEM = "Button 7";
const char STR_RC_FM_8[]		PROGMEM = "Button 8";
const char STR_RC_FM_9[]		PROGMEM = "Button 9";

const char STR_RC_TIME[]		PROGMEM = "Time";
const char STR_RC_ALARM[]		PROGMEM = "Alarm";
const char STR_RC_TIMER[]		PROGMEM = "Timer";
const char STR_RC_BRIGHTNESS[]	PROGMEM = "Brightness";
const char STR_RC_DEF_DISPLAY[]	PROGMEM = "Display mode";
const char STR_RC_NEXT_SPMODE[]	PROGMEM = "Spectrum mode";
const char STR_RC_FALLSPEED[]	PROGMEM = "Fall speed";

PGM_P const rcLabels[] PROGMEM = {
	STR_RC_STBY,
	STR_RC_MUTE,
	STR_RC_NEXT_SNDPAR,
	STR_RC_VOL_UP,
	STR_RC_VOL_DOWN,
	STR_RC_IN_0,
	STR_RC_IN_1,
	STR_RC_IN_2,
	STR_RC_IN_3,
	STR_RC_IN_4,
	STR_RC_IN_PREV,
	STR_RC_IN_NEXT,
	STR_RC_LOUDNESS,
	STR_RC_SURROUND,
	STR_RC_EFFECT_3D,
	STR_RC_TONE_DEFEAT,

	STR_RC_FM_RDS,
	STR_RC_FM_INC,
	STR_RC_FM_DEC,
	STR_RC_FM_MODE,
	STR_RC_FM_MONO,
	STR_RC_FM_STORE,
	STR_RC_FM_0,
	STR_RC_FM_1,
	STR_RC_FM_2,
	STR_RC_FM_3,
	STR_RC_FM_4,
	STR_RC_FM_5,
	STR_RC_FM_6,
	STR_RC_FM_7,
	STR_RC_FM_8,
	STR_RC_FM_9,

	STR_RC_TIME,
	STR_RC_ALARM,
	STR_RC_TIMER,
	STR_RC_BRIGHTNESS,
	STR_RC_DEF_DISPLAY,
	STR_RC_NEXT_SPMODE,
	STR_RC_FALLSPEED,
};

const char STR_IN_STATUS[]		PROGMEM = "BUTTONS & ENCODER";
const char STR_REMOTE[]			PROGMEM = "Remote";
const char STR_BUTTONS[]		PROGMEM = "Status";
const char STR_LEARN[]			PROGMEM = "REMOTE CONTROL";
const char STR_FUNCTION[]		PROGMEM = "Function";
const char STR_ADDRESS[]		PROGMEM = "Address";
const char STR_COMMAND[]		PROGMEM = "Command";
const char STR_RC_RC5[]			PROGMEM = "RC5";
const char STR_RC_NEC[]			PROGMEM = "NEC";
const char STR_RC_RC6[]			PROGMEM = "RC6";
const char STR_RC_SAM[]			PROGMEM = "SAM";
const char STR_RC_NONE[]		PROGMEM = "---";

const char STR_THRESHOLD[]		PROGMEM = "Threshold";
const char STR_DEGREE[]			PROGMEM = "\x7F\xDF""C";
const char STR_SENSOR1[]		PROGMEM = "Sensor 1: ";
const char STR_SENSOR2[]		PROGMEM = "Sensor 2: ";

const char STR_SPDIVSP[]		PROGMEM = "\x7F/\x7F";
const char STR_SPARRSP[]		PROGMEM = " => ";
const char STR_SPDOTSP[]		PROGMEM = "\x7F.\x7F";
const char STR_SPCOLSP[]		PROGMEM = "\x7F:\x7F";
const char STR_SP[]				PROGMEM = "\x7F";
const char STR_SPACE2[]			PROGMEM = "  ";
const char STR_SPACE3[]			PROGMEM = "   ";
const char STR_MINUS1[]			PROGMEM = "-";
const char STR_MINUS2[]			PROGMEM = "--";
const char STR_SPMINUS2[]		PROGMEM = " --";

const char STR_FM[]				PROGMEM = "FM ";
const char STR_STEREO[]			PROGMEM = "ST";
const char STR_MONO[]			PROGMEM = "MO";
const char STR_TUNE[]			PROGMEM = "\xDB\xDB\xD0\xDC\xDC";
const char STR_RDS[]			PROGMEM = "RDS";

const char STR_YEAR20[]			PROGMEM = "20";

const char STR_PREFIX_BIN[]		PROGMEM = "0b\x7F";
const char STR_PREFIX_HEX[]		PROGMEM = "0x\x7F";

static void showBar(int16_t min, int16_t max, int16_t value)
{
	uint8_t i, j;
	uint8_t color;

	if (min + max) {
		value = (uint16_t)91 * (value - min) / (max - min);
	} else {
		value = (int16_t)45 * value / max;
	}

	for (i = 0; i < 91; i++) {
		if (((min + max) && (value <= i)) || (!(min + max) &&
											  (((value > 0) && ((i < 45) || (value + 45 < i))) ||
											   ((value <= 0) && ((i > 45) || (value + 45 > i)))))) {
			color = 0x00;
		} else {
			color = 0x01;
		}
		if (!(i & 0x01)) {
			for (j = 28; j < 39; j++) {
				if (j == 33) {
					gdDrawPixel(i, j, 1);
				} else {
					gdDrawPixel(i, j, color);
				}
			}
		}
	}
	return;
}

static void writeStringEeprom(const uint8_t *string)
{
	uint8_t i;

	for (i = 0; i < STR_BUFSIZE; i++)
		strbuf[i] = eeprom_read_byte(&string[i]);

	writeString(strbuf);

	return;
}

static void writeStringPgm(const char *string)
{
	strcpy_P(strbuf, string);
	writeString(strbuf);

	return;
}

static void writeNum(int16_t number, uint8_t width, uint8_t lead, uint8_t radix)
{
	uint8_t numdiv;
	uint8_t sign = lead;
	int8_t i;

	if (number < 0) {
		sign = '-';
		number = -number;
	}

	for (i = 0; i < width; i++)
		strbuf[i] = lead;
	strbuf[width] = '\0';
	i = width - 1;

	while (number > 0 || i == width - 1) {
		numdiv = number % radix;
		strbuf[i] = numdiv + 0x30;
		if (numdiv >= 10)
			strbuf[i] += 7;
		i--;
		number /= radix;
	}

	if (i >= 0)
		strbuf[i] = sign;

	writeString(strbuf);

	return;
}

static void showParValue(int8_t value)
{
	gdLoadFont(font_ks0066_ru_24, 1, FONT_DIR_0);
	gdSetXY(94, 30);
	writeNum(value, 3, ' ', 10);

	return;
}

static void showParLabel(uint8_t label)
{
	gdLoadFont(font_ks0066_ru_24, 1, FONT_DIR_0);
	gdSetXY(0, 0);
	writeStringEeprom(txtLabels[label]);

	return;
}

static void showParIcon(uint8_t icon)
{
	uint8_t ic = icon;

	if (ic >= MODE_SND_GAIN0 && ic < MODE_SND_END)
		ic = eeprom_read_byte((uint8_t*)(EEPROM_INPUT_ICONS + (ic - MODE_SND_GAIN0)));
	if (ic < ICON24_END)
		icon = ic;

	gdSetXY(104, 2);
	gdWriteIcon24(icon);

	return;
}

static void drawSpCol(uint8_t xbase, uint8_t w, uint8_t btm, uint8_t val, uint8_t max)
{
	uint8_t i;

	val = (val < max ? btm - val : btm - max);

	for (i = 0; i < w; i++) {
		gdDrawVertLine(xbase + i, btm, val, 1);
		gdDrawVertLine(xbase + i, val > (btm - max) ? val - 1 : val, btm - max, 0);
	}

	return;
}

static void drawBarSpectrum(void)
{
	uint8_t x, xbase;
	uint8_t ybase;

	for (x = 0; x < GD_SIZE_X / 4 - 1; x++) {
		xbase = x * 3;

		ybase = (buf[x] + buf[x + 32]) * 3 / 8;
		drawSpCol(xbase, 2, 63, ybase, 23);
		gdDrawVertLine(xbase + 2, 63, 63 - 23, 0); // Clear space between bars
	}

	return;
}

static void drawMiniSpectrum(void)
{
	uint8_t x, xbase;
	uint8_t ybase;

	for (x = 0; x < GD_SIZE_X / 4 - 1; x++) {
		xbase = x * 3;
		ybase = (buf[x] * 5 / 2 + buf[x + 32] * 5 / 2) / 4;
		drawSpCol(xbase, 2, 63, ybase, 39);
	}

	return;
}

static void drawTm(uint8_t tm, const uint8_t *font)
{
	gdLoadFont(font, rtc.etm == tm ? 0 : 1, FONT_DIR_0);
	writeNum(*((int8_t*)&rtc + tm), 2, '0', 10);
	gdLoadFont(font, 1, FONT_DIR_0);

	return;
}

static void drawAm(uint8_t am, const uint8_t *font)
{
	gdLoadFont(font, alarm0.eam == am ? 0 : 1, FONT_DIR_0);
	writeNum(*((int8_t*)&alarm0 + am), 2, '0', 10);
	gdLoadFont(font, 1, FONT_DIR_0);

	return;
}

void displayInit(void)
{
	uint8_t i;
	uint8_t *addr;

	addr = (uint8_t*)EEPROM_LABELS_ADDR;
	i = 0;

	while (i < LABEL_END && addr < (uint8_t*)EEPROM_SIZE) {
		if (eeprom_read_byte(addr) != '\0') {
			txtLabels[i] = addr;
			addr++;
			i++;
			while (eeprom_read_byte(addr) != '\0' &&
				   addr < (uint8_t*)EEPROM_SIZE) {
				addr++;
			}
		} else {
			addr++;
		}
	}

	gdInit();

	brStby = eeprom_read_byte((uint8_t*)EEPROM_BR_STBY);
	brWork = eeprom_read_byte((uint8_t*)EEPROM_BR_WORK);
	spMode  = eeprom_read_byte((uint8_t*)EEPROM_SP_MODE);
	defDisplay = eeprom_read_byte((uint8_t*)EEPROM_DISPLAY);
	fallSpeed = eeprom_read_byte((uint8_t*)EEPROM_FALL_SPEED);
	if (fallSpeed > FALL_SPEED_FAST)
		fallSpeed = FALL_SPEED_FAST;

	return;
}

uint8_t **getTxtLabels(void)
{
	return txtLabels;
}

void setDefDisplay(uint8_t value)
{
	defDisplay = value;

	return;
}

uint8_t getDefDisplay()
{
	return defDisplay;
}

void nextRcCmd(void)
{
	IRData irBuf = getIrData();

	eeprom_update_byte((uint8_t*)EEPROM_RC_TYPE, irBuf.type);
	eeprom_update_byte((uint8_t*)EEPROM_RC_ADDR, irBuf.address);
	eeprom_update_byte((uint8_t*)EEPROM_RC_CMD + rcIndex, irBuf.command);

	// Re-read new codes array from EEPROM
	rcCodesInit();

	if (++rcIndex >= CMD_RC_END)
		rcIndex = CMD_RC_STBY;

	switchTestMode(rcIndex);

	return;
}

void switchTestMode(uint8_t index)
{
	rcIndex = index;
	setIrData(eeprom_read_byte((uint8_t*)EEPROM_RC_TYPE),
			  eeprom_read_byte((uint8_t*)EEPROM_RC_ADDR),
			  eeprom_read_byte((uint8_t*)EEPROM_RC_CMD + rcIndex));

	return;
}

void showRcInfo(void)
{
	IRData irBuf = getIrData();
	int16_t btnBuf = getBtnBuf();
	uint8_t encBuf = getEncBuf();
	gdLoadFont(font_ks0066_ru_08, 1, FONT_DIR_0);
	gdSetXY(10, 0);
	writeStringPgm(STR_IN_STATUS);

	gdSetXY(0, 10);
	writeStringPgm(STR_BUTTONS);
	gdSetXY(48, 10);
	writeStringPgm(STR_PREFIX_HEX);
	writeNum(btnBuf, 4, '0', 16);
	writeStringPgm(STR_SPDIVSP);
	writeStringPgm(STR_PREFIX_BIN);
	writeNum(encBuf, 2, '0', 2);

	gdSetXY(10, 20);
	writeStringPgm(STR_LEARN);

	gdSetXY(0, 30);
	writeStringPgm(STR_REMOTE);
	gdSetXY(48, 30);
	switch (irBuf.type) {
	case IR_TYPE_RC5:
		writeStringPgm(STR_RC_RC5);
		break;
	case IR_TYPE_NEC:
		writeStringPgm(STR_RC_NEC);
		break;
	case IR_TYPE_RC6:
		writeStringPgm(STR_RC_RC6);
		break;
	case IR_TYPE_SAM:
		writeStringPgm(STR_RC_SAM);
		break;
	default:
		writeStringPgm(STR_RC_NONE);
		break;
	}

	gdSetXY(0, 39);
	writeStringPgm(STR_ADDRESS);
	gdSetXY(48, 39);
	writeStringPgm(STR_PREFIX_HEX);
	writeNum(irBuf.address, 2, '0', 16);
	writeStringPgm(STR_SPARRSP);
	writeStringPgm(STR_PREFIX_HEX);
	writeNum(eeprom_read_byte((uint8_t*)EEPROM_RC_ADDR), 2, '0', 16);

	gdSetXY(0, 48);
	writeStringPgm(STR_FUNCTION);
	gdSetXY(48, 48);
	writeStringPgm((const char *)pgm_read_word(&rcLabels[rcIndex]));

	gdSetXY(0, 57);
	writeStringPgm(STR_COMMAND);
	gdSetXY(48, 57);
	writeStringPgm(STR_PREFIX_HEX);
	writeNum(irBuf.command, 2, '0', 16);
	writeStringPgm(STR_SPARRSP);
	writeStringPgm(STR_PREFIX_HEX);
	writeNum(eeprom_read_byte((uint8_t*)EEPROM_RC_CMD + rcIndex), 2, '0', 16);

	return;
}

void showRadio(uint8_t tune)
{
	tunerReadStatus();

	uint8_t i;

	uint8_t level = tunerLevel();
	uint8_t num = tunerStationNum();
	uint8_t favNum = tunerFavStationNum();
#ifdef _RDS
	uint8_t rdsFlag = rdsGetFlag();
#endif

	/* Frequency value */
	gdLoadFont(font_ks0066_ru_24, 1, FONT_DIR_0);
	gdSetXY(0, 0);
	writeStringPgm(STR_FM);
	writeNum(tuner.freq / 100, 3, ' ', 10);
	writeStringPgm(STR_SPDOTSP);
	writeNum(tuner.freq % 100, 2, '0', 10);

	/* Signal level */
	gdSetXY (112, 0);
	for (i = 0; i < 16; i+=2) {
		if (i <= level) {
			gdDrawLine(112 + i, 7, 112 + i, 7 - i / 2, 0x01);
		} else {
			gdDrawLine(112 + i, 6, 112 + i, 0, 0x00);
			gdDrawPixel(112 + i, 7, 0x01);
		}
	}

	/* Stereo indicator */
	gdLoadFont(font_ks0066_ru_08, 1, FONT_DIR_0);
	gdSetXY(116, 12);
	if (tuner.mono)
		writeStringPgm(STR_MONO);
	else if (tunerStereo())
		writeStringPgm(STR_STEREO);
	else
		writeStringPgm(STR_SPACE2);

	/* Favourite station number */
	gdSetXY(114, 23);
	gdWriteChar(0xF5);						/* Heart symbol */
	gdSetXY(122, 23);
	if (favNum)
		writeNum(favNum - 1, 1, ' ', 10);
	else
		writeStringPgm(STR_MINUS1);

	/* Station number */
	gdLoadFont(font_ks0066_ru_24, 1, FONT_DIR_0);
	gdSetXY(106, 30);
	if (num)
		writeNum(num, 2, ' ', 10);
	else
		writeStringPgm(STR_MINUS2);

	/* Frequency scale */
	showBar(tuner.fMin >> 4, tuner.fMax >> 4, tuner.freq >> 4);

	/* Select between RDS and spectrum mode */
#ifdef _RDS
	if (rdsFlag) {
		gdLoadFont(font_ks0066_ru_24, 1, FONT_DIR_0);
		gdSetFontFixed(12);
		gdSetXY(0, 40);
		writeString(rdsGetText());
	} else {
		gdDrawFilledRect(93, 40, 3, 24, 0);
#endif
		drawBarSpectrum();
#ifdef _RDS
	}
#endif

	gdLoadFont(font_ks0066_ru_08, 1, FONT_DIR_0);
	if (tune == MODE_RADIO_TUNE) {
		gdSetXY(103, 56);
		writeStringPgm(STR_TUNE);
	} else {
		gdSetXY(110, 56);
#ifdef _RDS
		if (tuner.rds)
			writeStringPgm(STR_RDS);
		else
			writeStringPgm(STR_SPACE3);
#else
		writeStringPgm(STR_SPACE3);
#endif
	}

	return;
}

void showMute(void)
{
	showParLabel(LABEL_MUTE);
	drawMiniSpectrum();

	gdSetXY(96, 32);
	if (aproc.mute)
		gdWriteIcon32(ICON32_MUTE_ON);
	else
		gdWriteIcon32(ICON32_MUTE_OFF);

	return;
}

void showLoudness(void)
{
	showParLabel(LABEL_LOUDNESS);
	drawMiniSpectrum();

	gdSetXY(96, 32);
	if (aproc.extra & APROC_EXTRA_LOUDNESS)
		gdWriteIcon32(ICON32_LOUDNESS_ON);
	else
		gdWriteIcon32(ICON32_LOUDNESS_OFF);

	return;
}

void showSurround()
{
	showParLabel(LABEL_SURROUND);
	drawMiniSpectrum();

	gdSetXY(96, 32);
	if (aproc.extra & APROC_EXTRA_SURROUND)
		gdWriteIcon32(ICON32_SURROUND_ON);
	else
		gdWriteIcon32(ICON32_SURROUND_OFF);

	return;
}

void showEffect3d()
{
	showParLabel(LABEL_EFFECT_3D);
	drawMiniSpectrum();

	gdSetXY(96, 32);
	if (aproc.extra & APROC_EXTRA_EFFECT3D)
		gdWriteIcon32(ICON32_EFFECT_3D_ON);
	else
		gdWriteIcon32(ICON32_EFFECT_3D_OFF);

	return;
}

void showToneDefeat()
{
	showParLabel(LABEL_TONE_DEFEAT);
	drawMiniSpectrum();

	gdSetXY(96, 32);
	if (aproc.extra & APROC_EXTRA_TONEDEFEAT)
		gdWriteIcon32(ICON32_TONE_DEFEAT_ON);
	else
		gdWriteIcon32(ICON32_TONE_DEFEAT_OFF);

	return;
}


void showBrWork(void)
{
	showParLabel(LABEL_BR_WORK);
	showBar(MIN_BRIGHTNESS, MAX_BRIGHTNESS, brWork);

	showParValue(brWork);
	drawBarSpectrum();
	showParIcon(ICON24_BRIGHTNESS);

	return;
}

void changeBrWork(int8_t diff)
{
	brWork += diff;
	if (brWork > MAX_BRIGHTNESS)
		brWork = MAX_BRIGHTNESS;
	if (brWork < MIN_BRIGHTNESS)
		brWork = MIN_BRIGHTNESS;
	setWorkBrightness();

	return;
}

void showSndParam(sndMode mode)
{
	sndParam *param = &sndPar[mode];

	showParLabel(mode);
	showParValue(((int16_t)(param->value) * (int8_t)pgm_read_byte(&param->grid->step) + 4) >> 3);
	showBar((int8_t)pgm_read_byte(&param->grid->min), (int8_t)pgm_read_byte(&param->grid->max), param->value);

	drawBarSpectrum();
	showParIcon(mode);

	gdLoadFont(font_ks0066_ru_08, 1, FONT_DIR_0);
	gdSetXY(116, 56);
	writeStringEeprom(txtLabels[LABEL_DB]);

	return;
}

void showTime(void)
{
	gdSetXY(4, 0);

	drawTm(RTC_HOUR, font_digits_32);
	writeStringPgm(STR_SPCOLSP);
	drawTm(RTC_MIN, font_digits_32);
	writeStringPgm(STR_SPCOLSP);
	drawTm(RTC_SEC, font_digits_32);

	gdSetXY(9, 32);

	drawTm(RTC_DATE, font_ks0066_ru_24);
	writeStringPgm(STR_SPDOTSP);
	drawTm(RTC_MONTH, font_ks0066_ru_24);
	writeStringPgm(STR_SPDOTSP);
	if (rtc.etm == RTC_YEAR)
		gdLoadFont(font_ks0066_ru_24, 0, FONT_DIR_0);
	writeStringPgm(STR_YEAR20);
	writeStringPgm(STR_SP);
	drawTm(RTC_YEAR, font_ks0066_ru_24);

	gdLoadFont(font_ks0066_ru_08, 1, FONT_DIR_0);
	gdSetXY(32, 56);
	writeStringEeprom(txtLabels[LABEL_SUNDAY + (rtcWeekDay() - 1) % 7]);

	return;
}

void showAlarm(void)
{
	uint8_t i;
	uint8_t *label;

	gdSetXY(4, 0);

	drawAm(ALARM_HOUR, font_digits_32);
	writeStringPgm(STR_SPCOLSP);
	drawAm(ALARM_MIN, font_digits_32);

	/* Draw input icon selection */
	if (alarm0.eam == ALARM_INPUT) {
		gdDrawFilledRect(99, 2, 3, 26, 1);
		gdDrawFilledRect(99, 28, 29, 3, 1);
	} else {
		gdDrawFilledRect(99, 2, 3, 26, 0);
		gdDrawFilledRect(99, 28, 29, 3, 0);
	}

	/* Check that input number less than CHAN_CNT */
	i = alarm0.input;
	if (i >= aproc.inCnt)
		i = 0;
	showParIcon(MODE_SND_GAIN0 + i);

	/* Draw weekdays selection rectangle */
	if (alarm0.eam == ALARM_WDAY) {
		gdDrawRect(0, 34, 128, 30, 1);
		gdDrawRect(1, 35, 126, 28, 1);
	} else {
		gdDrawRect(0, 34, 128, 30, 0);
		gdDrawRect(1, 35, 126, 28, 0);
	}

	/* Draw weekdays */
	gdLoadFont(font_ks0066_ru_08, 1, FONT_DIR_0);
	label = txtLabels[LABEL_WEEKDAYS];
	for (i = 0; i < 7; i++) {
		gdSetXY(5 + 18 * i, 38);
		gdWriteChar(eeprom_read_byte(&label[i * 2]));
		gdWriteChar(0x7F);
		gdWriteChar(eeprom_read_byte(&label[i * 2 + 1]));

		gdDrawRect(3 + 18 * i, 47, 14, 14, 1);
		if (alarm0.wday & (0x40 >> i))
			gdDrawFilledRect(5 + 18 * i, 49, 10, 10, 1);
		else
			gdDrawFilledRect(5 + 18 * i, 49, 10, 10, 0);
	}

	return;
}

void showTimer(int16_t timer)
{
	uint8_t x, xbase;
	uint8_t ybase;

	gdSetXY(4, 0);

	gdLoadFont(font_digits_32, 1, FONT_DIR_0);
	if (timer >= 0) {
		writeNum(timer / 3600, 2, '0', 10);
		writeStringPgm(STR_SPCOLSP);
		writeNum(timer / 60 % 60, 2, '0', 10);
		writeStringPgm(STR_SPCOLSP);
		writeNum(timer % 60, 2, '0', 10);
	} else {
		writeStringPgm(STR_MINUS2);
		writeStringPgm(STR_SPCOLSP);
		writeStringPgm(STR_MINUS2);
		writeStringPgm(STR_SPCOLSP);
		writeStringPgm(STR_MINUS2);
	}

	for (x = 0; x < GD_SIZE_X / 4; x++) {
		xbase = x << 2;

		ybase = (buf[x] + buf[x + 32]) / 2;
		drawSpCol(xbase, 3, 63, ybase, 31);
	}

	return;
}

void switchSpMode(void)
{
	if (++spMode >= SP_MODE_END)
		spMode = SP_MODE_METER;

	return;
}

void switchFallSpeed(void)
{
	fallSpeed++;
	if (fallSpeed > FALL_SPEED_FAST)
		fallSpeed = FALL_SPEED_LOW;

	return;
}


void getSpectrum(void)
{
	getSpData(fallSpeed);
	return;
}

void showSpectrum(void)
{
	uint16_t left, right;

	uint8_t x, xbase;
	uint8_t y, ybase;

	switch (spMode) {
	case SP_MODE_STEREO:
		for (x = 0; x < GD_SIZE_X / 6 + 1; x++) {
			xbase = x * 6;

			for (y = 0; y < GD_SIZE_Y; y += 32) {
				ybase = buf[x + y];
				drawSpCol(xbase, 2, 31 + y, ybase, 31);
				ybase += buf[x + y + 1];
				ybase /= 2;
				drawSpCol(xbase + 3, 2, 31 + y, ybase, 31);
			}
		}
		break;
	case SP_MODE_MIXED:
		for (x = 0; x < GD_SIZE_X / 6 + 1; x++) {
			xbase = x * 6;

			ybase = buf[x] + buf[x + 32];
			drawSpCol(xbase, 2, 63, ybase, 63);
			ybase += buf[x + 1] + buf[x + 32 + 1];
			ybase /= 2;
			drawSpCol(xbase + 3, 2, 63, ybase, 63);
		}
		break;
	default:
		gdLoadFont(font_ks0066_ru_08, 1, FONT_DIR_0);
		gdSetXY(0, 0);
		writeStringEeprom(txtLabels[LABEL_LEFT_CHANNEL]);
		gdSetXY(0, 36);
		writeStringEeprom(txtLabels[LABEL_RIGHT_CHANNEL]);
		left = 0;
		right = 0;
		for (x = 0; x < GD_SIZE_X / 4; x++) {
			left += buf[x];
			right += buf[x + 32];
		}
		left >>= 4;
		right >>= 4;

		for (x = 0; x < 43; x++) {
			for (y = 12; y < 27; y++) {
				if (x < left || y == 19) {
					gdDrawPixel(3 * x + 0, y, 1);
					gdDrawPixel(3 * x + 1, y, 1);
				} else {
					gdDrawPixel(3 * x + 0, y, 0);
					gdDrawPixel(3 * x + 1, y, 0);
				}
			}
			for (y = 48; y < 63; y++) {
				if (x < right || y == 55) {
					gdDrawPixel(3 * x + 0, y, 1);
					gdDrawPixel(3 * x + 1, y, 1);
				} else {
					gdDrawPixel(3 * x + 0, y, 0);
					gdDrawPixel(3 * x + 1, y, 0);
				}
			}
		}
		break;
	}

	return;
}

void setWorkBrightness(void)
{
	gdSetBrightness(brWork);

	return;
}

void setStbyBrightness(void)
{
	gdSetBrightness(brStby);

	return;
}

void displayPowerOff(void)
{
	eeprom_update_byte((uint8_t*)EEPROM_BR_STBY, brStby);
	eeprom_update_byte((uint8_t*)EEPROM_BR_WORK, brWork);
	eeprom_update_byte((uint8_t*)EEPROM_SP_MODE, spMode);
	eeprom_update_byte((uint8_t*)EEPROM_DISPLAY, defDisplay);
	eeprom_update_byte((uint8_t*)EEPROM_FALL_SPEED, fallSpeed);

	return;
}

void displayUpdate()
{

#if defined(SSD1306)
	ssd1306UpdateFb();
#endif

	return;
}
