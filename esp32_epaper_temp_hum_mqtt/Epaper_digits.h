#ifndef Epaper_digits
#define Epaper_digits

#include <Arduino.h>

extern const int digits_font_width;
extern const int digits_font_high;
extern const int digits_dot_width;
extern const int digits_celc_width;
extern const int digits_perc_width;

extern const unsigned char gImage_dot[];
extern const unsigned char gImage_celc[];
extern const unsigned char gImage_perc[];
extern const unsigned char gImage_temp[];
extern unsigned char gImage_digits[10][583];

enum Display_type {
  TEMPERATURE,
  HUMIDITY
};


class EpaperBigDigits {
private:
  void initEpaper();
  void drawDigit(int digit, int *x, int y, int font_high, int font_width);
  void drawSymbol(int *x, int y, int font_high, int font_width, const unsigned char gImage[]);
  void calc_digits(int digits[4], int *position, int *rest);
  void drawDigits(int x, int y, int digits[], Display_type display_type);
public:
  void digits_init(void);
  void clear_display();
  void drawFloat(int x, int y, float value, Display_type display_type);
};


#endif
