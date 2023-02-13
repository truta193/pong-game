#include "ge.h"

typedef struct v2f {
    float X;
    float Y;
} v2f;

typedef struct Player {
    v2f coords;
    uint8 score;
    float velocity;
    SpriteT *sprite;
} Player;

typedef struct Ball {
    v2f coords;
    v2f direction;
    float velocity;
    SpriteT *sprite;
} Ball;

Player player1;
Player player2;
Ball ball;

SpriteT *numbers09;

bool CollisionCheck(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
    if (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2) return true;
    return false;
};

float MapValuef(float a, float b, float c, float d, float value) {
    return MIN(c,d) + (ABS(d-c)/ABS(b-a)) * (value - MIN(a,b));
};

void ScoreUpdate() {
        SpriteDrawPartial(numbers09,player1.score*10-10,0, player1.score*10, 12, 2, layers[0]->sprite, 50, 50);
        SpriteDrawPartial(numbers09,player2.score*10-10,0, player2.score*10, 12, 2, layers[0]->sprite, 100, 50);
    
};

return_code UserInitEvent() { 
    PixelT playerRect[900];
    PixelT ballRect[225];
    for (uint32 i = 0; i < 900; i++) playerRect[i] = Pixel(255, 255, 255, 255);
    for (uint32 i = 0; i < 225; i++) ballRect[i] = Pixel(255,255,255,255);

    player1.sprite = SpriteCreate(10, 90, playerRect);
    player1.coords.X = 0.0f;
    player1.coords.Y = 0.0f;
    player1.velocity = 0.0f;
    player1.score = 1;
    
    player2.sprite = SpriteCreate(10, 90, playerRect);
    player2.coords.X = layers[0]->sprite->width - player2.sprite->width;
    player2.coords.Y = 0.0f;
    player2.velocity = 0.0f;
    player2.score = 1;

    ball.sprite = SpriteCreate(15, 15, ballRect);
    ball.coords.X = (layers[0]->sprite->width - ball.sprite->width) / 2 ;
    ball.coords.Y = (layers[0]->sprite->width - ball.sprite->height) / 2 ;
    ball.direction.X = -1.5f;
    ball.direction.Y = 0.5f;
    ball.velocity = 1.0f;

    numbers09 = SpriteLoadFromImage("nrs.png");
    ScoreUpdate();
    return OK; 
};

return_code UserUpdateEvent() { 
    SpriteClear(layers[0]->sprite, Pixel(0,0,0,0));
    if (keyboard[key_W].held) player1.velocity = 3.0f;
    if (keyboard[key_S].held) player1.velocity = -3.0f;
    if (keyboard[key_U].held) player2.velocity = 3.0f;
    if (keyboard[key_J].held) player2.velocity = -3.0f;
    
    
    if (player1.coords.Y < 0) player1.coords.Y = 0; else 
        if (player1.coords.Y+player1.sprite->height > layers[0]->sprite->height) player1.coords.Y = layers[0]->sprite->height-player1.sprite->height; 
        else player1.coords.Y += player1.velocity;
    SpriteDraw(player1.sprite, player1.coords.X, player1.coords.Y, 1, layers[0]->sprite);

    if (player2.coords.Y < 0) player2.coords.Y = 0; else 
        if (player2.coords.Y+player2.sprite->height > layers[0]->sprite->height) player2.coords.Y = layers[0]->sprite->height-player2.sprite->height; 
        else player2.coords.Y += player2.velocity; 
    SpriteDraw(player2.sprite, player2.coords.X, player2.coords.Y, 1, layers[0]->sprite);

    if (CollisionCheck(ball.coords.X, ball.coords.Y, ball.sprite->width, ball.sprite->height, player1.coords.X, player1.coords.Y, player1.sprite->width, player1.sprite->height)) {
        float diff = player1.coords.Y + ((float)player1.sprite->height)/2 - (ball.coords.Y + ((float)ball.sprite->height)/2);
        float mapV = MapValuef(player1.coords.Y, player1.coords.Y + ((float)player1.sprite->height)/2, 0.0f, 1.0f, player1.coords.Y + ABS(diff));
        ball.direction.X *= -1.0f;
        ball.direction.Y = diff<0?mapV:-mapV;
        ball.velocity += 0.1f;
    };

    if (CollisionCheck(ball.coords.X, ball.coords.Y, ball.sprite->width, ball.sprite->height, player2.coords.X, player2.coords.Y, player2.sprite->width, player2.sprite->height)) {
        float diff = player2.coords.Y + ((float)player2.sprite->height)/2 - (ball.coords.Y + ((float)ball.sprite->height)/2);
        float mapV = MapValuef(player2.coords.Y, player2.coords.Y + ((float)player2.sprite->height)/2, 0.0f, 1.0f, player2.coords.Y + ABS(diff));
        ball.direction.X *= -1.0f;
        ball.direction.Y = diff<0?mapV:-mapV;
        ball.velocity += 0.1f; 
    };

    if (ball.coords.X < 0) { 
        ball.coords.X = layers[0]->sprite->width/2;
        ball.coords.Y = layers[0]->sprite->height/2;
        ball.direction.X *= -1.0f;
        ball.velocity = 1.0f;
        ++player2.score;
        if (player2.score > 10) appRunning = false;
    };
    
    if (ball.coords.X + ball.sprite->width > layers[0]->sprite->width) {
        ball.coords.X = layers[0]->sprite->width/2; 
        ball.coords.Y = layers[0]->sprite->height/2;
        ball.direction.X *= -1.0f;
        ball.velocity = 1.0f;
        ++player1.score;
        if (player1.score > 10) appRunning = false;
    };

    ScoreUpdate();

    if (ball.coords.Y < 0 || ball.coords.Y + ball.sprite->height > layers[0]->sprite->height) ball.direction.Y *= -1.0f;

    ball.coords.X += ball.direction.X * ball.velocity;
    ball.coords.Y += ball.direction.Y * ball.velocity;
    SpriteDraw(ball.sprite, ball.coords.X, ball.coords.Y, 1, layers[0]->sprite);

    player1.velocity = 0.0f;
    player2.velocity = 0.0f;
    return OK; 
};

void UserTerminateEvent() { 
    SpriteDelete(player1.sprite);
    SpriteDelete(player2.sprite);
    SpriteDelete(ball.sprite);
    SpriteDelete(numbers09);
    return;
};

int main() {
    Construct(500, 500, 1, true, false);
    Start();
};
