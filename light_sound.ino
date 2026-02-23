//---------------------------------------------------------------------------- set color of WS2812B Neopixel LED
void led(uint8_t r, uint8_t g, uint8_t b) {
  
//  Serial.print("X r=");Serial.print(r);
//  Serial.print( " g=");Serial.print(g);
//  Serial.print( " b=");Serial.println(b);
  
  for (int i = 0; i < NUM_LEDS; i++)
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  pixels.show();
}

//---------------------------------------------------------------------------- play a short random melody with led ecffect
void triTone() {
  int freq = random(200, 1000);

  for (int i = 0; i < 3; i++) {
    freq *= random(2) == 0 ? .5 : 2;
    if (freq<50)freq*=4;
    if (freq>1500)freq/=4;    
    led(random(5) * 20, random(5) * 20, random(5) * 20);
    tone(SPEAKER, freq, 100);
    delay(100);
  }

}
