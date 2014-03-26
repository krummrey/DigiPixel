// FlappyPixel
// Use the A button to start the game and to move the pixel up.
// Avoid the green bars.
// If you reach 99 points rewrite drawScore to display three digit values ;)
//
// made for the digipixel http://www.bradsprojects.com/the-digipixel/
// with code from Brad and Crazyau
//
// 2014 Jan Krummrey
//
// todo:
// Display 3-digit scores
// or change color every 100 points for pipes and score

#include <DigiPixel.h>
#include <avr/pgmspace.h>

// leave the following line uncommented for use with a Digispark
DigiPixel digiPixel(3,0,5,2,1);  // LED Latch/Button Shift !load pin, LED/Button clock pin, LED Data Pin, LED Output Enable pin, Buttons data pin)

// leave the following line uncommented for use with an Arduino
// DigiPixel digiPixel(5,2,6,4,3);  // LED Latch/Button Shift !load pin, LED/Button clock pin, LED Data Pin, LED Output Enable pin, Buttons data pin)

// Program memory arrays
byte titleRed[56]  PROGMEM={
  0b00000,  0b00000,  0b00000,  0b00010,  0b00000,  0b01000,  0b00000,  0b00000,  0b11111,  0b10100,  0b00000,  0b11111,  0b00001,  0b00000,  0b01111,  0b10100,  0b01111,  0b00000,  0b11111,  0b10100,  0b01000,  0b00000,  0b11111,  0b10100,  0b01000,  0b00010,  0b11000,  0b00111,  0b11000,  0b00000,  0b01000,  0b00000,  0b11111,  0b10100,  0b01000,  0b00000,  0b11111,  0b00000,  0b11011,  0b00100,  0b11011,  0b00000,  0b11111,  0b10101,  0b00000,  0b11111,  0b00001,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00010,  0b00000,  0b01000,  0b00000};
byte titleGreen[56]PROGMEM={
  0b00001,  0b01011,  0b00001,  0b00001,  0b00001,  0b00011,  0b00001,  0b00001,  0b11111,  0b10101,  0b00001,  0b11111,  0b00001,  0b00001,  0b01111,  0b10101,  0b01111,  0b00001,  0b11111,  0b10101,  0b01001,  0b00001,  0b11111,  0b10101,  0b01001,  0b00001,  0b11001,  0b00111,  0b11001,  0b00001,  0b00011,  0b00001,  0b11111,  0b10101,  0b01001,  0b00001,  0b11111,  0b00001,  0b11011,  0b00101,  0b11011,  0b00001,  0b11111,  0b10101,  0b00001,  0b11111,  0b00001,  0b00001,  0b00001,  0b00001,  0b01011,  0b00001,  0b00001,  0b00001,  0b00011,  0b00001};
byte titleBlue[56] PROGMEM={
  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b11111,  0b10100,  0b01000,  0b00000,  0b11111,  0b00000,  0b11011,  0b00100,  0b11011,  0b00000,  0b11111,  0b10101,  0b00000,  0b11111,  0b00001,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000,  0b00000};

byte numberTable[30]PROGMEM={
  0b01111110, 0b01000010, 0b01111110, 0b00100010, 0b01111110, 0b00000010, 0b01001110, 0b01001010, 0b01111010, 0b01000010, 0b01001010, 0b01111110, 0b01111000, 0b00001000, 0b01111110, 0b01111010, 0b01001010, 0b01001110, 0b01111110, 0b01001010, 0b01001110, 0b01000000, 0b01000000, 0b01111110, 0b01111110, 0b01001010, 0b01111110, 0b01111010, 0b01001010, 0b01111110};

#define ScrollSpeedSave 7
#define DrawOffset 2

// Title Variables
byte ScrollSpeed = ScrollSpeedSave;
byte TextOffset = 0;
boolean drawCloud = true;

// bird values
int birdX;
int birdY;
const byte birdColor = 3;
const int birdFallSpeed = 200; // bigger = slower
long birdFallUpdate; // millis() when bird falling should be updated
const int birdMoveSpeed = 400; // bigger = slower
long birdMoveUpdate; // millis() when bird moving (pipes and background) should be updated
int score;
boolean birdIsHovering = false;
const int birdHoverDelayDefault = 30;
int birdHoverDelay = birdHoverDelayDefault;

// pipe values (2 pipes)
const byte numberOfPipes = 2;
int pipeX[numberOfPipes];
int pipeY[numberOfPipes];
const byte pipeColor = 2;

// cloud values
int clouds[] = {
  0, 1, 1, 0, 1, 0, 1, 1};
const byte cloudColor = 7;

// states (buttons, gameOver)
boolean buttonPressed = false;
byte buttonCounter = 0;
boolean gameOver = false;

// score digits
byte mSD = 0;
byte lSD = 0;

void setup(){
  titleLoop();
  restart();
}

void restart() {
  // start in middle
  birdX = 3;
  birdY = 3;

  score = 0;

  // set first move updates
  birdFallUpdate = millis() + birdFallSpeed;
  birdMoveUpdate = millis() + birdMoveSpeed;

  // set the first pipes
  for(int i = 0; i<numberOfPipes; i++)
  {
    pipeX[i] = 10 + (i*5);
    pipeY[i] = random(6);
  }

  // reset gameover
  gameOver = false;
  buttonCounter = 0;
}

