/*************************************************************************/
/*                                                                       */
/*                           TETRIS GAME                                 */
/*                                                                       */
/*************************************************************************/

/* Header files */

#include "graphics_compat.h"
#include <string.h>
// #include <graphics.h>
// #include <conio.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <dos.h>

/* Colors */

#define BLUE 1
#define BLUEDR 2
#define BLUEBR 3
#define RED 4
#define REDDR 5
#define REDBR 6
#define GREEN 7
#define GREENDR 8
#define GREENBR 9
#define PURPLE 10
#define PURPLEDR 11
#define PURPLEBR 12
#define YELLOW 13
#define YELLOWDR 14
#define YELLOWBR 15
#define BLANK 0
#define REFRESH 0

/* Matrix */

#define ROWS 20
#define COLS 15

/* Block shapes */

#define SHAPE_I 100
#define SHAPE_T 101
#define SHAPE_L 102
#define SHAPE_L2 103
#define SHAPE_O 104
#define SHAPE_S 105
#define SHAPE_S2 106

/* Directions or Border */

#define RIGHT 201
#define DOWN 203
#define TOP 30
#define LEFT 160

/* Keys */

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_ESC 27
#define KEY_ENTER 13
#define KEY_SPACE 32

/* Functions */

void Brick(int x, int y, char Color); // for drawing 3D bricks
void InitPalette();                   // for getting color pallete
void InitMatrix();                    // For initialize matrix
void GetImages();     // Draws images and saves them in memory for later use
void DisplayScreen(); // for main screen
void AssignShape(int Shape,
                 int Color); // Gives the shape to the current falling block
int DetectCollision(
    int Direction);           // Returns 1 when brick collides with something
int MoveBlock(int Direction); // Moves the falling block in the direction,
                              // returns 0 if block moved else returns 1
void DisplayBlock(int x, int y); // Display the blocks on screen
void DisplayNextShape();         // Display the next brick box
void GetNextBlock();             // Generate next block on screen
void RotateBlock();              // For rotating blocks
void UpdateScreen();             // For refresh the screen
void CheckForLine();             // For checking filled line
void RemoveLine(int Number);     // For removing the filled line
int GetRandomColor();            // For getting random color
int GetRandomShape();            // For getting random shape
void PrintScore();               // For printing score card
void PrintSpeed();               // For printing speed
void PrintLevel();               // For printing level
void PrintLinesCleared();        // For printing line cleared
void DrawBox(int x1, int y1, int x2, int y2, int Color1, int Color2,
             int Color3); // For drawing boxes
void SoundDrop();         // For sound
void StartScreen();       // For the start screen
int IncreaseSpeed();      // Increase the speed
int ShowGameOver();       // Show game over box
char MessageBox(const char *Message, int Width = 240,
                int Size = 3); // For drawing message box

/* Bitmaps */

void far *bmpBlueBrick; // void far is data type where far is a type modifier
void far *bmpRedBrick;
void far *bmpGreenBrick;
void far *bmpYellowBrick;
void far *bmpPurpleBrick;
void far *bmpBlankBrick;
void far *bmpScore;
void far *bmpSpeed;
void far *bmpLevel;
void far *bmpLinesCleared;
void far *bmpScreen;

/* Variables */

int BlockMatrix[4][4];        // Stores the shape of the current falling block
int ScreenLayout[COLS][ROWS]; // Will contain the layout with the falling block
int ScreenBackgroundLayout[COLS][ROWS]; // Will contain the layout without the
                                        // falling block
int PreviousScreenLayout[COLS][ROWS]; // Will contain the layout of the previous
                                      // screen
int LinesCleared = 0; // For lines cleared
int Level = 1;        // For level
int LineScore = 100;  // Increase score 100 for each line cleared
int Speed = 95;       // For speed
int CurrentShape;
int NextShape;
int NextColor;
int BlockX = 7;
int BlockY = 0;
int Quit = 0;
int GameOver = 0;
int SoundOn = 1;
char PScore[40];         // For keeping score as string
char PSpeed[40];         // For keeping speed as string
char PLevel[40];         // for keeping level as string
char PLinesCleared[40];  // for keeping lines cleared as string
unsigned long Score = 0; // For score

/* Shapes of the blocks */

