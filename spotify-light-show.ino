#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

void stripFill(uint8_t r, uint8_t g, uint8_t b);
void stripFillModulo(uint8_t r, uint8_t g, uint8_t b, uint16_t m = 4);
void stripFade(uint8_t r, uint8_t g, uint8_t b);
void stripFlow(uint8_t r, uint8_t g, uint8_t b);
void stripDrop(uint8_t r, uint8_t g, uint8_t b);
void stripBoop(uint8_t r, uint8_t g, uint8_t b);

#define LED_STRIP_PIN 14

ESP8266WiFiMulti WiFiMulti;

const uint16_t MAX_BEATS_LENGTH = 1000;
long beats[MAX_BEATS_LENGTH];

const uint16_t MAX_SECTIONS_LENGTH = 200;
long sections[MAX_SECTIONS_LENGTH];

const uint8_t NUM_SECTION_COLORS = 4; // how many different colors for sections
const uint8_t rs[NUM_SECTION_COLORS] = {0, 0, 0, 30};
const uint8_t gs[NUM_SECTION_COLORS] = {30, 30, 0, 0};
const uint8_t bs[NUM_SECTION_COLORS] = {30, 0, 30, 0};

void (*ledEffects[4])(uint8_t r, uint8_t g, uint8_t b) = {stripBoop,stripBoop,stripBoop,stripBoop};//{stripFill, stripFade, stripFlow, stripBoop};

// TODO rewrite with uint_32 etc

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(120, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.


void setup() {
  Serial.begin(115200);
  Serial.println("connecting");

  WiFiMulti.addAP("MIT GUEST", "");
  while (WiFiMulti.run() != WL_CONNECTED) delay(100);

  Serial.println("Connected!");

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  unsigned long startTime = millis();
  unsigned long currentTime = 0;
  
  int numBeats = getBeats(beats);
  int numSections = getSections(sections);
  int lastBeatIndex = 0;
  for (int i = 0; i < numBeats; i++) {
    Serial.println(beats[i]);
  }
//  int currentTime = getCurrentTime();

  while (true) {

    // resync every 1000 ms
    long gotCurrentTime = getCurrentTime();
    if (gotCurrentTime != -1) {
      Serial.println("resync");
      currentTime = gotCurrentTime;
      startTime = millis();
    }

    for (int i = 0; i < 5000; i++) {
      int beatIndex = getIndex(beats, millis() - startTime + currentTime, numBeats);
      int sectionIndex = getIndex(sections, millis() - startTime + currentTime, numSections);

      if (beatIndex > lastBeatIndex) {
        // write a color to the strip, because we have a new beat
//        stripFillModulo(rs[section % NUM_SECTION_COLORS], gs[section % NUM_SECTION_COLORS], bs[section % NUM_SECTION_COLORS]);
        uint8_t r = rs[sectionIndex % NUM_SECTION_COLORS];
        uint8_t g = gs[sectionIndex % NUM_SECTION_COLORS];
        uint8_t b = bs[sectionIndex % NUM_SECTION_COLORS];
        ledEffects[sectionIndex % 4](r, g, b); // w0w change magic number!!
        lastBeatIndex = beatIndex;
      } else {
//        stripFillModulo(0, 0, 0);
        ledEffects[sectionIndex % 4](0, 0, 0); // w0w change magic number!!
      }
      delay(10);
//      delay(30);
    }
  }
}

// refactor
int getBeats(long *beats) {
//void getBeats() {
  HTTPClient http;
  String json;

  http.begin("umbreon.mit.edu", 80, "/beats");
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    json = http.getString();
  } else {
    Serial.println("error");
    return -1;
  }

  http.end();

  // parse a JSON array. really sketchy
  json = json.substring(1, json.length() - 1);

  int i = 0;
  int j = 0;
  while (i < json.length()) {
    int delimiterIndex = json.indexOf(',', i);
    if (delimiterIndex < 0) break;
    beats[j] = json.substring(i, json.indexOf(',', i)).toInt();
    i = delimiterIndex + 1;
    j++;
  }

  return j;
}

