#include <FastSPI_LED2.h>

//const int ledCount = 185;
const int ledCount = 134;
const int actualLedCount = 84;
const int NUM_STRIPS = 2;
//CRGB leds[ledCount];
CRGB actualLeds[NUM_STRIPS][actualLedCount];

const byte hoopStart = 0;
const byte hoopEnd = 1;
byte hoop[3][2] = 
{ 
  {0,49},
  {50,89},
  {90,133}
};


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


void assert(bool eval, String errorMessage)
{
  if(!eval)
    Serial.println(errorMessage);
}

void setPixel(int currentPixel, CRGB c) 
{
  if(currentPixel < 0)
    actualLeds[1][ledCount-currentPixel] = c;
//  assert(currentPixel < ledCount, "currentPixel > ledCount");
  if(currentPixel < 50)
  {
//    Serial.print("leds[0][");Serial.print(currentPixel);Serial.print("] = ");Serial.println(c);
    actualLeds[0][currentPixel] = c;
  } 
  else
  {
//    Serial.println(currentPixel);
//    Serial.print("leds[1][");Serial.print(currentPixel-actualLedCount);Serial.print("] = ");Serial.println(c);
    actualLeds[1][currentPixel-50] = c;
  }
} 

CRGB getPixel(int currentPixel)
{
  //Serial.print("current pixel ");Serial.println(currentPixel);
  if(currentPixel < 0)
    return actualLeds[1][ledCount-currentPixel];

  if(currentPixel < 50)
  {
    return actualLeds[0][currentPixel];
  } 
  else
  {
    return actualLeds[1][currentPixel-50];
  }
}

int nextThird(int i) {
    int iN = i + (int)(ledCount / 3);
    if (iN >= ledCount) {
        iN = iN % ledCount;
    }
    return iN;
}   

void fillSolid(CRGB color)
{
  for(int i = 0;i<3;i++)
    fillHoop(i, color);
}

void fillRainbow()
{
  static byte hue = 0;
  fill_rainbow(actualLeds[0], actualLedCount,hue++ , 10);
  fill_rainbow(actualLeds[1], actualLedCount,hue , 10);
//  for(int i = 0;i<ledCount;i++)
//  {
//    setPixel(i, CHSV(hue++,255,255));
//  }
//  hue++;
  showLeds(5);
}

