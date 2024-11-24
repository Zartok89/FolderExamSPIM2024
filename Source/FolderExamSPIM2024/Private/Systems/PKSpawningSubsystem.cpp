// Fill out your copyright notice in the Description page of Project Settings.

#include "Systems/PKSpawningSubsystem.h"

#include "Components/PKMeshComponent.h"
#include "Components/PKPhysicsComponent.h"
#include "Components/PKTransformComponent.h"
#include <Systems/PKComponentManager.h>

APKEntityActor* UPKSpawningSubsystem::SpawnEntityActorAtLocation(const FVector& Location, const FRotator& Rotation,
	const FVector& Scale)
{
	UPKEntityManagerSubsystem* EntityManager = GetWorld()->GetSubsystem<UPKEntityManagerSubsystem>();
	if (!EntityManager)
	{
		UE_LOG(LogTemp, Error, TEXT("PKEntityManagerSubsystem not found!"));
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = nullptr;
	SpawnParams.Instigator = nullptr;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	APKEntityActor* EntityActor = GetWorld()->SpawnActor<APKEntityActor>(APKEntityActor::StaticClass(), Location, Rotation, SpawnParams);
	if (!EntityActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn APKEntityActor!"));
		return nullptr;
	}

	int32 EntityID = EntityManager->CreateEntity();
	EntityActor->SetEntityID(EntityID);

	PKComponentManager& ComponentManager = EntityManager->GetComponentManager();

	int32 TransformIndex = ComponentManager.AddTransformComponent(EntityID, Location, Rotation, Scale);
	EntityActor->SetTransformIndex(TransformIndex);

	int32 PhysicsIndex = ComponentManager.AddPhysicsComponent(EntityID, FVector::ZeroVector, FVector::ZeroVector, true);
	EntityActor->SetPhysicsIndex(PhysicsIndex);

	if (EntityActor->EntityMesh)
	{
		EntityActor->EntityMesh->SetWorldLocationAndRotation(Location, Rotation);
		EntityActor->EntityMesh->SetWorldScale3D(Scale);
	}

	UE_LOG(LogTemp, Warning, TEXT("Spawned PKEntityActor with Entity ID: %d at Location: %s"), EntityID, *Location.ToString());

	return EntityActor;
}

void UPKSpawningSubsystem::DespawnEntityActor(APKEntityActor* EntityActor)
{
	if (!EntityActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("DespawnEntityActor: EntityActor is null!"));
		return;
	}

	UPKEntityManagerSubsystem* EntityManager = GetWorld()->GetSubsystem<UPKEntityManagerSubsystem>();
	if (!EntityManager)
	{
		UE_LOG(LogTemp, Error, TEXT("PKEntityManagerSubsystem not found!"));
		return;
	}

	int32 EntityID = EntityActor->GetEntityID();

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