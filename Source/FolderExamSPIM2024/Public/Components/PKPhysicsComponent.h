#pragma once

#include "CoreMinimal.h"
#include "PKBaseComponent.h"

/**
 *
 */
class FOLDEREXAMSPIM2024_API PKPhysicsComponent : public PKBaseComponent
{
public:
	PKPhysicsComponent() : Velocity(FVector::OneVector), Acceleration(FVector::OneVector), bIsSimulating(false) {};

	/*
	 * Variables
	 */
	FVector Velocity;
	FVector Acceleration;
	bool bIsSimulating;
};