int getCurrentTime() {
  long currentTime = -1;
  long fetchStart = millis();
  HTTPClient http;
  String json;

  http.begin("umbreon.mit.edu", 80, "/time");
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    currentTime = http.getString().toInt();
  } else {
    Serial.println("error");
  }

  http.end();
  return currentTime + millis() - fetchStart;
}

// refactor
int getSections(long *sections) {
  HTTPClient http;
  String json;

  http.begin("umbreon.mit.edu", 80, "/sections");
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    json = http.getString();
  } else {
    Serial.println("error");
    return -1;
  }

  http.end();

  // parse a JSON array. really sketchy
  json = json.substring(1, json.length() - 1);

  int i = 0;
  int j = 0;
  while (i < json.length()) {
    int delimiterIndex = json.indexOf(',', i);
    if (delimiterIndex < 0) break;
    sections[j] = json.substring(i, json.indexOf(',', i)).toInt();
    i = delimiterIndex + 1;
    j++;
  }

  return j;
}

int getIndex(long *a, long x, int length) {  // get the first index of a greater than x
  for (int i = 0; i < length; i++) {
    if (a[i] > x) return i;
  }

  return length + 1;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// Fill strip with one color
// sketchy. REFACTOR
//void stripFill(uint32_t c) {
void stripFill(uint8_t r, uint8_t g, uint8_t b) { // add float d for decay?
  static uint16_t brightness; // uint8_t or uint16_t?
  static uint8_t last_r;
  static uint8_t last_g;
  static uint8_t last_b;

//  if ((r || g || b) && last_r != r || last_g != g || last_b != b) {
  if (r || g || b) {
    brightness = 255;
    last_r = r;
    last_g = g;
    last_b = b;
  } else {
    brightness = brightness * 9 / 10;
  }

  uint32_t color = strip.Color(last_r * brightness / 255, last_g * brightness / 255, last_b * brightness / 255);

  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
  }

  strip.show();
}

// Fill strip with one color
// sketchy. REFACTOR
//void stripFill(uint32_t c) {
void stripFillModulo(uint8_t r, uint8_t g, uint8_t b, uint16_t m) { // add float d for decay?
  static uint16_t brightness; // uint8_t or uint16_t?
  static uint16_t modulo = 0;
  static uint8_t last_r;
  static uint8_t last_g;
  static uint8_t last_b;

//  if ((r || g || b) && last_r != r || last_g != g || last_b != b) {
  if (r || g || b) {
    brightness = 255;
    last_r = r;
    last_g = g;
    last_b = b;
   modulo = (modulo + 1) % m;
  } else {
    brightness = brightness * 9 / 10;
  }

  uint32_t color = strip.Color(last_r * brightness / 255, last_g * brightness / 255, last_b * brightness / 255);

  strip.clear();

  for (uint16_t i = 0; i < strip.numPixels(); i++) {  // optimize?
    if (i % m == modulo) {
      strip.setPixelColor(i, color);
    }
  }

  strip.show();
}

// this does something interesting
void stripFillModulo1(uint8_t r, uint8_t g, uint8_t b, uint16_t m) { // add float d for decay?
  static uint16_t brightness; // uint8_t or uint16_t?
  static uint16_t modulo = 0;
  static uint8_t last_r;
  static uint8_t last_g;
  static uint8_t last_b;

  modulo = (modulo + 1) % m;

//  if ((r || g || b) && last_r != r || last_g != g || last_b != b) {
  if (r || g || b) {
    brightness = 255;
    last_r = r;
    last_g = g;
    last_b = b;
  } else {
    brightness = brightness * 9 / 10;
  }

  uint32_t color = strip.Color(last_r * brightness / 255, last_g * brightness / 255, last_b * brightness / 255);

  strip.clear();

  for (uint16_t i = 0; i < strip.numPixels(); i++) {  // optimize?
    if (i % m == modulo) {
      strip.setPixelColor(i, color);
    }
  }

  strip.show();
}

