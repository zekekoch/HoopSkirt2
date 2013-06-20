#include <FastSPI_LED2.h>

//const int ledCount = 185;
const int ledCount = 95;
const int NUM_STRIPS = 3;
CRGB leds[NUM_STRIPS][ledCount];

int BOTTOM_INDEX = 0;
int TOP_INDEX = int(ledCount/2);
int EVENODD = ledCount%2;

//CRGB ledsX[ledCount]; //-ARRAY FOR COPYING WHATS IN THE LED STRIP CURRENTLY (FOR CELL-AUTOMATA, ETC)
//-PERISTENT VARS
byte idex = 0;        //-LED INDEX (0 to ledCount-1
byte ihue = 0;        //-HUE (0-360)
int ibright = 0;     //-BRIGHTNESS (0-255)
int isat = 0;        //-SATURATION (0-255)
bool bounceForward = true;  //-SWITCH FOR COLOR BOUNCE (0-1)
int bouncedirection = 0;
float tcount = 0.0;      //-INC VAR FOR SIN LOOPS
int lcount = 0;      //-ANOTHER COUNTING VAR


void setPixel(int adex, CRGB c) {
    for(int i = 0;i<NUM_STRIPS;i++)
    {
        leds[i][adex] = c;
    }
}

//-SET THE COLOR OF A SINGLE RGB LED
void setPixel(int adex, int cred, int cgrn, int cblu) {
    for(int i = 0;i<NUM_STRIPS;i++)
    {
        leds[i][adex] = CRGB(cred, cgrn, cblu);
    }
}

// fill_solid -   fill a range of LEDs with a solid color
void fillSolid(byte strand, const CRGB& color)
{
 fill_solid( leds[strand], ledCount, color);
}

void fillSolid(CRGB color)
{
    for(int i = 0;i<NUM_STRIPS;i++)
        fillSolid(i, color);
}

void fillSolid(int cred, int cgrn, int cblu) { //-SET ALL LEDS TO ONE COLOR
    fillSolid(0, CRGB(cred, cgrn, cblu));    
}

//-FIND INDEX OF HORIZONAL OPPOSITE LED
int horizontal_index(int i) {
    //-ONLY WORKS WITH INDEX < TOPINDEX
    if (i == BOTTOM_INDEX) {return BOTTOM_INDEX;}
    if (i == TOP_INDEX && EVENODD == 1) {return TOP_INDEX + 1;}
    if (i == TOP_INDEX && EVENODD == 0) {return TOP_INDEX;}
    return ledCount - i;
}


//-FIND INDEX OF ANTIPODAL OPPOSITE LED
int antipodal_index(int i) {
    //int N2 = int(ledCount/2);
    int iN = i + TOP_INDEX;
    if (i >= TOP_INDEX) {iN = ( i + TOP_INDEX ) % ledCount; }
    return iN;
}


//-FIND ADJACENT INDEX CLOCKWISE
int adjacent_cw(int i) {
    int r;
    if (i < ledCount - 1) {
        r = i + 1;
    }
    else {
        r = 0;
    }
    return r;
}


//-FIND ADJACENT INDEX COUNTER-CLOCKWISE
int adjacent_ccw(int i) {
    int r;
    if (i > 0 && i <= ledCount) {
        r = i - 1;
    }
    else {
        r = ledCount - 1;
    }
    return r;
}

void HSVtoRGB(int hue, int sat, int val, CRGB& c) {
    hsv2rgb_rainbow(CHSV(hue, sat, val), c);
}

CRGB HSVtoRGB(int hue, int sat, int val) {
    CRGB c;
    hsv2rgb_rainbow(CHSV(hue, sat, val), c);
    return c;
}


void setup()
{
  // For safety (to prevent too high of a power draw), the test case defaults to
  // setting brightness to 25% brightness
  LEDS.setBrightness(8);
  
  LEDS.addLeds<WS2811,4, GRB>(leds[0], ledCount);
  LEDS.addLeds<WS2811,7, GRB>(leds[1], ledCount);
  LEDS.addLeds<WS2811,10, GRB>(leds[2], ledCount);
      
  Serial.begin(57600);
  fillSolid(0,0,0); //-BLANK STRIP
  
  LEDS.show();
}

