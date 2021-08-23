#include <arduinoFFT.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

arduinoFFT FFT = arduinoFFT();
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SAMPLES 128             // Must be a power of 2
#define SAMPLING_FREQUENCY 20000 // Hz, must be 40000 or less due to ADC conversion time. Determines maximum frequency that can be analysed by the FFT Fmax=sampleF/2.
#define amplitude 200            // Depending on your audio source level, you may need to increase this value

//unsigned int sampling_period_us;
unsigned long timer = 0;

byte peak[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double vReal[SAMPLES];
double vImag[SAMPLES];
//unsigned long newTime, oldTime;

void setBlocks(uint8_t row, uint8_t value);
void clear_peak();

byte value1[8] = {B00000, B00000, B00000, B00000, B00000, B00000, B00000, B11111,};
byte value2[8] = {B00000, B00000, B00000, B00000, B00000, B00000, B11111, B11111,};
byte value3[8] = {B00000, B00000, B00000, B00000, B00000, B11111, B11111, B11111,};
byte value4[8] = {B00000, B00000, B00000, B00000, B11111, B11111, B11111, B11111,};
byte value5[8] = {B00000, B00000, B00000, B11111, B11111, B11111, B11111, B11111,};
byte value6[8] = {B00000, B00000, B11111, B11111, B11111, B11111, B11111, B11111,};
byte value7[8] = {B00000, B11111, B11111, B11111, B11111, B11111, B11111, B11111,};
byte value8[8] = {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111,};

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  
  lcd.createChar(0, value1);
  lcd.createChar(1, value2);
  lcd.createChar(2, value3);
  lcd.createChar(3, value4);
  lcd.createChar(4, value5);
  lcd.createChar(5, value6);
  lcd.createChar(6, value7);
  lcd.createChar(7, value8);
  
//  sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQUENCY));
}

void loop() {
  clear_peak();
  if( millis() - timer > 1200){
    lcd.clear();
    timer = millis();
  }

  for (int i = 0; i < SAMPLES; i++) {
//    newTime = micros()-oldTime;
//    oldTime = newTime;
    vReal[i] = analogRead(A0); // A conversion takes about 1uS on an ESP32
    vImag[i] = 0;
//    while (micros() < (newTime + sampling_period_us)) { /* do nothing to wait */ }
  }
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
  for (int i = 2; i < (SAMPLES/2); i++){ 
    if (vReal[i] > 6) { // filter 
      if (i<=2 )setBlocks(0, (uint16_t)vReal[i], " 80hz ");
      else if (i >2   && i<=3 )setBlocks(1, (uint16_t)vReal[i], " 118hz ");
      else if (i >3   && i<=4 )setBlocks(2, (uint16_t)vReal[i], " 173hz ");
      else if (i >4  && i<=5 )setBlocks(3, (uint16_t)vReal[i], " 254hz ");
      else if (i >5   && i<=8 )setBlocks(4, (uint16_t)vReal[i], " 374hz ");
      else if (i >8   && i<=10 )setBlocks(5, (uint16_t)vReal[i], " 550hz ");
      else if (i >10   && i<=12 )setBlocks(6, (uint16_t)vReal[i], " 808hz ");
      else if (i >12   && i<=20 )setBlocks(7, (uint16_t)vReal[i], " 1188hz ");
      else if (i >20   && i<=28 )setBlocks(8, (uint16_t)vReal[i], " 1747hz ");
      else if (i >28 && i<=40 )setBlocks(9, (uint16_t)vReal[i], " 2568hz ");
      else if (i >40   && i<=46 )setBlocks(10, (uint16_t)vReal[i], " 3776hz ");
      else if (i >46   && i<=50 )setBlocks(11, (uint16_t)vReal[i], " 5551hz ");
      else if (i >50   && i<=53 )setBlocks(12, (uint16_t)vReal[i], " 8162hz ");
      else setBlocks(13, (uint16_t)vReal[i], " 12000hz ");
    }
  }
  Serial.println(" ");
}

void clear_peak(){
  for(uint8_t i = 0; i < 14; i++)peak[i] = 0;
}

void setBlocks(uint8_t row, uint16_t value, const char* text){
    int val = map((value >= 256) ? 256 : value, 0, 256, 0, 14);
    //Serial.print(text);
    if(val > peak[row]){
      peak[row] = val;
      
      lcd.setCursor(row,0);
      (peak[row] <= 7) ? lcd.print(" ") : lcd.write(peak[row]-8);
      
      lcd.setCursor(row, 1);
      (peak[row] <= 7) ? lcd.write(peak[row]) : lcd.write(7);
    }
    
}