int ShapeI[4][4] = {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0};
int ShapeT[4][4] = {0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0};
int ShapeL[4][4] = {0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int ShapeL2[4][4] = {1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int ShapeS[4][4] = {0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int ShapeS2[4][4] = {1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int ShapeO[4][4] = {1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/* The main Function */

int main() {
  int gd = DETECT, gm;
  int Return = 0;
  char Key, ScanCode;
  int Counter = 0; // Divide total delay & take multiple input
  initgraph(&gd, &gm, "c:\\tc\\bgi"); // initialize graphics mode
  randomize();                        // Randomize block's shapes & color
  cleardevice();                      // clear screen
  InitPalette();                      // for setting color pallete
  InitMatrix();                       // Initialize Matrix
  GetImages();                        // Saving the images
  StartScreen();                      // for start screen
  cleardevice();                      // clear screen
  AssignShape(GetRandomShape(), GetRandomColor()); // for the falling block
  NextShape = GetRandomShape();
  NextColor = GetRandomColor();
  DisplayScreen();    // Show main screen
  DisplayNextShape(); // show next brick
  MoveBlock(LEFT);    // keep the block on center & check game over
  while (kbhit())
    getch();                   // empty the keyboard input
  while (!Quit && !GameOver) { // Moving the blocks down
    if (++Counter >= Speed)    // For controling the speed
    {
      Counter = 0;
      MoveBlock(DOWN);
      SoundDrop();
    }
    if (kbhit()) // For the arrow keys
    {
      Key = getch();
      if (Key == 0) {
        ScanCode = getch();
        if (ScanCode == KEY_UP)
          RotateBlock();
        else if (ScanCode == KEY_LEFT)
          MoveBlock(LEFT);
        else if (ScanCode == KEY_RIGHT)
          MoveBlock(RIGHT);
        else if (ScanCode == KEY_DOWN) {
          Score++; // increase score
          PrintScore();
          MoveBlock(DOWN);
        }
        if (!Return)
          SoundDrop();
        Return = 0;
      } else if (Key == KEY_ENTER || Key == KEY_SPACE) // Rotating bricks
        RotateBlock();
      else if (Key == 'P' || Key == 'p') // For pause
      {
        MessageBox("  Paused");
        while (kbhit())
          getch(); // clear the keyboard input
        for (int x = 0; x < COLS; x++)
          for (int y = 0; y < ROWS; y++)
            PreviousScreenLayout[x][y] -= 1; // Clear the present screen layout
                                             // to refresh the whole screen
        UpdateScreen();          // refresh screen
      } else if (Key == KEY_ESC) // For quit
      {
        char ret = MessageBox("Are you sure, you want to Quit?", 563, 2);
        if (ret == 'y' || ret == 'Y' || ret == KEY_ENTER) {
          Quit = 1;
          break;
        }
        cleardevice(); // Clear the message box
        while (kbhit())
          getch(); // Clear the keyboard input
        for (int x = 0; x < COLS; x++)
          for (int y = 0; y < ROWS; y++)
            PreviousScreenLayout[x][y] -= 1; // Clear the present screen layout
                                             // to refresh the whole screen
        UpdateScreen();                      // refresh screen
        DisplayScreen();                     // show the main screen again
        DisplayNextShape();                  // show next brick box
      } else if (Key == 's' || Key == 'S')   // For sound on/off
      {
        SoundOn = !SoundOn;

      } else if (Key == 'a' || Key == 'A') // For author
      {
        MessageBox("Author: Aguntuk Group", 450);
        cleardevice(); // Clear the message box
        while (kbhit())
          getch(); // Clear the keyboard input
        for (int x = 0; x < COLS; x++)
          for (int y = 0; y < ROWS; y++)
            PreviousScreenLayout[x][y] -= 1; // Clear the present screen layout
                                             // to refresh the whole screen
        UpdateScreen();     // refresh screen
        DisplayScreen();    // show the main screen again
        DisplayNextShape(); // show next brick box
      }
    }
    delay(6); // For moving down the blocks slowly
  }
  if (GameOver) // For game over option
  {
    DisplayBlock(6, 0); // For display the top most brick
    ShowGameOver();     // For display game over message box
  }
  restorecrtmode(); // For closing graphicg mode
  return 0;
}

/* User defined functions */

/* For drawing 3D bricks */

void Brick(int x, int y, char Color) {
  int up[6] = {x, y, x + 20, y, x, y + 20}; // for upper border portion
  int lw[6] = {x + 20, y + 20, x + 20, y, x, y + 20}; // for lower border
                                                      // portion
  switch (Color) {
  case BLUE:
    setcolor(BLUEBR);
    setfillstyle(1, BLUEBR); // for upper border color
    fillpoly(3, up);
    setcolor(BLUEDR);
    setfillstyle(1, BLUEDR); // for lower border color
    fillpoly(3, lw);
    setcolor(BLUE);
    setfillstyle(1, BLUE); // for center box
    bar(x + 5, y + 5, x + 15, y + 15);
    break;
  case RED:
    setcolor(REDBR);
    setfillstyle(1, REDBR); // for upper border color
    fillpoly(3, up);
    setcolor(REDDR);
    setfillstyle(1, REDDR); // for lower border color
    fillpoly(3, lw);
    setcolor(RED);
    setfillstyle(1, RED); // for center box
    bar(x + 5, y + 5, x + 15, y + 15);
    break;
  case GREEN:
    setcolor(GREENBR);
    setfillstyle(1, GREENBR); // for upper border color
    fillpoly(3, up);
    setcolor(GREENDR);
    setfillstyle(1, GREENDR); // for lower border color
    fillpoly(3, lw);
    setcolor(GREEN);
    setfillstyle(1, GREEN); // for center box
    bar(x + 5, y + 5, x + 15, y + 15);
    break;
  case PURPLE:
    setcolor(PURPLEBR);
    setfillstyle(1, PURPLEBR); // for upper border color
    fillpoly(3, up);
    setcolor(PURPLEDR);
    setfillstyle(1, PURPLEDR); // for lower border color
    fillpoly(3, lw);
    setcolor(PURPLE);
    setfillstyle(1, PURPLE); // for center box
    bar(x + 5, y + 5, x + 15, y + 15);
    break;
  case YELLOW:
    setcolor(YELLOWBR);
    setfillstyle(1, YELLOWBR); // for upper border color
    fillpoly(3, up);
    setcolor(YELLOWDR);
    setfillstyle(1, YELLOWDR); // for lower border color
    fillpoly(3, lw);
    setcolor(YELLOW);
    setfillstyle(1, YELLOW); // for center box
    bar(x + 5, y + 5, x + 15, y + 15);
    break;
  }
}

/*For getting color pallete*/

void InitPalette() {     /*Blue Colors*/
  setpalette(BLUE, 1);   // Normal
  setpalette(BLUEDR, 8); // Dark
  setpalette(BLUEBR, 9); // Bright

  /*Red Colors*/
  setpalette(RED, 4);    // Normal
  setpalette(REDDR, 32); // Dark
  setpalette(REDBR, 36); // Bright

  /*Green Colors*/
  setpalette(GREEN, 2);    // Normal
  setpalette(GREENDR, 16); // Dark
  setpalette(GREENBR, 18); // Bright

  /*Purple Colors*/
  setpalette(PURPLE, 13);   // Normal
  setpalette(PURPLEDR, 33); // Dark
  setpalette(PURPLEBR, 61); // Bright

  /*Yellow Colors*/
  setpalette(YELLOW, 6);    // Normal
  setpalette(YELLOWDR, 48); // Dark
  setpalette(YELLOWBR, 62); // Bright
}

/*For saving images*/

void GetImages() {
  Brick(100, 100, BLUE);   // Draw blue brick
  Brick(130, 100, RED);    // Draw red brick
  Brick(160, 100, GREEN);  // draw green brick
  Brick(190, 100, YELLOW); // draw yellow brick
  Brick(220, 100, PURPLE); // draw purple brick

  // Taking the area of the images
  bmpBlueBrick = malloc(imagesize(0, 0, 20, 20));
  bmpRedBrick = malloc(imagesize(0, 0, 20, 20));
  bmpGreenBrick = malloc(imagesize(0, 0, 20, 20));
  bmpYellowBrick = malloc(imagesize(0, 0, 20, 20));
  bmpPurpleBrick = malloc(imagesize(0, 0, 20, 20));
  bmpBlankBrick = malloc(imagesize(0, 0, 20, 20));
  bmpScore = malloc(imagesize(501, 51, 607, 69));
  bmpSpeed = malloc(imagesize(501, 51, 607, 69));
  bmpLevel = malloc(imagesize(501, 51, 607, 69));
  bmpLinesCleared = malloc(imagesize(501, 51, 607, 69));
  bmpScreen = malloc(imagesize(0, 0, 640, 480));

  // Saving the images
  getimage(100, 100, 120, 120, bmpBlueBrick);
  getimage(130, 100, 150, 120, bmpRedBrick);
  getimage(160, 100, 180, 120, bmpGreenBrick);
  getimage(190, 100, 210, 120, bmpYellowBrick);
  getimage(220, 100, 240, 120, bmpPurpleBrick);
  getimage(0, 0, 20, 20, bmpBlankBrick);
  getimage(501, 51, 607, 69, bmpScore);
  getimage(501, 51, 607, 69, bmpSpeed);
  getimage(501, 51, 607, 69, bmpLevel);
  getimage(501, 51, 607, 69, bmpLinesCleared);
}

/* Initialize Matrix */

void InitMatrix() {
  for (int x = 0; x < COLS; x++)
    for (int y = 0; y < ROWS; y++) {
      ScreenLayout[x][y] = BLANK; // initialize all the value to default 0
      PreviousScreenLayout[x][y] = BLANK;
      ScreenBackgroundLayout[x][y] = BLANK;
    }
}

/*For main screen*/

void DisplayScreen() {
  setcolor(15);
  rectangle(156, 26, 478, 452); // for border of play box
  rectangle(158, 28, 476, 450); // for play box
  DrawBox(488, 300, 620, 450, YELLOWBR, YELLOW, YELLOWDR); // for next brick bar
  DrawBox(488, 28, 620, 290, YELLOWBR, YELLOW, YELLOWDR); // for information bar
  DrawBox(10, 255, 150, 450, YELLOWBR, YELLOW, YELLOWDR); // for instruction bar
  setcolor(PURPLE);
  settextstyle(0, 0, 1);
  outtextxy(500, 310, "Next");
  outtextxy(538, 310, "Brick");
  setcolor(PURPLE);
  outtextxy(500, 40, "Score");
  setcolor(GREEN);
  rectangle(500, 50, 608, 70); // for score box
  setcolor(PURPLE);
  settextstyle(0, 0, 1);
  outtextxy(500, 100, "Level");
  setcolor(GREEN);
  rectangle(500, 110, 608, 130); // for level box
  setcolor(PURPLE);
  outtextxy(500, 160, "Speed");
  setcolor(GREEN);
  rectangle(500, 170, 608, 190); // for speed box
  setcolor(PURPLE);
  outtextxy(500, 220, "Lines");
  outtextxy(547, 220, "Cleared");
  setcolor(GREEN);
  rectangle(500, 230, 608, 250); // for line cleared box
  PrintScore();                  // Display scorecard
  PrintSpeed();                  // Display speed
  PrintLevel();                  // Display level
  PrintLinesCleared();           // Display lines cleared
  setcolor(REDBR);
  settextstyle(10, 1, 4);
  outtextxy(18, 30, "Aguntuk"); // for name
  outtextxy(76, 60, "Bricks");
  setcolor(GREEN);
  settextstyle(8, 0, 2);
  outtextxy(20, 263, "KEYS:");
  settextstyle(0, 0, 1);
  setcolor(PURPLE);
  outtextxy(20, 290, "Left");
  setlinestyle(0, 0, 2);
  line(100, 293, 103, 290); //for left arrow's \
	line(100,293,103,296);                          //for left arrow's /
  line(100, 293, 109, 293); // for left arrow's hyphen
  outtextxy(20, 310, "Right");
  line(109, 313, 106, 310); //for right arrow's \
	line(100,313,109,313);                          //for right arrow's hyphen
  line(109, 313, 106, 316); // for right arrow's /
  outtextxy(20, 330, "Rotate");
  outtextxy(100, 330, "^");
  line(103, 330, 103, 340); // for up arrow's hyphen
  outtextxy(20, 350, "Drop");
  line(103, 350, 103, 360); // for down arrow's hyphen
  line(100, 357, 103, 360); //for down arrow's \
	line(106,357, 103, 360);			//for down arrow's /
  outtextxy(20, 370, "Pause");
  outtextxy(100, 370, "P");
  outtextxy(20, 390, "Sound");
  outtextxy(100, 390, "S");
  outtextxy(20, 410, "About");
  outtextxy(100, 410, "A");
  outtextxy(20, 430, "Exit");
  outtextxy(100, 430, "Esc");
  setcolor(GREEN);
}

/*Gives the shape to the current falling block*/

void AssignShape(int Shape, int Color) {
  int i, j;
  CurrentShape = Shape;
  switch (Shape) {
  case SHAPE_I:
    for (i = 0; i < 4; i++)
      for (j = 0; j < 4; j++)
        BlockMatrix[j][i] =
            ShapeI[i][j] *
            Color; // Change the value of matrix with assigned color
    break;
  case SHAPE_T:
    for (i = 0; i < 4; i++)
      for (j = 0; j < 4; j++)
        BlockMatrix[j][i] = ShapeT[i][j] * Color;
    break;
  case SHAPE_L:
    for (i = 0; i < 4; i++)
      for (j = 0; j < 4; j++)
        BlockMatrix[j][i] = ShapeL[i][j] * Color;
    break;
  case SHAPE_L2:
    for (i = 0; i < 4; i++)
      for (j = 0; j < 4; j++)
        BlockMatrix[j][i] = ShapeL2[i][j] * Color;
    break;
  case SHAPE_O:
    for (i = 0; i < 4; i++)
      for (j = 0; j < 4; j++)
        BlockMatrix[j][i] = ShapeO[i][j] * Color;
    break;
  case SHAPE_S:
    for (i = 0; i < 4; i++)
      for (j = 0; j < 4; j++)
        BlockMatrix[j][i] = ShapeS[i][j] * Color;
    break;
  case SHAPE_S2:
    for (i = 0; i < 4; i++)
      for (j = 0; j < 4; j++)
        BlockMatrix[j][i] = ShapeS2[i][j] * Color;
    break;
  default:
    break;
  }
}

/*For randomize block color*/

int GetRandomColor() {
  int Color = rand() % 5;
  switch (Color) {
  case 0:
    return BLUE;
  case 1:
    return RED;
  case 2:
    return GREEN;
  case 3:
    return YELLOW;
  case 4:
    return PURPLE;
  }
  return 0;
}

/*For randomize block shape*/

int GetRandomShape() {
  int Shape = rand() % 7;
  switch (Shape) {
  case 0:
    return SHAPE_I;
  case 1:
    return SHAPE_L;
  case 2:
    return SHAPE_T;
  case 3:
    return SHAPE_O;
  case 4:
    return SHAPE_S;
  case 5:
    return SHAPE_S2;
  case 6:
    return SHAPE_L2;
  }
  return 0;
}

/*For moving block*/

int MoveBlock(int Direction) {
  switch (Direction) {
  case LEFT:
    if (DetectCollision(LEFT))
      return 1;
    DisplayBlock(--BlockX, BlockY);
    break;
  case RIGHT:
    if (DetectCollision(RIGHT))
      return 1;
    DisplayBlock(++BlockX, BlockY);
    break;
  case DOWN:
    if (DetectCollision(DOWN)) {
      GetNextBlock(); // Generate new block on screen
      return 1;
    }
    DisplayBlock(BlockX, ++BlockY);
    break;
  case REFRESH:
    DisplayBlock(BlockX, BlockY);
    break;
  }
  return 0;
}

/*For rotate blocks*/

void RotateBlock() {
  int TempBlockMatrix[4][4];
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      TempBlockMatrix[i][j] = BlockMatrix[i][j];
  switch (CurrentShape) {
  case SHAPE_O: // in case O no change
    return;
  case SHAPE_L: // Changes 8 pos around (1,1) point
  case SHAPE_L2:
  case SHAPE_S:
  case SHAPE_S2:
  case SHAPE_T:
    BlockMatrix[0][0] = TempBlockMatrix[2][0];
    BlockMatrix[0][2] = TempBlockMatrix[0][0];
    BlockMatrix[2][2] = TempBlockMatrix[0][2];
    BlockMatrix[2][0] = TempBlockMatrix[2][2];

    BlockMatrix[0][1] = TempBlockMatrix[1][0];
    BlockMatrix[1][2] = TempBlockMatrix[0][1];
    BlockMatrix[2][1] = TempBlockMatrix[1][2];
    BlockMatrix[1][0] = TempBlockMatrix[2][1];
    break;
  case SHAPE_I:
    BlockMatrix[0][1] = TempBlockMatrix[1][0]; // Changes only 3 position
    BlockMatrix[1][0] = TempBlockMatrix[0][1];

    BlockMatrix[1][2] = TempBlockMatrix[2][1];
    BlockMatrix[2][1] = TempBlockMatrix[1][2];

    BlockMatrix[1][3] = TempBlockMatrix[3][1];
    BlockMatrix[3][1] = TempBlockMatrix[1][3];
    break;
  }
  if (DetectCollision(REFRESH)) {
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        BlockMatrix[i][j] = TempBlockMatrix[i][j];
    return;
  }
  MoveBlock(REFRESH);
}

/*Displays the blocks on screen*/

void DisplayBlock(int x, int y) {
  int i, j;
  for (i = 0; i < ROWS; i++)
    for (j = 0; j < COLS; j++)
      ScreenLayout[j][i] =
          ScreenBackgroundLayout[j][i]; // assign background to ScreenLayout

  for (i = 0; i < 4; i++) {
    if ((x + i) < 0 || (x + i) >= COLS)
      continue;
    for (j = 0; j < 4; j++) {
      if ((y + j) < 0 || (y + j) >= ROWS)
        continue;
      if (BlockMatrix[i][j] == 0)
        continue;
      ScreenLayout[x + i][y + j] =
          BlockMatrix[i][j]; // keep the value of blocks in ScreenLayout
    }
  }
  UpdateScreen(); // Print the block where ScreenLayout is found
}

/*Checking the free space for moving block*/

int DetectCollision(int Direction) {
  int x, y;
  int Bx = BlockX;
  int By = BlockY;
  switch (Direction) {
  case LEFT:
    --Bx;
    break;
  case RIGHT:
    ++Bx;
    break;
  case DOWN:
    By++;
    break;
  default:
    break;
  }

  /* Walls, floor and resting bricks */
  for (x = 0; x < 4; x++)
    for (y = 3; y >= 0; y--) // start checking from bottom side
    {
      if (BlockMatrix[x][y] == BLANK)
        continue;
      int Cx = Bx + x;
      int Cy = By + y;
      if (Cx < 0 || Cx >= COLS || Cy >= ROWS) // outside the play field
        return 1;
      if (Cy < 0) // still above the field, nothing to hit
        continue;
      if (ScreenBackgroundLayout[Cx][Cy] != BLANK)
        return 1;
    }

  return 0;
}

/*Generate the next block on screen*/

void GetNextBlock() {
  for (int x = 0; x < 4; x++)
    for (int y = 0; y < 4; y++) {
      if (BlockMatrix[x][y] == BLANK)
        continue;
      int Cx = BlockX + x;
      int Cy = BlockY + y;
      if (Cx < 0 || Cx >= COLS || Cy < 0 || Cy >= ROWS)
        continue;
      ScreenBackgroundLayout[Cx][Cy] = BlockMatrix[x][y]; // stop the block
    }
  CheckForLine();                    // checking if lines are filled
  AssignShape(NextShape, NextColor); // assign new block
  NextShape = GetRandomShape();      // assign next shape
  NextColor = GetRandomColor();      // assign next color
  DisplayNextShape();                // display next block
  BlockX = 7;                        // restore value of BlockX & BlockY
  BlockY = 0;
  if (MoveBlock(LEFT)) // if block collides with something return 1
  {
    GameOver = 1;
  }
}

/*Displays the next brick box*/

void DisplayNextShape() {
  int NextTop = 340;
  int NextLeft = 520;
  int TempMatrix[4][4]; // Store blocks with color value
  int i, j;

  switch (NextShape) {
  case SHAPE_I:
    for (i = 0; i < 4; i++)
      for (j = 0; j < 4; j++)
        TempMatrix[j][i] =
            ShapeI[i][j] *
            NextColor; // Change value of temp matrix with assigned color
    break;
  case SHAPE_T:
    for (i = 0; i < 4; i++)
      for (j = 0; j < 4; j++)
        TempMatrix[j][i] = ShapeT[i][j] * NextColor;
    break;
  case SHAPE_L:
    for (i = 0; i < 4; i++)
      for (j = 0; j < 4; j++)
        TempMatrix[j][i] = ShapeL[i][j] * NextColor;
    break;
  case SHAPE_L2:
    for (i = 0; i < 4; i++)
      for (j = 0; j < 4; j++)
        TempMatrix[j][i] = ShapeL2[i][j] * NextColor;
    break;
  case SHAPE_O:
    for (i = 0; i < 4; i++)
      for (j = 0; j < 4; j++)
        TempMatrix[j][i] = ShapeO[i][j] * NextColor;
    break;
  case SHAPE_S:
    for (i = 0; i < 4; i++)
      for (j = 0; j < 4; j++)
        TempMatrix[j][i] = ShapeS[i][j] * NextColor;
    break;
  case SHAPE_S2:
    for (i = 0; i < 4; i++)
      for (j = 0; j < 4; j++)
        TempMatrix[j][i] = ShapeS2[i][j] * NextColor;
    break;
  default:
    break;
  }

  for (int x = 0; x < 4; x++)
    for (int y = 0; y < 4; y++) {
      switch (TempMatrix[x][y]) // print blocks to desired position
      {
      case BLUE:
        putimage(NextLeft + x * 21, NextTop + y * 21, bmpBlueBrick, 0);
        break;
      case RED:
        putimage(NextLeft + x * 21, NextTop + y * 21, bmpRedBrick, 0);
        break;
      case GREEN:
        putimage(NextLeft + x * 21, NextTop + y * 21, bmpGreenBrick, 0);
        break;
      case YELLOW:
        putimage(NextLeft + x * 21, NextTop + y * 21, bmpYellowBrick, 0);
        break;
      case PURPLE:
        putimage(NextLeft + x * 21, NextTop + y * 21, bmpPurpleBrick, 0);
        break;
      default:
        putimage(NextLeft + x * 21, NextTop + y * 21, bmpBlankBrick, 0);
      }
    }
}

/*Refresh the display screen*/

void UpdateScreen() {
  for (int x = 0; x < COLS; x++)
    for (int y = 0; y < ROWS; y++) {
      if (PreviousScreenLayout[x][y] == ScreenLayout[x][y])
        continue; // No need to draw the same image again
      PreviousScreenLayout[x][y] = ScreenLayout[x][y];
      switch (ScreenLayout[x][y]) // Print blocks to desired position
      {
      case BLUE:
        putimage(LEFT + x * 21, TOP + y * 21, bmpBlueBrick, 0);
        break;
      case RED:
        putimage(LEFT + x * 21, TOP + y * 21, bmpRedBrick, 0);
        break;
      case GREEN:
        putimage(LEFT + x * 21, TOP + y * 21, bmpGreenBrick, 0);
        break;
      case YELLOW:
        putimage(LEFT + x * 21, TOP + y * 21, bmpYellowBrick, 0);
        break;
      case PURPLE:
        putimage(LEFT + x * 21, TOP + y * 21, bmpPurpleBrick, 0);
        break;
      default:
        putimage(LEFT + x * 21, TOP + y * 21, bmpBlankBrick, 0);
      }
    }
}

/*For sound*/

void SoundDrop() {
  if (SoundOn) // SoundOn is a variable for on/off sound
  {
    for (int i = 5; i < 10; i++) {
      sound(200 * i);
      delay(1);
      nosound();
    }
  }
}

/*For checking filled lines*/

void CheckForLine() {
  int RemoveLines[10];
  int Remove;
  int x, y;
  int factor = 1;
  for (y = 0; y < ROWS; y++) {
    Remove = 1;
    for (x = 0; x < COLS; x++) {
      if (ScreenLayout[x][y] == BLANK)
        Remove = 0;
    }
    if (Remove) {
      RemoveLine(y);
      Score += LineScore * factor;
      factor++;
      PrintScore();
    }
  }
  while (kbhit())
    getch(); // Empty the keyboard input
}

/*For removing the filled line*/

void RemoveLine(int LineNumber) {
  int i, j;
  int count = 0;
  int Temp[COLS];
  for (i = 0; i < COLS; i++) {
    Temp[i] = ScreenLayout[i][LineNumber]; // assign value to temp
  }
  for (j = 0; j < 10; j++) {
    for (i = 0; i < COLS; i++) // for dazzling filled line
    {
      ScreenLayout[i][LineNumber] =
          (count ? Temp[i] : BLANK); // if count value 0 set temp value else set
                                     // 0 to ScreenLayout
    }
    count = !count; // reverse count value
    UpdateScreen();
    delay(50);
  }
  for (i = LineNumber; i > 0; i--)
    for (j = 0; j < COLS; j++) {
      ScreenBackgroundLayout[j][i] =
          ScreenBackgroundLayout[j][i - 1]; // move down all blocks to fill the
                                            // blank space
    }
  for (j = 0; j < COLS; j++)
    ScreenBackgroundLayout[j][0] = BLANK; // assign value 0 to top most line
  for (i = 0; i < COLS; i++)
    for (j = 0; j < ROWS; j++)
      ScreenLayout[i][j] =
          ScreenBackgroundLayout[i]
                                [j]; // assign value to ScreenLayout for display
  ++LinesCleared;
  PrintLinesCleared();
  if (!(LinesCleared % 20)) // if 20 line cleared increase speed
    IncreaseSpeed();
}

int IncreaseSpeed() {
  if (Speed - 5 <= 0)
    return 1;
  Speed -= 5; // increase the speed, lower the number, higher the speed
  Level++;    // speed & level increase together
  PrintSpeed();
  PrintLevel();
  return 0;
}

/*For drawing box*/

void DrawBox(int x1, int y1, int x2, int y2, int Color1, int Color2,
             int Color3) {
  setlinestyle(0, 0, 1);
  setcolor(Color1);
  rectangle(x1, y1, x2, y2);
  setcolor(Color2);
  rectangle(x1 + 2, y1 + 2, x2 - 2, y2 - 2);
  setcolor(Color3);
  rectangle(x1 + 4, y1 + 4, x2 - 4, y2 - 4);
}

/*For printing score*/

void PrintScore() {
  settextstyle(7, 0, 1);
  sprintf(PScore, "%11lu", Score);
  putimage(501, 51, bmpScore, 0);
  outtextxy(520, 56, PScore);
}

/*For printing level*/

void PrintLevel() {
  settextstyle(7, 0, 1);
  sprintf(PLevel, "%11d", Level);
  putimage(501, 111, bmpLevel, 0);
  outtextxy(520, 116, PLevel);
}

/*For printing speed*/

void PrintSpeed() {
  settextstyle(7, 0, 1);
  sprintf(PSpeed, "%11d", 100 - Speed);
  putimage(501, 171, bmpSpeed, 0);
  outtextxy(520, 176, PSpeed);
}

/*For printing line cleared*/

void PrintLinesCleared() {
  settextstyle(7, 0, 1);
  sprintf(PLinesCleared, "%11d", LinesCleared);
  putimage(501, 231, bmpLinesCleared, 0);
  outtextxy(520, 236, PLinesCleared);
}

/*For showing game over box*/

int ShowGameOver() {
  MessageBox("Game Over");
  return 0;
}

/*For printing a message within box*/

char MessageBox(const char *Message, int Width,
                int Size) // Width defaults to 240 & size defaults to 3
{
  int Color = 0;
  int TextX = 320 - (int)(strlen(Message) * 6 * Size) / 2; // centre the text
  setfillstyle(10, YELLOWDR);
  bar(320 - (Width / 2) + 6, 206, 320 + (Width / 2) - 6, 274);
  DrawBox(320 - (Width / 2), 200, 320 + (Width / 2), 280, YELLOWBR, YELLOW,
          YELLOWDR);
  setfillstyle(10, GREENDR);
  setcolor(GREENBR);
  settextstyle(10, 0, Size);
  outtextxy(TextX, 210, Message);
  while (!kbhit()) {
    switch (Color++) {
    case 0:
      setcolor(GREENBR);
      break;
    case 1:
      setcolor(GREEN);
      break;
    case 2:
      setcolor(GREENDR);
      break;
    case 3:
      setcolor(GREEN);
      break;
    }
    outtextxy(TextX, 210, Message);
    delay(80);
    if (Color == 4)
      Color = 0;
  }
  setcolor(GREEN);
  return getch(); // return the last key pressed form the keyboard input
}

/*For starting screen*/

void StartScreen() {
  cleardevice();
  int Color = 0;
  int i, j;

  int Bricks[20][30] = {
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0,
      1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0,
      1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1,
      0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1,
      0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0,
      0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0,
      0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0,
      0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0,
      0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1,
      0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1,
      0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0,
      1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0,
      1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1,
      0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  setfillstyle(10, BLUEDR);
  bar(0, 0, 640, 480);
  for (j = 0; j < 20; j++)
    for (i = 0; i < 6; i++) {
      if (!Bricks[j][i])
        continue;
      putimage(i * 21, j * 21, bmpBlueBrick, 0); // For print B
    }
  for (j = 0; j < 20; j++)
    for (i = 6; i < 12; i++) {
      if (!Bricks[j][i])
        continue;
      putimage(i * 21, j * 21, bmpGreenBrick, 0); // For print R
    }
  for (j = 0; j < 20; j++)
    for (i = 12; i < 14; i++) {
      if (!Bricks[j][i])
        continue;
      putimage(i * 21, j * 21, bmpRedBrick, 0); // For print I
    }
  for (j = 0; j < 20; j++)
    for (i = 14; i < 20; i++) {
      if (!Bricks[j][i])
        continue;
      putimage(i * 21, j * 21, bmpYellowBrick, 0); // for print C
    }
  for (j = 0; j < 20; j++)
    for (i = 20; i < 25; i++) {
      if (!Bricks[j][i])
        continue;
      putimage(i * 21, j * 21, bmpPurpleBrick, 0); // for print K
    }
  for (j = 0; j < 20; j++)
    for (i = 25; i < 30; i++) {
      if (!Bricks[j][i])
        continue;
      putimage(i * 21, j * 21, bmpRedBrick, 0); // For print S
    }
  setcolor(PURPLEBR);
  settextstyle(4, 0, 3);
  outtextxy(104, 325, "Created by Aguntuk Group");
  setcolor(GREENBR);
  settextstyle(10, 0, 2);
  Color = 0;
  while (!kbhit()) // For dazzling the text
  {
    switch (Color++) {
    case 0:
      setcolor(GREENBR);
      break;
    case 1:
      setcolor(GREEN);
      break;
    case 2:
      setcolor(GREENDR);
      break;
    case 3:
      setcolor(GREEN);
      break;
    }
    outtextxy(152, 385, "Mawlana Bhashani Science and");
    outtextxy(194, 425, "Technology University");
    delay(80);
    if (Color == 4)
      Color = 0;
  }
}
