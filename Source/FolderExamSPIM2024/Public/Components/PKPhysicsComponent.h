#pragma once

#include "CoreMinimal.h"
#include "PKBaseComponent.h"

/**
 *
 */
class FOLDEREXAMSPIM2024_API PKPhysicsComponent : public PKBaseComponent
{
public:
	PKPhysicsComponent(int32 InEntityID, float InMass, const FVector& InVelocity, const FVector& InAcceleration, float InCollisionRadius, bool bSimulating)
		: EntityID(InEntityID), Mass(InMass), Velocity(InVelocity), Acceleration(InAcceleration), CollisionRadius(InCollisionRadius), bIsSimulating(bSimulating) {
	}

	/*
	 * Variables
	 */
	int32 EntityID;
	float Mass = 1.f;
	FVector Velocity = { 0.f, 10.f, 0.f };
	FVector Acceleration = { 0.f, 10.f, 0.f };;
	float CollisionRadius = 50.f;
	bool bIsSimulating = true;
};
