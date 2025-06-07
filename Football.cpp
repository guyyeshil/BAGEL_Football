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
        createCar({20,25},BLUE_CAR_TEX,{SDL_SCANCODE_W, SDL_SCANCODE_S,SDL_SCANCODE_A, SDL_SCANCODE_D});
        createCar({60,25},ORANGE_CAR_TEX,{SDL_SCANCODE_UP, SDL_SCANCODE_DOWN,SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT});
        createDataBar();

        if(DEBUG_MODE)
        {
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

        SDL_DestroySurface(surf);
        return true;
    }

    Football::Football()
    {
        if (!prepareWindowAndTexture())
            return;
        SDL_srand(time(nullptr));

        prepareBoxWorld();
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
        if (scoreFrameTex != nullptr)
            SDL_DestroyTexture(scoreFrameTex);
        if (ren != nullptr)
            SDL_DestroyRenderer(ren);
        if (win != nullptr)
            SDL_DestroyWindow(win);

        SDL_Quit();
    }

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
        b2Circle ballCircle = {{0,0},BALL_RADIUS};

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
        b2Circle carCircle = {{0,0},1.5};


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
        b2Body_SetFixedRotation(carBody,true);
    }

    void Football::createField() const
    {
        SDL_FRect FieldPosition = {FIELD_WIDTH/2,FIELD_HEIGHT/2,0,0};

        Entity::create().addAll(
            Transform{{FieldPosition.x,FieldPosition.y},0},
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

    void Football::createLeftGoalBars() const
    {
        b2ShapeDef barShapeDef = b2DefaultShapeDef();
        barShapeDef.density = 1;
        barShapeDef.material.friction = 0.6f;
        barShapeDef.material.restitution = 0.1f;

        b2Polygon leftBar = b2MakeBox(SIDE_BAR_WIDTH/2, BAR_HALF_THICKNESS);
        b2Polygon rightBar = b2MakeBox(SIDE_BAR_WIDTH/2, BAR_HALF_THICKNESS);
        b2Polygon backBar = b2MakeBox(BAR_HALF_THICKNESS, BACK_BAR_HEIGHT/2);

        b2BodyDef backBarBodyDef = b2DefaultBodyDef();
        backBarBodyDef.type = b2_staticBody;
        backBarBodyDef.position = {BACK_BAR_POS - BAR_HALF_THICKNESS, FIELD_HEIGHT/2};
        b2BodyId backBarBody = b2CreateBody(boxWorld, &backBarBodyDef);
        b2CreatePolygonShape(backBarBody, &barShapeDef, &backBar);

        b2BodyDef leftBarBodyDef = b2DefaultBodyDef();
        leftBarBodyDef.type = b2_staticBody;
        leftBarBodyDef.position = {BACK_BAR_POS + (SIDE_BAR_WIDTH/2) +BAR_HALF_THICKNESS, FIELD_HEIGHT/2 - BACK_BAR_HEIGHT/2 - BAR_HALF_THICKNESS };
        b2BodyId leftBarBody = b2CreateBody(boxWorld, &leftBarBodyDef);
        b2CreatePolygonShape(leftBarBody, &barShapeDef, &leftBar);

        b2BodyDef rightBarBodyDef = b2DefaultBodyDef();
        rightBarBodyDef.type = b2_staticBody;
        rightBarBodyDef.position = {BACK_BAR_POS + (SIDE_BAR_WIDTH/2) + BAR_HALF_THICKNESS, FIELD_HEIGHT/2 + BACK_BAR_HEIGHT/2 + BAR_HALF_THICKNESS };
        b2BodyId rightBarBody = b2CreateBody(boxWorld, &rightBarBodyDef);
        b2CreatePolygonShape(rightBarBody, &barShapeDef, &rightBar);
    }

    void Football::createRightGoalBars() const
    {
        b2ShapeDef barShapeDef = b2DefaultShapeDef();
        barShapeDef.density = 1;
        barShapeDef.material.friction = 0.6f;
        barShapeDef.material.restitution = 0.1f;

        b2Polygon leftBar = b2MakeBox(SIDE_BAR_WIDTH/2, BAR_HALF_THICKNESS);
        b2Polygon rightBar = b2MakeBox(SIDE_BAR_WIDTH/2, BAR_HALF_THICKNESS);
        b2Polygon backBar = b2MakeBox(BAR_HALF_THICKNESS, BACK_BAR_HEIGHT/2);

        b2BodyDef backBarBodyDef = b2DefaultBodyDef();
        backBarBodyDef.type = b2_staticBody;
        backBarBodyDef.position = {FIELD_WIDTH - BACK_BAR_POS + BAR_HALF_THICKNESS, FIELD_HEIGHT/2};
        b2BodyId backBarBody = b2CreateBody(boxWorld, &backBarBodyDef);
        b2CreatePolygonShape(backBarBody, &barShapeDef, &backBar);

        b2BodyDef leftBarBodyDef = b2DefaultBodyDef();
        leftBarBodyDef.type = b2_staticBody;
        leftBarBodyDef.position = {FIELD_WIDTH - BACK_BAR_POS - BAR_HALF_THICKNESS - (SIDE_BAR_WIDTH/2), FIELD_HEIGHT/2 - BAR_HALF_THICKNESS - BACK_BAR_HEIGHT/2 };
        b2BodyId leftBarBody = b2CreateBody(boxWorld, &leftBarBodyDef);
        b2CreatePolygonShape(leftBarBody, &barShapeDef, &leftBar);

        b2BodyDef rightBarBodyDef = b2DefaultBodyDef();
        rightBarBodyDef.type = b2_staticBody;
        rightBarBodyDef.position = {FIELD_WIDTH - BACK_BAR_POS - BAR_HALF_THICKNESS - (SIDE_BAR_WIDTH/2), FIELD_HEIGHT/2 + BAR_HALF_THICKNESS + BACK_BAR_HEIGHT/2 };
        b2BodyId rightBarBody = b2CreateBody(boxWorld, &rightBarBodyDef);
        b2CreatePolygonShape(rightBarBody, &barShapeDef, &rightBar);
    }

    void Football::applyDebugFunctions() const
    {
        createDebugBox();
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
	const float turn_speed = 5.0f;
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
    float speed_factor;
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
            speed_factor = current_speed / 10.0f;
            effective_turn_speed = turn_speed * speed_factor;
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


