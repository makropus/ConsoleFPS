#include <Windows.h>
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
using namespace std;
#include <string>
#include <cmath>

int screenWidth = 120;
int screenHeight= 40;

float playerX = 8.0f;
float playerY = 8.0f;
float playerAngle = 0;

float fov = 3.14159 / 4.0;


int main() {
    auto screen = new wchar_t[screenWidth*screenHeight];
    auto hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD bytesWritten = 0;

    wstring map;
    int mapWidth = 16;
    int mapHeight = 16;
    float maxDepth = 16.0f;

    map += L"################";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"################";

    //game loop
    while (true) {
        for (int x = 0; x < screenWidth; ++x) {
            float rayAngle = (playerAngle - fov / 2.0f) + ((float)x / (float)screenWidth) * fov;

            float distanceToWall = 0;
            bool hitWall = false;

            float eyeX = sinf(rayAngle);
            float eyeY = cosf(rayAngle);
            //cast a ray
            while (!hitWall && distanceToWall < maxDepth){
                distanceToWall += 0.1f;

                int testX = (int)(playerX + eyeX * distanceToWall);
                int testY = (int)(playerY + eyeY * distanceToWall);

                if (testX < 0 || testX >= mapWidth || testY < 0 || testY >= mapHeight){
                    hitWall = true;
                    distanceToWall = maxDepth;
                }
                else {
                    if(map[testY * mapWidth + testX] == '#'){
                        hitWall = true;
                    }
                }
            }
            int cellingEdge = (float)(screenHeight / 2.0) - screenHeight / ((float)distanceToWall);
            int floorEdge = screenHeight - cellingEdge;

            for (int y = 0; y < screenHeight; y++) {
                if(y < cellingEdge)
                    screen[y*screenWidth + x] = '.';
                else if (y > cellingEdge and y <= floorEdge)
                    screen[y*screenWidth + x] = '#';
                else
                    screen[y*screenWidth + x] = '&';
            }
        }
        screen[screenWidth * screenHeight] = '\0';
        WriteConsoleOutputCharacter(hConsole, (const char *) screen, screenWidth * screenHeight, {0, 0}, &bytesWritten);
    }
    return 0;
}
