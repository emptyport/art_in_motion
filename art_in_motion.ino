#include <Adafruit_NeoPixel.h>

#define PIN 6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(50, PIN, NEO_GRB + NEO_KHZ800);

int wait = 40;
int lifetime = 40;

// The light strand is set up in a snaking fashion to create
// the grid. This is the encoding for how that is.
int indexCoding[7][7] = { {  1,  2,  3,  4,  5,  6,  7 },
                          { 14, 13, 12, 11, 10,  9,  8 },
                          { 15, 16, 17, 18, 19, 20, 21 },
                          { 28, 27, 26, 25, 24, 23, 22 },
                          { 29, 30, 31, 32, 33, 34, 35 },
                          { 42, 41, 40, 39, 38, 37, 36 },
                          { 43, 44, 45, 46, 47, 48, 49 } };

// This matrix holds the R, G, B, R_step, G_step, B_step, and 
// number of steps lived for each position
int lightValues[7][7][7];

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  strip.begin();
  strip.show();
}

void loop() {
  growCurrentLife();
  createSpontaneousLife();
  updateLife();
  strip.show();
  delay(wait);
}

void growCurrentLife() {
  for(int i=0; i<7; i++) {
    for(int j=0; j<7; j++) {
      int nSteps = lightValues[i][j][6];
      int R = lightValues[i][j][0];
      int G = lightValues[i][j][1];
      int B = lightValues[i][j][2];
      // Don't want the pixel to be too old and being greater
      // than 0 makes sure we don't grow brand new life
      if(nSteps<10 && nSteps>0) {
        // Get neighboring pixels
        if(i>0) { processNeighbor(i-1, j, R, G, B); }
        if(i>0 && j>0) { processNeighbor(i-1, j-1, R, G, B); }
        if(i>0 && j<6) { processNeighbor(i-1, j+1, R, G, B); }
        if(j>0) { processNeighbor(i, j-1, R, G, B); }
        if(j<6) { processNeighbor(i, j+1, R, G, B); }
        if(i<6 && j>0) { processNeighbor(i+1, j-1, R, G, B); }
        if(i<6) { processNeighbor(i+1, j, R, G, B); }
        if(i<6 && j<6) { processNeighbor(i+1, j+1, R, G, B); }
      }
    }
  } 
}

void processNeighbor(int i, int j, int R, int G, int B) {
  int checkR = lightValues[i][j][0];
  int checkG = lightValues[i][j][1];
  int checkB = lightValues[i][j][2];
  int checkSteps = lightValues[i][j][6];
  
  if((checkR==0 && checkG==0 && checkB==0) || checkSteps>20) {
    int rand = random(200);
    if(rand<20) {
      int stepR = calculateStep(R);
      int stepG = calculateStep(G);
      int stepB = calculateStep(B);
      lightValues[i][j][0] = R;
      lightValues[i][j][1] = G;
      lightValues[i][j][2] = B;
      lightValues[i][j][3] = stepR;
      lightValues[i][j][4] = stepG;
      lightValues[i][j][5] = stepB;
      lightValues[i][j][6] = 0;
    }
  }
}

int calculateStep(int val) {
  int step = val / lifetime;
  if(step==0) { step = 1; }
  return step;
}

void createSpontaneousLife() {
  for(int i=0; i<7; i++) {
    for(int j=0; j<7; j++) {
      int R = lightValues[i][j][0];
      int G = lightValues[i][j][1];
      int B = lightValues[i][j][2];
      
      if(R==0 && G==0 && B==0) {
        int rand = random(200);
        if(rand<25) {
          R = random(256);
          G = random(256);
          B = random(256);
          int stepR = calculateStep(R);
          int stepG = calculateStep(G);
          int stepB = calculateStep(B);
          lightValues[i][j][0] = R;
          lightValues[i][j][1] = G;
          lightValues[i][j][2] = B;
          lightValues[i][j][3] = stepR;
          lightValues[i][j][4] = stepG;
          lightValues[i][j][5] = stepB;
          lightValues[i][j][6] = -40;
        }
      }
    }
  } 
}

int calculatePixel(int val, int stepVal, int nSteps) {
  int pixelValue = val - stepVal * abs(nSteps);
  if(pixelValue<0) { pixelValue = 0; }
  return pixelValue;
}

void updateLife() { 
  for(int i=0; i<7; i++) {
    for(int j=0; j<7; j++) {
      int R = lightValues[i][j][0];
      int G = lightValues[i][j][1];
      int B = lightValues[i][j][2];
      int stepR = lightValues[i][j][3];      
      int stepG = lightValues[i][j][4];
      int stepB = lightValues[i][j][5];
      int nSteps = lightValues[i][j][6];
      
      int mappedIndex = indexCoding[i][j];
      
      int calcR = calculatePixel(R, stepR, nSteps);
      int calcG = calculatePixel(G, stepG, nSteps);
      int calcB = calculatePixel(B, stepB, nSteps);
      strip.setPixelColor(mappedIndex, calcR, calcG, calcB);
      
      if(calcR==0 && calcG==0 && calcB==0) {
       lightValues[i][j][0] = 0; 
       lightValues[i][j][1] = 0; 
       lightValues[i][j][2] = 0; 
       lightValues[i][j][3] = 0; 
       lightValues[i][j][4] = 0; 
       lightValues[i][j][5] = 0; 
       lightValues[i][j][6] = 0; 
      }
      else {
        lightValues[i][j][6]++;
      }
    }
  }
}

