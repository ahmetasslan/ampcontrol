#include "gdfb.h"

#include <avr/pgmspace.h>

const uint8_t *_font;
static uint8_t fp[FONT_PARAM_COUNT];

static uint8_t _x, _y;

inline void gdInit(void)
{
#if defined(ST7920)
	st7920Init();
#elif defined(KS0108A) || defined(KS0108B)
	ks0108Init();
#endif

	return;
}

inline void gdClear(void) {
#if defined(ST7920)
	st7920Clear();
#elif defined(KS0108A) || defined(KS0108B)
	ks0108Clear();
#endif

	return;
}

void gdSetBrightness(uint8_t br)
{

#if defined(ST7920)
	st7920SetBrightness(br);
#elif defined(KS0108A) || defined(KS0108B)
	ks0108SetBrightness(br);
#endif
	return;
}

inline void gdDrawPixel(uint8_t x, uint8_t y, uint8_t color)
{
#if defined(ST7920)
	st7920DrawPixel(x, y, color);
#elif defined(KS0108A) || defined(KS0108B)
	ks0108DrawPixel(x, y, color);
#endif

	return;
}

void gdDrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color)
{
	int8_t sX, sY, dX, dY, err;
	int16_t err2;

	sX = x1 < x2 ? 1 : -1;
	sY = y1 < y2 ? 1 : -1;
	dX = sX > 0 ? x2 - x1 : x1 - x2;
	dY = sY > 0 ? y2 - y1 : y1 - y2;
	err = dX - dY;

	while (x1 != x2 || y1 != y2) {
		gdDrawPixel(x1, y1, color);
		err2 = err * 2;
		if (err2 > -dY / 2) {
			err -= dY;
			x1 += sX;
		}
		if (err2 < dX) {
			err += dX;
			y1 += sY;
		}
	}
	gdDrawPixel(x2, y2, color);

	return;
}

void gdDrawCircle(uint8_t x0, uint8_t y0, int16_t radius, uint8_t color)
{
	int8_t x, y;
	int16_t delta, err, px, py;

	x = 0;
	y = radius;
	delta = 2 - 2 * radius;
	err = 0;

	while (y >= x)
	{
		if ((px = x0 - x) >= 0 && (py = y0 - y) >= 0)
			gdDrawPixel(px, py, color);
		if ((px = x0 - y) >= 0 && (py = y0 - x) >= 0)
			gdDrawPixel(px, py, color);

		if ((px = x0 + x) < GD_SIZE_X && (py = y0 - y) >= 0)
			gdDrawPixel(px, py, color);
		if ((px = x0 + y) < GD_SIZE_X && (py = y0 - x) >= 0)
			gdDrawPixel(px, py, color);

		if ((px = x0 - x) >= 0 && (py = y0 + y) < GD_SIZE_Y)
			gdDrawPixel(px, py, color);
		if ((px = x0 - y) >= 0 && (py = y0 + x) < GD_SIZE_Y)
			gdDrawPixel(px, py, color);

		if ((px = x0 + x) < GD_SIZE_X && (py = y0 + y) < GD_SIZE_Y)
			gdDrawPixel(px, py, color);
		if ((px = x0 + y) < GD_SIZE_X && (py = y0 + x) < GD_SIZE_Y)
			gdDrawPixel(px, py, color);

		err = 2 * (delta + y) - 1;

		if(delta < 0 && err <= 0) {
			++x;
			delta += 2 * x + 1;
			continue;
		}
		err = 2 * (delta - x) - 1;
		if(delta > 0 && err > 0) {
			--y;
			delta += 1 - 2 * y;
			continue;
		}
		++x;
		delta += 2 * (x - y);
		--y;
	}
}

void gdSetXY(uint8_t x, uint8_t y)
{
	_x = x;
	_y = y;

	return;
}

void gdLoadFont(const uint8_t *font, uint8_t color, uint8_t direction)
{
	uint8_t i;

	_font = font + 5;
	for (i = 0; i < FONT_PARAM_COUNT - 1; i++)
		fp[i] = pgm_read_byte(font + i);
	fp[FONT_COLOR] = color;
	fp[FONT_DIRECTION] = direction;
}

void gdWriteChar(uint8_t code)
{
	uint8_t i;
	uint8_t j;
	uint8_t k;

	uint8_t pgmData;

	uint8_t spos = code - ((code >= 128) ? fp[FONT_OFTNA] : fp[FONT_OFTA]);

	uint16_t oft = 0;	/* Current symbol offset in array*/
	uint8_t swd = 0;	/* Current symbol width */

	for (i = 0; i < spos; i++) {
		swd = pgm_read_byte(_font + i);
		oft += swd;
	}
	swd = pgm_read_byte(_font + spos);

	oft *= fp[FONT_HEIGHT];
	oft += fp[FONT_CCNT];

	for (j = 0; j < fp[FONT_HEIGHT]; j++) {
		for (i = 0; i < swd; i++) {
			pgmData = pgm_read_byte(_font + oft + (swd * j) + i);
			if (!fp[FONT_COLOR])
				pgmData = ~pgmData;
			for (k = 0; k < 8; k++) {
				switch (fp[FONT_DIRECTION]) {
				case FONT_DIR_0:
					gdDrawPixel(_x + i, _y + (8 * j + k), pgmData & (1<<k));
					break;
				case FONT_DIR_90:
					gdDrawPixel(_x + (8 * j + k), _y - i, pgmData & (1<<k));
					break;
				case FONT_DIR_180:
					gdDrawPixel(_x - i, _y - (8 * j + k), pgmData & (1<<k));
					break;
				case FONT_DIR_270:
					gdDrawPixel(_x - (8 * j + k), _y + i, pgmData & (1<<k));
					break;
				}
			}
		}
	}
	switch (fp[FONT_DIRECTION]) {
	case FONT_DIR_0:
		gdSetXY(_x + swd, _y);
		break;
	case FONT_DIR_90:
		gdSetXY(_x, _y - swd);
		break;
	case FONT_DIR_180:
		gdSetXY(_x - swd, _y);
		break;
	case FONT_DIR_270:
		gdSetXY(_x, _y + swd);
		break;
	}

	return;
}

void gdWriteString(uint8_t *string)
{
	if (*string)
		gdWriteChar(*string++);
	while(*string) {
		gdWriteChar(fp[FONT_LTSPPOS]);
		gdWriteChar(*string++);
	}

	return;
}
