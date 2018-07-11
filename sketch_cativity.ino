#define RED_PIN 3
#define GREEN_PIN 6
#define BLUE_PIN 5

#define RED 0
#define GREEN 1
#define BLUE 2

#define PIR_PIN 7

#define ON_TIME 10000
#define TIME_WINDOW ( ( unsigned long ) ( 5L * 60L * 1000L ) )
#define MAX_HUE 767

/******************************************************************************
 * https://blog.adafruit.com/2012/03/14/constant-brightness-hsb-to-rgb-algorithm/
 * accepts hue, saturation and brightness values and outputs three 8-bit color
 * values in an array (color[])
 *
 * saturation (sat) and brightness (bright) are 8-bit values.
 *
 * hue (index) is a value between 0 and 767. hue values out of range are
 * rendered as 0.
 *
 *****************************************************************************/
void hsb2rgb(uint16_t index, uint8_t sat, uint8_t bright, uint8_t color[3])
{
  uint16_t r_temp, g_temp, b_temp;
  uint8_t index_mod;
  uint8_t inverse_sat = (sat ^ 255);

  index = index % 768;
  index_mod = index % 256;

  if (index < 256)
  {
    r_temp = index_mod ^ 255;
    g_temp = index_mod;
    b_temp = 0;
  }

  else if (index < 512)
  {
    r_temp = 0;
    g_temp = index_mod ^ 255;
    b_temp = index_mod;
  }

  else if ( index < 768)
  {
    r_temp = index_mod;
    g_temp = 0;
    b_temp = index_mod ^ 255;
  }

  else
  {
    r_temp = 0;
    g_temp = 0;
    b_temp = 0;
  }

  r_temp = ((r_temp * sat) / 255) + inverse_sat;
  g_temp = ((g_temp * sat) / 255) + inverse_sat;
  b_temp = ((b_temp * sat) / 255) + inverse_sat;

  r_temp = (r_temp * bright) / 255;
  g_temp = (g_temp * bright) / 255;
  b_temp = (b_temp * bright) / 255;

  color[RED]  = (uint8_t)r_temp;
  color[GREEN]  = (uint8_t)g_temp;
  color[BLUE] = (uint8_t)b_temp;
}


void setup() {
  // put your setup code here, to run once:
  pinMode ( RED_PIN, OUTPUT ) ;
  pinMode ( GREEN_PIN, OUTPUT ) ;
  pinMode ( BLUE_PIN, OUTPUT ) ;
  pinMode ( PIR_PIN, INPUT ) ;
  
  analogWrite ( RED_PIN, 0 ) ;
  analogWrite ( GREEN_PIN, 0 ) ;
  analogWrite ( BLUE_PIN, 0 ) ;

  Serial.begin( 9600 ) ;
  delay ( 1000 ) ;
  Serial.println ( MAX_HUE ) ;
}

void loop() {
  static unsigned long startTime = millis ( ) ;
  static int state = 0 ;
  static unsigned long  triggeredTime = 0L ;
  static int hue = 0 ;
  
  int pinState = digitalRead ( PIR_PIN ) ;
  unsigned long iterationTimer = millis ( ) ;
  uint8_t color[3] = { 0 } ;
  
  if( pinState || state ) 
  {
    if( state == 0 )
    {
      startTime = millis ( ) ;
      state = 1 ;
      triggeredTime += ON_TIME ;
      if ( triggeredTime > TIME_WINDOW )
      {
        triggeredTime = TIME_WINDOW ;
      }
    }                                             
    unsigned long delta = millis ( ) - ( unsigned long ) startTime ;
    if( delta <= ON_TIME )
    {
      // map the number of milliseconds on in TIME_WINDOW to hue values
      hue = map( triggeredTime, ON_TIME, TIME_WINDOW, 0, MAX_HUE ) ;
      hsb2rgb(hue, 255, 255, color ) ;
      analogWrite( RED_PIN, color[RED] ) ;
      analogWrite ( GREEN_PIN, color[GREEN] ) ;
      analogWrite ( BLUE_PIN, color[BLUE] ) ;
      delay( 10 ) ;
    }
    else
    {
      state = 0 ;
      startTime = millis ( ) ;
      analogWrite( RED_PIN, 0 ) ;
      analogWrite ( GREEN_PIN, 0 ) ;
      analogWrite ( BLUE_PIN, 0 ) ;
    }
  }
  else
  {
    if ( triggeredTime > 0 )
    {
        int iterationTime = millis ( ) - iterationTimer ;
        iterationTime = iterationTime < 10 ? 10 : iterationTime ;
        triggeredTime -= iterationTime  ;
        Serial.println ( hue ) ; 
    }
  }
  
}
