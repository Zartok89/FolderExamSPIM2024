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
		PKPhysicsComponent& PhysicsComponentA = ComponentManager->PhysicsComponents[i];

		if (!PhysicsComponentA.bIsSimulating)
		{
			continue;
		}

		PKTransformComponent* TransformComponentA = ComponentManager->GetTransformComponent(PhysicsComponentA.EntityID);
		if (!TransformComponentA)
		{
			//UE_LOG(LogTemp, Warning, TEXT("PhysicsSubsystem: TransformComponent not found for Entity %d"), PhysicsComponent.EntityID);
			continue;
		}

		PhysicsComponentA.Velocity += PhysicsComponentA.Acceleration * DeltaTime;
		TransformComponentA->Position += PhysicsComponentA.Velocity * DeltaTime;

		//UE_LOG(LogTemp, Log, TEXT("Entity %d Position: %s"), PhysicsComponent.EntityID, *TransformComponent->Position.ToString());

		for (int32 j = i + 1; j < ComponentManager->PhysicsComponents.Num(); ++j)
		{
			PKPhysicsComponent& PhysicsComponentB = ComponentManager->PhysicsComponents[j];

			if (!PhysicsComponentB.bIsSimulating)
			{
				continue;
			}

			PKTransformComponent* TransformComponentB = ComponentManager->GetTransformComponent(PhysicsComponentB.EntityID);
			if (!TransformComponentB)
			{
				continue;
			}

			// Check for overlap (simple AABB collision detection)
			FVector Delta = TransformComponentA->Position - TransformComponentB->Position;
			float DistanceSquared = Delta.SizeSquared();
			float MinDistance = PhysicsComponentA.CollisionRadius + PhysicsComponentB.CollisionRadius;

			if (DistanceSquared < MinDistance * MinDistance)
			{
				// Resolve collision
				ResolveCollision(*TransformComponentA, PhysicsComponentA, *TransformComponentB, PhysicsComponentB);
			}
		}

		AActor* ActorA = EntityManagerSubsystem->GetActorForEntity(PhysicsComponentA.EntityID);
		if (ActorA)
		{
			ActorA->SetActorLocation(TransformComponentA->Position);
		}
	}
}

TStatId UPKPhysicsSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(PKPhysicsSubsystem, STATGROUP_Tickables);
}

void UPKPhysicsSubsystem::ResolveCollision(PKTransformComponent& TransformA, PKPhysicsComponent& PhysicsA, PKTransformComponent& TransformB, PKPhysicsComponent& PhysicsB)
{
	const float GlobalRestitution = 0.8f;

	FVector Delta = TransformA.Position - TransformB.Position;
	float Distance = Delta.Size();

	if (Distance == 0.0f)
	{
		Delta = FVector::UpVector;
		Distance = 1.0f;
	}

	FVector Normal = Delta / Distance;

	float MinDistance = PhysicsA.CollisionRadius + PhysicsB.CollisionRadius;
	float PenetrationDepth = MinDistance - Distance;

	float TotalMass = PhysicsA.Mass + PhysicsB.Mass;
	float RatioA = PhysicsB.Mass / TotalMass;
	float RatioB = PhysicsA.Mass / TotalMass;

	TransformA.Position += Normal * PenetrationDepth * RatioA;
	TransformB.Position -= Normal * PenetrationDepth * RatioB;

	FVector RelativeVelocity = PhysicsA.Velocity - PhysicsB.Velocity;
	float VelocityAlongNormal = FVector::DotProduct(RelativeVelocity, Normal);

	if (VelocityAlongNormal > 0)
	{
		return;
	}

	float ImpulseMagnitude = -(1.0f + GlobalRestitution) * VelocityAlongNormal;
	ImpulseMagnitude /= ((1.0f / PhysicsA.Mass) + (1.0f / PhysicsB.Mass));

	FVector Impulse = ImpulseMagnitude * Normal;

	PhysicsA.Velocity += Impulse / PhysicsA.Mass;
	PhysicsB.Velocity -= Impulse / PhysicsB.Mass;

	if (bShouldDebug)
	{
		DrawDebugLine(GetWorld(), TransformA.Position, TransformB.Position, FColor::Red, false, 1.0f, 0, 2.0f);
		DrawDebugSphere(GetWorld(), TransformA.Position, PhysicsA.CollisionRadius, 12, FColor::Green, false, 1.0f);
		DrawDebugSphere(GetWorld(), TransformB.Position, PhysicsB.CollisionRadius, 12, FColor::Blue, false, 1.0f);
	}
}