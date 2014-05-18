#include<Wire.h>
#include<I2Cdev.h>
#include<MPU6050.h>
#include<SeeedTouchScreen.h>
#include<TFT.h>

#define uchar unsigned char
#define uint unsigned int

//#define Matrix_pinCLK    8
//#define Matrix_pinCS     9
//#define Matrix_pinDIN    10

#define Max7219_pinCLK    28
#define Max7219_pinCS     26
#define Max7219_pinDIN    24

//game state
#define Main              2
#define Welcome           1
#define GameOver          3

#define YP A2
#define XM A1
#define YM 54
#define XP 57

#define TS_MINX 116*2
#define TS_MAXX 890*2
#define TS_MINY 83*2
#define TS_MAXY 913*2

TouchScreen ts = TouchScreen(XP,YP,XM,YM);

MPU6050 accelgyro;

int touchX,touchY;
int16_t ax,ay,az;
int16_t gx,gy,gz;
float xAcc,yAcc;
int type = 0;
int Rbias = -2;
int Cbias = 2;
unsigned long t1 = 0;
unsigned long t2 = 0;
unsigned long t3 = 0;
unsigned long tLeft = 0;
unsigned long tRight = 0;
unsigned long tUp = 0;
unsigned long tDown = 0;
unsigned long tClear = 0;
unsigned long tAnimation = 0;
int count1 = 0;
int count2 = 0;
int count3 = 0;
int count4 = 0;
int countLeft = 0;
int countRight = 0;
int countUp = 0;
int countDown = 0;
boolean gameFailFlag = false;
int gameState = 1;
int fallSpeed = 0;
long oldScore[2] = {0,0};
long newScore[2] = {0,0};
int clearedLine = 0;
int work = 0;
int counter = 0;
float coefficient;
int animationFrame = 0;

uchar disp1[8][8]={

{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
};

uchar final[8][8]={
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
};

//little animation and "GAMEOVER"
uchar animation[17][8][8] = {
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,1,1,0,0,0},
    {0,0,0,1,1,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,1,1,1,1,0,0},
    {0,0,1,1,1,1,0,0},
    {0,0,1,1,1,1,0,0},
    {0,0,1,1,1,1,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0},
    {0,1,1,1,1,1,1,0},
    {0,1,1,1,1,1,1,0},
    {0,1,1,1,1,1,1,0},
    {0,1,1,1,1,1,1,0},
    {0,1,1,1,1,1,1,0},
    {0,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1},
  },
  {
    {0,0,0,0,0,0,0,0},
    {0,1,1,1,1,1,1,0},
    {0,1,1,1,1,1,1,0},
    {0,1,1,1,1,1,1,0},
    {0,1,1,1,1,1,1,0},
    {0,1,1,1,1,1,1,0},
    {0,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,1,1,1,1,0,0},
    {0,0,1,1,1,1,0,0},
    {0,0,1,1,1,1,0,0},
    {0,0,1,1,1,1,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,1,1,0,0,0},
    {0,0,0,1,1,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,1,1,1,0,0,0},
    {0,1,0,0,0,1,0,0},
    {0,1,0,0,0,0,0,0},
    {0,1,0,0,1,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,0,1,1,1,1,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,1,1,1,0,0,0},
    {0,1,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,1,1,1,1,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,1,0,0,0,1,0,0},
    {0,1,1,0,1,1,0,0},
    {0,1,0,1,0,1,0,0},
    {0,1,0,1,0,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,1,1,1,1,1,0,0},
    {0,1,0,0,0,0,0,0},
    {0,1,0,0,0,0,0,0},
    {0,1,1,1,1,0,0,0},
    {0,1,0,0,0,0,0,0},
    {0,1,0,0,0,0,0,0},
    {0,1,1,1,1,1,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,0,1,1,1,0,0,0},
    {0,1,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,0,1,1,1,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,1,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,0,1,0,1,0,0,0},
    {0,0,0,1,0,0,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,1,1,1,1,1,0,0},
    {0,1,0,0,0,0,0,0},
    {0,1,0,0,0,0,0,0},
    {0,1,1,1,1,0,0,0},
    {0,1,0,0,0,0,0,0},
    {0,1,0,0,0,0,0,0},
    {0,1,1,1,1,1,0,0},
    {0,0,0,0,0,0,0,0},
  },
  {
    {0,1,1,1,1,0,0,0},
    {0,1,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,1,1,1,1,0,0,0},
    {0,1,0,1,0,0,0,0},
    {0,1,0,0,1,0,0,0},
    {0,1,0,0,0,1,0,0},
    {0,0,0,0,0,0,0,0},
  },
};

