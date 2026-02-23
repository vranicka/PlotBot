
//---------------------------------------------------------------------------- calculate angle between the coordinates ABC
float calculateSignedAngleABC(float Ax, float Ay, float Bx, float By, float Cx, float Cy) {
  float BAx = Ax - Bx, BAy = Ay - By;
  float BCx = Cx - Bx, BCy = Cy - By;

  float dotProduct = BAx * BCx + BAy * BCy;
  float crossProduct = BAx * BCy - BAy * BCx;  // direction

  float angleRad = atan2(crossProduct, dotProduct);  // signed angle
  float angleDeg = angleRad * (180.0 / M_PI);

  return angleDeg;  // returned angle can be negative or positive
}

//---------------------------------------------------------------------------- calculate distance between 2 coordinates
float calculateDistanceBC(float Bx, float By, float Cx, float Cy) {   
  return sqrt((Cx - Bx) * (Cx - Bx) + (Cy - By) * (Cy - By));
}

//---------------------------------------------------------------------------- returns -1 for values<0, +1 for values >0 and 0 for 0
int sign(float x) {
    return (x > 0) - (x < 0);
}
