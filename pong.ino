#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define PLAYER_LEFT
#define PLAYER_RIGHT

#define PLAYER_BAR_LENGTH 10
#define PLAYER_BAR_WIDTH 1

#define PLAYER_LEFT 0
#define PLAYER_RIGHT 1

#define PLAYER_LEFT_INPUT A0
#define PLAYER_RIGHT_INPUT A1

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

int positionPlayerBarLeft = 0;
int positionPlayerBarRight = 0;
int game_running = 1;

void setup()   {
  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("PONG!");
  display.display();
  display.clearDisplay();
  delay(1000);
}

void loop() {
  display.clearDisplay();

  checkGameState();

  if (game_running == 0) {
    return;
  }

  int original_left = analogRead(PLAYER_LEFT_INPUT);
  positionPlayerBarLeft = map(original_left, 0, 1023, 0, 31 - PLAYER_BAR_LENGTH);
  drawPlayerBar(positionPlayerBarLeft, PLAYER_LEFT);

  int original_right = analogRead(PLAYER_RIGHT_INPUT);
  positionPlayerBarRight = map(original_right, 0, 1023, 0, 31 - PLAYER_BAR_LENGTH);
  drawPlayerBar(positionPlayerBarRight, PLAYER_RIGHT);

  drawBall();

  delay(10);
}

void drawPlayerBar(uint8_t position_y, uint8_t player) {
  if (position_y <= SSD1306_LCDHEIGHT - 1 - PLAYER_BAR_LENGTH) {
    int position_x;
    if (player == PLAYER_LEFT) {
      position_x = 0;
    } else if (player == PLAYER_RIGHT) {
      position_x = SSD1306_LCDWIDTH - 1;
    }
    display.drawLine(position_x, position_y, position_x, position_y + PLAYER_BAR_LENGTH, WHITE);
    display.display();
    delay(1);
  }
}

int ballPositionX = 64;
int ballPositionY = 16;
int ballDirectionX = 1;
int ballDirectionY = 1;

void drawBall() {
  display.drawPixel(ballPositionX, ballPositionY, WHITE);
  display.display();

  if (ballDirectionX == 1 && ballPositionX > SSD1306_LCDWIDTH - 1 - PLAYER_BAR_WIDTH) {
    ballDirectionX = -1;
  } else if (ballDirectionX == -1 && ballPositionX < 0 + 1 + PLAYER_BAR_WIDTH) {
    ballDirectionX = 1;
  }

  if (ballDirectionY == 1 && ballPositionY > SSD1306_LCDHEIGHT) {
    ballDirectionY = -1;
  } else  if (ballDirectionY == -1 && ballPositionY < 0) {
    ballDirectionY = 1;
  }

  ballPositionX += ballDirectionX;
  ballPositionY += ballDirectionY;
}

void checkGameState() {
  if (game_running == 0) {
    display.setCursor(0,0);
    display.println("GAME OVER!");
    display.display();
    display.clearDisplay();
    return;
  }

  if (ballPositionX == 1) {
    Serial.println(positionPlayerBarLeft);
    Serial.println(positionPlayerBarLeft + PLAYER_BAR_LENGTH);
    Serial.println(ballPositionY);
    Serial.println();
    if (positionPlayerBarLeft <= ballPositionY && (positionPlayerBarLeft + PLAYER_BAR_LENGTH) >= ballPositionY) {
      // TODO: boing flip
    } else {
      display.setCursor(0,0);
      display.println("PLAYER RIGHT WIN!");
      display.display();
      display.clearDisplay();
      delay(2000);
      game_running = 0;
    }
  } else if (ballPositionX == SSD1306_LCDWIDTH - 1) {
    Serial.println(positionPlayerBarRight);
    Serial.println(positionPlayerBarRight + PLAYER_BAR_LENGTH);
    Serial.println(ballPositionY);
    Serial.println();
    if (positionPlayerBarRight <= ballPositionY && (positionPlayerBarRight + PLAYER_BAR_LENGTH) >= ballPositionY) {
      // TODO: boing flip
    } else {
      display.setCursor(0,0);
      display.println("PLAYER LEFT WIN!");
      display.display();
      display.clearDisplay();
      delay(2000);
      game_running = 0;
    }
  }
}
