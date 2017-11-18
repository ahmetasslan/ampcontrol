#ifndef PINS_H
#define PINS_H

#include <avr/io.h>

#define CONCAT(x,y)         x ## y

#define DDR(x)              CONCAT(DDR,x)
#define PORT(x)             CONCAT(PORT,x)
#define PIN(x)              CONCAT(PIN,x)

/* Display data port*/
#define DISP_D0             D
#define DISP_D0_LINE        (1<<6)
#define DISP_D1             D
#define DISP_D1_LINE        (1<<7)
#define DISP_D2             B
#define DISP_D2_LINE        (1<<0)
#define DISP_D3             B
#define DISP_D3_LINE        (1<<1)
#define DISP_D4             B
#define DISP_D4_LINE        (1<<2)
#define DISP_D5             B
#define DISP_D5_LINE        (1<<3)
#define DISP_D6             B
#define DISP_D6_LINE        (1<<4)
#define DISP_D7             B
#define DISP_D7_LINE        (1<<5)
/* Display control port*/
#define DISP_DATA           C
#define DISP_DATA_LINE      (1<<1)
#define DISP_STROB          C
#define DISP_STROB_LINE     (1<<0)
#define DISP_CTRL1          D
#define DISP_CTRL1_LINE     (1<<4)
#define DISP_CTRL2          D
#define DISP_CTRL2_LINE     (1<<3)
/* Display backlight port */
#define DISP_BCKL           D
#define DISP_BCKL_LINE      (1<<5)

/* Remote control definitions*/
#define RC                  D
#define RC_LINE             (1<<2)

/* Standby/Mute port definitions */
#define STMU_MUTE           C
#define STMU_MUTE_LINE      (1<<2)
#define STMU_STBY           C
#define STMU_STBY_LINE      (1<<3)

/* KS0108 Data port*/
#define KS0108_D0           DISP_D0
#define KS0108_D0_LINE      DISP_D0_LINE
#define KS0108_D1           DISP_D1
#define KS0108_D1_LINE      DISP_D1_LINE
#define KS0108_D2           DISP_D2
#define KS0108_D2_LINE      DISP_D2_LINE
#define KS0108_D3           DISP_D3
#define KS0108_D3_LINE      DISP_D3_LINE
#define KS0108_D4           DISP_D4
#define KS0108_D4_LINE      DISP_D4_LINE
#define KS0108_D5           DISP_D5
#define KS0108_D5_LINE      DISP_D5_LINE
#define KS0108_D6           DISP_D6
#define KS0108_D6_LINE      DISP_D6_LINE
#define KS0108_D7           DISP_D7
#define KS0108_D7_LINE      DISP_D7_LINE
/* KS0108 control port*/
#define KS0108_DI           DISP_DATA
#define KS0108_DI_LINE      DISP_DATA_LINE
#define KS0108_E            DISP_STROB
#define KS0108_E_LINE       DISP_STROB_LINE
#define KS0108_CS1          DISP_CTRL1
#define KS0108_CS1_LINE     DISP_CTRL1_LINE
#define KS0108_CS2          DISP_CTRL2
#define KS0108_CS2_LINE     DISP_CTRL2_LINE
/* KS0108 Backlight port */
#define KS0108_BCKL         DISP_BCKL
#define KS0108_BCKL_LINE    DISP_BCKL_LINE

/* ST7920 Data port*/
#define ST7920_D0           DISP_D0
#define ST7920_D0_LINE      DISP_D0_LINE
#define ST7920_D1           DISP_D1
#define ST7920_D1_LINE      DISP_D1_LINE
#define ST7920_D2           DISP_D2
#define ST7920_D2_LINE      DISP_D2_LINE
#define ST7920_D3           DISP_D3
#define ST7920_D3_LINE      DISP_D3_LINE
#define ST7920_D4           DISP_D4
#define ST7920_D4_LINE      DISP_D4_LINE
#define ST7920_D5           DISP_D5
#define ST7920_D5_LINE      DISP_D5_LINE
#define ST7920_D6           DISP_D6
#define ST7920_D6_LINE      DISP_D6_LINE
#define ST7920_D7           DISP_D7
#define ST7920_D7_LINE      DISP_D7_LINE
/* ST7920 control port*/
#define ST7920_RS           DISP_DATA
#define ST7920_RS_LINE      DISP_DATA_LINE
#define ST7920_E            DISP_STROB
#define ST7920_E_LINE       DISP_STROB_LINE
#define ST7920_PSB          DISP_CTRL1
#define ST7920_PSB_LINE     DISP_CTRL1_LINE
/* ST7920 Backlight port */
#define ST7920_BCKL         DISP_BCKL
#define ST7920_BCKL_LINE    DISP_BCKL_LINE

/* ST7920 port*/
#define SSD1306_SCK         DISP_STROB
#define SSD1306_SCK_LINE    DISP_STROB_LINE
#define SSD1306_SDA         DISP_DATA
#define SSD1306_SDA_LINE    DISP_DATA_LINE

#endif /* PINS_H */