void loop(){
  updateGame();
  updateGraphics();

  digiPixel.saveButtonStates();
  digiPixel.drawScreen();
}

void updateGame(){

  if (!gameOver)
  {
    // move bird
    if (millis() > birdFallUpdate)
    {
      if (birdHoverDelay != 0)
      {
        birdHoverDelay--;
      }
      else{
        birdFallUpdate = millis() + birdFallSpeed;
        birdY = birdY - 1;
        if ((birdY < 0) || (birdY > 200)) // game over if under screen
        {
          birdY = 0;
          gameOver = true;
        }
      }
    }

    // flap at keypress (keep state to avoid key hold)
    if (digiPixel.buttonAPressed == true){
      if (!buttonPressed)
      {
        birdY = birdY + 1;
        buttonPressed = true;
        birdHoverDelay = birdHoverDelayDefault;
      }
    }
    else
    {
      buttonPressed = false;
    }

    // move background and pipes
    if (millis() > birdMoveUpdate)
    {
      birdMoveUpdate = millis() + birdMoveSpeed;

      // move clouds
      if (drawCloud == true)
      {
        for (int i = 0; i < 7; i ++)
        {
          clouds[i] = clouds[i+1];
        }
        clouds[7] = random(2); // 0 sky, 1 cloud
        drawCloud = false;
      }
      else
      {
        drawCloud = true;
      }

      // move pipes
      for(int i = 0; i<numberOfPipes; i++)
      {

        pipeX[i] = pipeX[i] - 1;

        if (pipeX[i] == birdX-1) // bird passes pipe
        {
          score++;
        }

        if (pipeX[i] < 0)
        {
          pipeY[i] = random(6);
          pipeX[i] = 9;
        }
      }
    }
    // check for collision with pipes
    for(int i = 0; i<numberOfPipes; i++)
    {

      if (birdX == pipeX[i])
      {
        if ((birdY < pipeY[i]) || (birdY > pipeY[i] + 2))
        {
          gameOver = true;
        }
      }
    }
  }
  else
  {
    waitForRestart();
  }
}

void updateGraphics(){
  digiPixel.clearScreen();
  // draw clouds
  for (int i = 0; i < 8; i++)
  {
    if (clouds[i] == 1)
    {
      digiPixel.setPixel(i, 7, cloudColor);
    }
  }
  // draw pipes
  for (int i = 0; i < numberOfPipes; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      if (pipeX[i] < 8)
      {
        if ((j < pipeY[i]) or (j > pipeY[i] + 2))
        {
          digiPixel.setPixel(pipeX[i], j, pipeColor);
        }
      }

    }
  }
  // draw bird
  digiPixel.setPixel(birdX, birdY, birdColor);

  // draw game over
  //
  if (gameOver)
  {
    drawScore();

  }
}

void waitForRestart()
{
  if (digiPixel.buttonAPressed == true){
    if (!buttonPressed)
    {
      buttonPressed = true;
    }
  }
  else
  {
    if (buttonPressed)
    {
      // only count button ups
      buttonCounter++;
    }
    buttonPressed = false;

  }
  if (buttonCounter > 1)
  {
    // reset after two button ups (2 to avoid direct restart)
    restart();
  }
}

void updateTitle(){
  if (ScrollSpeed != 0){
    ScrollSpeed--;
  }
  else{
    ScrollSpeed = ScrollSpeedSave;
    TextOffset++;
    if (TextOffset == sizeof(titleRed) - 8){
      TextOffset = 0;
    }

    // move clouds
    if (drawCloud == true)
    {
      for (int i = 0; i < 7; i ++)
      {
        clouds[i] = clouds[i+1];
      }
      clouds[7] = random(2); // 0 sky, 1 cloud
      drawCloud = false;
    }
    else
    {
      drawCloud = true;
    }
  }
}

void saveTitle(){
  digiPixel.clearScreen();
  // draw characters
  for (byte index = 0; index <= 7; index++){
    digiPixel.bufferRed[index] = pgm_read_dword(&titleRed[(index) + TextOffset]);
    digiPixel.bufferGreen[index] = pgm_read_dword(&titleGreen[(index) + TextOffset]);
    digiPixel.bufferBlue[index] = pgm_read_dword(&titleBlue[(index) + TextOffset]);
  }

  // draw clouds
  for (int i = 0; i < 8; i++)
  {
    if (clouds[i] == 1)
    {
      digiPixel.setPixel(i, 7, cloudColor);
    }
  }
}

void titleLoop(){
  while (1){
    updateTitle();
    saveTitle();
    digiPixel.saveButtonStates();
    digiPixel.drawScreen();
    if (digiPixel.buttonAPressed == true){
      return;
    }
  }
}

void drawScore(){
  mSD = score / 10;
  lSD = score % 10;
  digiPixel.clearScreen();
  for (byte index = 0; index <= 2; index++)
  {
    digiPixel.bufferRed[index] = pgm_read_dword(&numberTable[(index) + (mSD * 3)]);
  }
  for (byte index = 4; index <= 6; index++)
  {
    digiPixel.bufferRed[index] = pgm_read_dword(&numberTable[(index - 4) + (lSD * 3)]);
  }
  digiPixel.drawScreen();
  waitForRestart();
}




