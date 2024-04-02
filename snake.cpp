#include <DoubleLinkedList.h>

struct Pos {
  short x, y;
};

//ROWS = HIGH
#define ROW1 9
#define ROW2 4
#define ROW3 14
#define ROW4 6
#define ROW5 10
#define ROW6 15
#define ROW7 11
#define ROW8 17
 
//COLS = LOW
#define COL1 5
#define COL2 12
#define COL3 13
#define COL4 8
#define COL5 16
#define COL6 7
#define COL7 3
#define COL8 2

int rows[] = {ROW1, ROW2, ROW3, ROW4, ROW5, ROW6, ROW7, ROW8};
int cols[] = {COL1, COL2, COL3, COL4, COL5, COL6, COL7, COL8};

bool row1[8] = {0,0,0,0,0,0,0,0};
bool row2[8] = {0,0,0,0,0,0,0,0};
bool row3[8] = {0,0,0,0,0,0,0,0};
bool row4[8] = {0,0,0,0,1,0,0,0};
bool row5[8] = {0,0,0,0,1,0,0,0};
bool row6[8] = {0,0,0,0,0,0,0,0};
bool row7[8] = {0,0,0,0,0,0,0,0};
bool row8[8] = {0,0,0,0,0,0,0,0};

bool * screen[] = {row1, row2, row3, row4, row5, row6, row7, row8};

void fillScreenArray();


int tick = 0;
int delta = 50;
DoubleLinkedList<Pos> snake;

int snakeLength = 2;
Pos moveVector({0,-1});
Pos lastMove(moveVector);
Pos apple({4,0});
bool appleEaten = false;

bool gameOver = false;

void initializeSnake() {
  snake.append({4,4});
  snake.append({4,3});
  screen[apple.y][apple.x] = true;
}

void delayV2(int delay, int j){
  unsigned long startMicros = micros();
  unsigned long currentMicros = micros();

  int buttonUpRight = analogRead(A4);
  int buttonDownLeft = analogRead(A5);

  if (buttonUpRight > 1000) {
    if (lastMove.y != 1) moveVector = {0,-1};
  } else if (buttonUpRight > 900) {
    if (lastMove.x != -1) moveVector = {1,0};
  }

  if (buttonDownLeft > 1000) {
    if (lastMove.y != -1) moveVector = {0,1};
  } else if (buttonDownLeft > 900) {
    if (lastMove.x != 1) moveVector = {-1,0};
  }


  if (tick == 0) {
    if (gameOver && j == 0) {
      invert();
    } else {
      switch (j) {
        case 0:
          moveSnake();
          lastMove.x = moveVector.x;
          lastMove.y = moveVector.y;
          break;

        case 1:
          checkCollision();
          break;

        case 2:
          if (!gameOver) checkApple();
          break;

        case 3:
          break;

        case 6:
          if (!gameOver) fillScreenArray();
          break;
        case 7:
          if (!gameOver) appleEaten = false;
          break;
      }
    }
  }
  
  while (currentMicros - startMicros < delay) {
    currentMicros = micros();
  }
}




void fillScreenArray() {
  auto headPtr = snake.rbegin();

  int x = (*headPtr).x;
  int y = (*headPtr).y;

  screen[y][x] = true;

  Pos* pos = snake.get(0);

  if (!appleEaten) {
    if (x != pos->x || y != pos->y) screen[pos->y][pos->x] = false;

    snake.remove(0);
  } 
}


void checkCollision() {
  auto headPtr = snake.rbegin();
  auto tail = snake.get(0);

  int headX = (*headPtr).x;
  int headY = (*headPtr).y;

  if (!screen[headY][headX]) return;
  if (headX == apple.x && headY == apple.y) return;
  if (headX == (*tail).x && headY == (*tail).y) return;


  //Game over

  gameOver = true;
  moveVector = {0, 0};
  delta = 40;
}

void moveSnake() {
  auto headPtr = snake.rbegin();

  int x = (*headPtr).x + moveVector.x;
  int y = (*headPtr).y + moveVector.y;

  if (x <= -1) x = 7;
  if (x >= 8) x = 0;
  if (y <= -1) y = 7;
  if (y >= 8) y = 0;

  snake.append({x,y});
}

void checkApple() {
  auto headPtr = snake.rbegin();

  int x = (*headPtr).x;
  int y = (*headPtr).y;

  if (x==apple.x && y==apple.y) {
    snakeLength++;
    appleEaten = true;
    
    int r = random(0,64-snakeLength);
    int ax = 0;
    int ay = 0;
    while (r > 0) {
      if (ax != 7) {
        ax++;
      } else {
        ay++;
        ax = 0;
      }
      
      if (!screen[ay][ax]) r--;
    }

    apple = {ax,ay};
    screen[apple.y][apple.x] = true;


    delta = 50 - snakeLength/2;
  }
}





void setup() {
  for (int i = 2; i <= 17; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }

  Serial.begin(9600);

  int rows[] = {ROW1, ROW2, ROW3, ROW4, ROW5, ROW6, ROW7, ROW8};
  int cols[] = {COL1, COL2, COL3, COL4, COL5, COL6, COL7, COL8};

  initializeSnake();
}


void display() {
  for (int j = 0; j <= 7; j++){
    for (int i = 0; i <= 7; i++) {
      digitalWrite(rows[i], LOW);
    }

    for (int i = 0; i <=7; i++) {
      digitalWrite(cols[i], !screen[j][i]);
    }
    
    digitalWrite(rows[j], HIGH);

    delayV2(1500, j);
  }
  tick++;
  if (tick >= delta) tick = 0;
}

void invert() {
  for (int j = 0; j <= 7; j++) {
    for (int i = 0; i <= 7; i++) {
      screen[j][i] = !screen[j][i];
    }
  }
}

void loop() {
  display();
}