// Fill strip with one color
// sketchy. REFACTOR
//void stripFill(uint32_t c) {
void stripFade(uint8_t r, uint8_t g, uint8_t b) { // don't fade completely
  static uint16_t brightness; // uint8_t or uint16_t?
  static uint8_t last_r;
  static uint8_t last_g;
  static uint8_t last_b;

//  if ((r || g || b) && last_r != r || last_g != g || last_b != b) {
  if (r || g || b) {
    brightness = 255;
    last_r = r;
    last_g = g;
    last_b = b;
  } else {
    brightness = _max(brightness * 9 / 10, 10);
  }

  uint32_t color = strip.Color(last_r * brightness / 255, last_g * brightness / 255, last_b * brightness / 255);

  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
  }

  strip.show();
}

// Fill strip with one color
// sketchy. REFACTOR
//void stripFill(uint32_t c) {
void stripFlow(uint8_t r, uint8_t g, uint8_t b) { // doesn't decay completely
  const uint16_t COLORS_MAX_SIZE = 120; // REFACTOR; this needs to change with number of LED's
  static uint32_t colors[COLORS_MAX_SIZE];
  static uint16_t colorsIndex = 0;

  colorsIndex = colorsIndex ? colorsIndex - 1 : COLORS_MAX_SIZE - 1;

//  if ((r || g || b) && last_r != r || last_g != g || last_b != b) {
  if (r || g || b) {
    colors[colorsIndex] = strip.Color(r, g, b);
  } else {
    colors[colorsIndex] = 0;
  }

  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, colors[(i + colorsIndex) % COLORS_MAX_SIZE]);
  }

  delay(20);

  strip.show();
}

// sketchy. REFACTOR
//void stripFill(uint32_t c) {
void stripDrop(uint8_t r, uint8_t g, uint8_t b) { 
  const uint16_t COLORS_MAX_SIZE = 120; // REFACTOR; this needs to change with /HALF/ the number of LED's
  static uint32_t colors[COLORS_MAX_SIZE];
  static uint16_t colorsIndex = 0;

  colorsIndex = colorsIndex ? colorsIndex - 1 : COLORS_MAX_SIZE - 1;

  if (r || g || b) {
    colors[colorsIndex] = strip.Color(r, g, b);
  } else {
    colors[colorsIndex] = 0;
  }

  uint16_t halfNumPixels = strip.numPixels() / 2;
  for (uint16_t i = 0; i < halfNumPixels; i++) {    // TODO: what if there's an odd number of pixels?
    strip.setPixelColor(i + halfNumPixels, colors[(i + colorsIndex) % COLORS_MAX_SIZE]);
    strip.setPixelColor(halfNumPixels - i, colors[(i + colorsIndex) % COLORS_MAX_SIZE]);
  }

  strip.show();
}

void stripBoop(uint8_t r, uint8_t g, uint8_t b) {   // what are these names
  static uint16_t boopRadius; // uint8_t or uint16_t?
  static uint8_t last_r;
  static uint8_t last_g;
  static uint8_t last_b;
  static uint8_t frameCount = 0;
  static uint8_t numBeats = 0;
  

  uint16_t numBoopsAlongStrip = 10;
  uint16_t boopSegmentSize = strip.numPixels()/numBoopsAlongStrip;
  
//  if ((r || g || b) && last_r != r || last_g != g || last_b != b) {
  if (r || g || b) {
    boopRadius = boopSegmentSize/2;
    last_r = r;
    last_g = g;
    last_b = b;
    numBeats ++;
  } else {
    frameCount++;
    if(frameCount % 3 == 0){
      boopRadius = _max(boopRadius - 1, 0);
    }
  }
  uint32_t color = strip.Color(last_r, last_g, last_b);

  for (uint16_t i = 0; i < strip.numPixels(); i++) {    // TODO: what if there's an odd number of pixels?
    uint16_t localBoopCoordinate = i % boopSegmentSize;
    if(numBeats % 2 == 0){
      strip.setPixelColor(i, abs(localBoopCoordinate - boopSegmentSize/2) < boopRadius? color : 0);
    }
    else{
      strip.setPixelColor(i, abs(localBoopCoordinate - boopSegmentSize/2) > (boopSegmentSize/2 - boopRadius)? color : 0);
    }
  }
  strip.show();
}
