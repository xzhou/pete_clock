#include "FastLED.h"  // library controlling LED
#include "DS3231.h"   // library controlling RTC clock module
#include "TimeLib.h"

#define NUM_LEDS 50
#define DATA_PIN 6

// led strip
CRGB leds[NUM_LEDS];
// RTC module
DS3231  rtc(SDA, SCL);

const char *MonthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

bool getSystemDateAndTime(Time& t) {
  // Get Date
  char month[12];
  int day, year;

  if (sscanf(__DATE__, "%s %d %d", month, &day, &year) != 3)
    return false;
  // Find month index
  int i;
  for (i = 0; i < 12; i ++) {
    if (strcmp(month, MonthName[i]) == 0)
    break;
  }

  if (i >= 12) return false;
  t.date = day;
  t.mon = i + 1;
  t.year = year;

  // Get Time
  int hour, min, sec;
  if (sscanf(__TIME__, "%d:%d:%d", &hour, &min, &sec) != 3)
    return false;

  t.hour = hour;
  t.min = min;
  t.sec = sec;
  
  return true;
}

// Get the information from computer and set the time for RTC module.
void setDateAndTime() {
  Serial.print("hello");
  Time t;
  if (getSystemDateAndTime(t)) {
    Serial.print(t.hour);
    Serial.print(t.min);
    Serial.print(t.sec);
    rtc.setTime(t.hour, t.min, t.sec);
    rtc.setDate(t.date, t.mon, t.year);
  }
}

void decToBinary(int n, int result[], int r_size) {
  int i = 0;
  while (n > 0 && i < r_size) {
    result[r_size - i - 1] = n % 2;
    n /= 2;
    i ++;
  }
// Debug
//  Serial.print("=");
//  for (int k = 0; k < r_size; k++) {
//    Serial.print(result[k]);
//  }
//  Serial.print("\n");
}

// 4 bit representation
void setHour(int h) {
  if (h > 23) {
    return;
  }
  if (h > 12) {
    h -= 12;
  }
  const int HOUR_BITS = 4;
  int bin_hour[HOUR_BITS] = {};
  decToBinary(h, bin_hour, HOUR_BITS);
  for (int i = 0; i < HOUR_BITS; i ++) {
    leds[i] = bin_hour[i] == 0 ? CRGB::Black : CRGB::Red;
  }
}

void setMin(int m) {
  if (m > 60) {
    return;
  }
  const int MIN_BITS = 6;
  int bin_min[MIN_BITS] = {};
  decToBinary(m, bin_min, MIN_BITS);
  for (int i = 0; i < MIN_BITS; i ++) {
    leds[i + 4] = bin_min[i] == 0 ? CRGB::Black : CRGB::Green;
  }
}

void setSec(int s) {
  if (s > 60) {
    return;
  }
  const int SEC_BITS = 6;
  int bin_sec[SEC_BITS] = {};
  decToBinary(s, bin_sec, SEC_BITS);
  for (int i = 0; i < SEC_BITS; i ++) {
    leds[i + 10] = bin_sec[i] == 0 ? CRGB::Black : CRGB::Blue;
  }
}

void showTime(Time t) {
  setHour(t.hour);
  setMin(t.min);
  setSec(t.sec);
  FastLED.show();
}

void setup() {
  rtc.begin();
  delay(200);
  Serial.begin(115200);
  setDateAndTime();
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

void loop() {
    Time t = rtc.getTime();
    Serial.print(rtc.getTimeStr());
    Serial.print("\n");
    showTime(t);
    delay(1000);
}