void fillHoop(byte whichHoop, CRGB color)
{
  for(int i = hoop[whichHoop][hoopStart];i<hoop[whichHoop][hoopEnd];i++)
    setPixel(i,color);
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

void setup()
{
  // For safety (to prevent too high of a power draw), the test case defaults to
  // setting brightness to 25% brightness
  LEDS.setBrightness(32);
  
  LEDS.addLeds<WS2811,2, GRB>(actualLeds[0], actualLedCount);
  LEDS.addLeds<WS2811,3, GRB>(actualLeds[1], actualLedCount);
      
  Serial.begin(9600);
  fillSolid(CRGB::Black); //-BLANK STRIP
  showLeds(1);
}

void drawTracer(byte trailLength, bool isForward)
{
  byte trailDecay = (255-64)/trailLength;

  fillSolid(CRGB::Purple);

  for(int i = 0;i<trailLength;i++)
  {
      setPixel(adjacent_cw(idex-i), CHSV(0, 255, 255 - trailDecay*i));
      setPixel(antipodal_index(adjacent_cw(idex-i)), CHSV(128, 255, 255 - trailDecay*i));
  }  
}

void nonReactiveFade(CRGB baseColor, CRGB highlightColor) { //-BOUNCE COLOR (SIMPLE MULTI-LED FADE)
    static long lastBounceTime;
    const int bounceInterval = 250;
    
    static byte phase = 0;
    static byte trailLength = 8;
    byte trailDecay = (255-64)/trailLength;

    fillSolid(baseColor);
    
    drawTracer(trailLength, bounceForward);
    if (bounceForward) {
        if (++idex == ledCount) {
            bounceForward = !bounceForward;
            idex--;
        }
    } else {
        //todo: the trail is running off the array
        if (--idex == 0) {
            bounceForward = !bounceForward;
        }
    }
    
}

void rotatingRainbow()
{
    static byte hue = 0;
    for(int i = 0;i < NUM_STRIPS;i++)
    {
        fill_rainbow(actualLeds[i], ledCount, hue++, 10);
    }
}

void colorWipe(CRGB color)
{
  for(int i = 0;i<ledCount;i++)
  {
    setPixel(i, color);
    showLeds(10);
  }
}



void setEveryNth(byte index, byte num, CRGB color)
{
  for (int i = 0;i<ledCount;i++)
  {
    if (i%num == index)
        setPixel(i, color);
  }
  showLeds(200);
}

void everyThirdEntrance(CRGB firstColor, CRGB secondColor, CRGB thirdColor)
{
  setEveryNth(0, 3, firstColor);
  setEveryNth(1, 3, secondColor);
  setEveryNth(2, 3, thirdColor);
}

void threeColorEntrance(CRGB firstColor, CRGB secondColor, CRGB thirdColor)
{
  setEveryNth(0, 9, firstColor);
  setEveryNth(1, 9, firstColor);
  setEveryNth(2, 9, firstColor);
  setEveryNth(3, 9, secondColor);
  setEveryNth(4, 9, secondColor);
  setEveryNth(5, 9, secondColor);
  setEveryNth(6, 9, thirdColor);
  setEveryNth(7, 9, thirdColor);
  setEveryNth(8, 9, thirdColor);
}

void threeColorAlternate(boolean firstLoop, CRGB firstColor, CRGB secondColor, CRGB thirdColor)
{
  threeColorEntrance(firstColor, secondColor, thirdColor);
  for (int i = 0;i<ledCount;i++)
  {
    switch(i%3)
    {
      case 0:
        setPixel(i, firstColor);
        break;
      case 1:
        setPixel(i, secondColor);
        break;
      case 2:
        setPixel(i, thirdColor);
        break;
    }
  }
}


void threeColorWipe(CRGB firstColor, CRGB secondColor, CRGB thirdColor)
{
  for (int i = 0;i<ledCount;i++)
  {
    switch(i%9)
    {
      case 0:
      case 1:
      case 2:          
        setPixel(i, firstColor);
        break;
      case 3:
      case 4:
      case 5:
        setPixel(i, secondColor);
        break;
      case 6:
      case 7:
      case 8:
        setPixel(i, thirdColor);
        break;
    }
  }
}

void rotateStrip(long ms)
{
  rotateStrip(ms, 75);
}

void rotateStrip(long duration, long delayms)
{
  long endTime = millis() + duration;

  while(millis() < endTime)
  {

    // store the last pixel
    CRGB temp = getPixel(ledCount-1);
    //Serial.print("#");Serial.print(temp.r);Serial.print(":");Serial.print(temp.g);Serial.print(":");Serial.print(temp.b);Serial.println();

    // copy the last one to the previous one
    for(int i = ledCount-1; i > 0; i--) 
    {
      //Serial.print("set pixel ");Serial.print(i);Serial.print(" to ");Serial.print(i-1);Serial.print(":");Serial.print((int)getPixel(i-1));Serial.println();
      setPixel(i, getPixel(i-1));
    }

    //set the first one to temportarily stored one
    setPixel(0, temp);
    showLeds(delayms);
  }
}

void showLeds(long delayms)
{
//  for(int i = 0;i<ledCount;i++)
//  {
//    setPixel(i, leds[i]);
//  }

  LEDS.show();
  delay(delayms);
}


int xAccel;
void loop()
{
  const CRGB highlightColor = CRGB::Red;
  const CRGB contrastColor = CRGB::LimeGreen;
  const CRGB baseColor = CRGB::Purple;
  const CRGB color4 = CRGB::Black;

/*
  fillSolid(CRGB::Black);
  showLeds(0);

  threeColorWipe(contrastColor, baseColor, baseColor);
  rotateStrip(30);


  fillHoop(0,CRGB::Red);
  showLeds(2000);

  fillHoop(1,CRGB::Blue);
  showLeds(2000);

  fillHoop(2,CRGB::Green);
  showLeds(2000);
*/

  colorWipe(contrastColor);
  colorWipe(highlightColor);
  colorWipe(baseColor);

  threeColorWipe(contrastColor, baseColor, baseColor);
  rotateStrip(6000, 75);

  threeColorWipe(contrastColor, baseColor, highlightColor);
  rotateStrip(6000, 75);

  for(int i = 50;i>0;i--)
  {
    threeColorWipe(contrastColor, baseColor, highlightColor);
    rotateStrip(50+i, 25);
    threeColorWipe(highlightColor, contrastColor, baseColor);
    rotateStrip(50+i, 25);
    threeColorWipe(baseColor, highlightColor, contrastColor);
    rotateStrip(50+i, 25);
  }
  for (int i = 0;i<1000;i++)
  {
      fillRainbow();
  }

//  threeColorWipe(contrastColor, baseColor, highlightColor);
//  rotateStrip(6000);

//  threeColorWipe(baseColor, baseColor, highlightColor);
//  rotateStrip(3000);

  for (int i = 0;i<1000;i++)
  {
      nonReactiveFade(baseColor, highlightColor);
      showLeds(30);
  }

}

