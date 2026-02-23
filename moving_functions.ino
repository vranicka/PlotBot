//---------------------------------------------------------------------------- moves forward or backward for distance (mm)
//                                                                             direction depends on global direction value
bool move(float distance, bool bounceAtObstacle = true) {
  uint16_t targetValue = uint16_t(stepsPerMM * abs(distance));
  uint16_t halfTarget = targetValue / 2;
  delayValue = 2000;  // stepper delay
  for (uint16_t i = 0; i < targetValue; i++) {
    switchStepper(1, direction);
    switchStepper(2, direction);
    if (i < halfTarget) {
      delayValue -= rampValue;
    } else {
      delayValue += rampValue;
    }
    delayMicroseconds(constrain(delayValue, minDelay, maxDelay));
    if (bounceAtObstacle) bounce();  // check for physical obstacles and bounce if flag is set
  }
  stopSteppers();
}

//---------------------------------------------------------------------------- check bumper switches and return to "new line"
void bounce() {

 
  bool penState = drawing;
  bool newLineStarted = false;
  bool newLineFinished = false;

  if (checkBumper()) {
    penUp();
    turn(180, 1);
    move(10);
    delay(20);
    turn(90, -1);      
    move(20);
    delay(20);
    turn(90, 1); 
    move(420);        //move until new bumper check, 
    turn (180);
//    newLineStarted = false;
//    newLineFinished = true;
  }

  if (penState) {
    penDown();  //                                             reset pen state
  }
}


//---------------------------------------------------------------------------- forward (mm) - only accepts positive values
void forward(float distance) {
  direction = 1;
  move(distance);
}

//---------------------------------------------------------------------------- backward (mm) - only accepts positive values
void back(float distance) {
  direction = -1;
  move(distance);
}

//---------------------------------------------------------------------------- turn (degrees, direction 1= clockwise / -1= counterclockwise)
void turn(float degrees, int turningDirection = 1) {

  if (degrees < 0) {  // got negative angle? flip direction and angle
    degrees = -degrees;
    turningDirection *= -1;
  }

  while (degrees >= 360) { degrees -= 360; }

  uint16_t targetValue = uint16_t(degrees * stepsPer360Rotation / 360 + .5);
  uint16_t halfTarget = targetValue / 2;
  delayValue = 2000;  // stepper delay

  for (uint16_t i = 0; i < targetValue; i++) {  // turn
    switchStepper(1, -turningDirection);
    switchStepper(2, turningDirection);
    if (i < halfTarget) {
      delayValue -= rampValue;
    } else {
      delayValue += rampValue;
    }
    delayMicroseconds(constrain(delayValue, minDelay, maxDelay));
  }
}

//---------------------------------------------------------------------------- optimized turn (angle)
//                                                                             turns sometimes faster by changing global direction if angle > 90°

void smartTurn(float angle) {

  while (angle > 180) { angle -= 360; }  //                                   normalize to -180 ... 180 ---
  while (angle <= -180) { angle += 360; }

  float optimizedAngle = angle;

  //                                                                          optimize if the angle requires turning more than 90°
  if (optimizedAngle > 90) {
    direction *= -1;                        //                                flip forward/backward
    optimizedAngle = optimizedAngle - 180;  //                                shrink turn into -90 ... 0
  } else if (optimizedAngle < -90) {
    direction *= -1;                        //                                flip forward/backward
    optimizedAngle = optimizedAngle + 180;  //                                shrink turn into 0 ... +90
  } else if (optimizedAngle == 180 || optimizedAngle == -180) {
    //                                                                        special case: exact 180° turn → flip direction instead
    direction *= -1;
    optimizedAngle = 0;  //                                                   no need to rotate
  }
  turn(optimizedAngle, 1);  //                                                execute the turn
}

//---------------------------------------------------------------------------- draw a vector drawing based on a list of coordinates
void drawCoordinates(float scale = 1.0) {
  int amountOfCoordinates = sizeof(coordinates) / sizeof(coordinates[0]);  // get number of coordinates

  float ax = -2;  //                                                          default coordinates for starting (A) and pivot (B) point in one line leading to 0,0
  float ay = 128;
  float bx = -1;
  float by = 128;
  for (int i = 0; i < amountOfCoordinates; i++) {  //                         go through each coordinate


    if (coordinates[i][2]) {  //                                              lower / lift pen
      penDown();
    } else {
      penUp();
    }
    float cx = coordinates[i][0];
    float cy = 255 - coordinates[i][1];

    float turningAngle = 180 - calculateSignedAngleABC(ax, ay, bx, by, cx, cy);  // calculate how the angle (ABC) deviates from a straight line
    float distanceBC = calculateDistanceBC(bx, by, cx, cy) * scale;

    smartTurn(turningAngle);  //                                              turn
    move(distanceBC);         //                                              move (BC)

    ax = bx;  //                                                              shift points (BC) to (AB) as we will get a new (C)
    ay = by;
    bx = cx;
    by = cy;
  }
}



