#pragma once

#include "CoreMinimal.h"
#include "PKBaseComponent.h"

/**
 *
 */
class FOLDEREXAMSPIM2024_API PKTransformComponent : public PKBaseComponent
{
public:
	PKTransformComponent(int32 InEntityID, const FVector& InPosition, const FRotator& InRotation, const FVector& InScale)
		: EntityID(InEntityID), Position(InPosition), Rotation(InRotation), Scale(InScale) {
	}

	/*
	 * Variables
	 */
	int32 EntityID;
	FVector Position;
	FRotator Rotation;
	FVector Scale;
};
