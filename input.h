#ifndef INPUT_H
#define INPUT_H

#include <inttypes.h>

/* Buttons definitions */
#define BTN_DDR			DDRD
#define BTN_PIN			PIND
#define BTN_PORT		PORTD

#define BTN_5		(1<<PD7)
#define BTN_2		(1<<PD4)
#define BTN_3		(1<<PD5)
#define BTN_4		(1<<PD6)
#define BTN_1		(1<<PD0)

#define BTN_TEST_INPUT	(BTN_1 | BTN_2)
#define BTN_MASK		(BTN_1 | BTN_2 | BTN_3 | BTN_4 | BTN_5)

/* Encoder definitions */
#define ENC_DDR			DDRD
#define ENC_PIN			PIND
#define ENC_PORT		PORTD

#define ENC_A			(1<<PD2)
#define ENC_B			(1<<PD1)
#define ENC_AB			(ENC_A | ENC_B)
#define ENC_0			0

/* Standby/Mute/Fan port definitions */
#define SMF_DDR DDRC
#define SMF_PORT PORTC

#define FAN		(1<<PC2)
#define MUTE	(1<<PC5)
#define STDBY	(1<<PC6)

/* RC5 support definitions */
#define RC5_CMD_COUNT	16

enum {
	CMD_RC5_STBY,
	CMD_RC5_MUTE,
	CMD_RC5_MENU,
	CMD_RC5_VOL_UP,
	CMD_RC5_VOL_DOWN,
	CMD_RC5_INPUT_0,
	CMD_RC5_INPUT_1,
	CMD_RC5_INPUT_2,
	CMD_RC5_INPUT_3,
	CMD_RC5_NEXT_INPUT,
	CMD_RC5_TIME,
	CMD_RC5_BACKLIGHT,
	CMD_RC5_SP_MODE,
	CMD_RC5_LOUDNESS,
	CMD_RC5_CHAN_UP,
	CMD_RC5_CHAN_DOWN,

	CMD_RC5_STBY_LONG,
	CMD_RC5_MUTE_LONG,
	CMD_RC5_MENU_LONG,
	CMD_RC5_VOL_UP_LONG,
	CMD_RC5_VOL_DOWN_LONG,
	CMD_RC5_INPUT_0_LONG,
	CMD_RC5_INPUT_1_LONG,
	CMD_RC5_INPUT_2_LONG,
	CMD_RC5_INPUT_3_LONG,
	CMD_RC5_NEXT_INPUT_LONG,
	CMD_RC5_TIME_LONG,
	CMD_RC5_BACKLIGHT_LONG,
	CMD_RC5_SP_MODE_LONG,
	CMD_RC5_LOUDNESS_LONG,
	CMD_RC5_CHAN_UP_LONG,
	CMD_RC5_CHAN_DOWN_LONG,

	CMD_BTN_1,
	CMD_BTN_2,
	CMD_BTN_3,
	CMD_BTN_4,
	CMD_BTN_5,
	CMD_BTN_1_LONG,
	CMD_BTN_2_LONG,
	CMD_BTN_3_LONG,
	CMD_BTN_4_LONG,
	CMD_BTN_5_LONG,
	CMD_BTN_TESTMODE,

	CMD_EMPTY = 0xFF
};

// Handling long press actions */
#define SHORT_PRESS		100
#define LONG_PRESS		600

void btnInit(uint8_t *code, uint8_t addr);

int8_t getEncoder(void);
uint8_t getBtnCmd(void);
uint8_t getRC5Cmd(void);
uint16_t getRC5Buf(void);

void setDisplayTime(uint8_t value);
uint8_t getDisplayTime(void);

#endif /* INPUT_H */