void nonReactiveFade() { //-BOUNCE COLOR (SIMPLE MULTI-LED FADE)
    static long lastBounceTime;
    const int bounceInterval = 250;
    
    static byte phase = 0;
    phase++;
    static byte trailLength = 5;
    if (phase % 8 == 0)
        trailLength = random(4,12);
    Serial.print("trailLength: ");Serial.println(trailLength);
    byte trailDecay = (255-64)/trailLength;
    Serial.print("trailDecay: ");Serial.println(trailDecay);
    static byte hue = random(0,10);

    fillSolid(HSVtoRGB(200, 255, 128));
    
    if (bounceForward) {
        Serial.print("bouncing forward");Serial.println(idex);
        for(int i = 0;i<trailLength;i++)
        {
            setPixel(adjacent_cw(idex-i), HSVtoRGB(hue, 255, 255 - trailDecay*i));
        }
        idex++;
        if (idex == ledCount) {
            bounceForward = !bounceForward;
            idex--;
        }
    } else {
        Serial.print("bouncing backwards idex:");Serial.print(idex);
        //todo: the trail is running off the array
        for(int i = 0;i<trailLength;i++)
        {
            setPixel(adjacent_ccw(idex+i), HSVtoRGB(hue, 255, 255 - trailDecay*i));
        }
        idex--;
        if (idex == 0) {
            bounceForward = !bounceForward;
        }
    }
    
}


int xAccel;
void loop()
{
    Serial.println("Hello World");
    Serial.println(sizeof(leds));
    
  nonReactiveFade();
  LEDS.setBrightness(64);
  //fillSolid(CRGB::HotPink);
  LEDS.show();
  //fillSolid(CRGB::Pink);
  delay(10);
  LEDS.show();
	// delay(delayTime);
}

/*
 void looped() {
 if (Serial.available())
 {
 lastInputLed = getLedFromSerial();
 }
 
 int delayTime = 75;
 // upper v
 clearLeds();
 race(0, 255, 50,70);
 race(0, 255, 0, 20);
 FastSPI_LED.show();
 delay(delayTime);
 
 clearLeds();
 //lower v
 setRange(0, 128, 137,167);
 setRange(0, 128, 70, 99);
 FastSPI_LED.show();
 
 delay(delayTime);
 clearLeds();
 FastSPI_LED.show();
 delay(delayTime);
 
 //bottom ring
 setRange(0, 255, 369,396);
 setRange(0, 255, 396, 421);
 FastSPI_LED.show();
 delay(delayTime);
 
 clearLeds();
 //middle ring
 setRange(0, 255, 281,307);
 setRange(0, 255, 307, 331);
 FastSPI_LED.show();
 delay(delayTime);
 
 clearLeds();
 //upper ring
 setRange(0, 255, 199,222);
 setRange(0, 128, 222, 245);
 FastSPI_LED.show();
 delay(delayTime);
 
 //  leds[lastInputLed].r = 255;
 //  leds[lastInputLed].g = 0;
 //  leds[lastInputLed].b = 0;
 
 FastSPI_LED.show();
 }
 
 void playFrame()
 {
 // check time
 // clear leds
 clearLeds();
 // loop over the list of animations
 for (int i = 0;i<animationCount;i++)
 {
 //    animations[i]
 }
 // show the leds
 // delay until the next frame
 }
 
 
 
 int getLedFromSerial() {
 // if there's any serial available, read it:
 while (Serial.available() > 0) {
 
 // look for the next valid integer in the incoming serial stream:
 int iLed = Serial.parseInt();
 // look for the newline. That's the end of your
 // sentence:
 if (Serial.read() == '\n') {
 // constrain the values to 0 - 255 and invert
 // if you're using a common-cathode LED, just use "constrain(color, 0, 255);"
 iLed = constrain(iLed, 0, ledCount);
 Serial.print("led ");
 Serial.println(iLed);
 
 return iLed;
 }
 }
 }
 
 void colorFromAccelerormeter()
 {
 byte x = analogRead(A0);
 
 clearLeds();
 for (int i = 0;i < ledCount;i++)
 {
 if (x > 128)
 leds[i].r = x;
 else
 leds[i].b = x;
 }
 }
 
 void race(byte color, byte intensity, int first, int last)
 {
 for(int i = first;i< last;i++)
 {
 leds[i].r = intensity;
 }
 }
 
 void setRange(byte color, byte intensity, int first, int last)
 {
 for (int i = first; i < last;i++)
 {
 switch(color)
 {
 case 0:
 leds[i].r = intensity;
 break;
 case 1:
 leds[i].g = intensity;
 break;
 case 2:
 leds[i].b = intensity;
 break;
 }
 }
 
 }
 
 
 void progress() {
 for (int frame = 0;frame < ledCount;frame++)
 {
 clearLeds();
 for(int i = frame; i < ledCount; i++ )
 {
 leds[i].r = 255;
 leds[i].g = 0;
 leds[i].b = 0;
 }
 FastSPI_LED.show();
 delay(5);
 }
 }
 */