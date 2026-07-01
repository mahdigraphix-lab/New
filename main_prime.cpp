#include <bits/stdc++.h>
#include <SDL.h>

using namespace std;

// ===============================
// Window and grid configuration
// ===============================

const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 400;
const int CELL_SIZE = 20;

const int COLS = WINDOW_WIDTH / CELL_SIZE;
const int ROWS = WINDOW_HEIGHT / CELL_SIZE;

// ===============================
// Game data types
// ===============================

// Snake movement directions
enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

// Represents one block/cell in the grid
struct Cell
{
    int x;
    int y;
};

// ===============================
// Helper functions
// ===============================

// Check if two cells are in the same position
bool sameCell(Cell a, Cell b)
{
    return a.x == b.x && a.y == b.y;
}

// Reset snake to the starting position
void resetSnake(vector<Cell>& snake, Direction& dir)
{
    snake.clear();

    // Snake starts with 3 blocks near the center
    snake.push_back({COLS / 2, ROWS / 2});
    snake.push_back({COLS / 2 + 1, ROWS / 2});
    snake.push_back({COLS / 2 + 2, ROWS / 2});

    // Initial movement direction
    dir = LEFT;
}

// Change direction while preventing opposite-direction movement
void changeDirection(Direction& dir, Direction newDir)
{
    if (dir == UP && newDir == DOWN) return;
    if (dir == DOWN && newDir == UP) return;
    if (dir == LEFT && newDir == RIGHT) return;
    if (dir == RIGHT && newDir == LEFT) return;

    dir = newDir;
}

// Get the next head position based on current direction
Cell getNewHead(Cell head, Direction dir)
{
    Cell newHead = head;

    if (dir == UP) newHead.y--;
    if (dir == DOWN) newHead.y++;
    if (dir == LEFT) newHead.x--;
    if (dir == RIGHT) newHead.x++;

    return newHead;
}

// Move the snake forward
void moveSnake(vector<Cell>& snake, Direction dir, bool grow)
{
    Cell head = snake[0];
    Cell newHead = getNewHead(head, dir);

    // Add new head at the front
    snake.insert(snake.begin(), newHead);

    // Remove tail if snake does not eat food
    if (!grow)
    {
        snake.pop_back();
    }
}

// Check if snake hits the window border
bool hitWall(vector<Cell>& snake)
{
    Cell head = snake[0];

    return head.x < 0 || head.x >= COLS || head.y < 0 || head.y >= ROWS;
}

// Check if snake hits its own body
bool hitItself(vector<Cell>& snake)
{
    Cell head = snake[0];

    for (int i = 1; i < snake.size(); i++)
    {
        if (sameCell(head, snake[i]))
        {
            return true;
        }
    }

    return false;
}

// Generate food at a random position not occupied by snake
Cell generateFood(vector<Cell>& snake)
{
    Cell food;
    bool valid = false;

    while (!valid)
    {
        food.x = rand() % COLS;
        food.y = rand() % ROWS;

        valid = true;

        // Check if food appears on the snake
        for (Cell part : snake)
        {
            if (sameCell(food, part))
            {
                valid = false;
                break;
            }
        }
    }

    return food;
}

// Draw one cell/block on the screen
void drawCell(SDL_Renderer* renderer, Cell cell, int r, int g, int b)
{
    SDL_Rect rect;

    rect.x = cell.x * CELL_SIZE;
    rect.y = cell.y * CELL_SIZE;
    rect.w = CELL_SIZE;
    rect.h = CELL_SIZE;

    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderFillRect(renderer, &rect);
}

// ===============================
// Main function
// ===============================

int main(int argc, char* argv[])
{
    // Seed random number generator
    srand(time(0));

    // ===============================
    // SDL initialization
    // ===============================

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cout << "SDL initialization failed!" << endl;
        return 1;
    }

    // Create game window
    SDL_Window* window = SDL_CreateWindow(
        "Snake Game Without Class",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (window == NULL)
    {
        cout << "Window creation failed!" << endl;
        SDL_Quit();
        return 1;
    }

    // Create renderer for drawing
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL)
    {
        cout << "Renderer creation failed!" << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // ===============================
    // Game variables
    // ===============================

    vector<Cell> snake;
    Direction dir;

    resetSnake(snake, dir);

    Cell food = generateFood(snake);

    int score = 0;

    bool running = true;
    bool gameOver = false;

    SDL_Event event;

    // ===============================
    // Main game loop
    // ===============================

    while (running)
    {
        // ===============================
        // Input handling
        // ===============================

        while (SDL_PollEvent(&event))
        {
            // Close button clicked
            if (event.type == SDL_QUIT)
            {
                running = false;
            }

            // Keyboard input
            if (event.type == SDL_KEYDOWN)
            {
                // Press ESC to quit
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    running = false;
                }

                // Change direction only while game is running
                if (!gameOver)
                {
                    if (event.key.keysym.sym == SDLK_UP)
                    {
                        changeDirection(dir, UP);
                    }
                    else if (event.key.keysym.sym == SDLK_DOWN)
                    {
                        changeDirection(dir, DOWN);
                    }
                    else if (event.key.keysym.sym == SDLK_LEFT)
                    {
                        changeDirection(dir, LEFT);
                    }
                    else if (event.key.keysym.sym == SDLK_RIGHT)
                    {
                        changeDirection(dir, RIGHT);
                    }
                }

                // Press R to restart after game over
                if (gameOver && event.key.keysym.sym == SDLK_r)
                {
                    resetSnake(snake, dir);
                    food = generateFood(snake);
                    score = 0;
                    gameOver = false;
                }
            }
        }

        // ===============================
        // Game logic
        // ===============================

        if (!gameOver)
        {
            // Predict next head position before moving
            Cell newHead = getNewHead(snake[0], dir);

            bool grow = false;

            // If next head reaches food, snake grows
            if (sameCell(newHead, food))
            {
                grow = true;
                score += 50;
            }

            // Move snake forward
            moveSnake(snake, dir, grow);

            // Generate new food after eating old food
            if (grow)
            {
                food = generateFood(snake);
            }

            // Check collision after movement
            if (hitWall(snake) || hitItself(snake))
            {
                gameOver = true;
            }
        }

        // ===============================
        // Window title update
        // ===============================

        string title;

        if (!gameOver)
        {
            title = "Snake Game | Score: " + to_string(score);
        }
        else
        {
            title = "Game Over | Final Score: " + to_string(score) + " | Press R to Restart";
        }

        SDL_SetWindowTitle(window, title.c_str());

        // ===============================
        // Drawing section
        // ===============================

        // Clear screen using black background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw food in red
        drawCell(renderer, food, 255, 0, 0);

        // Draw snake
        for (int i = 0; i < snake.size(); i++)
        {
            if (i == 0)
            {
                drawCell(renderer, snake[i], 0, 200, 0); // Snake head
            }
            else
            {
                drawCell(renderer, snake[i], 0, 255, 0); // Snake body
            }
        }

        // Show everything on the window
        SDL_RenderPresent(renderer);

        // Control game speed
        SDL_Delay(150);
    }

    // ===============================
    // Clean up SDL resources
    // ===============================

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}