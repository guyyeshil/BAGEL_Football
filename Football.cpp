#include "Football.h"
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <box2d/box2d.h>
#include "lib/box2d/src/body.h"
#include "bagel.h"
using namespace bagel;
using namespace std;


namespace football {

    void Football::prepareBoxWorld()
    {
        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = {0,0};
        boxWorld = b2CreateWorld(&worldDef);

        createField();
        createBall();
        createCar(left_team_car_middle_start_position,BLUE_CAR_TEX,{SDL_SCANCODE_W, SDL_SCANCODE_S,SDL_SCANCODE_A, SDL_SCANCODE_D},LEFT,REGULAR_SIZE);
        createCar(right_team_car_middle_start_position,ORANGE_CAR_TEX,{SDL_SCANCODE_UP, SDL_SCANCODE_DOWN,SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT},RIGHT,REGULAR_SIZE);
        createDataBar();
        createPowerUp(size_up_boost_position,SIZE_UP_TEX,{true,false,true,{SDL_GetTicks(),0,false}});
        createPowerUp(speed_up_boost_position,SPEED_UP_TEX,{false,true,true,{SDL_GetTicks(),0,false}});

        createGameTimer(); // ADD THIS LINE - Initialize timer

        if (DEBUG_MODE) {
            applyDebugFunctions();
        }
    }

    bool Football::prepareWindowAndTexture()
    {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            cout << SDL_GetError() << endl;
            return false;
        }

        if (!SDL_CreateWindowAndRenderer(
                "B.A.G.E.L FOOTBALL", WIN_WIDTH, WIN_HEIGHT, 0, &win, &ren)) {
            cout << SDL_GetError() << endl;
            return false;
        }

        SDL_Surface *surf = IMG_Load("res/ball.png");
        if (surf == nullptr) {
            cout << SDL_GetError() << endl;
            return false;
        }

        ballTex = SDL_CreateTextureFromSurface(ren, surf);
        if (ballTex == nullptr) {
            cout << SDL_GetError() << endl;
            return false;
        }

        surf = IMG_Load("res/field.png");
        if (surf == nullptr) {
            cout << SDL_GetError() << endl;
            return false;
        }

        fieldTex = SDL_CreateTextureFromSurface(ren, surf);
        if (fieldTex == nullptr) {
            cout << SDL_GetError() << endl;
            return false;
        }

        surf = IMG_Load("res/cars.png");
        if (surf == nullptr) {
            cout << SDL_GetError() << endl;
            return false;
        }

        carsTex = SDL_CreateTextureFromSurface(ren, surf);
        if (carsTex == nullptr) {
            cout << SDL_GetError() << endl;
            return false;
        }

        surf = IMG_Load("res/scoreFrame.png");
        if (surf == nullptr) {
            cout << SDL_GetError() << endl;
            return false;
        }

        scoreFrameTex = SDL_CreateTextureFromSurface(ren, surf);
        if (scoreFrameTex == nullptr) {
            cout << SDL_GetError() << endl;
            return false;
        }
        //timer
        surf = IMG_Load("res/timeSprite.png");
        if (surf == nullptr) {
            cout << SDL_GetError() << endl;
            return false;
        }
        digitTex = SDL_CreateTextureFromSurface(ren, surf);
        if (digitTex == nullptr) {
            cout << SDL_GetError() << endl;
            return false;
        }

        surf = IMG_Load("res/powerUps.png");
        if (surf == nullptr) {
            cout << SDL_GetError() << endl;
            return false;
        }

        powerUpsTex = SDL_CreateTextureFromSurface(ren, surf);
        if (powerUpsTex == nullptr) {
            cout << SDL_GetError() << endl;
            return false;
        }


