#include "LedControl.h" 

#define Y_JOY1 A1
#define Y_JOY2 A0

struct Paddle {
  int paddleX,paddleY;
};

struct Ball {
  int ballX, ballY;
  int ballSpeedX, ballSpeedY;
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
int valY1,valY2,paddleHeight = 3, ballSpawned = 0, periodPaddles = 100, periodBall = 1000, gameState = 0;
Paddle paddle1[3] = { {2 , 0}, {3 , 0}, {4 , 0} };
Paddle paddle2[3] = { {2 , 7}, {3 , 7}, {4 , 7} };
Ball ball;
Player player1,player2;

void drawPaddles() {
 for(int i = 0; i < paddleHeight; i++) {
     lc.setLed(0,paddle1[i].paddleX,paddle1[i].paddleY,1);  
     lc.setLed(0,paddle2[i].paddleX,paddle2[i].paddleY,1);
 }
}

void spawnBall() {
  ball.ballX = random(3,5);
  ball.ballY = random(3,5);
  int randX = random(0,10);
  int randY = random(0,10);
  if(randX % 2 == 0) ball.ballSpeedX = 1;
  else ball.ballSpeedX = -1;
  if(randY % 2 == 0) ball.ballSpeedY = 1;
  else ball.ballSpeedY = -1;
  ballSpawned = 1;
}

void drawBall() {
  lc.setLed(0,ball.ballX,ball.ballY,1);  
}

void lowerPaddle(Paddle p[],int n) {
  if(p[0].paddleX + n <= 7) p[0].paddleX += n;
  for(int i = 1; i < paddleHeight; i++) {
    p[i].paddleX = p[i-1].paddleX - 1;
  }
}

void raisePaddle(Paddle p[],int n) {
  if(p[0].paddleX - n >= 0) p[0].paddleX -= n;
  for(int i = 1; i < paddleHeight; i++) {
    p[i].paddleX = p[i-1].paddleX + 1;
  }
}

void detectBoundCollision() {
  if(ball.ballY < 0 || ball.ballY > 7) ball.ballSpeedY *= -1;  
}

void checkIfScored() {
  if(ball.ballX <= 0 || ball.ballX >= 7) {
    if(ball.ballX <= 0) player2.score++;
    else if(ball.ballX >= 7) player1.score++;
   // reset();
 /* Serial.println("Scor player1:");
  Serial.println(player1.score);
  Serial.println("Scor player2:");
  Serial.println(player2.score); */
   ballSpawned = 0;
   gameState = 0;
  }
}

//this function isn't properly working
void detectPaddleCollision() {
  if(ball.ballX >= paddle1[0].paddleX && ball.ballX <= paddle1[paddleHeight - 1].paddleX && ball.ballY == paddle1[0].paddleY + 1) { //check if the ball is next to the first paddle
    Serial.println("OK1");
    if(ball.ballSpeedX == -1)         //if heading towards the paddle
      ball.ballSpeedX *= -1;         //change direction on X axis
  }
  //check collision for the second paddle
  if(ball.ballX >= paddle2[0].paddleX && ball.ballX <= paddle2[paddleHeight - 1].paddleX && ball.ballY == paddle2[0].paddleY - 1) {
    Serial.println("OK2");
    if(ball.ballSpeedX == 1)
      ball.ballSpeedX *= -1;
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
  ball.ballX += ball.ballSpeedX;
  ball.ballY += ball.ballSpeedY;
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
  if(millis() - timeOfLastBallMove > periodBall) 
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
  while (millis() < currentTime + 1000) {}
  displayMatrix(two);
  while (millis() < currentTime + 2000) {}
  displayMatrix(one);
  while (millis() < currentTime + 3000) {}
  displayMatrix(GO);
  while (millis() < currentTime + 4000) {}
}

void setPaddles() {
 // paddle1[3] = { {2 , 0}, {3 , 0}, {4 , 0} };
 // paddle2[3] = { {2 , 7}, {3 , 7}, {4 , 7} };
 for (int i = 0; i < 3; i++) {
    paddle1[i].paddleX = i + 2;
    paddle1[i].paddleY = 0;
    paddle2[i].paddleX = i + 2;
    paddle2[i].paddleY = 7;
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
   // playCountdown();
    setPaddles();
    gameState = 1;
  } else {
    if(ballSpawned == 0) spawnBall();
    updateGame();
  }
}
