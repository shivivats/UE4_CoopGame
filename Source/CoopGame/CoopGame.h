// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// helps make the surface types more readable whenever used in other code
#define SURFACE_FLESHDEFAULT		SurfaceType1
#define SURFACE_FLESHVULNERABLE		SurfaceType2

// collision channels for traces
#define COLLISION_WEAPON			ECC_GameTraceChannel1
#define COLLISION_WEAPON_PENETRABLE ECC_GameTraceChannel2
#define COLLISION_INTERACTACTION	ECC_GameTraceChannel3