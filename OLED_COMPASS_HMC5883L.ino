#include "U8glib.h"
#include <Wire.h>
#include <HMC5883L.h>

// Version 1.0
// Arduino 1.0.6

// https://github.com/jarzebski/Arduino-HMC5883L
// https://code.google.com/p/u8glib/

HMC5883L compass;

U8GLIB_SSD1306_128X64 u8g(10, 9, 12, 11,13);

int z = 0;
int x,y,p;
char nap[3];

void setup(void) {
  
  compass.begin();
  
  pinMode(A2, OUTPUT);           
  digitalWrite(A2, HIGH);  
  
   compass.setRange(HMC5883L_RANGE_1_3GA);
   compass.setMeasurementMode(HMC5883L_CONTINOUS);
   compass.setDataRate(HMC5883L_DATARATE_30HZ);
   compass.setSamples(HMC5883L_SAMPLES_8);
   // Set calibration offset. See HMC5883L_calibration.ino
   compass.setOffset(0, 0);
  
}

void loop(void) {

   z = round(get_compass());
    
  u8g.firstPage();  
  
  do {
    
    u8g.drawCircle(96, 32, 30);
    u8g.setFont(u8g_font_unifont);
    u8g.drawStr(1,10,nap);

    u8g.setPrintPos(1,25);
    u8g.print(round(get_compass()));


  //  u8g.setPrintPos(1,40);
  //  u8g.print(z);
    
    z = round(get_compass());
    
    p = z+180; if (p < 0) p=p*-1;
    x = 96 - (29 * cos(z*(3.14/180)));
    y = 32 -(29 * sin(z*(3.14/180)));
    u8g.drawLine(96,32,x,y);
    u8g.drawCircle(x,y, 3);
    
    x = 96 - (29 * cos(p*(3.14/180)));
    y = 32 -(29 * sin(p*(3.14/180)));
    u8g.drawLine(96,32,x,y);
       
 } while( u8g.nextPage() );
 
  z = round(get_compass());
  
  if (z > 0 & z < 90)       strcpy(nap,"NE");
  if (z > 90 & z < 180)   strcpy(nap,"ES");
  if (z > 180 & z < 270) strcpy(nap,"SW");
  if (z > 270 & z < 360) strcpy(nap,"WN");
  
    
  delay(100);

}

float get_compass( void ) {
  
  Vector norm = compass.readNormalize();
  
  float heading = atan2(norm.YAxis, norm.XAxis);

  // Set declination angle on your location and fix heading
  // You can find your declination on: http://magnetic-declination.com/
  // (+) Positive or (-) for negative
  // For Bytom / Poland declination angle is 4'26E (positive)
  // Formula: (deg + (min / 60.0)) / (180 / M_PI);
  
  float declinationAngle = (4.0 + (26.0 / 60.0)) / (180 / M_PI);
  heading += declinationAngle;

  // Correct for heading < 0deg and heading > 360deg
  if (heading < 0) { heading += 2 * PI; }

  if (heading > 2 * PI) { heading -= 2 * PI; }

  // Convert to degrees
  
  float headingDegrees = heading * 180/M_PI; 

  return(headingDegrees);
}
