#include "LedControl.h" 

#define Y_JOY1 A1
#define Y_JOY2 A0
#define SECOND 1000

struct Paddle {
  int X,Y;
};

struct Ball {
  int X, Y;
  int directionX, directionY;
};

struct Player {
  int score;
};

LedControl lc = LedControl(12, 11, 10, 1); //DIN, CLK, LOAD, No. DRIVER

int three[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,1,1,1,1,1,1,0},
  {0,0,0,0,0,0,1,0},
  {0,0,0,0,0,0,1,0},
  {0,1,1,1,1,1,1,0},
  {0,0,0,0,0,0,1,0},
  {0,0,0,0,0,0,1,0},
  {0,1,1,1,1,1,1,0}
};

int two[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,1,1,1,1,1,1,0},
  {0,0,0,0,0,0,1,0},
  {0,0,0,0,0,0,1,0},
  {0,1,1,1,1,1,1,0},
  {0,1,0,0,0,0,0,0},
  {0,1,0,0,0,0,0,0},
  {0,1,1,1,1,1,1,0}
};

int one[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,1,0,0,0},
  {0,0,0,1,1,0,0,0},
  {0,0,1,0,1,0,0,0},
  {0,0,0,0,1,0,0,0},
  {0,0,0,0,1,0,0,0},
  {0,0,0,0,1,0,0,0},
  {0,0,0,0,1,0,0,0}
};

int GO[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {1,1,1,1,0,1,1,1},
  {1,0,0,0,0,1,0,1},
  {1,0,1,1,0,1,0,1},
  {1,0,0,1,0,1,0,1},
  {1,1,1,1,0,1,1,1},
  {0,0,0,0,0,0,0,0}
};

unsigned long currentTime, timeOfLastBallMove, timeOfLastUpdate = 0, timeOfLastFirstPaddleMove = 0, timeOfLastSecondPaddleMove = 0;
int valY1,valY2,paddleHeight = 3, ballSpawned = 0, periodPaddles = 100, periodBall = 1000, gameState = 0, speedIncrement = 0;
Paddle paddle1[3] = { {2 , 0}, {3 , 0}, {4 , 0} };
Paddle paddle2[3] = { {2 , 7}, {3 , 7}, {4 , 7} };
Ball ball;
Player player1,player2;

void drawPaddles() {
 for (int i = 0; i < paddleHeight; i++) {
     lc.setLed(0,paddle1[i].X,paddle1[i].Y,1);  
     lc.setLed(0,paddle2[i].X,paddle2[i].Y,1);
 }
}

void spawnBall() {
  ball.X = random(3,5);
  ball.Y = random(3,5);
  int randX = random(0,10);
  int randY = random(0,10);
  if (randX % 2 == 0) 
    ball.directionX = 1;
  else 
    ball.directionX = -1;
  if(randY % 2 == 0) 
    ball.directionY = 1;
  else 
    ball.directionY = -1;
  ballSpawned = 1;
}

void drawBall() {
  lc.setLed(0,ball.X,ball.Y,1);  
}

void lowerPaddle(Paddle p[],int n) {
  if (p[0].X == 0) 
    return;
  for (int i = 0; i < paddleHeight; i++) {
    p[i].X -= n;
  }
}

void raisePaddle(Paddle p[],int n) {
  if (p[paddleHeight - 1].X == 7) 
    return;
  for (int i = 0; i < paddleHeight; i++) {
    p[i].X += n;
  }
}

void detectBoundCollision() {
  if (ball.X <= 0 || ball.X >= 7) ball.directionX *= -1;  
}

void checkIfScored() {
  if(ball.Y <= 0 || ball.Y >= 7) {
    if(ball.Y <= 0) 
      player2.score++;
    else if(ball.Y >= 7) 
      player1.score++;
   ballSpawned = 0;
   gameState = 0;
   speedIncrement = 0;
  }
}

void detectPaddleCollision() {
  if(ball.X >= paddle1[0].X && ball.X <= paddle1[paddleHeight - 1].X && ball.Y == paddle1[0].Y + 1
    ) {//check if the ball is next to the first paddle         //if heading towards the paddle
      ball.directionY *= -1;         //change direction on X axis
      speedIncrement += 50;
  }
  //check collision for the second paddle
  if(ball.X >= paddle2[0].X && ball.X <= paddle2[paddleHeight - 1].X && ball.Y == paddle2[0].Y - 1) {
      ball.directionY *= -1;
      speedIncrement += 50;
  }
}

void movePaddles() {
  valY1 = analogRead(Y_JOY1);
  valY2 = analogRead(Y_JOY2);
  if(millis() - timeOfLastFirstPaddleMove > periodPaddles) {
    if(valY1 < 400 ) {
      raisePaddle(paddle1,1);
    }
    if(valY1 > 600) {
      lowerPaddle(paddle1,1);
    }
    timeOfLastFirstPaddleMove = millis();
  }
  if(millis() - timeOfLastSecondPaddleMove > periodPaddles) {
   if(valY2 < 400) {
     raisePaddle(paddle2,1);
   }
   if(valY2 > 600) {
      lowerPaddle(paddle2,1);
    }
    timeOfLastSecondPaddleMove = millis();
  }
}

void updateBall() {
  ball.X += ball.directionX;
  ball.Y += ball.directionY;
  timeOfLastBallMove= millis();
  detectPaddleCollision();
  detectBoundCollision();
  checkIfScored();
}

void draw() {
  if (millis() - timeOfLastUpdate > 100) {
  lc.clearDisplay(0);
  drawPaddles();
  drawBall();
  movePaddles();
  if(millis() - timeOfLastBallMove > periodBall - speedIncrement) 
  updateBall();
  currentTime = millis();
  timeOfLastUpdate = millis();
  }
}

void updateGame() {
    draw();
}

void displayMatrix(int m[8][8]) {
  for(int i = 0; i < 8; i++)
    for(int j = 0; j < 8; j++)
      lc.setLed(0,i,j,m[i][j]);
}

void playCountdown() {
  currentTime = millis();
  displayMatrix(three);
  while (millis() < currentTime + SECOND) {}
  displayMatrix(two);
  while (millis() < currentTime + 2*SECOND) {}
  displayMatrix(one);
  while (millis() < currentTime + 3*SECOND) {}
  displayMatrix(GO);
  while (millis() < currentTime + 4*SECOND) {}
}

void setPaddles() {
 // paddle1[3] = { {2 , 0}, {3 , 0}, {4 , 0} };
 // paddle2[3] = { {2 , 7}, {3 , 7}, {4 , 7} };
 for (int i = 0; i < 3; i++) {
    paddle1[i].X = i + 2;
    paddle1[i].Y = 0;
    paddle2[i].X = i + 2;
    paddle2[i].Y = 7;
  }
}

void setup() {
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, 2); // sets brightness 
  lc.clearDisplay(0);// clear screen
  Serial.begin(9600);
  randomSeed(analogRead(2));
  player1.score = 0;
  player2.score = 0;
}

void loop() {
  if(!gameState) {
    playCountdown();
    setPaddles();
    gameState = 1;
  } else {
    if(ballSpawned == 0) spawnBall();
    updateGame();
  }
}