uchar pattern[19][9] = 
{
    { 1,1,0,2,1,2,2,2,1  },   //     *
    { 1,1,1,2,1,3,2,2,2  },   //    ***
    { 0,2,1,2,2,2,1,3,3  },   //
    { 1,1,0,2,1,2,1,3,0  },   
    
    { 0,0,1,0,0,1,0,2,5  },   //       *
    { 0,0,1,0,2,0,2,1,6  },   //     ***
    { 2,0,2,1,2,2,1,2,7  },   //
    { 0,0,0,1,1,1,2,1,4  },   //
    
    { 1,0,2,0,2,1,2,2,9  },   //    *
    { 2,0,0,1,1,1,2,1,10 },   //    ***
    { 0,0,0,1,0,2,1,2,11 },   
    { 0,0,1,0,2,0,0,1,8  },   
    
    { 0,1,0,2,1,2,1,3,13 },   //    **
    { 1,1,2,1,0,2,1,2,12 },   //     **
    
    { 1,0,0,1,1,1,0,2,15 },   //      **
    { 0,1,1,1,1,2,2,2,14 },   //     **
    
    { 1,0,1,1,1,2,1,3,17 },   //    ****
    { 0,1,1,1,2,1,3,1,16 },   
    
    { 0,1,1,1,0,2,1,2,18 }    //    **
                              //    **
};

uchar displayInBytes[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};



int bottomCheck(int Rbias, int Cbias, uchar type, uchar exist[8][8])
{
  int i;
  int CheckResult = 0;
  for(i = 0; i < 4; i++)
  {
     if ((Rbias + pattern[type][2*i] >= 0) && (Rbias + pattern[type][2*i] <= 6))
     {
       if(exist[Rbias + pattern[type][2*i] + 1][Cbias + pattern[type][2*i+1]] == 1)
        {
          CheckResult = 1;
        }
     } 
  }
    return CheckResult;
}

int rightCheck(int Rbias, int Cbias, uchar type, uchar exist[8][8])
{
  int i;
  int CheckResult = 0;
  for(i = 0; i < 4; i++)
  {
     if (Rbias + pattern[type][2*i] >= 0)
     {
       if(exist[Rbias + pattern[type][2*i]][Cbias + pattern[type][2*i+1] + 1] == 1)
         CheckResult = 1;
     } 
  }
    return CheckResult;
}

int leftCheck(int Rbias, int Cbias, uchar type, uchar exist[8][8])
{
  int i;
  int CheckResult = 0;
  for(i = 0; i < 4; i++)
  {
     if (Rbias + pattern[type][2*i] >= 0)
     {
       if(exist[Rbias + pattern[type][2*i]][Cbias + pattern[type][2*i+1] - 1] == 1)
         CheckResult = 1;
     } 
  }
    return CheckResult;
}

int changeCheck(int Rbias, int Cbias, uchar type, uchar exist[8][8])
{
  int i;
  int CheckResult = 0;
  for(i = 0; i < 4; i++)
  {
     if (Rbias + pattern[type][2*i] >= 0)
     {
       if(exist[Rbias + pattern[pattern[type][8]][2*i]][Cbias + pattern[pattern[type][8]][2*i+1]] == 1)
         {
           CheckResult = 1;
         }
       if(Cbias + (int)pattern[pattern[type][8]][2*i+1] >= 8 || Cbias + (int)pattern[pattern[type][8]][2*i+1] < 0)
       {
           CheckResult = 1; 
       }
     } 
  }
  return CheckResult;
}

