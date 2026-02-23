//---------------------------------------------------------------------------- write a string in a specified size (mm)
void plotText(String str, int scale) {

  for (int i = 0; i < str.length(); i++) {
    triTone();
    char c = char(str.charAt(i));
    if (byte(c) != 195) {
      if (c == ' ') {
        move(scale);
      } else {
        plotChar(c, scale);
      }
    }
    delay(50);
  }
  if (direction == -1) {
    turn(180);
    direction = 1;
  }
}

//---------------------------------------------------------------------------- write one character
void plotChar(uint8_t character, float scale) {

  uint8_t index = ASCIItoIndex(character);
  scale = scale / 4;

  float ax = -2;  //                                    default coordinates for starting (A) and pivot (B) point in one line leading to 0,0
  float ay = 0;
  float bx = -1;
  float by = 0;
  float xMax = 0;


  for (int i = 0; i < 14; i++) {  //                    go through each fontCoordinates of the character
    int v = fontCoordinates[index][i];
    if (v == 200) {  //                                 no more fontCoordinatess in this array - move to end of letter
      break;
    }
    if (v == 222) {  //                                 plot single point
      penDown();
      penUp();
    } else {

      bool drawLine = 0;  //                            extract draw / nodraw
      if (v > 99) {
        drawLine = 1;
        v -= 100;
      }

      if (drawLine) {  //                               lower / lift pen
        penDown();
      } else {
        penUp();
      }

      float cx = int(v / 10);        //                 extract x for point (C)
      float cy = int(v - cx * 10);   //                 extract y for point (C)
      if (cx > xMax) { xMax = cx; }  //                 remember maximum x value for next character position

      float turningAngle = 180 - calculateSignedAngleABC(ax, ay, bx, by, cx, cy);  // calculate how the angle (ABC) deviates from a straight line
      float distanceBC = calculateDistanceBC(bx, by, cx, cy) * scale;

      smartTurn(turningAngle);  // turn
      move(distanceBC);             // move (BC)

      ax = bx;  // shift points (BC) to (AB) as we will get a new (C)
      ay = by;
      bx = cx;
      by = cy;
    }
  }
  // all fontCoordinatess of the character are drawn, let's move to the origin (0,0) of the next character
  penUp();

  float cx = xMax + 1;
  float cy = 0;

  float turningAngle = 180 - calculateSignedAngleABC(ax, ay, bx, by, cx, cy);  // calculate how the angle (ABC) deviates from a straight line
  float distanceBC = calculateDistanceBC(bx, by, cx, cy) * scale;

  smartTurn(turningAngle);  // turn
  move(distanceBC);             // move (BC)

  ax = bx;  // shift points (BC) to (AB) as we will get a new imaginary (C) to orient the robot
  ay = by;
  bx = cx;
  by = cy;

  cx = xMax + 2;
  cy = 0;

  turningAngle = 180 - calculateSignedAngleABC(ax, ay, bx, by, cx, cy);
  smartTurn(turningAngle);
}





//---------------------------------------------------------------------------- convert ASCII to index of fontCoordinates array
int ASCIItoIndex(uint8_t c) {
  uint8_t index = 38;
  if (uint8_t(c) > 64 and uint8_t(c) < 91) {  //A...Z
    index = uint8_t(c) - 65;
  }
  if (uint8_t(c) > 96 and uint8_t(c) < 123) {  //A...Z
    index = uint8_t(c) - 97;
  }
  if (uint8_t(c) > 47 and uint8_t(c) < 58) {  //0...9
    index = uint8_t(c) - 22;
  }
  if (uint8_t(c) == 164 || uint8_t(c) == 132) {  //ä,Ä
    index = 39;
  }
  if (uint8_t(c) == 182 || uint8_t(c) == 150) {  //ö,Ö
    index = 40;
  }
  if (uint8_t(c) == 188 || uint8_t(c) == 156) {  //ü,Ü
    index = 41;
  }
  if (uint8_t(c) == 44) {  // ,
    index = 42;
  }
  if (uint8_t(c) == 45) {  // -
    index = 43;
  }
  if (uint8_t(c) == 46) {  // .
    index = 44;
  }
  if (uint8_t(c) == 33) {  // !
    index = 45;
  }
  if (uint8_t(c) == 63) {  // ?
    index = 46;
  }

  if (uint8_t(c) == 123) { /*{ ß*/
    index = 47;
  }
  if (uint8_t(c) == 39) { /*'*/
    index = 48;
  }
  if (uint8_t(c) == 38) { /*&*/
    index = 49;
  }
  if (uint8_t(c) == 43) { /*+*/
    index = 50;
  }
  if (uint8_t(c) == 58) { /*:*/
    index = 51;
  }
  if (uint8_t(c) == 59) { /*;*/
    index = 52;
  }
  if (uint8_t(c) == 34) { /*"*/
    index = 53;
  }
  if (uint8_t(c) == 35) { /*#*/
    index = 54;
  }
  if (uint8_t(c) == 40) { /*(*/
    index = 55;
  }
  if (uint8_t(c) == 41) { /*)*/
    index = 56;
  }
  if (uint8_t(c) == 61) { /*=*/
    index = 57;
  }
  if (uint8_t(c) == 64) { /*@*/
    index = 58;
  }
  if (uint8_t(c) == 42) { /***/
    index = 59;
  }
  if (uint8_t(c) == 125) { /*} Smiley*/
    index = 60;
  }
  if (uint8_t(c) == 126) { /*~ Open mouth Smiley*/
    index = 61;
  }
  if (uint8_t(c) == 36) { /*$ Heart*/
    index = 62;
  }
  return index;
}
