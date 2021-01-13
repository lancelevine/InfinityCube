#include <FastLED.h>

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN1    7
#define DATA_PIN2    6
#define DATA_PIN3    5
#define DATA_PIN4    4
#define DATA_PIN5    3
#define DATA_PIN6    2
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    72
CRGB leds[NUM_LEDS*6];
char incoming_command = "h";
char incoming_command2 = "o";

#define BRIGHTNESS          220
#define FRAMES_PER_SECOND  120

void setup() {
  delay(3000); // 3 second delay for recovery
  Serial.begin(9600);
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN1,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,DATA_PIN2,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,DATA_PIN3,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,DATA_PIN4,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,DATA_PIN5,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,DATA_PIN6,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  Serial.println("fuck");
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
//SimplePatternList gPatterns = { phaser, phaser_colors2, phaser_colors4 };
//SimplePatternList gPatterns = { phaser, phaser2, phaser3 };
SimplePatternList gPatterns = { juggle2, juggle, juggle3, sinelon, rainbow, confetti, sinelon, juggle, bpm };
//SimplePatternList gPatterns = { pulser2 };

//SimplePatternList gPatterns = { juggle3 };
//SimplePatternList gPatterns = { cylon, cylon, cylon, cylon, sinelon, juggle };
//SimplePatternList gPatterns = { rainbow };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

uint8_t gHueN = 0; // rotating "base color" used by many of the patterns
uint8_t speedN = 3; // rotating "base color" used by many of the patterns
uint8_t fadeN = 20; // rotating "base color" used by many of the patterns

void fadeall(int fadefactor) { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(fadefactor); } }
  
int duplicates = 10;//50
int delaytime = 1;
int starthue = 128;
int huemultiplyer = 64;
bool reverse = false;
bool reversedirection = false;
byte counter = 0;
int brightness = BRIGHTNESS;
int fps = FRAMES_PER_SECOND;

void loop()
{
  //check_for_input();
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  FastLED.setBrightness(brightness);

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/fps); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; starthue += 16; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 5 ) {gHueN+=16; speedN=random8(10); } // change patterns periodically
  EVERY_N_SECONDS( 10 ) { nextPattern(); reversedirection = !reversedirection;fadeN=random8(15,25);} // change patterns periodically
}

void check_for_input() {
  //if (animation_change_timeout > 100) {
    if (Serial.available() > 0) {
      // read the incoming byte:
      //incoming_command = Serial.read();
      //incoming_command2 = Serial.read();
      char command = Serial.read();

      //String command = Serial.readString();
      //char command[2];
      //command[0] = incoming_command;
      //command[1] = incoming_command2;
      
      // say what you got:
      //Serial.print("I received: ");
      Serial.println(command);
      //Serial.print(incoming_command);
      //Serial.println(incoming_command2);
      //animate = false;

      int brightnessfactor = 10;
      int fpsfactor = 20;

      switch(command) {
        case 'a':
        if((brightness+brightnessfactor) < 255)
        brightness += brightnessfactor;
        break;
        case 'b':
        if ((brightness-brightnessfactor) > 0)
        brightness -= brightnessfactor;
        break;
        case 'c':
        fps += fpsfactor;
        break;
        case 'd':
        if((fps-fpsfactor) > 10)
        fps -= fpsfactor;
        break;
      }
      Serial.print("Brightness: ");
      Serial.println(brightness);
      Serial.print("fps: ");
      Serial.println(fps);
    }
  //}
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  if((gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns) == 0) {
    // modifications before restarting the loop
    duplicates++;
  }
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void fadetorainbow()
{
  int fadeamount = 1000;
  CRGB templeds[NUM_LEDS];
  fill_rainbow( templeds, NUM_LEDS, 0, 7);
  for( int i = 0; i < NUM_LEDS; i++) {
    if(leds[i].r < templeds[i].r) {
      leds[i].r = leds[i].r + ((templeds[i].r-leds[i].r)/FRAMES_PER_SECOND); //t=128 leds = 64 64+64/120
    } else {
      leds[i].r = leds[i].r - ((leds[i].r-templeds[i].r)/FRAMES_PER_SECOND);
    }
    if(leds[i].g < templeds[i].g) {
      leds[i].g = leds[i].g + ((templeds[i].g-leds[i].g)/FRAMES_PER_SECOND); //t=128 leds = 64 64+64/120
    } else {
      leds[i].g = leds[i].g - ((leds[i].g-templeds[i].g)/FRAMES_PER_SECOND);
    }
    if(leds[i].r < templeds[i].r) {
      leds[i].b = leds[i].b + ((templeds[i].b-leds[i].b)/FRAMES_PER_SECOND); //t=128 leds = 64 64+64/120
    } else {
      leds[i].b = leds[i].b - ((leds[i].b-templeds[i].b)/FRAMES_PER_SECOND);
    }
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 26, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void juggle2() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, fadeN);
  byte dothue = gHueN;
  byte dotsat = 255;
  if((gHue % 8 ) == 0){
    dothue += 8;
  }
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+speedN, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 128;
  }
}

