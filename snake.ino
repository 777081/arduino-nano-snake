#include <Wire.h>

#include <Adafruit_SH110X.h>

#include <stdlib.h>
#include <time.h>
#include <util/delay.h>

#define MAX_SNAKE_LENGTH 100
#define LEFT 13
//UP = LEFT - 1
//RIGHT = LEFT - 2
//DOWN = LEFT - 3

#define FRAME_DELAY 200

//SO L-R on odd numbers and U-D on even - easier direction checking


Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &Wire, -1);

uint8_t apple = 0; //x stored on 1st 4 bits, y on last 4
uint8_t length = 0;
uint8_t body[MAX_SNAKE_LENGTH]; //x stored on 1st 4 bits, y on last 4
uint8_t head = 0;
uint8_t direction = 2;
uint8_t tempDir = 5;

bool gameover = false;

void generateApple(){
  Serial.println("gen_apple");
  bool setApple = false;
  uint8_t new_pos;
  while(!setApple){
    bool collision = false;
    new_pos = rand() % 256;
    uint8_t i = 0;
    while(i < length+1 && !collision){
        if(body[(i+head) % MAX_SNAKE_LENGTH] == new_pos){
          collision = true;
        }
        i++;
    }
    if(!collision){
      setApple = true;
    }
  }
  apple = new_pos;
  Serial.println("done_apple");
}

void drawSegment(uint8_t pos){
  display.fillRect(4*(pos >> 4),
  4*(pos & 15),
  4,
  4,
  SH110X_WHITE);
}

void drawApple(uint8_t pos){
  display.fillCircle(4*(pos >> 4)+2,
  4*(pos & 15)+2,
  2,
  SH110X_WHITE);
}

bool updateSnake(){
  /*
  true - invalid move (game over)
  false - valid move
  */

  uint8_t tempHead = body[head];
  uint8_t x = tempHead >> 4;
  uint8_t y = tempHead & 15;
  switch (direction) {
    case 0:
      if(x == 0) return true;
      tempHead -= 16;
      break;
    case 1:
      if(y == 0) return true;
      tempHead -= 1;
      break;
    case 2:
      if(x == 15) return true;
      tempHead += 16;
      break;
    case 3:
      if(y == 15) return true;
      tempHead += 1;
      break;
  }

  for(uint8_t i = 3; i <= length; i++){
    /*optimization - collision impossible with head and first 2 elements
    checking starts on 3rd*/
    if(tempHead == body[(head+i) % MAX_SNAKE_LENGTH]){
      return true;
    }
  }

  head = (head + MAX_SNAKE_LENGTH - 1) % MAX_SNAKE_LENGTH;
  body[head] = tempHead;
  return false;
}

uint8_t getDirection(){
  /*
  0 = LEFT
  1 = UP
  2 = RIGHT
  3 = DOWN
  5 = NO_CHANGE
  */
  for(uint8_t i = 0; i < 4; i++){
    if(!digitalRead(LEFT-i)){
      return i;
    }
  }
  return 5;
}



void reset(){
  body[0]=72;
  length = 0;
  head = 0;
  direction = 2;
  gameover = false;
  generateApple();
}


void setup()   {
  srand(analogRead(A0));
  pinMode(LEFT, INPUT_PULLUP);
  pinMode(LEFT-1, INPUT_PULLUP);
  pinMode(LEFT-2, INPUT_PULLUP);
  pinMode(LEFT-3, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("START");
  display.begin(0x3c, true); // Address 0x3C default
  display.setTextSize(1);      // Text scale
  display.setTextColor(SH110X_WHITE);
  display.clearDisplay();
  display.display();
  body[0]=72;
  Serial.println("999");
  generateApple();
}

void drawWall(){
  display.fillRect(65,
  0,
  2,
  64,
  SH110X_WHITE);
}

void printScore(){
  display.setCursor(70, 10);
  display.print("Scr: ");
  display.print(length);
}

void loop() {
  Serial.println("loop");
  display.clearDisplay();

  if(!gameover){
    tempDir = getDirection();

    if(tempDir != 5 && tempDir%2 != direction%2){
      direction = tempDir;
    }
    gameover = updateSnake();

    if(body[head] == apple){
      generateApple();
      if(length < MAX_SNAKE_LENGTH-1) ++length;
    }
    drawWall();
    for(uint8_t i = 0; i < length + 1; i++){
      drawSegment(body[(i+head) % MAX_SNAKE_LENGTH]);
      Serial.print(body[(i+head) % MAX_SNAKE_LENGTH]);
   }

   drawApple(apple);
   printScore();

 }

 else{
   for(uint8_t i = 0; i < length + 1; i++){
     drawSegment(body[(i+head) % MAX_SNAKE_LENGTH]);
   }
   display.display();
   _delay_ms(FRAME_DELAY);
   display.clearDisplay();
   display.display();
   if(getDirection()!=5){
     reset();
   }
 }
  display.display();
  _delay_ms(FRAME_DELAY);
}
