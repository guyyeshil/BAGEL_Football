//
// Created by guyye on 01/06/2025.
//

#include "Football.h"
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <box2d/box2d.h>

#include "lib/box2d/src/body.h"
using namespace std;

#include "bagel.h"
using namespace bagel;


namespace football {

    void Football::createBall() const
    {
        b2BodyDef ballBodyDef = b2DefaultBodyDef();
        ballBodyDef.type = b2_dynamicBody;
        ballBodyDef.fixedRotation = false;
        ballBodyDef.position = {ball_start_position.x, ball_start_position.y};

        b2ShapeDef ballShapeDef = b2DefaultShapeDef();
        ballShapeDef.enableSensorEvents = true;
        ballShapeDef.density = 1;
        ballShapeDef.material.friction = 0.0;//todo
        ballShapeDef.material.restitution = 0.8f;
        b2Circle ballCircle = {0,0,BALL_RADIUS};

        b2BodyId ballBody = b2CreateBody(boxWorld, &ballBodyDef);
        b2Body_SetLinearDamping(ballBody, 1.5f);//todo
        b2CreateCircleShape(ballBody, &ballShapeDef, &ballCircle);

        Entity ballEntity = Entity::create();
        ballEntity.addAll(
            Transform{{ball_start_position},0},
            Drawable{{BALL_TEX}, {BALL_RADIUS * 2, BALL_RADIUS * 2}, ballTex},
            Collider{ballBody},
            Ball{}
        );
        b2Body_SetUserData(ballBody, new ent_type{ballEntity.entity()});
    }

    void Football::createCar(const SDL_FPoint& position, const SDL_FRect& tex, const Keys& keys) const
    {
        b2BodyDef carBodyDef = b2DefaultBodyDef();
        carBodyDef.type = b2_dynamicBody;
        carBodyDef.position = {position.x, position.y};

        b2ShapeDef carShapeDef = b2DefaultShapeDef();
        carShapeDef.density = 1;
        carShapeDef.material.friction = 0.5;//todo
        carShapeDef.material.restitution = 0.9f;
        b2Circle carCircle = {0,0,1.5};


        b2BodyId carBody = b2CreateBody(boxWorld, &carBodyDef);
        b2Body_SetLinearDamping(carBody, 1.5f);//todo
        b2CreateCircleShape(carBody, &carShapeDef, &carCircle);

        Entity carEntity = Entity::create();
        carEntity.addAll(
            Transform{{position},0},
            Intent{},
            Keys{keys},
            Drawable{{tex}, {3, 3}, carsTex},
            Collider{carBody}
        );
    }


    void Football::createField() const
    {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_staticBody;
        bodyDef.position = {FIELD_WIDTH/2,FIELD_HEIGHT/2};

        b2BodyId body = b2CreateBody(boxWorld, &bodyDef);

        Entity::create().addAll(
            Transform{{bodyDef.position.x,bodyDef.position.y},0},
            Drawable{FIELD_TEX, {FIELD_WIDTH, FIELD_HEIGHT}, fieldTex}
        );

        createFieldBorders();
        createLeftGoalBars();
        createRightGoalBars();
    }

