#pragma once

#include "CoreMinimal.h"
#include "PKBaseComponent.h"

/**
 * 
 */
class FOLDEREXAMSPIM2024_API PKTransformComponent : public PKBaseComponent
{
public:
	PKTransformComponent() : Position(FVector::OneVector), Rotation(FRotator::ZeroRotator), Scale(FVector(1.0f)) {}

	/*
	 * Variables
	 */
	FVector Position;
	FRotator Rotation;
	FVector Scale;

};
