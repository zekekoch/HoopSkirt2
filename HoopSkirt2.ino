#include <FastSPI_LED2.h>

//const int ledCount = 185;
const int ledCount = 134;
const int actualLedCount = 84;
const int NUM_STRIPS = 2;
//CRGB leds[ledCount];
CRGB actualLeds[NUM_STRIPS][actualLedCount];

// statically link in the controllers to avoid 'new'
WS2811Controller800Khz<2, GRB> ledController0;
WS2811Controller800Khz<3, GRB> ledController1;

// this holds the 
const byte hoopStart = 0;
const byte hoopEnd = 1;
byte hoop[3][2] = 
{ 
  {0,50},
  {50,90},
  {90,134}
};

const int BottomIndex = 0;
const int TopIndex = int(ledCount/2);
const int isEven = ledCount%2;

//-PERISTENT VARS
byte idex = 0;        //-LED INDEX (0 to ledCount-1

// this function solves the problem of mapping a single 
// virtual strip into two smaller physical strips
// it has some hard coded constants (hacks).  Be careful.
//
// TODO: I think that there is also an off by one error here
void setPixel(int currentPixel, CRGB c) 
{
  if(currentPixel < 0)
  {
    return;
  }
  else if(currentPixel < 50)
  {
    actualLeds[0][currentPixel] = c;
  } 
  else
  {
    actualLeds[1][currentPixel-50] = c;
  }
} 

CRGB getPixel(int currentPixel)
{
  if(currentPixel < 0)
  {
    return CRGB::Black;
  }
  else if(currentPixel < 50)
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
  fill_rainbow(actualLeds[0], actualLedCount, hue++, 10);
  fill_rainbow(actualLeds[1], actualLedCount, hue, 10);
  showLeds(0);
}

void fillHoop(byte whichHoop, CRGB color)
{
  for(int i = hoop[whichHoop][hoopStart];i<hoop[whichHoop][hoopEnd];i++)
    setPixel(i,color);
}


//-FIND INDEX OF HORIZONAL OPPOSITE LED
int horizontal_index(int i) 
{
    //-ONLY WORKS WITH INDEX < TOPINDEX
    if (i == BottomIndex) {return BottomIndex;}
    if (i == TopIndex && isEven == 1) {return TopIndex + 1;}
    if (i == TopIndex && isEven == 0) {return TopIndex;}
    return ledCount - i;
}


//-FIND INDEX OF ANTIPODAL OPPOSITE LED
int antipodal_index(int i) {
    //int N2 = int(ledCount/2);
    int iN = i + TopIndex;
    if (i >= TopIndex) {iN = ( i + TopIndex ) % ledCount; }
    return iN;
}


//-FIND ADJACENT INDEX CLOCKWISE
int adjacent_cw(int i) 
{
    int r;
    if (i < ledCount - 1) 
    {
        r = i + 1;
    }
    else 
    {
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
  
  LEDS.addLeds(&ledController0, actualLeds[0], actualLedCount);
  LEDS.addLeds(&ledController1, actualLeds[1], actualLedCount);
      
  Serial.begin(9600);

  fillSolid(CRGB::Black); //-BLANK STRIP
  showLeds(0);
}

void drawTracer(byte trailLength, bool isForward, CRGB baseColor, CRGB highlightColor, CRGB contrastColor)
{

  baseColor /= 2;
  fillSolid(baseColor);

  CRGB c1 = highlightColor;
  CRGB c2 = contrastColor;
  byte fade = trailLength/2;

  for(int i = 0;i<trailLength;i++)
  {
    // setPixel eats any "offscreen" pixels
//    setPixel(adjacent_cw(idex-i), CHSV(50, 255, 255 - trailDecay*i));
//    setPixel(antipodal_index(adjacent_cw(idex-i)), CHSV(75, 255, 255 - trailDecay*i));
    setPixel(adjacent_cw(idex-i), c1);
    setPixel(antipodal_index(adjacent_cw(idex-i)), c2);
    c1 += baseColor/fade;
    c2 += baseColor/trailLength;
  }  
}

void nonReactiveFade(CRGB baseColor, CRGB highlightColor, CRGB contrastColor) { //-BOUNCE COLOR (SIMPLE MULTI-LED FADE)
    static boolean bounceForward = true;
    
    static byte trailLength = 6;

    fillSolid(baseColor);
    
    drawTracer(trailLength, bounceForward, baseColor, highlightColor, contrastColor);
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
  unsigned long endTime = millis() + duration;

  while(millis() < endTime)
  {
    // store the last pixel
    CRGB temp = getPixel(ledCount-1);

    // copy the last one to the previous one
    for(int i = ledCount-1; i > 0; i--) 
    {
      setPixel(i, getPixel(i-1));
    }

    //set the first one to temportarily stored one
    setPixel(0, temp);
    showLeds(delayms);
  }
}

void showLeds(long delayms)
{
  LEDS.show();
  delay(delayms);
}

void rotateTriads(byte speed, CRGB baseColor, CRGB contrastColor, CRGB highlightColor)
{
  for(int i = 10;i>0;i--)
    {
      threeColorWipe(contrastColor, baseColor, highlightColor);
      rotateStrip(speed*2, speed+i);
      threeColorWipe(highlightColor, contrastColor, baseColor);
      rotateStrip(speed*2, speed+i);
      threeColorWipe(baseColor, highlightColor, contrastColor);
      rotateStrip(speed*2, speed+i);
    }  
}

int xAccel;
void loop()
{
  const CRGB Pinkish = 0xFF007F;
  const CRGB Purpleish = 0x00FF7F;

  const CRGB highlightColor = Pinkish;  // redish
  const CRGB contrastColor = CRGB::SpringGreen; // purplish
  const CRGB baseColor = CRGB::Fuchsia; /// 0x7F00FF; // purplish

  for (int i = 0;i<1000;i++)
  {
    nonReactiveFade(baseColor, highlightColor, contrastColor);
    showLeds(30);
  }


  colorWipe(contrastColor);
  colorWipe(highlightColor);
  colorWipe(baseColor);

  threeColorWipe(contrastColor, baseColor, baseColor);
  rotateStrip(6000, 75);

  threeColorWipe(contrastColor, baseColor, highlightColor);
  rotateStrip(6000, 75);

  rotateTriads(50, 0x00FF7F, 0xFF00FF, 0xFF007F); // standard colors
  rotateTriads(40, 0x00FF7F, 0xFE00FF, 0xFF0000); // bring on the red
  rotateTriads(50, 0xFF0000, 0x00FF7F, 0x007FFF); // bring on the blue
  rotateTriads(60, 0x007FFF, 0xFF0000, 0xFEFF00); // bring on the yellos

  for (int i = 0;i<1000;i++)
  {
      fillRainbow();
  }

  for (int i = 0;i<1000;i++)
  {
      nonReactiveFade(baseColor, highlightColor, contrastColor);
      showLeds(30);
  }

}