//---------------------------------------------------------------------------- circles and spirals

bool spiral(double initialRadiusMM, double finalRadiusMM, double revolutions = 1.0, bool clockwise = true, bool stopAtSwitch = true, bool stopAtSensor = false, bool togglePenAtSensor = false) {

  double radialIncrementPerRevolution = (finalRadiusMM - initialRadiusMM) / revolutions;  //  calculate the amount the radius should increase for each full revolution

  const int SEGMENTS_PER_DEGREE = 5;  //                                                      calculate the spiral path in small angular segment
  long totalAngularSegments = (long)(revolutions * 360.0 * SEGMENTS_PER_DEGREE);
  double angleIncrementRad = (2.0 * PI) / (360.0 * SEGMENTS_PER_DEGREE);  //                  angle increment in radians per segment

  //                                                                                          calculate a total distance to travel for the entire spiral
  double totalDistance = 0.0;
  for (long i = 0; i < totalAngularSegments; i++) {
    double currentAngleRad = (double)i * angleIncrementRad;
    double currentRadiusMM = initialRadiusMM + (currentAngleRad / (2 * PI)) * radialIncrementPerRevolution;

    // --- THIS IS THE CORRECTED CALCULATION ---
    double radiusOuterWheel = currentRadiusMM + (wheelDistance / 2.0);
    double radiusInnerWheel = currentRadiusMM - (wheelDistance / 2.0);
    if (radiusInnerWheel < 0.0) radiusInnerWheel = -radiusInnerWheel;  //                     account for tight turns
    double arcLengthOuter = radiusOuterWheel * angleIncrementRad;
    double arcLengthInner = radiusInnerWheel * angleIncrementRad;
    double arcLengthCenter = (arcLengthOuter + arcLengthInner) / 2.0;

    totalDistance += arcLengthCenter;
  }

  long estimatedTotalOuterStepsForRamp = (long)(totalDistance * stepsPerMM);
  if (estimatedTotalOuterStepsForRamp < 100) estimatedTotalOuterStepsForRamp = 100;
  long halfTargetOuterStepsForRamp = estimatedTotalOuterStepsForRamp / 2;
  delayValue = 2000;

  double distanceTraveled = 0.0;        //                                                   tracks the total distance moved
  long innerStepAccumulator = 0;        //                                                   accumulates "credit" for inner motor steps
  const long ACCUMULATOR_SCALE = 1000;  //                                                   scaling factor for precision in integer arithmetic

  double finalDistance = totalDistance;       //                                             the correct total distance is now assigned
  double toggleDistance = totalDistance * 2;  //                                             default distance in which pen will be toggled (behind final distance)

  long currentAngularSegment = 0;
  while (distanceTraveled < finalDistance) {
    //                                                                                       calculate the robot's center radius for this specific angular point
    //                                                                                       ensures even radial spacing per revolution.
    double currentRadiusMM = initialRadiusMM + ((double)currentAngularSegment * angleIncrementRad / (2 * PI)) * radialIncrementPerRevolution;
    currentAngularSegment++;

    //                                                                                       determine wheel directions and effective radii for the current curve segment
    int dirMotor1 = 1;
    int dirMotor2 = 1;
    double radiusOuterWheel;
    double radiusInnerWheel;

    if (currentRadiusMM > wheelDistance / 2.0) {
      radiusOuterWheel = currentRadiusMM + (wheelDistance / 2.0);
      radiusInnerWheel = currentRadiusMM - (wheelDistance / 2.0);
    } else if (fabs(currentRadiusMM - (wheelDistance / 2.0)) < 0.01) {  //                   when inner wheel is stationary
      radiusOuterWheel = currentRadiusMM + (wheelDistance / 2.0);
      radiusInnerWheel = 0.0;
    } else {  //                                                                             currentRadiusMM < wheelDistance / 2.0 (wheels move in opposite directions for tight turns)
      radiusOuterWheel = (wheelDistance / 2.0) + currentRadiusMM;
      radiusInnerWheel = (wheelDistance / 2.0) - currentRadiusMM;

      if (clockwise) {
        dirMotor1 = -1;  //                                                                  set motor directions
        dirMotor2 = 1;   //
      } else {
        dirMotor1 = 1;   //
        dirMotor2 = -1;  //
      }
    }

    //                                                                                       calculate the ratio of steps between outer and inner wheel for this segment
    double stepsRatio = 1.0;
    if (radiusInnerWheel > 0.0) {
      stepsRatio = radiusOuterWheel / radiusInnerWheel;
    }

    //                                                                                       calculate how many physical steps the outer wheel *should* take for this small angular segment
    long requiredOuterStepsForSegment = (long)(radiusOuterWheel * angleIncrementRad * stepsPerMM + 0.5);  // +0.5 for proper rounding
    if (requiredOuterStepsForSegment == 0 && radiusOuterWheel > 0) {                                      // ensure at least one step if turning
      requiredOuterStepsForSegment = 1;
    }

    //                                                                                       execute the physical steps required for this angular segment
    for (long k = 0; k < requiredOuterStepsForSegment; k++) {
      //                                                                                     determine which motor is outer/inner based on clockwise direction
      uint8_t outerMotorNum, innerMotorNum;
      int outerMotorDir, innerMotorDir;

      if (clockwise) {
        outerMotorNum = 2;
        outerMotorDir = dirMotor2;
        innerMotorNum = 1;
        innerMotorDir = dirMotor1;
      } else {
        outerMotorNum = 1;
        outerMotorDir = dirMotor1;
        innerMotorNum = 2;
        innerMotorDir = dirMotor2;
      }

      //                                                                                     step the outer motor (always moves for this segment)
      switchStepper(outerMotorNum, outerMotorDir);
      innerStepAccumulator += ACCUMULATOR_SCALE;

      // conditionally step the inner motor based on the calculated ratio for this segment
      if (radiusInnerWheel > 0.0 && innerStepAccumulator >= stepsRatio * ACCUMULATOR_SCALE) {
        switchStepper(innerMotorNum, innerMotorDir);
        innerStepAccumulator -= stepsRatio * ACCUMULATOR_SCALE;
      }

      //                                                                                     distance calculation
      double distanceOuterStep = 1.0 / stepsPerMM;
      double distanceInnerStep = (radiusInnerWheel > 0.0) ? distanceOuterStep * (radiusInnerWheel / radiusOuterWheel) : 0.0;
      double distanceCenterStep = (distanceOuterStep + distanceInnerStep) / 2.0;
      distanceTraveled += distanceCenterStep;

      //                                                                                     collision detection
      if (stopAtSwitch && checkBumper()) {
        stopSteppers();
        return false;
      }

      bool sensorCollision = checkSensor();  //                                              check photo sensor (only once)

      if (stopAtSensor && sensorCollision) {
        tone(SPEAKER, 500, 500);
        finalDistance = min(distanceTraveled + sensorOffset, finalDistance);  //             recalculate target distance
        stopAtSensor = false;                                                 //             set the stop at sensor flag to false (we want to move uninterrupted until the line)
      }

      if (togglePenAtSensor && sensorCollision && toggleDistance > finalDistance) {
        tone(SPEAKER, random(1000, 2000), 50);
        toggleDistance = distanceTraveled + sensorOffset;  //                                recalculate target distance (move the additional distance between pen and photo sensor)
      }

      if (distanceTraveled > toggleDistance) {  //                                           arrived at toggle distance? toggle the pen
        toggleDistance = finalDistance * 2;
        if (drawing == 1) {
          penUp();
        } else {
          penDown();
        }
      }

      //                                                                                    apply the acceleration / deceleration ramp based on total distance
      if (distanceTraveled < finalDistance / 2.0) {
        delayValue -= rampValue;
      } else {
        delayValue += rampValue;
      }
      delayMicroseconds(constrain(delayValue, minDelay, maxDelay));


      if (distanceTraveled >= finalDistance) break;  //                                     break out of this inner loop if the final distance is reached
    }

    if (distanceTraveled >= finalDistance) break;  //                                       break out of the outer loop if the final distance is reached
  }

  stopSteppers();
  return true;
}