int reachBottom(int Rbias, int Cbias, uchar type, uchar exist[8][8])
{
  int i;
  int CheckResult = 0;
  for(i = 0; i < 4; i++)
  {
    if (Rbias + pattern[type][2*i] == 7)
    CheckResult = 1;
  }
  return CheckResult;
}

int reachRight(int Rbias, int Cbias, uchar type, uchar exist[8][8])
{
  int i;
  int CheckResult = 0;
  for (i = 0; i < 4; i++)
    {
      if(Cbias + pattern[type][2*i + 1] == 7)
      CheckResult = 1;
    }
  return CheckResult;
}

int reachLeft(int Rbias, int Cbias, uchar type, uchar exist[8][8])
{
  int i;
  int CheckResult = 0;
  for (i = 0; i < 4; i++)
    {
      if(Cbias + pattern[type][2*i + 1] == 0)
      CheckResult = 1;
    }
  return CheckResult;
}

boolean gameFailCheck(uchar exist[8][8]){
  if((exist[0][3] || exist[0][4]) == 1)
    return true;
  else 
    return false;
}

//clear one line
long clearLine(uchar ledMatrix[8][8])
{
  work = 0;
  counter = 0;
  int i,j,k,n;
  for(i = 8; i > 0; i--)
  {
    for(j = 8; j > 0; j--)
    {
      if(ledMatrix[i-1][j-1] == 1)
      {
        counter++;
      }
    }
    if(counter == 8)
    {
      for(n = 0; n < 8; n++){
        ledMatrix[i-1][n] = 0;
      }
      for(j = i; j > 0; j--)
      {
        for(k = 0; k < 8; k++)
        {
          if(j == 1)
          {
            ledMatrix[j-1][k] = 0;
          }
          else
          {
            ledMatrix[j-1][k] = ledMatrix[j-2][k];
          }
        }
      }
      /*for(int o = 0; o < 8; o++){
        for(int l = 0; l < 8; l++){
          Serial.print(ledMatrix[o][l]);
        }
        Serial.println();
      }*/
      //Serial.print("line:");Serial.println(i);
      i++;
      work++;
    }
    counter = 0;
    //Serial.print("counter");Serial.println(counter);
  }
  //Serial.println(work);
  //newScore[0] = newScore[0] + 100*work*work;
  return work;
}

//add patterns to existed matrix
void patternAdd(int Rbias, int Cbias, uchar type, uchar exist[8][8], uchar fianl[8][8])
{
   uint i;
   uint j,k;
   for(j=0;j<8;j++)
     for(k=0;k<8;k++)
     {
       final[j][k] = exist[j][k];
     }
   for (i = 0; i < 4; i++)
   {
     if(Rbias + pattern[type][2*i] >= 0)
      {
        final[Rbias + pattern[type][2*i]][Cbias + pattern[type][2*i + 1]] = 1; 
      }
   }
}


//clear the screen
void totalClear(uchar exist[8][8])
{
  uint i,j;
  for(i=0;i<8;i++)
   for(j = 0;j<8;j++)
  exist[i][j] = 0; 
}


//convert the matrix to the form that can be used by MAX7219
void convert(uchar source[8][8], uchar target[8])
{
    uint i,j;
    for (i = 0; i < 8; i++)
    {
        for(j = 0; j < 8; j++)
       {
           target[i] = (target[i] << 1) | (source[i][j]);
       } 
    }
}


//send information to MAX7219
void Write_Max7219_byte(uchar DATA)         
{
	uchar i;    
	digitalWrite(Max7219_pinCS,LOW);
	for(i=8;i>=1;i--)
	{		  
		digitalWrite(Max7219_pinCLK,LOW);
		if(DATA&0x80)
		{
			digitalWrite(Max7219_pinDIN,HIGH);
		}
		else
		{
			digitalWrite(Max7219_pinDIN,LOW);
		}
		DATA=DATA<<1;
		digitalWrite(Max7219_pinCLK,HIGH);
	}                                 
}
void Write_Max7219(uchar address,uchar dat)
{ 
	digitalWrite(Max7219_pinCS,LOW);
	Write_Max7219_byte(address);          
	Write_Max7219_byte(dat);              
	digitalWrite(Max7219_pinCS,HIGH);                       
}