void juggle3() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, fadeN);
  byte dothue = gHueN;
  byte dotsat = 255;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(random8(255), 200, 255);
    dothue += 128;
  }
}

void cubey() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  byte dotsat = 255;
  for( int i = 0; i < 6; i++) {
    for ( int j = 0; j < NUM_LEDS; j++ ) {
          leds[150] = CHSV(dothue, 200, 255);
    }
        dothue += 32;
  }
}

void phaser() {
  static uint8_t hue = 0;
  // First slide the led in one direction
  for(int i = 0; i < NUM_LEDS; i++) {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show(); 
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } //fade all
    // Wait a little bit before we loop around and do it again
    delay(beatsin16( 26, 0, 10 ));
  }
}

void phaser_colors2() {
  static uint8_t hue = 0;
  // First slide the led in one direction
  for(int i = 0; i < NUM_LEDS; i = i + 1) {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    hue = hue + 128;
    // Show the leds
    FastLED.show(); 
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } //fade all
    // Wait a little bit before we loop around and do it again
    delay(beatsin16( 26, 0, 10 ));
  }
}

void phaser_colors4() {
  static uint8_t hue = 0;
  // First slide the led in one direction
  for(int i = 0; i < NUM_LEDS; i = i + 1) {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    hue = hue + 64;
    // Show the leds
    FastLED.show(); 
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } //fade all
    // Wait a little bit before we loop around and do it again
    delay(beatsin16( 26, 0, 10 ));
  }
}

void phaser2() {
  static uint8_t hue = 0;
  // First slide the led in one direction
  for(int i = 0; i < NUM_LEDS; i = i + 2) {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show(); 
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } //fade all
    // Wait a little bit before we loop around and do it again
    delay(beatsin16( 26, 0, 10 ));
  }
}

void phaser3() {
  static uint8_t hue = 0;
  // First slide the led in one direction
  for(int i = 0; i < NUM_LEDS; i = i + 3) {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show(); 
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } //fade all
    // Wait a little bit before we loop around and do it again
    delay(beatsin16( 26, 0, 10 ));
  }
}

void shimmer() {
  static uint8_t hue = 0;
  int fillamount = 18;
  // First slide the led in one direction
  for(int j = 0; j < 4; j++) {
    // Set the i'th led to red 
    for(int i = NUM_LEDS; i > NUM_LEDS - random8(8); i--) {
    leds[i-(j*NUM_LEDS/4)] = CRGB(255,255,255);//CHSV(hue + (NUM_LEDS - i)*8, 255, 255);
    }
    for(int i = 0; i < random8(8); i++) {
    leds[i+(j*NUM_LEDS/4)] = CRGB(255,255,255);//CHSV(hue + i*8, 255, 255);
    }
  }
    for(int j = 0; j < 4; j++) {
    // Set the i'th led to red 
    for(int i = NUM_LEDS; i > NUM_LEDS - random8(7); i--) {
    leds[i-(j*NUM_LEDS/4)- 9] = CRGB(255,0,0);//CHSV(hue + (NUM_LEDS - i)*8, 255, 255);
    }
    for(int i = 0; i < random8(7); i++) {
    leds[i+(j*NUM_LEDS/4)+ 9] = CRGB(255,0,0);//CHSV(hue + i*8, 255, 255);
    }
  }
      FastLED.show(); 
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(220); } //fade all
    // Wait a little bit before we loop around and do it again
    //delay(beatsin16( 26, 0, 10 ));
    delay(50);
    if(fillamount == 0){
      fillamount == 18;
    } else if(fillamount == 18) {
      fillamount = 0;
    }
    hue++;  
}

