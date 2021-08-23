#include "arduinoFFT.h"
arduinoFFT FFT = arduinoFFT();

#define SAMPLES 128             // Must be a power of 2
#define SAMPLING_FREQUENCY 20000 // Hz, must be 40000 or less due to ADC conversion time. Determines maximum frequency that can be analysed by the FFT Fmax=sampleF/2.
#define amplitude 200            // Depending on your audio source level, you may need to increase this value

unsigned int sampling_period_us;
unsigned long microseconds;

byte peak[] = {0,0,0,0,0,0,0};
double vReal[SAMPLES];
double vImag[SAMPLES];
unsigned long newTime, oldTime;

void setup() {
  Serial.begin(115200);
sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQUENCY));
}

void loop() {
for (int i = 0; i < SAMPLES; i++) {
    newTime = micros()-oldTime;
    oldTime = newTime;
    vReal[i] = analogRead(A0); // A conversion takes about 1uS on an ESP32
    vImag[i] = 0;
    while (micros() < (newTime + sampling_period_us)) { /* do nothing to wait */ }
  }
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
  for (int i = 2; i < (SAMPLES/2); i++){ // Don't use sample 0 and only first SAMPLES/2 are usable. Each array eleement represents a frequency and its value the amplitude.
    if (vReal[i] > 80) { // Add a crude noise filter, 10 x amplitude or more
      if (i<=3 ){
        Serial.print(vReal[i]); // 125Hz
        Serial.print(" 1 -> 80hz ");
      }
      if (i >3   && i<=10 ) {
        Serial.print(vReal[i]); // 125Hz
        Serial.print(" 2 -> 171hz ");
      }
       if (i >10   && i<=15 ) {
        Serial.print(vReal[i]); // 125Hz
        Serial.print(" 3 -> 364hz ");
      }
       if (i >15   && i<=18 ) {
        Serial.print(vReal[i]); // 125Hz
        Serial.print(" 4 -> 775hz ");
      }
       if (i >18   && i<=24 ) {
        Serial.print(vReal[i]); // 125Hz
        Serial.print(" 5 -> 1652hz ");
      }
       if (i >24   && i<=35 ) {
        Serial.print(vReal[i]); // 125Hz
        Serial.print(" 6 -> 3521hz ");
      }
       if (i >35   && i<=50 ) {
        Serial.print(vReal[i]); // 125Hz
        Serial.print(" 7 -> 7506hz ");
      }
 
      if (i >50) {
        Serial.print(vReal[i]); // 125Hz
        Serial.print(" 8 -> 16000hz");
      }
      
    }
  }
  Serial.println("");
  

}
