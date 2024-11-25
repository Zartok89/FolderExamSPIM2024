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

	QuadTree = new PKQuadTree(WorldBoundsMin, WorldBoundsMax, 4, 5);
}

void UPKPhysicsSubsystem::Deinitialize()
{
	Super::Deinitialize();

	if (QuadTree)
	{
		delete QuadTree;
		QuadTree = nullptr;
	}

	UE_LOG(LogTemp, Log, TEXT("PhysicsSubsystem deinitialized."));
}

void UPKPhysicsSubsystem::Tick(float DeltaTime)
{
	if (!EntityManagerSubsystem)
	{
		EntityManagerSubsystem = GetWorld()->GetSubsystem<UPKEntityManagerSubsystem>();
		if (!EntityManagerSubsystem)
		{
			return;
		}
	}

	if (!ComponentManager)
	{
		return;
	}

	QuadTree->Clear();

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
			continue;
		}

		PhysicsComponent.Velocity += PhysicsComponent.Acceleration * DeltaTime;
		TransformComponent->Position += PhysicsComponent.Velocity * DeltaTime;

		AActor* Actor = EntityManagerSubsystem->GetActorForEntity(PhysicsComponent.EntityID);
		if (Actor)
		{
			Actor->SetActorLocation(TransformComponent->Position);
		}

		QuadTree->Insert(
			PhysicsComponent.EntityID,
			FVector2D(TransformComponent->Position.X, TransformComponent->Position.Y),
			PhysicsComponent.CollisionRadius
		);
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
			continue;
		}

		TArray<int32> NearbyEntities;
		QuadTree->Query(
			FVector2D(TransformComponentA->Position.X - PhysicsComponentA.CollisionRadius, TransformComponentA->Position.Y - PhysicsComponentA.CollisionRadius),
			FVector2D(TransformComponentA->Position.X + PhysicsComponentA.CollisionRadius, TransformComponentA->Position.Y + PhysicsComponentA.CollisionRadius),
			NearbyEntities
		);

		for (int32 NearbyEntityID : NearbyEntities)
		{
			if (NearbyEntityID == PhysicsComponentA.EntityID)
			{
				continue;
			}

			PKPhysicsComponent* PhysicsComponentB = ComponentManager->GetPhysicsComponent(NearbyEntityID);
			PKTransformComponent* TransformComponentB = ComponentManager->GetTransformComponent(NearbyEntityID);

			if (!PhysicsComponentB || !TransformComponentB || !PhysicsComponentB->bIsSimulating)
			{
				continue;
			}

			FVector Delta = TransformComponentA->Position - TransformComponentB->Position;
			float DistanceSquared = Delta.SizeSquared();
			float MinDistance = PhysicsComponentA.CollisionRadius + PhysicsComponentB->CollisionRadius;

			if (DistanceSquared < MinDistance * MinDistance)
			{
				ResolveCollision(*TransformComponentA, PhysicsComponentA, *TransformComponentB, *PhysicsComponentB);
			}
		}
	}

	if (bShouldDebug)
	{
		QuadTree->DrawDebug(GetWorld(), 0.1f);
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