//initialization MAX7219
void Init_MAX7219(void)
{
	Write_Max7219(0x09, 0x00);      
	Write_Max7219(0x0a, 0x01);      
	Write_Max7219(0x0b, 0x07);      
	Write_Max7219(0x0c, 0x01);      
	Write_Max7219(0x0f, 0x00);      
}


//display the matrix
void TotalDisplay(uchar display[8][8])
{
       convert(display,displayInBytes);
       uchar i;
	for(i=1;i<9;i++)
	{
	    Write_Max7219(i,displayInBytes[i-1]);
	}
}

void rightShift(uchar disp1[8][8])
{
        uchar lastColumn[8] = {disp1[0][7],disp1[1][7],disp1[2][7],disp1[3][7],disp1[4][7],disp1[5][7],disp1[6][7],disp1[7][7]};
        uint i,j;
        for(i = 0; i < 8; i++)
        {
          for(j = 8; j >= 1; j--)
          {
            if(j >= 2)
              disp1[i][j-1] = disp1[i][j-2];
            else
              disp1[i][j-1] = lastColumn[i]; 
          }
        }   
}

void welcomeScreen(){
  Tft.drawString("Welcome to",62,50,2,WHITE);
  Tft.drawString("Arduino Tetris",40,75,2,WHITE);
  Tft.drawString("Please choose a",30,150,2,WHITE);
  Tft.drawString("level",93,170,2,WHITE);
  Tft.drawRectangle(70,190,100,30,WHITE);
  Tft.drawRectangle(70,230,100,30,WHITE);
  Tft.drawRectangle(70,270,100,30,WHITE);
  Tft.drawString("Easy",95,198,2,WHITE);
  Tft.drawString("Medium",84,238,2,WHITE);
  Tft.drawString("Hard",95,278,2,WHITE);
}

void runtimeScreen(){
  Tft.drawString("Current score:",40,70,2,WHITE);
  Tft.drawString("Highest score:",40,160,2,WHITE);
  Tft.drawNumber(newScore[0],40,100,2,WHITE);
  Tft.drawNumber(newScore[1],40,190,2,WHITE);
}

void gameOverScreen(){
  Tft.drawString("Game Over",67,40,2,WHITE);
  Tft.drawString("Your score:",40,90,2,WHITE);
  Tft.drawString("Highest score:",40,170,2,WHITE);
  Tft.drawNumber(newScore[0],40,120,2,WHITE);
  Tft.drawNumber(newScore[1],40,200,2,WHITE);
  Tft.drawRectangle(40,230,160,40,WHITE);
  Tft.drawString("Try again",65,243,2,WHITE);
}

void gameOverAnimation(){
  if(animationFrame < 9){
    if(millis() - tAnimation >= 200){
      TotalDisplay(animation[animationFrame]);
      tAnimation = millis();
      animationFrame++;
    }
  }
  else{
    if(millis() - tAnimation >= 800){
      TotalDisplay(animation[animationFrame]);
      tAnimation = millis();
      animationFrame++;
      if(animationFrame == 17)
        animationFrame = 0;
    }
  }
}

void setup(void)
{
	Wire.begin();
        accelgyro.initialize();
        Tft.init();
        pinMode(Max7219_pinCLK,OUTPUT);
	pinMode(Max7219_pinCS,OUTPUT);
	pinMode(Max7219_pinDIN,OUTPUT);
        randomSeed(analogRead(0));
        Serial.begin(9600);
	delay(300);
	Init_MAX7219();
        type = analogRead(0) % 19;
}



