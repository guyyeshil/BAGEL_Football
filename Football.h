#pragma once
#include <SDL3/SDL.h>
#include <box2d/box2d.h>
#define LEFT true
#define RIGHT false

#ifndef FOOTBALL_H
#define FOOTBALL_H

#endif //FOOTBALL_H

namespace football {
    using GameTimer = struct {
        uint64_t start_time;
        uint64_t game_duration_ms;
        bool is_running;
    };
    //tag to identify if digit
    using TimerDigit = struct {};

    static constexpr uint64_t GAME_DURATION_MS = 2 * 60 * 1000 + 30 * 1000; //2:30 minutes
    //textures of digits
    static constexpr SDL_FRect DIGIT_TEX_0 = {0, 0, 229, 417};
    static constexpr SDL_FRect DIGIT_TEX_1 = {229, 0, 229, 417};
    static constexpr SDL_FRect DIGIT_TEX_2 = {458, 0, 229, 417};
    static constexpr SDL_FRect DIGIT_TEX_3 = {687, 0, 229, 417};
    static constexpr SDL_FRect DIGIT_TEX_4 = {916, 0, 229, 417};
    static constexpr SDL_FRect DIGIT_TEX_5 = {0, 417, 229, 417};
    static constexpr SDL_FRect DIGIT_TEX_6 = {229, 417, 229, 417};
    static constexpr SDL_FRect DIGIT_TEX_7 = {458, 417, 229, 417};
    static constexpr SDL_FRect DIGIT_TEX_8 = {686, 417, 229, 417};
    static constexpr SDL_FRect DIGIT_TEX_9 = {916, 417, 229, 417};
    // static constexpr SDL_FRect COLON_TEX = {320, 0, 16, 417};

    using Transform = struct { SDL_FPoint position; float angle; };
    using Drawable = struct { SDL_FRect part; SDL_FPoint size; SDL_Texture* tex; };
    using Intent = struct { bool up, down, left, right; };
    using Keys = struct { SDL_Scancode up, down, left, right; };
    using Collider = struct { b2BodyId body; };
    using Ball = struct{};
    using StartingPosition = struct { SDL_FPoint position; float angle; };

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
        void gameSetupPrams();
        void createBall() const;
        void createCar(const SDL_FPoint& position, const SDL_FRect& tex, const Keys& keys, bool side) const;
        void createField() const;
        void createFieldBorders() const;
        void createLeftGoalBars() const;
        void createRightGoalBars() const;
        void createGoalSensor(bool isLeftGoal) const;
        void createDataBar() const;
        void createScoreFrame() const;

        //debug:
        inline static bool DEBUG_MODE = false;
        void applyDebugFunctions()const;
        void consolePrintDebugData()const;
        void renderDebugFunctions() const;
        void drawSensorDebug(const float xPos, const float yPos, const float width, const float height) const;
        void createDebugBox()const;

        //systems:
        static void input_system() ;
        static void move_system() ;
        void physic_system() const;
        void score_system();
        void draw_system() const;
        void reset_location_system() const;
        //timer
        void createGameTimer() const;
        void timer_system();
        SDL_FRect getDigitTexture(int digit) const;


        //Game Data:
        static constexpr int	FPS = 60;
        static constexpr float	GAME_FRAME = 1000.f/FPS;
        static constexpr int	WIN_WIDTH = 1200;
        static constexpr int	WIN_HEIGHT = 900;

        static constexpr float	BOX_SCALE = 16;
        b2WorldId boxWorld = b2_nullWorldId;
        int LeftTeamScore = 0;
        int RightTeamScore = 0;


        //Physical System Sizes:
        static constexpr float	BALL_RADIUS = 0.75;
        static constexpr float	FIELD_WIDTH = 75;
        static constexpr float	FIELD_HEIGHT = 50;
        static constexpr float	CAR_HEIGHT = 1.9;
        static constexpr float	CAR_RECTANGLE_WIDTH = 2.9;
        static constexpr float	CAR_WIDTH = CAR_RECTANGLE_WIDTH + CAR_HEIGHT/2;

                //goals:
        static constexpr float SIDE_BAR_WIDTH = FIELD_WIDTH * (1 / 24.f);
        static constexpr float BACK_BAR_HEIGHT = FIELD_HEIGHT * (2.3 / 16.f);
        static constexpr float BACK_BAR_POS = FIELD_WIDTH * (1.85 / 24.f);
        static constexpr float BAR_HALF_THICKNESS = 0.1;


            //Positions:
        SDL_FPoint ball_start_position = {FIELD_WIDTH/2, FIELD_HEIGHT/2};
        SDL_FPoint left_team_car_middle_start_position = {FIELD_WIDTH/4, FIELD_HEIGHT/2};
        SDL_FPoint left_team_car_upper_start_position = {FIELD_WIDTH/4, FIELD_HEIGHT/4};
        SDL_FPoint left_team_car_lower_start_position = {FIELD_WIDTH/4, FIELD_HEIGHT * 3/4};
        SDL_FPoint right_team_car_middle_start_position = {FIELD_WIDTH * 3/4, FIELD_HEIGHT/2};
        SDL_FPoint right_team_car_upper_start_position = {FIELD_WIDTH * 3/4, FIELD_HEIGHT/4};
        SDL_FPoint right_team_car_lower_start_position = {FIELD_WIDTH * 3/4, FIELD_HEIGHT * 3/4};





        //Renders:
            //tex in photo (location and size-pixel):
        static constexpr SDL_FRect BALL_TEX = {0, 0, 612, 612};
        static constexpr SDL_FRect BLUE_CAR_TEX = {0, 0, 1054, 512};
        static constexpr SDL_FRect ORANGE_CAR_TEX = {0, 1240, 1054, 512};
        static constexpr SDL_FRect FIELD_TEX = {0, 0, 681, 454};
        static constexpr SDL_FRect SCOUR_FRAME_TEX = {0, 0, 660, 403};

        SDL_Texture* ballTex;
        SDL_Texture* fieldTex;
        SDL_Texture* carsTex;
        SDL_Texture* scoreFrameTex;
        //with digit to draw
        SDL_Texture* digitTex;

        SDL_Renderer* ren;
        SDL_Window* win;


        //Tools:
        static constexpr float	RAD_TO_DEG = 57.2958f;
        static constexpr const char* senGoalLeftText = "BallEnterToLeftGoal";
        static constexpr const char* senGoalRightText = "BallEnterToLeftGoal";


    };
}