void shimmer2() {
  static uint8_t hue = 0;
  // First slide the led in one direction
  for(int j = 0; j < 4; j++) {
    // Set the i'th led to red 
    for(int i = NUM_LEDS; i > NUM_LEDS - random8(14); i--) {
    // Set the i'th led to red 
    leds[i-(j*NUM_LEDS/4)] = CHSV(hue + (NUM_LEDS - i)*8, 255, 255);
    }
    for(int i = 0; i < random8(14); i++) {
    // Set the i'th led to red 
    leds[i+(j*NUM_LEDS/4)] = CHSV(hue + i*8, 255, 255);
    }

  }
      FastLED.show(); 
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(220); } //fade all
    // Wait a little bit before we loop around and do it again
    //delay(beatsin16( 26, 0, 10 ));
    delay(50);
    hue++;  
}

void pulser() {
  static uint8_t hue = 0;
    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 4; j++) {
          leds[i+(j*NUM_LEDS/4)] = CHSV(hue + i*8, 255-i*10, 255-i*20);
          leds[(NUM_LEDS-i)-(j*NUM_LEDS/4)] = CHSV(hue + i*8, 255-i*10, 255-i*20);
        }
      FastLED.show(); 
      for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(200); } //fade all
      delay(70);
    }
    hue+=32;  
}

  DEFINE_GRADIENT_PALETTE( heatmap_gp ) {
  0,     0,  0,  0,   //black
128,   255,  0,  0,   //red
224,   255,255,  0,   //bright yellow
255,   255,255,255 }; //full white

  DEFINE_GRADIENT_PALETTE( heatmap_gp2 ) {
  0,     0,  0,  255,   //black
64,   0,  128,  255,   //blue
128,   255,0,  255,   //bright yellow
192,   255,255,255 }; //full white

void pulser2() {

CRGBPalette16 myPal = heatmap_gp2;

  
  static uint8_t hue = 0;
    for(int i = 0; i < 19; i++) {
        for(int j = 0; j < 4; j++) {
          //leds[i+(j*NUM_LEDS/4)] = ColorFromPalette( myPal, hue + i*8 + j*64); // normal palette access
          //leds[(NUM_LEDS-i)-(j*NUM_LEDS/4)] = ColorFromPalette( myPal, hue + i*8 + j * 64); // normal palette access
          leds[i+(j*NUM_LEDS/4)] = CHSV(hue + i*8 + j*64, 255-i*5, 255-i*10);
          leds[(NUM_LEDS-i)-(j*NUM_LEDS/4)] = CHSV(hue + i*8 + j * 64, 255-i*5, 255-i*10);
        }
      FastLED.show(); 
      for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(200); } //fade all
      delay(50);
    }
    hue+=32;  
}

void cylon() {
  cylon(duplicates, starthue, huemultiplyer, delaytime);
}

void cylon(int duplicates, int hue, int huemultiplyer, int delaytime) {
  // First slide the led in one direction
  for(int i = 0; i < NUM_LEDS/duplicates; i++) {
    // Set the i'th led to red 
    for (int j = 0; j < duplicates; j++) {
      int lednum = i + (NUM_LEDS/duplicates) * j;
      if(reversedirection) { lednum = NUM_LEDS - lednum - 1; }
      //leds[lednum] = CHSV(hue + random8(4*j,64 + 4*j)/*(j*random8(4,12))*//*+= (huemultiplyer)*/, 255, 255);
      leds[2] = CHSV(hue + random8(4,64)/*(j*random8(4,12))*//*+= (huemultiplyer)*/, 255, 255);
      //leds[i + (NUM_LEDS/duplicates) * j] = CHSV(hue += (8 * j), 255, 255);

    }
    // Show the leds
    FastLED.show(); 
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    //fadeall(200 + (i*1));
    fadeall(random8(220,230));//random8(170,230));
    // Wait a little bit before we loop around and do it again
    //delay(delaytime);
    delay(1);//random8(1,7));
  }
}
