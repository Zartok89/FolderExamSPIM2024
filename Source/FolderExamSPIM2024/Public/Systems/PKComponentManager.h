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
	PKComponentManager() = default;
	~PKComponentManager() = default;

	int32 AddTransformComponent(int32 EntityID, const FVector& Position, const FRotator& Rotation, const FVector& Scale)
	{
		if (EntityToTransformIndex.Contains(EntityID))
		{
			UE_LOG(LogTemp, Warning, TEXT("Entity %d already has a Transform Component!"), EntityID);
			return EntityToTransformIndex[EntityID];
		}

		int32 Index = TransformComponents.Add(PKTransformComponent(EntityID, Position, Rotation, Scale));
		EntityToTransformIndex.Add(EntityID, Index);
		return Index;
	}

	PKTransformComponent* GetTransformComponent(int32 EntityID)
	{
		if (EntityToTransformIndex.Contains(EntityID))
		{
			int32 Index = EntityToTransformIndex[EntityID];
			return &TransformComponents[Index];
		}
		return nullptr;
	}

	int32 AddPhysicsComponent(int32 EntityID, const FVector& Velocity, const FVector& Acceleration, bool bIsSimulating)
	{
		if (EntityToPhysicsIndex.Contains(EntityID))
		{
			UE_LOG(LogTemp, Warning, TEXT("Entity %d already has a Physics Component!"), EntityID);
			return EntityToPhysicsIndex[EntityID];
		}

		int32 Index = PhysicsComponents.Add(PKPhysicsComponent(EntityID, Velocity, Acceleration, bIsSimulating));
		EntityToPhysicsIndex.Add(EntityID, Index);
		return Index;
	}

	PKPhysicsComponent* GetPhysicsComponent(int32 EntityID)
	{
		if (EntityToPhysicsIndex.Contains(EntityID))
		{
			int32 Index = EntityToPhysicsIndex[EntityID];
			return &PhysicsComponents[Index];
		}
		return nullptr;
	}

	void RemoveAllComponents(int32 EntityID)
	{
		if (EntityToTransformIndex.Contains(EntityID))
		{
			int32 Index = EntityToTransformIndex[EntityID];
			TransformComponents.RemoveAtSwap(Index);
			EntityToTransformIndex.Remove(EntityID);

			for (auto& Pair : EntityToTransformIndex)
			{
				if (Pair.Value == TransformComponents.Num())
				{
					Pair.Value = Index;
					break;
				}
			}
		}

		if (EntityToPhysicsIndex.Contains(EntityID))
		{
			int32 Index = EntityToPhysicsIndex[EntityID];
			PhysicsComponents.RemoveAtSwap(Index);
			EntityToPhysicsIndex.Remove(EntityID);

			for (auto& Pair : EntityToPhysicsIndex)
			{
				if (Pair.Value == PhysicsComponents.Num())
				{
					Pair.Value = Index;
					break;
				}
			}
		}
	}

	/*
	 * Variables
	 */
	TArray<PKTransformComponent> TransformComponents;
	TMap<int32, int32> EntityToTransformIndex;

	TArray<PKPhysicsComponent> PhysicsComponents;
	TMap<int32, int32> EntityToPhysicsIndex;
};
