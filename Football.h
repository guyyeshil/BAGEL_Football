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

    class Football
    {
    public:
        Football();
        ~Football();

        /// game loop
        void run();
        /// ensures initialization succeeded (ctor)
        //bool valid() const;

    private:
        void prepareBoxWorld();
        bool prepareWindowAndTexture();
        void createBall() const;
        void createCar(const SDL_FPoint& position, const SDL_FRect& tex, const Keys& keys) const;
        void createField() const;
        void createFieldBorders() const;
        void createLeftGoalBars() const;
        void createRightGoalBars() const;
        void createDataBar() const;
        void createScoreFrame() const;


        //systems:
        void input_system() const;
        void move_system() const;
        void physic_system() const;
        //void score_system() const;
        void draw_system() const;


        //Game Data:
        static constexpr int	FPS = 60;
        static constexpr float	GAME_FRAME = 1000.f/FPS;
        static constexpr int	WIN_WIDTH = 1200;
        static constexpr int	WIN_HEIGHT = 900;

        static constexpr float	BOX_SCALE = 16;
        b2WorldId boxWorld = b2_nullWorldId;

        // PHYSICAL SYSTEM SIZES
        static constexpr float	BALL_RADIUS = 0.75;
        static constexpr float	FIELD_WIDTH = 75;
        static constexpr float	FIELD_HEIGHT = 50;
        static constexpr float	CAR_HEIGHT = 2.35;
        static constexpr float	CAR_WIDTH = 4.7;
                //goals:
        static constexpr float SIDE_BAR_WIDTH = 3.125;//FIELD_WIDTH * (1 / 24)
        static constexpr float BACK_BAR_HEIGHT = 7.5;//FIELD_HEIGHT * (3 / 20)

            //Positions:
        SDL_FPoint ball_start_position = {FIELD_WIDTH/2, FIELD_HEIGHT/2};


        //Renders:
            //tex in photo (location and size-pixel):
        static constexpr SDL_FRect BALL_TEX = {0, 0, 612, 612};
        static constexpr SDL_FRect BLUE_CAR_TEX = {0, 135, 135, 135};
        static constexpr SDL_FRect ORANGE_CAR_TEX = {0, 270, 135, 135};
        static constexpr SDL_FRect FIELD_TEX = {0, 0, 681, 454};
        static constexpr SDL_FRect SCOUR_FRAME_TEX = {0, 0, 660, 403};

        SDL_Texture* ballTex;
        SDL_Texture* fieldTex;
        SDL_Texture* carsTex;
        SDL_Texture* scoreFrameTex;

        SDL_Renderer* ren;
        SDL_Window* win;


        //Tools:
        static constexpr float	RAD_TO_DEG = 57.2958f;
    };
}