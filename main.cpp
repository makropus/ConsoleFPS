#include <Windows.h>
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
using namespace std;
#include <string>
#include <cmath>
#include <chrono>
#include <vector>
#include <algorithm>

int screenWidth = 200;
int screenHeight= 60;

float playerX = 12.0f;
float playerY = 6.0f;
float playerAngle = 3.14159f / 4.0f;
float fov = 3.14159f / 4.0f;
float rotationSpeed = 1.0f;
float moveSpeed = 5.0f;


int main() {
    auto screen = new wchar_t[screenWidth*screenHeight];
    auto hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD bytesWritten = 0;

    wstring map;
    int mapWidth = 16;
    int mapHeight = 16;
    float maxDepth = 20.0f;

    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();

    map += L"################";
    map += L"#..............#";
    map += L"#......#########";
    map += L"#..............#";
    map += L"#.....#####....#";
    map += L"#.....#........#";
    map += L"#..............#";
    map += L"#...##.##......#";
    map += L"#...#...#......#";
    map += L"#...#...#......#";
    map += L"#...#####......#";
    map += L"#..............#";
    map += L"#.#.#.#.#.#.#..#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"################";

    //game loop
    while (true) {
        tp2 = chrono::system_clock::now();
        chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float elapsedTimeCount = elapsedTime.count();

        //Controls
        if(GetAsyncKeyState((unsigned short)'A') & 0x8000)
            playerAngle += -rotationSpeed * elapsedTimeCount;
        if(GetAsyncKeyState((unsigned short)'D') & 0x8000)
            playerAngle -= -rotationSpeed * elapsedTimeCount;

        if(GetAsyncKeyState((unsigned short)'W') & 0x8000) {
            playerX += sinf(playerAngle) * moveSpeed * elapsedTimeCount;
            playerY += cosf(playerAngle) * moveSpeed * elapsedTimeCount;
            if(map[(int)playerX* mapWidth + (int)playerY] == '#'){
                playerX -= sinf(playerAngle) * moveSpeed * elapsedTimeCount;
                playerY -= cosf(playerAngle) * moveSpeed * elapsedTimeCount;
            }
        }

         if(GetAsyncKeyState((unsigned short)'S') & 0x8000) {
            playerX -= sinf(playerAngle) * moveSpeed * elapsedTimeCount;
            playerY -= cosf(playerAngle) * moveSpeed * elapsedTimeCount;
             if(map[(int)playerX* mapWidth + (int)playerY] == '#') {
                 playerX += sinf(playerAngle) * moveSpeed * elapsedTimeCount;
                 playerY += cosf(playerAngle) * moveSpeed * elapsedTimeCount;
             }
        }

        if(GetAsyncKeyState((unsigned short)'S') & 0x8000)
            playerAngle += -0.1f * elapsedTimeCount;

        for (int x = 0; x < screenWidth; x++) {
            float rayAngle = (playerAngle - fov / 2.0f) + ((float)x / (float)screenWidth) * fov;

            float distanceToWall = 0.0f;
            float stepSize = 0.1f;

            bool hitWall = false;
            bool hitBoundary = false;

            float eyeX = sinf(rayAngle);
            float eyeY = cosf(rayAngle);

            //cast a ray
            while (!hitWall && distanceToWall < maxDepth){
                distanceToWall += stepSize;

                int testX = (int)(playerX + eyeX * distanceToWall);
                int testY = (int)(playerY + eyeY * distanceToWall);

                if (testX < 0 || testX >= mapWidth || testY < 0 || testY >= mapHeight){
                    hitWall = true;
                    distanceToWall = maxDepth;
                }
                else {
                    if(map.c_str()[testX * mapWidth + testY] == '#'){
                        hitWall = true;
                        vector<pair<float, float>> p;

                        for (int tx = 0; tx < 2; ++tx) {
                            for (int ty = 0; ty < 2; ++ty) {
                                float vy = (float)testY + ty - playerY;
                                float vx = (float)testX + tx - playerX;
                                float d = sqrt(vx*vx + vy*vy);
                                float dot = (eyeX * vx / d) + (eyeY * vy / d);
                                p.push_back(make_pair(d, dot));
                            }
                        }
                        sort(p.begin(), p.end(), []
                            (const pair<float, float> &left, const pair<float, float> &right)
                            {return left.first < right.first; });
                        float bound = 0.005f;
                        if(acos(p.at(0).second) < bound) hitBoundary = true;
                        if(acos(p.at(1).second) < bound) hitBoundary = true;
                    }
                }
            }
            int cellingEdge = (float)(screenHeight / 2.0) - screenHeight / ((float)distanceToWall);
            int floorEdge = screenHeight - cellingEdge;

            short shade = ' ';

            if (distanceToWall <= maxDepth / 4.0f)			shade = 0x2588;	// Very close
            else if (distanceToWall < maxDepth / 3.0f)		shade = 0x2593;
            else if (distanceToWall < maxDepth / 2.0f)		shade = 0x2592;
            else if (distanceToWall < maxDepth)				shade = 0x2591;
            else											shade = ' ';

            if(hitBoundary) shade = ' ';

            for (int y = 0; y < screenHeight; y++) {
                if(y <= cellingEdge)
                    screen[y*screenWidth + x] = ' ';
                else if (y > cellingEdge  && y <= floorEdge)
                    screen[y*screenWidth + x] = shade;
                else{
                    // Shade floor based on distance
                    float b = 1.0f - (((float)y - screenHeight/2.0f) / ((float)screenHeight / 2.0f));
                    if (b < 0.25)		shade = '#';
                    else if (b < 0.5)	shade = 'x';
                    else if (b < 0.75)	shade = '.';
                    else if (b < 0.9)	shade = '-';
                    else				shade = ' ';
                    screen[y*screenWidth + x] = shade;
                }

            }
        }
        swprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f ",
                   playerX, playerY, playerAngle * 180 / 3.14159f, 1.0f/elapsedTimeCount);
        // Display Map
        for (int nx = 0; nx < mapWidth; nx++)
            for (int ny = 0; ny < mapWidth; ny++)
            {
                screen[(ny+1)*screenWidth + nx] = map[ny * mapWidth + nx];
            }
        screen[((int)playerX+1) * screenWidth + (int)playerY] = 'P';

        screen[screenWidth * screenHeight - 1] = '\0';
        WriteConsoleOutputCharacterW(hConsole, screen, screenWidth * screenHeight, {0, 0}, &bytesWritten);
    }
    return 0;
}