    void Football::createFieldBorders() const
    {
        b2ShapeDef borderShapeDef = b2DefaultShapeDef();
        borderShapeDef.density = 1;
        borderShapeDef.material.friction = 0.6f;
        borderShapeDef.material.restitution = 0.1f;

        b2Polygon widthBorder = b2MakeBox(FIELD_WIDTH, 0.2f);
        b2Polygon HeightBorder = b2MakeBox(0.2f, FIELD_HEIGHT);

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

    void Football::createLeftGoalBars() const
    {
        b2ShapeDef barShapeDef = b2DefaultShapeDef();
        barShapeDef.density = 1;
        barShapeDef.material.friction = 0.6f;
        barShapeDef.material.restitution = 0.1f;

        b2Polygon leftBar = b2MakeBox(SIDE_BAR_WIDTH/2, 0.2f);
        b2Polygon rightBar = b2MakeBox(SIDE_BAR_WIDTH/2, 0.2f);
        b2Polygon backBar = b2MakeBox(0.2f, BACK_BAR_HEIGHT/2);

        b2BodyDef backBarBodyDef = b2DefaultBodyDef();
        backBarBodyDef.type = b2_staticBody;
        backBarBodyDef.position = {5.625f - 0.2f, FIELD_HEIGHT/2};
        b2BodyId backBarBody = b2CreateBody(boxWorld, &backBarBodyDef);
        b2CreatePolygonShape(backBarBody, &barShapeDef, &backBar);

        b2BodyDef leftBarBodyDef = b2DefaultBodyDef();
        leftBarBodyDef.type = b2_staticBody;
        leftBarBodyDef.position = {5.625f + 0.2f + (SIDE_BAR_WIDTH/2), FIELD_HEIGHT/2 - 0.2f - BACK_BAR_HEIGHT/2 };
        b2BodyId leftBarBody = b2CreateBody(boxWorld, &leftBarBodyDef);
        b2CreatePolygonShape(leftBarBody, &barShapeDef, &leftBar);

        b2BodyDef rightBarBodyDef = b2DefaultBodyDef();
        rightBarBodyDef.type = b2_staticBody;
        rightBarBodyDef.position = {5.625f + 0.2f + (SIDE_BAR_WIDTH/2), FIELD_HEIGHT/2 + 0.2f + BACK_BAR_HEIGHT/2 };
        b2BodyId rightBarBody = b2CreateBody(boxWorld, &rightBarBodyDef);
        b2CreatePolygonShape(rightBarBody, &barShapeDef, &rightBar);
    }

    void Football::createRightGoalBars() const
    {
        b2ShapeDef barShapeDef = b2DefaultShapeDef();
        barShapeDef.density = 1;
        barShapeDef.material.friction = 0.6f;
        barShapeDef.material.restitution = 0.1f;

        b2Polygon leftBar = b2MakeBox(SIDE_BAR_WIDTH/2, 0.2f);
        b2Polygon rightBar = b2MakeBox(SIDE_BAR_WIDTH/2, 0.2f);
        b2Polygon backBar = b2MakeBox(0.2f, BACK_BAR_HEIGHT/2);

        b2BodyDef backBarBodyDef = b2DefaultBodyDef();
        backBarBodyDef.type = b2_staticBody;
        backBarBodyDef.position = {FIELD_WIDTH-5.625f + 0.2f, FIELD_HEIGHT/2};
        b2BodyId backBarBody = b2CreateBody(boxWorld, &backBarBodyDef);
        b2CreatePolygonShape(backBarBody, &barShapeDef, &backBar);

        b2BodyDef leftBarBodyDef = b2DefaultBodyDef();
        leftBarBodyDef.type = b2_staticBody;
        leftBarBodyDef.position = {FIELD_WIDTH-5.625f - 0.2f - (SIDE_BAR_WIDTH/2), FIELD_HEIGHT/2 - 0.2f - BACK_BAR_HEIGHT/2 };
        b2BodyId leftBarBody = b2CreateBody(boxWorld, &leftBarBodyDef);
        b2CreatePolygonShape(leftBarBody, &barShapeDef, &leftBar);

        b2BodyDef rightBarBodyDef = b2DefaultBodyDef();
        rightBarBodyDef.type = b2_staticBody;
        rightBarBodyDef.position = {FIELD_WIDTH-5.625f - 0.2f - (SIDE_BAR_WIDTH/2), FIELD_HEIGHT/2 + 0.2f + BACK_BAR_HEIGHT/2 };
        b2BodyId rightBarBody = b2CreateBody(boxWorld, &rightBarBodyDef);
        b2CreatePolygonShape(rightBarBody, &barShapeDef, &rightBar);
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

        SDL_DestroySurface(surf);
        return true;
    }


    void Football::prepareBoxWorld()
    {
        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = {0,0};
        boxWorld = b2CreateWorld(&worldDef);
    }

    Football::Football()
    {
        if (!prepareWindowAndTexture())
            return;
        SDL_srand(time(nullptr));

        prepareBoxWorld();
        //prepareWalls();//todo
        createField();
        createBall();
        createCar({20,25},BLUE_CAR_TEX,{SDL_SCANCODE_W, SDL_SCANCODE_S,SDL_SCANCODE_A, SDL_SCANCODE_D});
        createCar({60,25},ORANGE_CAR_TEX,{SDL_SCANCODE_UP, SDL_SCANCODE_DOWN,SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT});


    }

    Football::~Football()
    {
        if (b2World_IsValid(boxWorld))
            b2DestroyWorld(boxWorld);
        if (ballTex != nullptr)
            SDL_DestroyTexture(ballTex);
        if (carsTex != nullptr)
            SDL_DestroyTexture(carsTex);
        if (fieldTex != nullptr)
            SDL_DestroyTexture(fieldTex);
        if (ren != nullptr)
            SDL_DestroyRenderer(ren);
        if (win != nullptr)
            SDL_DestroyWindow(win);

        SDL_Quit();
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

    void Football::input_system() const
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

    void Football::move_system() const
    {
        static const Mask mask = MaskBuilder()
            .set<Intent>()
            .set<Collider>()
            .build();

        const float forward_Ride_Strength = 500.0f; //todo
        const float backward_Ride_Strength = 200.0f; //todo
        const float side_Ride_Strength = 350.0f;// todo
        const float maxSpeed = 10.0f; // todo

        for (ent_type e{0}; e.id <= World::maxId().id; ++e.id) {
            if (World::mask(e).test(mask)) {
                const auto& intent = World::getComponent<Intent>(e);
                const auto& collider = World::getComponent<Collider>(e);

                b2Vec2 force = {0.0f, 0.0f};

                if (intent.up)
                    force.y -= forward_Ride_Strength;
                if (intent.down)
                    force.y += forward_Ride_Strength;
                if (intent.left)
                    force.x -= forward_Ride_Strength;
                if (intent.right)
                    force.x += forward_Ride_Strength;

                b2Body_ApplyForceToCenter(collider.body, force, true);



                // הגבלת מהירות
                // b2Vec2 velocity = b2Body_GetLinearVelocity(collider.body);
                // printf("Velocity: (%f, %f)\n", velocity.x, velocity.y);
                // float speed = b2Length(velocity);

                // if (speed > maxSpeed) {
                //     float scale = maxSpeed / speed;
                //     velocity.x *= scale;
                //     velocity.y *= scale;
                //     b2Body_SetLinearVelocity(collider.body, velocity);
                // }
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

    void Football::run() {

        // draw_system();
        //
        // SDL_Delay(5000);

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

            draw_system();

            auto end = SDL_GetTicks();
            if (end-start < GAME_FRAME) {
                SDL_Delay(GAME_FRAME - (end-start));
            }
            start += GAME_FRAME;

            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_EVENT_QUIT)
                    quit = true;
                else if ((e.type == SDL_EVENT_KEY_DOWN) && (e.key.scancode == SDL_SCANCODE_ESCAPE))
                    quit = true;
            }
        }
    }

}


