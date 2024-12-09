// Fill out your copyright notice in the Description page of Project Settings.

#include "Systems/PKSpawningSubsystem.h"
#include <Systems/PKComponentManager.h>

APKEntityActor* UPKSpawningSubsystem::SpawnEntityActorAtLocation(const FVector& Location, const FRotator& Rotation, const FVector& Scale, FVector Velocity, FVector Acceleration, float Mass, float CollisionRadius)
{
	// Retrieve the entity manager subsystem from the world
	UPKEntityManagerSubsystem* EntityManager = GetWorld()->GetSubsystem<UPKEntityManagerSubsystem>();
	if (!EntityManager)
	{
		UE_LOG(LogTemp, Error, TEXT("PKEntityManagerSubsystem not found!"));
		return nullptr;
	}

	// Set up spawn parameters for the actor
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = nullptr;
	SpawnParams.Instigator = nullptr;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Get the component manager from the entity manager
	PKComponentManager& ComponentManager = EntityManager->GetComponentManager();

	// Create a new entity and get its unique ID
	int32 EntityID = EntityManager->CreateEntity();

	// Add components to the entity
	int32 TransformIndex = ComponentManager.AddTransformComponent(EntityID, Location, Rotation, Scale);
	int32 PhysicsIndex = ComponentManager.AddPhysicsComponent(EntityID, Mass, Velocity, Acceleration, CollisionRadius, true);

	// Spawn the actor in the world

	APKEntityActor* EntityActor = GetWorld()->SpawnActor<APKEntityActor>(APKEntityActor::StaticClass(), Location, Rotation, SpawnParams);
	if (EntityActor)
	{
		// Map the entity ID to the actor in the entity manager
		EntityManager->MapEntityToActor(EntityID, EntityActor);

		// Set the transform and physics component indices in the actor
		EntityActor->SetTransformIndex(TransformIndex);
		EntityActor->SetPhysicsIndex(PhysicsIndex);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn actor for Entity %d"), EntityID);
	}

	// Set the entity ID in the actor
	EntityActor->SetEntityID(EntityID);

	// If the actor has a mesh component, set its transform properties
	if (EntityActor->EntityMesh)
	{
		EntityActor->EntityMesh->SetWorldLocationAndRotation(Location, Rotation);
		EntityActor->EntityMesh->SetWorldScale3D(Scale);
	}

	return EntityActor;
}

void UPKSpawningSubsystem::DespawnEntityActor(APKEntityActor* EntityActor)
{
	if (!EntityActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("DespawnEntityActor: EntityActor is null!"));
		return;
	}

	// Retrieve the entity manager subsystem from the world
	UPKEntityManagerSubsystem* EntityManager = GetWorld()->GetSubsystem<UPKEntityManagerSubsystem>();
	if (!EntityManager)
	{
		UE_LOG(LogTemp, Error, TEXT("PKEntityManagerSubsystem not found!"));
		return;
	}

	// Get the entity ID associated with the actor
	int32 EntityID = EntityActor->GetEntityID();

	// Attempt to destroy the entity in the entity manager
	if (EntityManager->DestroyEntity(EntityID))
	{
		PKComponentManager& ComponentManager = EntityManager->GetComponentManager();
		ComponentManager.RemoveAllComponents(EntityID);

		UE_LOG(LogTemp, Warning, TEXT("Successfully destroyed entity with ID: %d"), EntityID);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to destroy entity with ID: %d"), EntityID);
	}

	EntityActor->Destroy();
	UE_LOG(LogTemp, Warning, TEXT("Destroyed APKEntityActor with Entity ID: %d"), EntityID);
}