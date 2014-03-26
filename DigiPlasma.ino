// DigiPlasma
// made for the digipixel http://www.bradsprojects.com/the-digipixel/
// 2014 Jan Krummrey
// Version 0.1 
// Working example, but way to slow. Need to work on color conversion and make use of tables

/*  based on  Color cycling plasma   
 Copyright (c) 2009 Ben Combee.  All right reserved.
 Copyright (c) 2009 Ken Corey.  All right reserved.
 Copyright (c) 2008 Windell H. Oskay.  All right reserved.
 Copyright (c) 2011 Sam C. Lin All Rights Reserved
 
 This demo is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This demo is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 
 Version 0.1 for DigiPixel
 */

#include <DigiPixel.h>

// leave the following line uncommented for use with a Digispark
//DigiPixel digiPixel(3,0,5,2,1);  // LED Latch/Button Shift !load pin, LED/Button clock pin, LED Data Pin, LED Output Enable pin, Buttons data pin)

// leave the following line uncommented for use with an Arduino
DigiPixel digiPixel(5,2,6,4,3);  // LED Latch/Button Shift !load pin, LED/Button clock pin, LED Data Pin, LED Output Enable pin, Buttons data pin)



typedef struct
{
  unsigned char r;
  unsigned char g;
  unsigned char b;
} 
ColorRGB;

//a color with 3 components: h, s and v
typedef struct 
{
  unsigned char h;
  unsigned char s;
  unsigned char v;
} 
ColorHSV;

unsigned char plasma[8][8];
long paletteShift;
unsigned char color;

unsigned char modulor = 32;
unsigned char threshhold = modulor /2;


//Converts an HSV color to RGB color
void HSVtoRGB(void *vRGB, void *vHSV) 
{
  float r, g, b, h, s, v; //this function works with floats between 0 and 1
  float f, p, q, t;
  int i;
  ColorRGB *colorRGB=(ColorRGB *)vRGB;
  ColorHSV *colorHSV=(ColorHSV *)vHSV;

  h = (float)(colorHSV->h / 256.0);
  s = (float)(colorHSV->s / 256.0);
  v = (float)(colorHSV->v / 256.0);

  //if saturation is 0, the color is a shade of grey
  if(s == 0.0) {
    b = v;
    g = b;
    r = g;
  }
  //if saturation > 0, more complex calculations are needed
  else
  {
    h *= 6.0; //to bring hue to a number between 0 and 6, better for the calculations
    i = (int)(floor(h)); //e.g. 2.7 becomes 2 and 3.01 becomes 3 or 4.9999 becomes 4
    f = h - i;//the fractional part of h

    p = (float)(v * (1.0 - s));
    q = (float)(v * (1.0 - (s * f)));
    t = (float)(v * (1.0 - (s * (1.0 - f))));

    switch(i)
    {
    case 0: 
      r=v; 
      g=t; 
      b=p; 
      break;
    case 1: 
      r=q; 
      g=v; 
      b=p; 
      break;
    case 2: 
      r=p; 
      g=v; 
      b=t; 
      break;
    case 3: 
      r=p; 
      g=q; 
      b=v; 
      break;
    case 4: 
      r=t; 
      g=p; 
      b=v; 
      break;
    case 5: 
      r=v; 
      g=p; 
      b=q; 
      break;
    default: 
      r = g = b = 0; 
      break;
    }
  }
  colorRGB->r = (int)(r * 255.0);
  colorRGB->g = (int)(g * 255.0);
  colorRGB->b = (int)(b * 255.0);
}

float
dist(float a, float b, float c, float d) 
{
  return sqrt((c-a)*(c-a)+(d-b)*(d-b));
}


byte colorName(unsigned char r, unsigned char g, unsigned char b)
{
  color=0;
  r = r % modulor;
  g = g % modulor;
  g = g % modulor;

  if ((r>threshhold) and (g<threshhold) and (b<threshhold)) color = 1;
  if ((r<threshhold) and (g>threshhold) and (b<threshhold)) color = 2;
  if ((r>threshhold) and (g>threshhold) and (b<threshhold)) color = 3;
  if ((r<threshhold) and (g<threshhold) and (b>threshhold)) color = 4;
  if ((r>threshhold) and (g<threshhold) and (b>threshhold)) color = 5;
  if ((r<threshhold) and (g>threshhold) and (b>threshhold)) color = 6;
  if ((r>threshhold) and (g>threshhold) and (b>threshhold)) color = 7;
  return color;
}

void
plasma_morph()
{
  unsigned char x,y;
  float value;
  ColorRGB colorRGB;
  ColorHSV colorHSV;

  for(y = 0; y < 8; y++)
    for(x = 0; x < 8; x++) {
      {
        value = sin(dist(x + paletteShift, y, 128.0, 128.0) / 32.0)
          + sin(dist(x, y, 64.0, 64.0) / 32.0)
            + sin(dist(x, y + paletteShift / 7, 192.0, 64) / 28.0)
              + sin(dist(x, y, 192.0, 100.0) / 32.0);
        colorHSV.h=(unsigned char)((value) * 128)&0xff;
        colorHSV.s=255; 
        colorHSV.v=(unsigned char)((value) * 128)&0xff;
        //	colorHSV.v=255; // Original
        HSVtoRGB(&colorRGB, &colorHSV);


        digiPixel.setPixel(x, y, colorName(colorRGB.r, colorRGB.g, colorRGB.b));
      }
      // This has to go but right now, it is too slow without
      digiPixel.drawScreen();
    }
  paletteShift++;

  // This should be the only time the screen is redrawn, but too slow right now
  //digiPixel.drawScreen();
}

void setup()
{
  // start with morphing plasma, but allow going to color cycling if desired.
  paletteShift=128000;
  unsigned char bcolor;

  //generate the plasma once
  for(unsigned char y = 0; y < 8; y++)
    for(unsigned char x = 0; x < 8; x++)
    {
      //the plasma buffer is a sum of sines
      bcolor = (unsigned char)
        (
        128.0 + (128.0 * sin(x*8.0 / 16.0))
          + 128.0 + (128.0 * sin(y*8.0 / 16.0))
          ) / 2;
      plasma[x][y] = bcolor;
    }
}

void loop()
{
  plasma_morph();
}