        SDL_DestroySurface(surf);
        return true;
    }

    void Football::gameSetupPrams() {
        LeftTeamScore = 0;
        RightTeamScore = 0;
    }

    Football::Football() {
        if (!prepareWindowAndTexture())
            return;
        SDL_srand(time(nullptr));
        gameSetupPrams();
        prepareBoxWorld();
    }

    Football::~Football() {
        if (b2World_IsValid(boxWorld))
            b2DestroyWorld(boxWorld);
        if (ballTex != nullptr)
            SDL_DestroyTexture(ballTex);
        if (carsTex != nullptr)
            SDL_DestroyTexture(carsTex);
        if (fieldTex != nullptr)
            SDL_DestroyTexture(fieldTex);
        if (scoreFrameTex != nullptr)
            SDL_DestroyTexture(scoreFrameTex);
        if (powerUpsTex != nullptr)
            SDL_DestroyTexture(powerUpsTex);
        if (digitTex != nullptr)
            SDL_DestroyTexture(digitTex);
        if (ren != nullptr)
            SDL_DestroyRenderer(ren);
        if (win != nullptr)
            SDL_DestroyWindow(win);

        SDL_Quit();
    }

    void Football::createBall() const {
        b2BodyDef ballBodyDef = b2DefaultBodyDef();
        ballBodyDef.type = b2_dynamicBody;
        ballBodyDef.fixedRotation = false;
        ballBodyDef.position = {ball_start_position.x, ball_start_position.y};

        b2ShapeDef ballShapeDef = b2DefaultShapeDef();
        ballShapeDef.enableSensorEvents = true;
        ballShapeDef.density = 0.5;
        ballShapeDef.material.friction = 0.5;//todo
        ballShapeDef.material.restitution = 0.8f;
        b2Circle ballCircle = {{0, 0}, BALL_RADIUS};

        b2BodyId ballBody = b2CreateBody(boxWorld, &ballBodyDef);
        b2Body_SetLinearDamping(ballBody, 1.5f);//todo
        b2Body_SetAngularDamping(ballBody, 1.5f); // todo
        b2CreateCircleShape(ballBody, &ballShapeDef, &ballCircle);

        Entity ballEntity = Entity::create();
        ballEntity.addAll(
            Transform{{ball_start_position},0},
            Drawable{{BALL_TEX}, {BALL_RADIUS * 2, BALL_RADIUS * 2}, ballTex},
            Collider{ballBody},
            Ball{},
            StartingPosition{{ball_start_position},0}
        );
        b2Body_SetUserData(ballBody, new ent_type{ballEntity.entity()});
    }

    ent_type Football::createCar(const SDL_FPoint& position, const SDL_FRect& tex, const Keys& keys, bool side, float size_scale) const
    {
        b2BodyDef carBodyDef = b2DefaultBodyDef();
        carBodyDef.type = b2_dynamicBody;
        carBodyDef.fixedRotation = true;
        carBodyDef.position = {position.x, position.y};

        b2BodyId carBody = b2CreateBody(boxWorld, &carBodyDef);
        b2Body_SetLinearDamping(carBody, 1.5f);//todo

        b2ShapeDef carShapeDef = b2DefaultShapeDef();
        carShapeDef.density = 1;
        carShapeDef.material.friction = 0.5;//todo
        carShapeDef.material.restitution = 0.9f;
        carShapeDef.enableSensorEvents = true;

        if (side == LEFT) {
            b2Polygon carRectangle = b2MakeOffsetBox(
                        CAR_RECTANGLE_WIDTH * size_scale / 2.0f, // half-width
                        CAR_HEIGHT * size_scale / 2.0f,          // half-height
                        (b2Vec2){-(CAR_WIDTH-CAR_RECTANGLE_WIDTH) * size_scale / 2, 0.0f},
                        b2MakeRot(0.0f)
                    );
            b2CreatePolygonShape(carBody, &carShapeDef, &carRectangle);

            b2Circle carCircle = {{CAR_RECTANGLE_WIDTH * size_scale/2 - (CAR_WIDTH-CAR_RECTANGLE_WIDTH) * size_scale/2,0},CAR_HEIGHT * size_scale/2};
            b2CreateCircleShape(carBody, &carShapeDef, &carCircle);
        } else {
            b2Polygon carRectangle = b2MakeOffsetBox(
                        CAR_RECTANGLE_WIDTH * size_scale / 2.0f, // half-width
                        CAR_HEIGHT * size_scale / 2.0f,          // half-height
                        (b2Vec2){(CAR_WIDTH-CAR_RECTANGLE_WIDTH) * size_scale/2, 0.0f},
                        b2MakeRot(0.0f)
                    );
            b2CreatePolygonShape(carBody, &carShapeDef, &carRectangle);

            b2Circle carCircle = {{-CAR_RECTANGLE_WIDTH * size_scale/2 + (CAR_WIDTH-CAR_RECTANGLE_WIDTH) * size_scale/2,0},CAR_HEIGHT * size_scale/2};
            b2CreateCircleShape(carBody, &carShapeDef, &carCircle);
        }


        Entity carEntity = Entity::create();
        carEntity.addAll(
            Transform{{position},0},
            Intent{},
            Keys{keys},
            Drawable{{tex}, {CAR_WIDTH * size_scale, CAR_HEIGHT * size_scale}, carsTex},
            Collider{carBody},
            Car{side},
            StartingPosition{{position},0}
        );

        b2Body_SetUserData(carBody, new ent_type{carEntity.entity()});
        return carEntity.entity();
    }

    void Football::createPowerUp(const SDL_FPoint &position, const SDL_FRect &tex, const PowerUp powerUp) const
    {

        b2BodyDef powerUpBodyDef = b2DefaultBodyDef();
        powerUpBodyDef.type = b2_staticBody;
        powerUpBodyDef.position = {position.x, position.y};

        b2BodyId powerUpBody = b2CreateBody(boxWorld, &powerUpBodyDef);

        b2ShapeDef powerUpShapeDef = b2DefaultShapeDef();
        powerUpShapeDef.isSensor = true;
        powerUpShapeDef.enableSensorEvents = true;
        powerUpShapeDef.userData = nullptr;

        b2Circle powerUpCircle = {{0,0},POWER_UP_CIRCLE_RADIUS};
        b2CreateCircleShape(powerUpBody, &powerUpShapeDef, &powerUpCircle);

        Entity powerUpEntity = Entity::create();
        powerUpEntity.addAll(
            Transform{{position}, 0.0f},
            Drawable{{tex}, {POWER_UP_CIRCLE_RADIUS * 2, POWER_UP_CIRCLE_RADIUS * 2}, powerUpsTex},
            PowerUp{powerUp}
        );

        b2Body_SetUserData(powerUpBody, new ent_type{powerUpEntity.entity()});

    }


    void Football::createField() const {
        SDL_FRect FieldPosition = {FIELD_WIDTH / 2, FIELD_HEIGHT / 2, 0, 0};

        Entity::create().addAll(
                Transform{{FieldPosition.x, FieldPosition.y}, 0},
                Drawable{FIELD_TEX, {FIELD_WIDTH, FIELD_HEIGHT}, fieldTex}
        );

        createFieldBorders();
        createLeftGoalBars();
        createRightGoalBars();
    }

    void Football::createFieldBorders() const {
        b2ShapeDef borderShapeDef = b2DefaultShapeDef();
        borderShapeDef.density = 1;
        borderShapeDef.material.friction = 0.6f;
        borderShapeDef.material.restitution = 0.1f;

        b2Polygon widthBorder = b2MakeBox(FIELD_WIDTH, BAR_HALF_THICKNESS);
        b2Polygon HeightBorder = b2MakeBox(BAR_HALF_THICKNESS, FIELD_HEIGHT);

        b2BodyDef upBorderBodyDef = b2DefaultBodyDef();
        upBorderBodyDef.type = b2_staticBody;
        upBorderBodyDef.position = {FIELD_WIDTH / 2, FIELD_HEIGHT + 0.1f};
        b2BodyId upBorderBody = b2CreateBody(boxWorld, &upBorderBodyDef);
        b2CreatePolygonShape(upBorderBody, &borderShapeDef, &widthBorder);

        b2BodyDef downBorderBodyDef = b2DefaultBodyDef();
        downBorderBodyDef.type = b2_staticBody;
        downBorderBodyDef.position = {FIELD_WIDTH / 2, -0.1f};
        b2BodyId downBorderBody = b2CreateBody(boxWorld, &downBorderBodyDef);
        b2CreatePolygonShape(downBorderBody, &borderShapeDef, &widthBorder);

        b2BodyDef rightBorderBodyDef = b2DefaultBodyDef();
        rightBorderBodyDef.type = b2_staticBody;
        rightBorderBodyDef.position = {FIELD_WIDTH + 0.1f, FIELD_HEIGHT / 2};
        b2BodyId rightBorderBody = b2CreateBody(boxWorld, &rightBorderBodyDef);
        b2CreatePolygonShape(rightBorderBody, &borderShapeDef, &HeightBorder);

        b2BodyDef leftBorderBodyDef = b2DefaultBodyDef();
        leftBorderBodyDef.type = b2_staticBody;
        leftBorderBodyDef.position = {-0.1f, FIELD_HEIGHT / 2};
        b2BodyId leftBorderBody = b2CreateBody(boxWorld, &leftBorderBodyDef);
        b2CreatePolygonShape(leftBorderBody, &borderShapeDef, &HeightBorder);
    }

    void Football::createLeftGoalBars() const {
        b2ShapeDef barShapeDef = b2DefaultShapeDef();
        barShapeDef.density = 1;
        barShapeDef.material.friction = 0.6f;
        barShapeDef.material.restitution = 0.1f;

        b2Polygon leftBar = b2MakeBox(SIDE_BAR_WIDTH / 2, BAR_HALF_THICKNESS);
        b2Polygon rightBar = b2MakeBox(SIDE_BAR_WIDTH / 2, BAR_HALF_THICKNESS);
        b2Polygon backBar = b2MakeBox(BAR_HALF_THICKNESS, BACK_BAR_HEIGHT / 2);

        b2BodyDef backBarBodyDef = b2DefaultBodyDef();
        backBarBodyDef.type = b2_staticBody;
        backBarBodyDef.position = {BACK_BAR_POS - BAR_HALF_THICKNESS, FIELD_HEIGHT / 2};
        b2BodyId backBarBody = b2CreateBody(boxWorld, &backBarBodyDef);
        b2CreatePolygonShape(backBarBody, &barShapeDef, &backBar);

        b2BodyDef leftBarBodyDef = b2DefaultBodyDef();
        leftBarBodyDef.type = b2_staticBody;
        leftBarBodyDef.position = {BACK_BAR_POS + (SIDE_BAR_WIDTH / 2) + BAR_HALF_THICKNESS,
                                   FIELD_HEIGHT / 2 - BACK_BAR_HEIGHT / 2 - BAR_HALF_THICKNESS};
        b2BodyId leftBarBody = b2CreateBody(boxWorld, &leftBarBodyDef);
        b2CreatePolygonShape(leftBarBody, &barShapeDef, &leftBar);

        b2BodyDef rightBarBodyDef = b2DefaultBodyDef();
        rightBarBodyDef.type = b2_staticBody;
        rightBarBodyDef.position = {BACK_BAR_POS + (SIDE_BAR_WIDTH / 2) + BAR_HALF_THICKNESS,
                                    FIELD_HEIGHT / 2 + BACK_BAR_HEIGHT / 2 + BAR_HALF_THICKNESS};
        b2BodyId rightBarBody = b2CreateBody(boxWorld, &rightBarBodyDef);
        b2CreatePolygonShape(rightBarBody, &barShapeDef, &rightBar);

        createGoalSensor(true);
    }

    void Football::createRightGoalBars() const {
        b2ShapeDef barShapeDef = b2DefaultShapeDef();
        barShapeDef.density = 1;
        barShapeDef.material.friction = 0.6f;
        barShapeDef.material.restitution = 0.1f;

        b2Polygon leftBar = b2MakeBox(SIDE_BAR_WIDTH / 2, BAR_HALF_THICKNESS);
        b2Polygon rightBar = b2MakeBox(SIDE_BAR_WIDTH / 2, BAR_HALF_THICKNESS);
        b2Polygon backBar = b2MakeBox(BAR_HALF_THICKNESS, BACK_BAR_HEIGHT / 2);

        b2BodyDef backBarBodyDef = b2DefaultBodyDef();
        backBarBodyDef.type = b2_staticBody;
        backBarBodyDef.position = {FIELD_WIDTH - BACK_BAR_POS + BAR_HALF_THICKNESS, FIELD_HEIGHT / 2};
        b2BodyId backBarBody = b2CreateBody(boxWorld, &backBarBodyDef);
        b2CreatePolygonShape(backBarBody, &barShapeDef, &backBar);

        b2BodyDef leftBarBodyDef = b2DefaultBodyDef();
        leftBarBodyDef.type = b2_staticBody;
        leftBarBodyDef.position = {FIELD_WIDTH - BACK_BAR_POS - BAR_HALF_THICKNESS - (SIDE_BAR_WIDTH / 2),
                                   FIELD_HEIGHT / 2 - BAR_HALF_THICKNESS - BACK_BAR_HEIGHT / 2};
        b2BodyId leftBarBody = b2CreateBody(boxWorld, &leftBarBodyDef);
        b2CreatePolygonShape(leftBarBody, &barShapeDef, &leftBar);

        b2BodyDef rightBarBodyDef = b2DefaultBodyDef();
        rightBarBodyDef.type = b2_staticBody;
        rightBarBodyDef.position = {FIELD_WIDTH - BACK_BAR_POS - BAR_HALF_THICKNESS - (SIDE_BAR_WIDTH / 2),
                                    FIELD_HEIGHT / 2 + BAR_HALF_THICKNESS + BACK_BAR_HEIGHT / 2};
        b2BodyId rightBarBody = b2CreateBody(boxWorld, &rightBarBodyDef);
        b2CreatePolygonShape(rightBarBody, &barShapeDef, &rightBar);

        createGoalSensor(false);
    }

    void Football::createGoalSensor(const bool isLeftGoal) const {
        b2BodyDef sensorDef = b2DefaultBodyDef();
        sensorDef.type = b2_staticBody;

        const float xPos = isLeftGoal ?
                           (BACK_BAR_POS + BAR_HALF_THICKNESS * 2) :
                           (FIELD_WIDTH - BACK_BAR_POS - BAR_HALF_THICKNESS * 2);

        const float yPos = FIELD_HEIGHT / 2;
        sensorDef.position = {xPos, yPos};

        b2BodyId sensorBody = b2CreateBody(boxWorld, &sensorDef);

        b2Polygon sensorShape = b2MakeBox(
                0.05f,
                (BACK_BAR_HEIGHT / 2)
        );

        b2ShapeDef sensorShapeDef = b2DefaultShapeDef();
        sensorShapeDef.isSensor = true;
        sensorShapeDef.enableSensorEvents = true;

        b2Filter filter;
        filter.categoryBits = 0x0002;
        filter.maskBits = 0x0001;
        sensorShapeDef.filter = filter;

        b2ShapeId sensorShapeId = b2CreatePolygonShape(
                sensorBody,
                &sensorShapeDef,
                &sensorShape
        );

        if (isLeftGoal)
            b2Shape_SetUserData(sensorShapeId, (void *) senGoalLeftText);
        else
            b2Shape_SetUserData(sensorShapeId, (void *) senGoalRightText);
    }

    void Football::applyDebugFunctions() const {
        createDebugBox();
    }

    void Football::renderDebugFunctions() const
    {
        drawSensorDebug((BACK_BAR_POS + BAR_HALF_THICKNESS * 2), FIELD_HEIGHT / 2, 0.05f, (BACK_BAR_HEIGHT / 2));
    }

    void Football::createDebugBox() const
    {
        const float width = 2.0f;
        const float height = 2.0f;
        const SDL_FPoint position = {0.0f, 0.0f};
        const SDL_Color color = {255, 0, 0, 255};

        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = {position.x, position.y};
        bodyDef.fixedRotation = true;

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 3.0f;
        shapeDef.material.friction = 0.3f;
        shapeDef.material.restitution = 0.1f;

        b2Polygon boxShape = b2MakeBox(width / 2.0f, height / 2.0f);

        b2BodyId body = b2CreateBody(boxWorld, &bodyDef);
        b2CreatePolygonShape(body, &shapeDef, &boxShape);

        const int pixelW = static_cast<int>(width * BOX_SCALE);
        const int pixelH = static_cast<int>(height * BOX_SCALE);

        SDL_Texture* colorTex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, pixelW, pixelH);
        SDL_SetTextureBlendMode(colorTex, SDL_BLENDMODE_BLEND);

        SDL_SetRenderTarget(ren, colorTex);
        SDL_SetRenderDrawColor(ren, color.r, color.g, color.b, color.a);
        SDL_RenderClear(ren);
        SDL_SetRenderTarget(ren, nullptr);

        Entity debugBox = Entity::create();
        debugBox.addAll(
                Transform{position, 0.0f},
                Drawable{
                        {0, 0, static_cast<float>(pixelW), static_cast<float>(pixelH)},
                        {width, height},
                        colorTex
                },
                Collider{body},
                Keys{SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT},
                Intent{}
        );
    }

    void Football::drawSensorDebug(const float xPos, const float yPos, const float width, const float height) const
    {
        SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(ren, 255, 0, 0, 100); // אדום שקוף

        SDL_FRect sensorRect = {
                xPos * BOX_SCALE - width * BOX_SCALE / 2,
                yPos * BOX_SCALE - height * BOX_SCALE / 2,
                width * BOX_SCALE,
                height * BOX_SCALE
        };
        cout << "pixel loc (" << sensorRect.x <<"," << sensorRect.y <<")" << endl;
        SDL_RenderFillRect(ren, &sensorRect);
    }


    void Football::consolePrintDebugData() const {
        cout << "Score Game " << LeftTeamScore << ":" << RightTeamScore << endl;
    }

    void Football::createDataBar() const
    {
        createScoreFrame();
    }

    void Football::createScoreFrame() const
    {
        SDL_FRect scoreFramePosition = {(WIN_WIDTH/BOX_SCALE)/2.0f,FIELD_HEIGHT + ((WIN_HEIGHT/BOX_SCALE)-FIELD_HEIGHT)/2.0f,0,0};

        Entity::create().addAll(
                Transform{{scoreFramePosition.x,scoreFramePosition.y},0},
                Drawable{SCOUR_FRAME_TEX, {((WIN_HEIGHT/BOX_SCALE)-FIELD_HEIGHT-2)*(5/3.f), ((WIN_HEIGHT/BOX_SCALE)-FIELD_HEIGHT-2)}, scoreFrameTex}
        );
    }

    SDL_FRect Football::getDigitTexture(int digit) const
    {
        switch(digit) {
            case 0: return DIGIT_TEX_0;
            case 1: return DIGIT_TEX_1;
            case 2: return DIGIT_TEX_2;
            case 3: return DIGIT_TEX_3;
            case 4: return DIGIT_TEX_4;
            case 5: return DIGIT_TEX_5;
            case 6: return DIGIT_TEX_6;
            case 7: return DIGIT_TEX_7;
            case 8: return DIGIT_TEX_8;
            case 9: return DIGIT_TEX_9;
            default: return DIGIT_TEX_0;
        }
    }
    //todo
    //---------------------------------------------
    void Football::createGameTimer() const
    {
        // Create timer entity
        Entity timerEntity = Entity::create();
        timerEntity.add(GameTimer{SDL_GetTicks(), GAME_DURATION_MS, true});

        // Position timer to the right of the scoreboard
        float scoreboard_center_x = (WIN_WIDTH/BOX_SCALE)/2.0f;
        float scoreboard_width = ((WIN_HEIGHT/BOX_SCALE)-FIELD_HEIGHT-2)*(5/3.f);
        float timer_start_x = scoreboard_center_x + (scoreboard_width/2.0f) + 3.0f; // 3 units to the right
        float timer_y = FIELD_HEIGHT + ((WIN_HEIGHT/BOX_SCALE)-FIELD_HEIGHT)/2.0f;
        float digit_width = 4.5f;  // Smaller digits
        float digit_height = 5.5f;

        // Minutes digit
        Entity minutesEntity = Entity::create();
        minutesEntity.addAll(
                Transform{{timer_start_x, timer_y}, 0},
                Drawable{DIGIT_TEX_2, {digit_width, digit_height}, digitTex},
                TimerDigit{}  // Tag this as a timer digit
        );

        // // Colon
        // Entity colonEntity = Entity::create();
        // colonEntity.addAll(
        //     Transform{{timer_start_x + digit_width + 0.2f, timer_y}, 0},
        //     Drawable{COLON_TEX, {digit_width * 0.5f, digit_height}, digitTex}
        // );
        //
        // Seconds tens digit
        Entity secondsTensEntity = Entity::create();
        secondsTensEntity.addAll(
                Transform{{timer_start_x + digit_width * 1.7f, timer_y}, 0},
                Drawable{DIGIT_TEX_3, {digit_width, digit_height}, digitTex},
                TimerDigit{}  // Tag this as a timer digit
        );

        // Seconds ones digit
        Entity secondsOnesEntity = Entity::create();
        secondsOnesEntity.addAll(
                Transform{{timer_start_x + digit_width * 2.7f, timer_y}, 0},
                Drawable{DIGIT_TEX_0, {digit_width, digit_height}, digitTex},
                TimerDigit{}  // Tag this as a timer digit
        );
    }

    void Football::timer_system()
    {
        static const Mask timer_mask = MaskBuilder()
                .set<GameTimer>()
                .build();

        static const Mask digit_mask = MaskBuilder()
                .set<Transform>()
                .set<Drawable>()
                .set<TimerDigit>()  // Only entities with TimerDigit tag
                .build();

        // Find timer entity
        for (ent_type e{0}; e.id <= World::maxId().id; ++e.id) {
            if (World::mask(e).test(timer_mask)) {
                auto& gameTimer = World::getComponent<GameTimer>(e);

                if (gameTimer.is_running) {
                    Uint64 current_time = SDL_GetTicks();
                    Uint64 elapsed_time = current_time - gameTimer.start_time;
                    Uint64 remaining_time = 0;

                    if (elapsed_time < gameTimer.game_duration_ms) {
                        remaining_time = gameTimer.game_duration_ms - elapsed_time;
                    } else {
                        // Game time is up
                        remaining_time = 0;
                        gameTimer.is_running = false;
                        cout << "Game time finished!" << endl;
                    }

                    // Convert remaining time to minutes and seconds
                    Uint64 total_seconds = remaining_time / 1000;
                    int minutes = total_seconds / 60;
                    int seconds = total_seconds % 60;
                    int seconds_tens = seconds / 10;
                    int seconds_ones = seconds % 10;

                    // Update timer digit entities
                    int digit_index = 0;
                    for (ent_type digit_e{0}; digit_e.id <= World::maxId().id; ++digit_e.id) {
                        if (World::mask(digit_e).test(digit_mask)) {
                            auto& drawable = World::getComponent<Drawable>(digit_e);

                            switch(digit_index) {
                                case 0: // Minutes
                                    drawable.part = getDigitTexture(minutes);
                                    break;
                                case 1: // Seconds tens
                                    drawable.part = getDigitTexture(seconds_tens);
                                    break;
                                case 2: // Seconds ones
                                    drawable.part = getDigitTexture(seconds_ones);
                                    break;
                            }
                            digit_index++;
                        }
                    }
                }
                break; // Only one timer entity expected
            }
        }

        SDL_RenderPresent(ren);
    }

    class InputSystem
    {
    public:
        void update() {
            for (int i = 0; i < _entities.size(); ++i) {
                ent_type e = _entities[i];
                if (!World::mask(e).test(mask)) {
                    _entities[i] = _entities[_entities.size()-1];
                    _entities.pop();
                    --i;
                    continue;
                }
            }
        }
        void updateEntities() {
            for (int i = 0; i < World::sizeAdded(); ++i) {
                const AddedMask& am = World::getAdded(i);

                if ((!am.prev.test(mask)) && (am.next.test(mask))) {
                    _entities.push(am.e);
                }
            }
        }

        InputSystem()
        {
            for (ent_type e{0}; e.id <= World::maxId().id; ++e.id) {
                if (World::mask(e).test(mask)) {
                    _entities.push(e);
                }
            }
        }
    private:
        Bag<ent_type,100> _entities;

        static const inline Mask mask = MaskBuilder()
                .set<Keys>()
                .set<Intent>()
                .build();
    };

    void Football::input_system()
    {
        static const Mask mask = MaskBuilder()
            .set<Keys>()
            .set<Intent>()
            .build();

        SDL_PumpEvents();
        const bool* keys = SDL_GetKeyboardState(nullptr);

        for (ent_type e{0}; e.id <= World::maxId().id; ++e.id) {
            if (World::mask(e).test(mask)) {
                const auto& ent_keys = World::getComponent<Keys>(e);
                auto& intent = World::getComponent<Intent>(e);

                intent.up = keys[ent_keys.up];
                intent.down = keys[ent_keys.down];
                intent.left = keys[ent_keys.left];
                intent.right = keys[ent_keys.right];
            }
        }
    }

    void Football::move_system()
    {
        static const Mask mask = MaskBuilder()
            .set<Intent>()
            .set<Collider>()
            .set<Transform>()
            .build();

        const float forward_force = 300.0f;
        const float backward_force = 150.0f;
        const float turn_speed = 15.0f;
        const float max_speed = 15.0f;
        const float turn_damping = 0.95f;
        b2Vec2 velocity;
        float current_speed;
        float steering_input;
        float angle_change;
        float angle_rad;
        float effective_turn_speed;
        bool is_accelerating;
        bool is_steering;
        const float min_steering_speed = 0.5f; //minimum speed needed for steering
        float forward_x;
        float forward_y;
        bool moving_forward;
        float car_angle;
        float velocity_angle;

        b2Vec2 force;
        for (ent_type e{0}; e.id <= World::maxId().id; ++e.id) {
            if (World::mask(e).test(mask)) {
                const auto& intent = World::getComponent<Intent>(e);
                const auto& collider = World::getComponent<Collider>(e);
                auto& transform = World::getComponent<Transform>(e);
                is_accelerating = (intent.up && !intent.down) || (intent.down && !intent.up);
                is_steering = intent.left ^ intent.right;

                velocity = b2Body_GetLinearVelocity(collider.body);
                current_speed = b2Length(velocity) / 10;
                 if (is_steering && (is_accelerating || current_speed > min_steering_speed)) {
                    if (current_speed > 0.1f) { //cant steer too slow
                        car_angle = transform.angle;
                        velocity_angle = std::atan2(velocity.y, velocity.x) * RAD_TO_DEG;
                        if (velocity_angle < 0) velocity_angle += 360.0f;
                        float angle_diff = std::abs(velocity_angle - car_angle);
                    if (angle_diff > 180.0f) angle_diff = 360.0f - angle_diff;
                    //if the difference between angle car pointing to velocity <= 90 -> moving forward
                    moving_forward = (angle_diff <= 90.0f);
                }
                if ((moving_forward && intent.right) || (!moving_forward && intent.left)) {
                    steering_input = 1;
                }

                else {
                     steering_input = -1;
                 }

                current_speed = current_speed / 10.0f;
                effective_turn_speed = turn_speed * current_speed;
                angle_change = steering_input * effective_turn_speed;
                transform.angle += angle_change;
                if (transform.angle > 360.0f) transform.angle -= 360.0f;
                if (transform.angle < 0.0f) transform.angle += 360.0f;
                //update physics body rotation
                angle_rad = transform.angle / RAD_TO_DEG;
                b2Body_SetTransform(
                    collider.body,
                    b2Body_GetPosition(collider.body),
                    b2MakeRot(angle_rad)
                );
            }

                if (is_accelerating) {
                    force = {0.0f, 0.0f};
                    angle_rad = transform.angle / RAD_TO_DEG;
                    forward_x = std::cos(angle_rad);
                    forward_y = std::sin(angle_rad);
                    if (intent.up) {
                        force.x = forward_x * forward_force;
                        force.y = forward_y * forward_force;
                    } else {
                        force.x = forward_x * backward_force * -1.0f;
                        force.y = forward_y * backward_force* -1.0f;
                    }
                    b2Body_ApplyForceToCenter(collider.body, force, true);
                }

            }
        }
    }


    void Football::physic_system() const
    {
        static const Mask mask = MaskBuilder()
            .set<Collider>()
            .set<Transform>()
            .build();
        static constexpr float	BOX2D_STEP = 1.f/FPS;

        b2World_Step(boxWorld, BOX2D_STEP, 4);

        for (ent_type e{0}; e.id <= World::maxId().id; ++e.id) {
            if (World::mask(e).test(mask)) {
                b2Transform transform = b2Body_GetTransform(World::getComponent<Collider>(e).body);
                World::getComponent<Transform>(e) = {
                    {transform.p.x, transform.p.y},
                    RAD_TO_DEG * b2Rot_GetAngle(transform.q)
                };
            }
        }
    }

    void Football::score_system()
    {
        const auto sensorE = b2World_GetSensorEvents(boxWorld);
        for (int i = 0; i < sensorE.endCount; ++i) {
            b2ShapeId sensorShape = sensorE.endEvents[i].visitorShapeId;
            b2BodyId sensorBody = b2Shape_GetBody(sensorShape);
            const char* goalType = static_cast<const char*>(b2Body_GetUserData(sensorBody));
            if (goalType)
            {
                if (strcmp(goalType, senGoalLeftText) == 0)
                    RightTeamScore++;

                else if (strcmp(goalType, senGoalRightText) == 0)
                    LeftTeamScore++;
            }
            //reset_location_system();
        }
    }

    void Football::draw_system() const
    {
        static const Mask mask = MaskBuilder()
                .set<Transform>()
                .set<Drawable>()
                .build();

        SDL_RenderClear(ren);

        for (ent_type e{0}; e.id <= World::maxId().id; ++e.id) {
            if (World::mask(e).test(mask)) {
                const auto& draw = World::getComponent<Drawable>(e);
                const auto& transform = World::getComponent<Transform>(e);

                const SDL_FRect dst = {
                        (transform.position.x - draw.size.x/2) * BOX_SCALE,
                        (transform.position.y - draw.size.y/2) * BOX_SCALE,
                        draw.size.x * BOX_SCALE, draw.size.y * BOX_SCALE};

                SDL_RenderTextureRotated(
                        ren, draw.tex, &draw.part, &dst, transform.angle,
                        nullptr, SDL_FLIP_NONE);
            }
        }

        if(DEBUG_MODE)
        {
            renderDebugFunctions();
            consolePrintDebugData();
        }
        SDL_RenderPresent(ren);
    }

    void Football::reset_location_system() const
    {
        static const Mask mask = MaskBuilder()
            .set<StartingPosition>()
            .set<Transform>()
            .set<Collider>()
            .build();

        for (ent_type e{0}; e.id <= World::maxId().id; ++e.id) {
            if (World::mask(e).test(mask)) {

                auto& transform = World::getComponent<Transform>(e);
                const auto& startPos = World::getComponent<StartingPosition>(e);
                auto& body = World::getComponent<Collider>(e).body;

                transform.position = startPos.position;
                transform.angle = startPos.angle;
                b2Body_SetTransform(body,{startPos.position.x,startPos.position.y},b2Body_GetRotation(body));
            }
        }
    }

    void Football::pick_power_up_system() const
    {
        const auto se = b2World_GetSensorEvents(boxWorld);
        static const Mask powerUpMask = MaskBuilder().set<PowerUp>().build();
        static const Mask carMask = MaskBuilder().set<Car>().build();

        if (se.endCount>0)
            cout<< se.endCount<<endl;
        for (int i = 0; i < se.endCount; ++i) {
            b2BodyId powerUpBodyId = b2Shape_GetBody(se.beginEvents[i].sensorShapeId);
            b2BodyId carBodyId = b2Shape_GetBody(se.beginEvents[i].visitorShapeId);
            auto *powerUpEntity = static_cast<ent_type*>(b2Body_GetUserData(powerUpBodyId));
            auto *carEntity = static_cast<ent_type*>(b2Body_GetUserData(carBodyId));

            if (World::mask(*powerUpEntity).test(powerUpMask) && World::mask(*carEntity).test(carMask)) {

                auto& powerUp = World::getComponent<PowerUp>(*powerUpEntity);

                if (powerUp.available) {
                    give_power_up(carBodyId,*carEntity,powerUp);
                    disablePowerUp(powerUp,*powerUpEntity);
                }
            }
        }
    }

    void Football:: give_power_up(b2BodyId carBodyId, ent_type carEntity, PowerUp powerUp) const
    {
        if (powerUp.bigger)
            change_car_size(carBodyId,carEntity,BIGGER_SIZE);
        if (powerUp.faster)
            give_faster_power_up(carBodyId,carEntity);

        World::addComponent(carEntity,CarryPowerUp{powerUp.bigger, powerUp.faster,{SDL_GetTicks(),POWER_UP_TIMER,false}});
    }

    void Football:: change_car_size(b2BodyId oldBody, bagel::ent_type oldEntityId, float size_scale) const
    {
        b2Vec2 pos = b2Body_GetPosition(oldBody);
        b2Vec2 vel = b2Body_GetLinearVelocity(oldBody);
        b2Rot angle = b2Body_GetTransform(oldBody).q;

        float angleRadians = atan2(angle.s, angle.c);
        float angleDegrees = angleRadians * RAD_TO_DEG;


        auto& oldBodyTransform = World::getComponent<Transform>(oldEntityId);
        auto & oldBodyIntent = World::getComponent<Intent>(oldEntityId);
        auto& oldBodyDrawable = World::getComponent<Drawable>(oldEntityId);
        auto& oldBodyKeys = World::getComponent<Keys>(oldEntityId);
        auto& oldBodySide = World::getComponent<Car>(oldEntityId).side;
        auto& oldBodyStartPos = World::getComponent<StartingPosition>(oldEntityId);


        World::destroyEntity(oldEntityId);
        delete static_cast<ent_type*>(b2Body_GetUserData(oldBody));
        b2DestroyBody(oldBody);

        ent_type newEntityId = createCar({pos.x, pos.y}, oldBodyDrawable.part, oldBodyKeys, oldBodySide, size_scale);

        auto& newBodyTransform = World::getComponent<Transform>(newEntityId);
        newBodyTransform = oldBodyTransform;
        newBodyTransform.angle = angleDegrees;


        auto& newBodyIntent = World::getComponent<Intent>(newEntityId);
        newBodyIntent = oldBodyIntent;

        auto& newBodyStartPos = World::getComponent<StartingPosition>(newEntityId);
        newBodyStartPos = oldBodyStartPos;

        auto& newBody = World::getComponent<Collider>(newEntityId).body;

        b2Body_SetLinearVelocity(newBody, vel);
        b2Body_SetTransform(newBody, pos, angle);
    }

    void Football:: give_faster_power_up(b2BodyId carBodyId, bagel::ent_type carEntity) const
    {

    }

    void Football:: remove_faster_power_up(b2BodyId carBodyId, bagel::ent_type carEntity) const
    {

    }

    void Football::remove_power_up_system() const
    {
        static const Mask carryPowerUpMask = MaskBuilder().set<CarryPowerUp>().build();

        for (ent_type e{0}; e.id <= World::maxId().id; ++e.id) {
            if (World::mask(e).test(carryPowerUpMask)) {

                auto& carryPowerUp = World::getComponent<CarryPowerUp>(e);
                auto& bodyId = World::getComponent<Collider>(e).body;
                auto& timer = carryPowerUp.time_remaining_timer;

                if (SDL_GetTicks() - timer.start_time >= timer.time_remaining){
                    if (carryPowerUp.bigger)
                        change_car_size(bodyId,e,REGULAR_SIZE);
                    if (carryPowerUp.faster)
                        remove_faster_power_up(bodyId,e);
                    World::delComponent<CarryPowerUp>(e);
                }
            }
        }
    }

    void Football::update_power_up_timer_system() const {

        static const Mask powerUpMask = MaskBuilder().set<PowerUp>().build();

        for (ent_type e{0}; e.id <= World::maxId().id; ++e.id) {
            if (World::mask(e).test(powerUpMask)) {

                auto& powerUp = World::getComponent<PowerUp>(e);
                auto& timer = powerUp.time_out_timer;

                if (!powerUp.available && SDL_GetTicks() - timer.start_time >= timer.time_remaining){
                    enablePowerUp(powerUp, e);
                }
            }
        }
    }

    void Football::enablePowerUp(PowerUp& powerUp, ent_type powerUpEntity) const
    {
        powerUp.available = true;

        if (powerUp.bigger)
            World::addComponent(powerUpEntity, Drawable{SIZE_UP_TEX,{POWER_UP_CIRCLE_RADIUS * 2, POWER_UP_CIRCLE_RADIUS * 2},powerUpsTex});
        if (powerUp.faster)
            World::addComponent(powerUpEntity, Drawable{SPEED_UP_TEX,{POWER_UP_CIRCLE_RADIUS * 2, POWER_UP_CIRCLE_RADIUS * 2},powerUpsTex});

        cout << "powerUp enabled" << endl;
    }

    void Football::disablePowerUp(PowerUp& powerUp, ent_type powerUpEntity) const
    {
        powerUp.available = false;
        powerUp.time_out_timer.start_time = SDL_GetTicks();
        powerUp.time_out_timer.time_remaining = POWER_UP_TIME_OUT_TIMER;

        World::delComponent<Drawable>(powerUpEntity);

        cout << "powerUp disabled" << endl;
    }






    void Football::run()
    {
        SDL_SetRenderDrawColor(ren, 0,0,0,255);
        auto start = SDL_GetTicks();
        bool quit = false;

        InputSystem is;

        while (!quit) {
            is.updateEntities();
            //first updateEntities() for all systems

            is.update();
            //then update() for all systems

            World::step();
            //finally World::step() to clear added() array

            input_system();
            move_system();
            physic_system();
            //score_system();
            timer_system(); // ADD THIS LINE - Call timer system every frame
            pick_power_up_system();
            remove_power_up_system();
            update_power_up_timer_system();
            draw_system();

            auto end = SDL_GetTicks();
            if (end-start < GAME_FRAME) {
                SDL_Delay(GAME_FRAME - (end-start));
            }
            start += GAME_FRAME;

            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                quit = (e.type == SDL_EVENT_QUIT) ||
                       (e.type == SDL_EVENT_KEY_DOWN && e.key.scancode == SDL_SCANCODE_ESCAPE);
            }
        }
    }
}


