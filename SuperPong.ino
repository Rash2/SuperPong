#include "LedControl.h" 
#include <LiquidCrystal.h>

#define V0_PIN 9
#define Y_JOY1 A1
#define Y_JOY2 A0
#define X_JOY1 A3
#define X_JOY2 A2
#define SECOND 1000
#define MAXIMUM_THRESHOLD 600
#define MINIMUM_THRESHOLD 400
#define MATRIX_UPPER_LIMIT 7
#define MATRIX_LOWER_LIMIT 0
#define PADDLE_DELAY 100
#define BALL_DELAY 1000
#define SCORE_LIMIT 7

LiquidCrystal lcd(2, 3, 4, 5, 6 , 7);

//declare structures with attributes 

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

//define matrix as the frames for the countdown animation

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
int valY1,valY2,valX1,valX2,paddleHeight = 3, ballSpawned = 0, gameState = 0, speedIncrement = 0, randomOne;
Paddle paddle1[3], paddle2[3];
Ball ball;
Player player1,player2;
bool isPlaying = false, player1Ready = false, player2Ready = false, gameOver = false;  

void drawPaddles() {
 for (int i = 0; i < paddleHeight; i++) {
     lc.setLed(0,paddle1[i].X,paddle1[i].Y,1);  
     lc.setLed(0,paddle2[i].X,paddle2[i].Y,1);
 }
}

//give a random spawn point for the ball at the start of each round
//to simulate getting a 50% chance , we choose a number from 0 - 10
//and check whether it's odd or even

void spawnBall() {
  ball.X = random(3,5);
  ball.Y = random(3,5);
  int randX = random(0,10);
  int randY = random(0,10);
  
  if (randX % 2 == 0) {
    ball.directionX = 1;
  } else {
    ball.directionX = -1;
  }
  
  if (randY % 2 == 0) {
    ball.directionY = 1;
  } else {
    ball.directionY = -1;
  }
  
  ballSpawned = 1;
}

void drawBall() {
  lc.setLed(0,ball.X,ball.Y,1);  
}

//if the paddle isn't at the lowest possible position
//we lower it with n positions

void lowerPaddle(Paddle p[],int n) {
  if (p[0].X == MATRIX_LOWER_LIMIT) 
    return;
  for (int i = 0; i < paddleHeight; i++) {
    p[i].X -= n;
  }
}

//if the paddle isn't at the highest possible position
//we raise it with n positions

void raisePaddle(Paddle p[],int n) {
  if (p[paddleHeight - 1].X == MATRIX_UPPER_LIMIT) 
    return;
  for (int i = 0; i < paddleHeight; i++) {
    p[i].X += n;
  }
}

//if the ball hits the 'ceiling' or the 'floor', it bounces

void detectBoundCollision() {
  if (ball.X <= MATRIX_LOWER_LIMIT || ball.X >= MATRIX_UPPER_LIMIT) {
    ball.directionX *= -1;  
  }
}

void checkIfScored() {
  if (ball.Y <= MATRIX_LOWER_LIMIT || ball.Y >= MATRIX_UPPER_LIMIT) {
    if (ball.Y <= MATRIX_LOWER_LIMIT) {
      player2.score++;
    } else if(ball.Y >= MATRIX_UPPER_LIMIT) {
      player1.score++;
    }
    
   displayInGameMessage(player1.score, player2.score);
   ballSpawned = 0;
   gameState = 0;
   speedIncrement = 0;
  }
}

//for paddle collision, we check if the ball is right next to the paddle
//and give it a random X direction to make the game more challenging;
//we also increase the ball speed after each paddle hit, and make it
//accelerate slower after a certain amount

void detectPaddleCollision() {
  if (ball.X >= paddle1[0].X && ball.X <= paddle1[paddleHeight - 1].X && ball.Y == paddle1[0].Y + 1) {         
      ball.directionY *= -1;    
                                                                           
      if (speedIncrement <= 500) {
        speedIncrement += 100;
      } else {
        speedIncrement += 20;
      }
      
      if(ball.X > MATRIX_LOWER_LIMIT && ball.X < MATRIX_UPPER_LIMIT) {
        randomOne = random(0,10);
        if (randomOne % 2 == 0) 
         ball.directionX = 1;
       else 
         ball.directionX = -1; 
      }
  }

  if(ball.X >= paddle2[0].X && ball.X <= paddle2[paddleHeight - 1].X && ball.Y == paddle2[0].Y - 1) {
      ball.directionY *= -1;
      
      if (speedIncrement <= 500) {
        speedIncrement += 100;
      } else {
        speedIncrement += 20;
      }
      
      if(ball.X > MATRIX_LOWER_LIMIT && ball.X < MATRIX_UPPER_LIMIT) {
       randomOne = random(0,10);
       if (randomOne % 2 == 0) 
        ball.directionX = 1;
       else 
         ball.directionX = -1; 
      }
  }
}

