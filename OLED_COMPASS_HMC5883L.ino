#include "U8glib.h"
#include <Wire.h>
#include <HMC5883L.h>
#include "RTClib.h"

// Version 1.0
// Arduino 1.0.6

// https://github.com/jarzebski/Arduino-HMC5883L
// https://code.google.com/p/u8glib/

HMC5883L compass;

U8GLIB_SSD1306_128X64 u8g(10, 9, 12, 11,13);

int x,y,p,z,north;

RTC_Millis rtc;

void setup(void) {

  compass.begin();

  pinMode(A2, OUTPUT);           
  digitalWrite(A2, LOW);  

  compass.setRange(HMC5883L_RANGE_1_3GA);
  compass.setMeasurementMode(HMC5883L_CONTINOUS);
  compass.setDataRate(HMC5883L_DATARATE_30HZ);
  compass.setSamples(HMC5883L_SAMPLES_8);
  // Set calibration offset. See HMC5883L_calibration.ino
  compass.setOffset(0, 0);

  // rtc.begin(DateTime(F(__DATE__), F(__TIME__))); 

  rtc.adjust(DateTime(2015, 1, 1, 0, 0, 0));

}

void loop(void) {

  DateTime now = rtc.now();

  north  = round(get_compass());

  u8g.firstPage();  

  do {

    u8g.drawCircle(96, 32, 30);

    get_dir_print(1,10); // Печать направления

    u8g.setFont(u8g_font_unifont);
    u8g.setPrintPos(1,30);
    u8g.print(round(get_compass())); // Печать азимута    
    u8g.setFont(u8g_font_unifont);
    u8g.print(char(176)); // Печатаем значок градуса

    u8g.setPrintPos(1,60);
    if (now.hour() < 10) u8g.print(0);
    u8g.print(now.hour(), DEC);
    u8g.print(':');
    if (now.minute() < 10) u8g.print(0);
    u8g.print(now.minute(), DEC);
    u8g.print(':');
    if (now.second() < 10) u8g.print(0);
    u8g.print(now.second(), DEC);

    north = round(get_compass());
    
    if (north >= 0 && north <= 5)  { 
      digitalWrite(A2, HIGH); 
    } else if (north >= 355 && north <= 360) {
      digitalWrite(A2, HIGH); 
    } else {      
      digitalWrite(A2, LOW);
    }

    draw_line();
    
    p = north - 180; 
    if (p > 360) p = north - 180;
    
    x = 96 - (29 * cos(p*(3.14/180)));
    y = 32 -(29 * sin(p*(3.14/180)));
    
    u8g.drawCircle(x,y, 3);

  } 
  while( u8g.nextPage() );

  delay(100);

}

void draw_line( void ) {
  
    int r = 0;
    
    p = -180; 
    
    x = 96 - (29 * cos(r*(3.14/180)));
    y = 32 - (29 * sin(r*(3.14/180)));

    u8g.drawLine(96,32,x,y);
    
    x = 96 - (29 * cos(p*(3.14/180)));
    y = 32 -(29 * sin(p*(3.14/180)));

    u8g.drawLine(96,32,x,y);
    u8g.drawDisc(x,y, 3);

}

float get_compass( void ) {

  Vector norm = compass.readNormalize();

  float heading = atan2(norm.YAxis, norm.XAxis);

  // Set declination angle on your location and fix heading
  // You can find your declination on: http://magnetic-declination.com/
  // (+) Positive or (-) for negative
  // For Bytom / Poland declination angle is 4'26E (positive)
  // Formula: (deg + (min / 60.0)) / (180 / M_PI);
  // Moscow склонение 10'47
  
  float declinationAngle = (10.0 + (47.0 / 60.0)) / (180 / M_PI);
  
  // heading += declinationAngle;

  // Correct for heading < 0 deg and heading > 360 deg
  
  if (heading < 0) { 
    heading += 2 * PI; 
  }

  if (heading > 2 * PI) { 
    heading -= 2 * PI; 
  }

  // Convert to degrees

  float headingDegrees = heading * 180/M_PI; 

  return(headingDegrees);
}

// Принт на русском буквы направления
// Расстояние между буквами примерно 10

void print_dir(char a, int x, int y) {

  u8g.setFont(u8g_font_unifont_0_8);
  u8g.setPrintPos(x,y);

  if (a=='N') u8g.print(char(193));    // C
  if (a=='S') u8g.print(char(206));    // Ю 
  if (a=='E') u8g.print(char(178));    // В   
  if (a=='W') u8g.print(char(183));   // З

}

void get_dir_print( int x, int y) {

  z = round(get_compass());

  if (z > 0 & z < 90)       { 
    print_dir('N',x,y);  
    print_dir('E',x+10,y);  
  }
  if (z > 90 & z < 180)   { 
    print_dir('E',x,y);  
    print_dir('S',x+10,y);  
  }
  if (z > 180 & z < 270) { 
    print_dir('S',x,y);  
    print_dir('W',x+10,y); 
  }
  if (z > 270 & z < 360) { 
    print_dir('W',x,y); 
    print_dir('N',x+10,y);  
  }


}

