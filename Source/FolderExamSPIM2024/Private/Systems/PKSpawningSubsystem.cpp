// Fill out your copyright notice in the Description page of Project Settings.

#include "Systems/PKSpawningSubsystem.h"

#include "Components/PKMeshComponent.h"
#include "Components/PKPhysicsComponent.h"
#include "Components/PKTransformComponent.h"

APKEntityActor* UPKSpawningSubsystem::SpawnEntityActorAtLocation(const FVector& Location, const FRotator& Rotation,
	const FVector& Scale)
{
	// Get the Entity Manager Subsystem
	UPKEntityManagerSubsystem* EntityManager = GetWorld()->GetSubsystem<UPKEntityManagerSubsystem>();
	if (!EntityManager)
	{
		UE_LOG(LogTemp, Error, TEXT("PKEntityManagerSubsystem not found!"));
		return nullptr;
	}

	// Spawn the Entity Actor in the world
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

	// Register the actor as an entity
	int32 EntityID = EntityManager->CreateEntity();
	EntityActor->SetEntityID(EntityID);

	// Add a Transform Component
	TSharedPtr<PKTransformComponent> TransformComponent = MakeShared<PKTransformComponent>();
	TransformComponent->Position = Location;
	TransformComponent->Rotation = Rotation;
	TransformComponent->Scale = Scale;
	EntityManager->GetComponentManager().AddComponent<PKTransformComponent>(EntityID, TEXT("Transform"), TransformComponent);

	// Add a Physics Component
	TSharedPtr<PKPhysicsComponent> PhysicsComponent = MakeShared<PKPhysicsComponent>();
	PhysicsComponent->Velocity = FVector::ZeroVector;
	PhysicsComponent->Acceleration = FVector::ZeroVector;
	PhysicsComponent->bIsSimulating = true;
	EntityManager->GetComponentManager().AddComponent<PKPhysicsComponent>(EntityID, TEXT("Physics"), PhysicsComponent);

    // Add a Mesh Component  
    TSharedPtr<UPKMeshComponent> MeshComponent = MakeShared<UPKMeshComponent>();  
    EntityManager->GetComponentManager().AddComponent<UPKMeshComponent>(EntityID, TEXT("Mesh"), MeshComponent);  

    // Sync the Mesh Component with the Actor  
    if (MeshComponent.IsValid())  
    {  
        UStaticMeshComponent* Mesh = MeshComponent->GetMeshComponent();  
        if (Mesh)  
        {  
            // Attach the mesh to the actor  
            Mesh->AttachToComponent(EntityActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);  

            // Set the initial transform of the mesh  
            Mesh->SetWorldLocationAndRotation(Location, Rotation);  
            Mesh->SetWorldScale3D(Scale);  
        }  
    }  

    // Log the spawn  
    UE_LOG(LogTemp, Log, TEXT("Spawned APKEntityActor with Entity ID: %d at Location: %s"), EntityID, *Location.ToString());  

    return EntityActor;  
}