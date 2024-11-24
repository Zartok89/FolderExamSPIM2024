#pragma once

#include "CoreMinimal.h"
#include "Components/PKPhysicsComponent.h"
#include "Components/PKTransformComponent.h"

class PKBaseComponent;

/**
 *
 */
class FOLDEREXAMSPIM2024_API PKComponentManager
{
public:

	int32 AddTransformComponent(int32 EntityID, FVector Position, FRotator Rotation, FVector Scale)
	{
		int32 Index = TransformComponents.Add(PKTransformComponent(EntityID, Position, Rotation, Scale));
		EntityToTransformIndex.Add(EntityID, Index);
		return Index;
	}

	int32 AddPhysicsComponent(int32 EntityID, FVector Velocity, FVector Acceleration, bool bSimulating)
	{
		int32 Index = PhysicsComponents.Add(PKPhysicsComponent(EntityID, Velocity, Acceleration, bSimulating));
		EntityToPhysicsIndex.Add(EntityID, Index);
		return Index;
	}

	PKTransformComponent* GetTransformComponent(int32 EntityID)
	{
		if (EntityToTransformIndex.Contains(EntityID))
		{
			return &TransformComponents[EntityToTransformIndex[EntityID]];
		}
		return nullptr;
	}

	PKPhysicsComponent* GetPhysicsComponent(int32 EntityID)
	{
		if (EntityToPhysicsIndex.Contains(EntityID))
		{
			return &PhysicsComponents[EntityToPhysicsIndex[EntityID]];
		}
		return nullptr;
	}

	void RemoveAllComponents(int32 EntityID)
	{
		if (EntityToTransformIndex.Contains(EntityID))
		{
			TransformComponents.RemoveAtSwap(EntityToTransformIndex[EntityID]);
			EntityToTransformIndex.Remove(EntityID);
		}

		if (EntityToPhysicsIndex.Contains(EntityID))
		{
			PhysicsComponents.RemoveAtSwap(EntityToPhysicsIndex[EntityID]);
			EntityToPhysicsIndex.Remove(EntityID);
		}
	}

	/*
	 * Variables
	 */
	TArray<PKTransformComponent> TransformComponents;
	TArray<PKPhysicsComponent> PhysicsComponents;

	TMap<int32, int32> EntityToTransformIndex;
	TMap<int32, int32> EntityToPhysicsIndex;
};