void movePaddles() {
  valY1 = analogRead(Y_JOY1);
  valY2 = analogRead(Y_JOY2);
  if (millis() - timeOfLastFirstPaddleMove > PADDLE_DELAY) {
    if (valY1 < MINIMUM_THRESHOLD ) {
      raisePaddle(paddle1,1);
    }
    if (valY1 > MAXIMUM_THRESHOLD) {
      lowerPaddle(paddle1,1);
    }
    timeOfLastFirstPaddleMove = millis();
  }
  if (millis() - timeOfLastSecondPaddleMove > PADDLE_DELAY) {
   if (valY2 < MINIMUM_THRESHOLD) {
     raisePaddle(paddle2,1);
   }
   if (valY2 > MAXIMUM_THRESHOLD) {
      lowerPaddle(paddle2,1);
    }
    timeOfLastSecondPaddleMove = millis();
  }
}

void updateBall() {
  ball.X += ball.directionX;
  ball.Y += ball.directionY;
  timeOfLastBallMove = millis();
  detectPaddleCollision();
  detectBoundCollision();
  checkIfScored();
}

void updateGame() {
  if (millis() - timeOfLastUpdate > 100) {
  lc.clearDisplay(0);
  drawPaddles();
  drawBall();
  movePaddles();
  
  if (millis() - timeOfLastBallMove > BALL_DELAY - speedIncrement) {
    updateBall();
  }
  currentTime = millis();
  timeOfLastUpdate = millis();
  }
}


void displayMatrix(int m[8][8]) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      lc.setLed(0,i,j,m[i][j]);
    }
  }
}

//show the countdown animation before each round

void playCountdown() {
  displayMatrix(three);
  delay(SECOND);
  displayMatrix(two);
  delay(SECOND);
  displayMatrix(one);
  delay(SECOND);
  displayMatrix(GO);
  delay(SECOND);
}

//set the paddle in the starting position before each round

void setPaddles() {
 // paddle1[3] = { {2 , MATRIX_LOWER_LIMIT}, {3 , MATRIX_LOWER_LIMIT}, {4 , MATRIX_LOWER_LIMIT} };
 // paddle2[3] = { {2 , MATRIX_UPPER_LIMIT}, {3 , MATRIX_UPPER_LIMIT}, {4 , MATRIX_UPPER_LIMIT} };
 for (int i = 0; i < 3; i++) {
    paddle1[i].X = i + 2;
    paddle1[i].Y = MATRIX_LOWER_LIMIT;
    paddle2[i].X = i + 2;
    paddle2[i].Y = MATRIX_UPPER_LIMIT;
  }
}

void displayPreGameMessage(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Move joysticks");
  lcd.setCursor(0, 1);
  lcd.print("to play!Get to 7");
}

//upload the players' score to the lcd

void displayInGameMessage(int sc1, int sc2) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Player 1:");
  lcd.setCursor(10,0);
  lcd.print(sc1);
  lcd.setCursor(0,1);
  lcd.print("Player 2:");
  lcd.setCursor(10,1);
  lcd.print(sc2);
}

//for the game to start, each player has to move his or her joystick in any direction

void checkIfPlayersReady() {
  valX1 = analogRead(X_JOY1);
  valX2 = analogRead(X_JOY2);
  valY1 = analogRead(Y_JOY1);
  valY2 = analogRead(Y_JOY2);
  
  if (valY1 < MINIMUM_THRESHOLD || valY1 > MAXIMUM_THRESHOLD || valX1 < MINIMUM_THRESHOLD || valX1 > MAXIMUM_THRESHOLD) player1Ready = true;
  if (valY2 < MINIMUM_THRESHOLD || valY2 > MAXIMUM_THRESHOLD || valX2 < MINIMUM_THRESHOLD || valX2 > MAXIMUM_THRESHOLD) player2Ready = true;

  if (player1Ready == true && player2Ready == true) {
    isPlaying = true;
    displayInGameMessage(player1.score,player2.score);
  }
}

void endGame() {
  gameState = 2;
  player1.score = 0;
  player2.score = 0;
  gameOver = true;
}

void displayEndgameMessage() {
  lcd.clear();
  lc.clearDisplay(0);
  lcd.setCursor(4,0);
  if (player1.score > player2.score) {
    lcd.print("Player 1 won!");
  } else {
    lcd.print("Player 2 won!");
  }
  lcd.setCursor(4,1);
  lcd.print("Goodbye !");
}

void setup() {
  lc.shutdown(0, false); 
  lc.setIntensity(0, 2);  
  lc.clearDisplay(0);
  Serial.begin(9600);
  
  randomSeed(analogRead(2));
  
  player1.score = 0;
  player2.score = 0;

  //Setting lcd
  lcd.begin(16, 2);

  pinMode(V0_PIN, OUTPUT);
  analogWrite(V0_PIN, 90);
  
  displayPreGameMessage();
}

void playGame() {
  if (isPlaying == true) {
   if (gameState == 0) {
      playCountdown();
      setPaddles();
      gameState = 1;
    } else if(gameState == 1) {
      if (ballSpawned == 0) {
        spawnBall();
      }
      updateGame();
   }
   if (player1.score == SCORE_LIMIT || player2.score == SCORE_LIMIT) {
    endGame();
    isPlaying = false;
   }
  } else {
    checkIfPlayersReady();
    if(gameOver == true) {
      displayEndgameMessage();
    }
  }
}

void loop() {
  playGame();
}
