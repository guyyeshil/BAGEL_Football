#pragma once
#include "Football.h"

#include "Football.h"

constexpr Bagel Params{
	.DynamicResize = false
};

BAGEL_STORAGE(GameTimer,PackedStorage);
BAGEL_STORAGE(TimerDigit,PackedStorage);
BAGEL_STORAGE(Intent,PackedStorage);
BAGEL_STORAGE(Keys,PackedStorage);
BAGEL_STORAGE(Ball,TaggedStorage);
BAGEL_STORAGE(GoalLeft,TaggedStorage);
BAGEL_STORAGE(GoalRight,TaggedStorage);
BAGEL_STORAGE(Car,PackedStorage);
BAGEL_STORAGE(StartingPosition,PackedStorage);
BAGEL_STORAGE(PowerUp,PackedStorage);
BAGEL_STORAGE(CarryPowerUp,PackedStorage);
BAGEL_STORAGE(LeftScoreDigit,TaggedStorage);
BAGEL_STORAGE(RightScoreDigit,TaggedStorage);
BAGEL_STORAGE(EndGameMsg,TaggedStorage);
