#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Joystick pins
const int VRx = A0;
const int VRy = A1;
const int SW = 2;  // Button pin

// Snake variables
int snakeX[100], snakeY[100];  // Snake coordinates
int snakeLength = 5;
int foodX, foodY;  // Food coordinates
int dirX = 1, dirY = 0;  // Initial direction (moving right)
bool gameRunning = false;

void setup() {
  pinMode(SW, INPUT_PULLUP);  // Joystick button
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  
  // Display the Snake Game logo
  drawLogo();
  display.display();

  // Wait for user to press joystick to start the game
  while (digitalRead(SW) == HIGH) {
    // Just wait until the joystick button is pressed
  }

  // Initialize game after button press
  startGame();
}

void loop() {
  if (gameRunning) {
    readJoystick();
    updateSnake();
    checkCollision();
    drawGame();
    delay(100);  // Control game speed
  }
}

void drawLogo() {
  display.clearDisplay();
  
  // Draw the game console remote
  display.drawRoundRect(40, 10, 48, 28, 5, SSD1306_WHITE);  // Game console body (rounded rectangle)
  display.drawCircle(52, 24, 4, SSD1306_WHITE);             // Left circular joystick (D-Pad)
  display.drawLine(50, 24, 54, 24, SSD1306_WHITE);          // Horizontal D-Pad
  display.drawLine(52, 22, 52, 26, SSD1306_WHITE);          // Vertical D-Pad
  display.fillCircle(72, 24, 2, SSD1306_WHITE);             // Right button (A/B button)

  // Draw the Snake Game text under the console remote
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(35, 45);
  display.print("Snake Game");

  display.display();
}


void startGame() {
  // Initialize snake position
  snakeLength = 5;  // Reset snake length
  for (int i = 0; i < snakeLength; i++) {
    snakeX[i] = 64 - i;  // Horizontal start
    snakeY[i] = 32;
  }

  // Place the food
  placeFood();
  
  gameRunning = true;
}

void readJoystick() {
  int xVal = analogRead(VRx);
  int yVal = analogRead(VRy);

  const int thresholdLow = 300;
  const int thresholdHigh = 700;

  if (xVal < thresholdLow && dirX == 0) {
    dirX = -1;
    dirY = 0;
  } else if (xVal > thresholdHigh && dirX == 0) {
    dirX = 1;
    dirY = 0;
  } else if (yVal < thresholdLow && dirY == 0) {
    dirX = 0;
    dirY = -1;
  } else if (yVal > thresholdHigh && dirY == 0) {
    dirX = 0;
    dirY = 1;
  }
}

void updateSnake() {
  // Move the snake body (shift the positions)
  for (int i = snakeLength - 1; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }
  
  // Move the head in the current direction
  snakeX[0] += dirX * 4;  // Adjusted step size for larger segments
  snakeY[0] += dirY * 4;

  // Check if the snake head is close enough to the food
  if (isFoodEaten(snakeX[0], snakeY[0], foodX, foodY)) {
    snakeLength++;  // Increase the snake length
    placeFood();    // Place a new food
  }
}

bool isFoodEaten(int snakeHeadX, int snakeHeadY, int foodX, int foodY) {
  // Check if the snake's head is near or touching the food
  int snakeSize = 4;  // Snake segment size
  int foodSize = 4;   // Food size

  // Check if the boundaries of the snake head overlap with the food
  bool overlapX = (snakeHeadX < foodX + foodSize) && (snakeHeadX + snakeSize > foodX);
  bool overlapY = (snakeHeadY < foodY + foodSize) && (snakeHeadY + snakeSize > foodY);

  // If both X and Y boundaries overlap, the snake is eating the food
  return overlapX && overlapY;
}

void checkCollision() {
  if (snakeX[0] < 0 || snakeX[0] >= SCREEN_WIDTH || snakeY[0] < 0 || snakeY[0] >= SCREEN_HEIGHT) {
    gameOver();
  }

  for (int i = 1; i < snakeLength; i++) {
    if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
      gameOver();
    }
  }
}

void drawGame() {
  display.clearDisplay();

  // Draw the snake with larger size
  for (int i = 0; i < snakeLength; i++) {
    display.fillRect(snakeX[i], snakeY[i], 6, 6, SSD1306_WHITE);  // Larger snake segments
  }

  // Draw the food with a larger size
  display.fillRect(foodX, foodY, 6, 6, SSD1306_WHITE);  // Larger food
  display.display();
}

void placeFood() {
  // Place food at random locations that fit the larger size
  foodX = random(0, (SCREEN_WIDTH / 6)) * 6;
  foodY = random(0, (SCREEN_HEIGHT / 6)) * 6;
}


void gameOver() {
  gameRunning = false;
  display.clearDisplay();
  
  // Creative Game Over screen with Snake logo
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.print("Game Over");
  
  // Draw snake-like shape or pattern
  display.setTextSize(1);
  display.setCursor(10, 40);
  display.print("Press to Restart");
  
  display.display();
  
  // Wait for user to press joystick button to restart
  while (digitalRead(SW) == HIGH) {
    // Wait here until the joystick button is pressed
  }

  // Restart the game
  startGame();
}