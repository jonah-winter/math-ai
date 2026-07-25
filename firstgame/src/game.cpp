#include "raylib.h"
#include <cmath>
#include <vector>
#include <iostream>
#include <format>
int main() {
        int Height { 800 } ;
        int Width { 600 } ;
        InitWindow(Width, Height, "Gravity Simulation") ;
        SetTargetFPS(60) ;
        float posx { Width * 0.75f } ;
        float radius { 50 } ;
        float floor { Height - radius } ;
        float posy { (Height * 0.25f) } ;
        float gravity { 19.6f} ;
        float velocity { 0 } ;
        float deltatime { GetFrameTime() } ;
        bool isstopped {false} ;
        while (!WindowShouldClose()) {
        BeginDrawing() ;
        ClearBackground(BLACK) ;
        deltatime = GetFrameTime() ;
        DrawLine(0, floor, Width, floor, RAYWHITE) ;
        if (isstopped == true) {
                        posy = floor - radius ;
                        velocity = 0 ;
        }
        else {
                velocity += gravity ;
                posy += velocity * deltatime ;
                if ((posy + radius) >= floor) {
                        posy = floor - radius ;
                        if (velocity < 40.0f) {
                                isstopped = true ;
                        }
                        else {
                                velocity *= -0.75f ;
                        }
        }
        }
        DrawCircle(posx, posy, radius, ORANGE) ;
        DrawCircle(posx, posy, radius * 0.7f, YELLOW) ;
        float realpos { Height - posy - (Height * (1/8)) - 100 } ;
        DrawText(TextFormat("Y Position is : %.0f", realpos), Width * 0.6f, Height * 0.05f, 20, GRAY) ;
        DrawText(TextFormat("Speed is : %.1f", abs(velocity) * 0.1f), Width * 0.1f, Height * 0.05f, 20, GRAY) ;
        EndDrawing() ;
        }
        return 0 ;
}