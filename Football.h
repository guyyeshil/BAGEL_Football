//
// Created by guyye on 01/06/2025.
//
#pragma once
#include <SDL3/SDL.h>
#include <box2d/box2d.h>

#ifndef FOOTBALL_H
#define FOOTBALL_H

#endif //FOOTBALL_H

namespace football {

    using Transform = struct { SDL_FPoint position; float angle; };
    using Drawable = struct { SDL_FRect part; SDL_FPoint size; SDL_Texture* tex; };
    using Intent = struct { bool up, down, left, right; };
    using Keys = struct { SDL_Scancode up, down, left, right; };
    using Collider = struct { b2BodyId body; };
    using Ball = struct{};
    using Net = struct{ b2ShapeId net;};// לא בטוח צריך את הb2ShapeId

    class Football
    {
    public:
        Football();
        ~Football();

        /// game loop
        void run();
        /// ensures initialization succeeded (ctor)
        bool valid() const;

    private:
        void input_system() const;
        void move_system() const;
        void physic_system() const;
        void score_system() const;
        void draw_system() const;

        void createBall() const;
        void createCar(const SDL_FPoint& position, const SDL_FRect& tex, const Keys& keys) const;
        void createField() const;
        void createPad(const SDL_FRect&, const SDL_FPoint&, const Keys&) const;

        bool prepareWindowAndTexture();
        void prepareBoxWorld();
        void prepareWalls() const;

        static constexpr int	WIN_WIDTH = 1300;
        static constexpr int	WIN_HEIGHT = 900;
        static constexpr int	FPS = 60;

        static constexpr float	GAME_FRAME = 1000.f/FPS;
        static constexpr float	RAD_TO_DEG = 57.2958f;

        static constexpr float WANTED_FIELD_PIXEL_WIDTH = 1200;
        static constexpr float WANTED_FIELD_PIXEL_HEIGHT = 800;
        static constexpr float WANTED_BALL_PIXEL = 30;
        static constexpr float WANTED_CAR_PIXEL = 68;


        static constexpr SDL_FRect BALL_TEX = {0, 0, 612, 612};
        static constexpr SDL_FRect BLUE_CAR_TEX = {0, 135, 135, 135};
        static constexpr SDL_FRect ORANGE_CAR_TEX = {0, 270, 135, 135};
        static constexpr SDL_FRect FIELD_TEX = {0, 0, 3900, 2600};

        static constexpr float	BOX_SCALE = 1;
        static constexpr float	BALL_TEX_SCALE = WANTED_BALL_PIXEL/BALL_TEX.w;
        static constexpr float	CAR_TEX_SCALE = WANTED_CAR_PIXEL/BLUE_CAR_TEX.w;
        static constexpr float	FIELD_TEX_SCALE = WANTED_FIELD_PIXEL_HEIGHT/FIELD_TEX.w;

        SDL_Texture* ballTex;
        SDL_Texture* fieldTex;
        SDL_Texture* carsTex;
        SDL_Renderer* ren;
        SDL_Window* win;

        b2WorldId boxWorld = b2_nullWorldId;

        SDL_FPoint ball_start_position = {WIN_WIDTH/2/BOX_SCALE, WIN_HEIGHT/2/BOX_SCALE};

    };


}