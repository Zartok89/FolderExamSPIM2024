// Fill out your copyright notice in the Description page of Project Settings.

#include "Systems/PKPhysicsSubsystem.h"

void UPKPhysicsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);  

    EntityManagerSubsystem = GetWorld()->GetSubsystem<UPKEntityManagerSubsystem>();  
    if (!EntityManagerSubsystem)  
    {  
        UE_LOG(LogTemp, Error, TEXT("PhysicsSubsystem: Failed to get EntityManagerSubsystem!"));  
    }  
    else  
    {  
        UE_LOG(LogTemp, Log, TEXT("PhysicsSubsystem: Successfully retrieved EntityManagerSubsystem."));  
    }

	ComponentManager = &EntityManagerSubsystem->GetComponentManager();
	if (!ComponentManager)
	{
		UE_LOG(LogTemp, Error, TEXT("PhysicsSubsystem: Failed to get ComponentManager!"));
	}

	UE_LOG(LogTemp, Log, TEXT("PhysicsSubsystem initialized."));
}

void UPKPhysicsSubsystem::Deinitialize()
{
	Super::Deinitialize();

	UE_LOG(LogTemp, Log, TEXT("PhysicsSubsystem deinitialized."));
}

void UPKPhysicsSubsystem::Tick(float DeltaTime)
{
	if (!EntityManagerSubsystem)
	{
		EntityManagerSubsystem = GetWorld()->GetSubsystem<UPKEntityManagerSubsystem>();
		if (!EntityManagerSubsystem)
		{
			//UE_LOG(LogTemp, Error, TEXT("PhysicsSubsystem: Failed to get EntityManagerSubsystem!"));
			return;
		}
		else
		{
			//UE_LOG(LogTemp, Log, TEXT("PhysicsSubsystem: Successfully retrieved EntityManagerSubsystem."));
		}
	}

	if (!ComponentManager)
	{
		//UE_LOG(LogTemp, Error, TEXT("PhysicsSubsystem: ComponentManager is null!"));
		return;
	}

	for (int32 i = 0; i < ComponentManager->PhysicsComponents.Num(); ++i)
	{
		PKPhysicsComponent& PhysicsComponent = ComponentManager->PhysicsComponents[i];

		if (!PhysicsComponent.bIsSimulating)
		{
			continue;
		}

		PKTransformComponent* TransformComponent = ComponentManager->GetTransformComponent(PhysicsComponent.EntityID);
		if (!TransformComponent)
		{
			//UE_LOG(LogTemp, Warning, TEXT("PhysicsSubsystem: TransformComponent not found for Entity %d"), PhysicsComponent.EntityID);
			continue;
		}

		PhysicsComponent.Velocity += PhysicsComponent.Acceleration * DeltaTime;

		TransformComponent->Position += PhysicsComponent.Velocity * DeltaTime;

		//UE_LOG(LogTemp, Log, TEXT("Entity %d Position: %s"), PhysicsComponent.EntityID, *TransformComponent->Position.ToString());

		AActor* Actor = EntityManagerSubsystem->GetActorForEntity(PhysicsComponent.EntityID);
		if (Actor)
		{
			Actor->SetActorLocation(TransformComponent->Position);
			//UE_LOG(LogTemp, Log, TEXT("Entity %d Transform Position: %s"), PhysicsComponent.EntityID, *TransformComponent->Position.ToString());
			//UE_LOG(LogTemp, Log, TEXT("Actor Position: %s"), *Actor->GetActorLocation().ToString());
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("PhysicsSubsystem: Actor not found for Entity %d"), PhysicsComponent.EntityID);
		}
	}
}

TStatId UPKPhysicsSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(PKPhysicsSubsystem, STATGROUP_Tickables);
}