void loop()
{
        switch(gameState){
          case Welcome:{
            totalClear(final);
            totalClear(disp1);
            TotalDisplay(final);
            welcomeScreen();
            Point p = ts.getPoint();
            touchX = map(p.x, TS_MINX, TS_MAXX, 0, 240);
            touchY = map(p.y, TS_MINY, TS_MAXY, 0, 320);
            if (touchX >= 70 && touchX <= 170 && touchY >= 190 && touchY <= 220){
              Tft.fillRectangle(70,190,100,30,GRAY1);
              Tft.drawString("Easy",95,198,2,WHITE);
              fallSpeed = 2000;
              totalClear(disp1);
              gameState = Main;
              coefficient = 1;
              Tft.fillScreen();
              runtimeScreen();
            }
            else if(touchX >= 70 && touchX <= 170 && touchY >= 230 && touchY <= 260){
              Tft.fillRectangle(70,230,100,30,GRAY1);
              Tft.drawString("Medium",84,238,2,WHITE);
              fallSpeed = 1500;
              totalClear(disp1);
              for (int i = 0; i < 8; i++){
                disp1[7][i] = random(299) % 2;
              }
              gameState = Main;
              coefficient = 1.2;
              Tft.fillScreen();
              runtimeScreen();
            }
            else if(touchX >= 70 && touchX <= 170 && touchY >= 270 && touchY <= 300){
              Tft.fillRectangle(70,270,100,30,GRAY1);
              Tft.drawString("Hard",95,278,2,WHITE);
              fallSpeed = 1000;
              totalClear(disp1);
              for (int j = 7; j > 5; j--){
                for (int i = 0; i < 8; i++){
                  disp1[j][i] = random(299) % 2;
                }
              }
              gameState = Main;
              coefficient = 1.4;
              Tft.fillScreen();
              runtimeScreen();
            }
            break;
          }
          case Main:{
            accelgyro.getMotion6(&ax,&ay,&az,&gx,&gy,&gz);
            xAcc = ax/16384.0;
            yAcc = ay/16384.0;
            if(countLeft == 0){
              tLeft = millis();
              countLeft = 1;
            }
            else if(xAcc >= 0.55 && leftCheck(Rbias,Cbias,type,disp1) == 0 && reachLeft(Rbias,Cbias,type,disp1) == 0 && countLeft == 1 && (millis() - tLeft) >= 200){
              Cbias--;
              countLeft = 0;
            }
            
            if(countRight == 0){
              tRight = millis();
              countRight = 1;
            }
            else if(xAcc <= -0.55 && rightCheck(Rbias,Cbias,type,disp1) == 0 && reachRight(Rbias,Cbias,type,disp1) == 0 && countRight == 1 && (millis() - tRight) >= 200){
              countRight = 0;
              Cbias++;
            }
            
            if(countUp == 0){
              tUp = millis();
              countUp = 1;
            }
            else if(yAcc >= 0.55 && bottomCheck(Rbias,Cbias,type,disp1) == 0 && reachBottom(Rbias,Cbias,type,disp1) == 0 && countUp == 1 && (millis() - tUp) >= 200){
              Rbias++;
              countUp = 0;
            }
            
            if(countDown == 0){
              tDown = millis();
              countDown = 1;
            }
            else if(yAcc <= -0.55 && changeCheck(Rbias,Cbias,type,disp1) == 0 && countDown == 1 && (millis() - tDown) >= 200){
              countDown = 0;
              type = pattern[type][8];
            }
            
            /*switch (Serial.read())
            {
              case 0x61:   //A left
              {
              if(leftCheck(Rbias,Cbias,type,disp1) == 0)
              {
                 if(reachLeft(Rbias,Cbias,type,disp1) == 0)  
                {
                  Cbias--;
                }
              }
              break;
              }
              case 0x73:    //S  down
              {
              if(bottomCheck(Rbias,Cbias,type,disp1) == 0)
               {
                 if(reachBottom(Rbias,Cbias,type,disp1) == 0)
                 {
                   Rbias++;
                 }
               }
              break;
              }
              case 0x64:    //D  right
              {
              if(rightCheck(Rbias,Cbias,type,disp1) == 0)
                 { 
                   if(reachRight(Rbias,Cbias,type,disp1) == 0)
                   {
                     Cbias++;
                   }
                 }
              break;
              }
              case 0x77:     //W  up
              {
              if(changeCheck(Rbias,Cbias,type,disp1) == 0)
                  type = pattern[type][8];
              break;
              }
            }*/
            
            
            if(count3 == 0)
            {
              t3 = millis();
              count3 = 1;
            }
            if((count3 == 1) && ((millis() - t3) >= fallSpeed))
            {
              count3 = 0;
              if((bottomCheck(Rbias,Cbias,type,disp1) == 0) && (reachBottom(Rbias,Cbias,type,disp1) == 0))
              Rbias++;
            }
            
            
            
            patternAdd(Rbias, Cbias, type, disp1, final);
            TotalDisplay(final);         
             
            //check if the object reach other objects or reach the bottom
            if((bottomCheck(Rbias,Cbias,type,disp1) == 1)&&(count1 == 0))
            {
              t1 = millis();
              count1 = 1;
            }
            if((bottomCheck(Rbias,Cbias,type,disp1) == 1)&&(count1 == 1)&&((millis() - t1) >= 1000))
            {
                clearedLine = clearLine(final);
                if(clearedLine >= 1)
                {
                    newScore[0] = newScore[0] + (long)100*clearedLine*clearedLine*coefficient;
                    delay(250);
                    clearedLine = 0;
                }
               for(int i = 0; i < 8; i++)
                 for(int j = 0; j < 8; j++)
                 {
                   disp1[i][j] = final[i][j];
                 }
               if(newScore[0] > oldScore[0]){
                 Tft.fillRectangle(40,100,150,30,BLACK);
                 Tft.drawNumber(newScore[0],40,100,2,WHITE);
                 oldScore[0] = newScore[0];
               }
               if(newScore[0] > newScore[1]){
                 newScore[1] = newScore[0];
                 Tft.fillRectangle(40,190,150,30,BLACK);
                 Tft.drawNumber(newScore[1],40,190,2,WHITE);
                 oldScore[1] = newScore[1];
               }
               gameFailFlag = gameFailCheck(disp1);
               count1 = 0;
               Rbias = -2;
               Cbias = 2;
               type = millis() % 19;
            }
            
            if((reachBottom(Rbias,Cbias,type,disp1) == 1) && (count2 == 0))
            {
              t2 = millis();
              count2 = 1;
            }
            if((reachBottom(Rbias,Cbias,type,disp1) == 1)&&(count2 == 1)&&((millis() - t2) >= 1000))
            {
              clearedLine = clearLine(final);
              if(clearedLine >= 1)
              {
                  newScore[0] = newScore[0] + (long)100*clearedLine*clearedLine*coefficient;
                  delay(250);
                  clearedLine = 0;
              }
              for(int i = 0; i < 8; i++)
                 for(int j = 0; j < 8; j++)
                 {
                   disp1[i][j] = final[i][j];
                 }
               if(newScore[0] > oldScore[0]){
                 Tft.fillRectangle(40,100,150,30,BLACK);
                 Tft.drawNumber(newScore[0],40,100,2,WHITE);
                 oldScore[0] = newScore[0];
               }
               if(newScore[0] > newScore[1]){
                 newScore[1] = newScore[0];
                 Tft.fillRectangle(40,190,150,30,BLACK);
                 Tft.drawNumber(newScore[1],40,190,2,WHITE);
                 oldScore[1] = newScore[1];
               }
               gameFailFlag = gameFailCheck(disp1);
               count2 = 0;
               Rbias = -2;
               Cbias = 2;
               type = millis() % 19;     
            }
            
            if(gameFailFlag){
              gameState = GameOver;
              Tft.fillScreen();
              gameOverScreen();
            }
            break;
          }
          case GameOver:{
            gameOverAnimation();
            Point p = ts.getPoint();
            touchX = map(p.x, TS_MINX, TS_MAXX, 0, 240);
            touchY = map(p.y, TS_MINY, TS_MAXY, 0, 320);
            if(touchX >= 40 && touchX <= 200 && touchY >= 230 && touchY <= 270){
              Tft.fillRectangle(40,230,160,40,GRAY1);
              Tft.drawString("Try again",65,243,2,WHITE);
              gameState = Welcome;
              totalClear(disp1);
              totalClear(final);
              for(int o = 0; o < 8; o++){
                for(int l = 0; l < 8; l++){
                  Serial.print(disp1[o][l]);
                }
                Serial.println();
              }
              Serial.println();
              gameFailFlag = false;
              animationFrame = 0;
              Tft.fillScreen();
              newScore[0] = 0;
              oldScore[0] = 0;
            }
            break;
          }
        }
}
