#pragma once

#include "CoreMinimal.h"
#include "PKBaseComponent.h"

/**
 *
 */
class FOLDEREXAMSPIM2024_API PKPhysicsComponent : public PKBaseComponent
{
public:
	PKPhysicsComponent(int32 InEntityID, const FVector& InVelocity, const FVector& InAcceleration, bool bSimulating)
		: EntityID(InEntityID), Velocity(InVelocity), Acceleration(InAcceleration), bIsSimulating(bSimulating) {
	}

	/*
	 * Variables
	 */
	int32 EntityID;
	FVector Velocity;
	FVector Acceleration;
	bool bIsSimulating;
};
