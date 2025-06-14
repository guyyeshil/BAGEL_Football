#pragma once
#include <SDL3/SDL.h>
#include <box2d/box2d.h>
#include "bagel.h"
#define LEFT true
#define RIGHT false

#ifndef FOOTBALL_H
#define FOOTBALL_H

#endif //FOOTBALL_H

namespace football {


    static constexpr uint64_t GAME_DURATION_MS = 2 * 60 * 1000 + 30 * 1000; //2:30 minutes


    using GameTimer = struct {
        uint64_t start_time;
        uint64_t game_duration_ms;
        bool is_running;
    };
    using TimerDigit = struct {};                                                       //tag to identify if digit
    using Transform = struct { SDL_FPoint position; float angle; };
    using Drawable = struct { SDL_FRect part; SDL_FPoint size; SDL_Texture* tex; };
    using Intent = struct { bool up, down, left, right; };
    using Keys = struct { SDL_Scancode up, down, left, right; };
    using Collider = struct { b2BodyId body; };
    using Ball = struct{};
    using GoalLeft = struct{};
    using GoalRight = struct{};
    using Car = struct{ bool side; };
    using StartingPosition = struct { SDL_FPoint position; float angle; };
    using Timer = struct {Uint64 start_time;  float time_remaining; bool paused;};
    using PowerUp = struct { bool bigger, faster, available; Timer time_out_timer; };
    using CarryPowerUp = struct { bool bigger, faster; Timer time_remaining_timer; };

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
        bagel::ent_type createCar(const SDL_FPoint& position, const SDL_FRect& tex, const Keys& keys, bool side, float size_scale) const;
        void createPowerUp(const SDL_FPoint& position, const SDL_FRect& tex, const PowerUp powerUp) const;
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
        void pick_power_up_system() const;
        void remove_power_up_system() const;
        void update_power_up_timer_system() const;
        void give_power_up(b2BodyId carBodyId, bagel::ent_type e, PowerUp powerUp) const;
        void change_car_size(b2BodyId carBodyId, bagel::ent_type e, float size_scale) const;
        void give_faster_power_up(b2BodyId carBodyId, bagel::ent_type e) const;
        void remove_faster_power_up(b2BodyId carBodyId, bagel::ent_type e) const;
        void enablePowerUp(PowerUp& powerUp, bagel::ent_type powerUpEntity)const;
        void disablePowerUp(PowerUp& powerUp, bagel::ent_type powerUpEntity)const;






        //Game Data:
        static constexpr int	FPS = 60;
        static constexpr float	GAME_FRAME = 1000.f/FPS;
        static constexpr int	WIN_WIDTH = 1200;
        static constexpr int	WIN_HEIGHT = 900;

        static constexpr float	BOX_SCALE = 16;
        b2WorldId boxWorld = b2_nullWorldId;
        int leftTeamScore = 0;
        int rightTeamScore = 0;


            //Physical System Sizes:
        static constexpr float	BALL_RADIUS = 0.75;
        static constexpr float	FIELD_WIDTH = 75;
        static constexpr float	FIELD_HEIGHT = 50;
        static constexpr float	CAR_HEIGHT = 1.9;
        static constexpr float	CAR_RECTANGLE_WIDTH = 2.9;
        static constexpr float	CAR_WIDTH = CAR_RECTANGLE_WIDTH + CAR_HEIGHT/2;
        static constexpr float	REGULAR_SIZE = 1.0;
        static constexpr float	BIGGER_SIZE = 1.3;
        static constexpr float	POWER_UP_CIRCLE_RADIUS = 1.0;



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
        SDL_FPoint speed_up_boost_position = {FIELD_WIDTH/2, FIELD_HEIGHT * 3/4};
        SDL_FPoint size_up_boost_position = {FIELD_WIDTH/2, FIELD_HEIGHT/4};





        //Renders:
            //tex in photo (location and size-pixel):
        static constexpr SDL_FRect BALL_TEX = {0, 0, 612, 612};
        static constexpr SDL_FRect BLUE_CAR_TEX = {0, 0, 1054, 512};
        static constexpr SDL_FRect ORANGE_CAR_TEX = {0, 1240, 1054, 512};
        static constexpr SDL_FRect FIELD_TEX = {0, 0, 681, 454};
        static constexpr SDL_FRect SCOUR_FRAME_TEX = {0, 0, 660, 403};
        static constexpr SDL_FRect SPEED_UP_TEX = {0, 0, 1020, 1020};
        static constexpr SDL_FRect SIZE_UP_TEX = {1040, 0, 1020, 1020};
            //textures of digits
        static constexpr SDL_FRect DIGIT_TEX_0 = {0, 0, 232, 417};
        static constexpr SDL_FRect DIGIT_TEX_1 = {232, 0, 232, 417};
        static constexpr SDL_FRect DIGIT_TEX_2 = {464, 0, 231, 417};
        static constexpr SDL_FRect DIGIT_TEX_3 = {695, 0, 232, 417};
        static constexpr SDL_FRect DIGIT_TEX_4 = {927, 0, 232, 417};
        static constexpr SDL_FRect DIGIT_TEX_5 = {0, 417, 231, 417};
        static constexpr SDL_FRect DIGIT_TEX_6 = {232, 417, 232, 417};
        static constexpr SDL_FRect DIGIT_TEX_7 = {464, 417, 232, 417};
        static constexpr SDL_FRect DIGIT_TEX_8 = {695, 417, 231, 417};
        static constexpr SDL_FRect DIGIT_TEX_9 = {927, 417, 232, 417};
        // static constexpr SDL_FRect COLON_TEX = {320, 0, 16, 417};

        SDL_Texture* ballTex;
        SDL_Texture* fieldTex;
        SDL_Texture* carsTex;
        SDL_Texture* scoreFrameTex;
        //with digit to draw
        SDL_Texture* digitTex;
        SDL_Texture* powerUpsTex;


        SDL_Renderer* ren;
        SDL_Window* win;


        //Tools:
        static constexpr float	RAD_TO_DEG = 57.2958f;

        static constexpr float	POWER_UP_TIME_OUT_TIMER = 10000.0f;
        static constexpr float	POWER_UP_TIMER = 5000.0f;

    };
}