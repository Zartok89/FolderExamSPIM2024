// Fill out your copyright notice in the Description page of Project Settings.

#include "Systems/PKPhysicsSubsystem.h"

#include "DrawDebugHelpers.h"
#include "Engine/World.h"

void UPKPhysicsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	EntityManagerSubsystem = GetWorld()->GetSubsystem<UPKEntityManagerSubsystem>();

	ComponentManager = &EntityManagerSubsystem->GetComponentManager();

	MaxEntitiesPerNode = 1;
	MaxDepth = 10;
	QuadTree = NewObject<UPKQuadTree>(UPKQuadTree::StaticClass());
	QuadTree->Initialize(WorldBoundsMin, WorldBoundsMax, MaxEntitiesPerNode, MaxDepth);
}

// Update the QuadTree by clearing and reinitializing it
void UPKPhysicsSubsystem::UpdateQuadTree()
{
	if (QuadTree)
	{
		QuadTree->Clear();
		QuadTree->Initialize(WorldBoundsMin, WorldBoundsMax, MaxEntitiesPerNode, MaxDepth);
	}
	else
	{
		QuadTree = NewObject<UPKQuadTree>(UPKQuadTree::StaticClass());
		QuadTree->Initialize(WorldBoundsMin, WorldBoundsMax, MaxEntitiesPerNode, MaxDepth);
	}
}

void UPKPhysicsSubsystem::Deinitialize()
{
	Super::Deinitialize();

	if (QuadTree)
	{
		QuadTree = nullptr;
	}
}

void UPKPhysicsSubsystem::Tick(float DeltaTime)
{
	// Ensure the EntityManagerSubsystem is valid
	if (!EntityManagerSubsystem)
	{
		EntityManagerSubsystem = GetWorld()->GetSubsystem<UPKEntityManagerSubsystem>();
		if (!EntityManagerSubsystem)
		{
			return;
		}
	}

	// Ensure the ComponentManager is valid
	if (!ComponentManager)
	{
		return;
	}

	// Clear the QuadTree for the current frame
	QuadTree->Clear();

	// Update physics components and insert them into the QuadTree
	for (int32 i = 0; i < ComponentManager->PhysicsComponents.Num(); ++i)
	{
		PKPhysicsComponent& PhysicsComponent = ComponentManager->PhysicsComponents[i];

		// Skip components that are not simulating
		if (!PhysicsComponent.bIsSimulating)
		{
			continue;
		}

		// Get the transform component associated with the entity
		PKTransformComponent* TransformComponent = ComponentManager->GetTransformComponent(PhysicsComponent.EntityID);
		if (!TransformComponent)
		{
			continue;
		}

		// Update velocity and apply friction
		PhysicsComponent.Velocity += PhysicsComponent.Acceleration * DeltaTime;
		float Friction = QuadTree->GetFrictionForPosition(FVector2D(TransformComponent->Position.X, TransformComponent->Position.Y));
		PhysicsComponent.Velocity *= Friction;

		// Update the position based on the velocity
		TransformComponent->Position.X += PhysicsComponent.Velocity.X * DeltaTime;
		TransformComponent->Position.Y += PhysicsComponent.Velocity.Y * DeltaTime;
		TransformComponent->Position.Z = 30.f;

		// Handle boundary collisions
		FVector2D RootBoundsMin = QuadTree->RootNode->BoundsMin;
		FVector2D RootBoundsMax = QuadTree->RootNode->BoundsMax;
		HandleBoundaryCollision(TransformComponent->Position, PhysicsComponent.Velocity, PhysicsComponent.Acceleration,
			PhysicsComponent.CollisionRadius, RootBoundsMin, RootBoundsMax);

		// Update the actor's position in the world
		AActor* Actor = EntityManagerSubsystem->GetActorForEntity(PhysicsComponent.EntityID);
		if (Actor)
		{
			Actor->SetActorLocation(TransformComponent->Position);
		}

		// Insert the entity into the QuadTree
		QuadTree->Insert(PhysicsComponent.EntityID, FVector2D(TransformComponent->Position.X, TransformComponent->Position.Y), PhysicsComponent.CollisionRadius);
	}

	// Check for collisions between entities
	for (int32 i = 0; i < ComponentManager->PhysicsComponents.Num(); ++i)
	{
		PKPhysicsComponent& PhysicsComponentA = ComponentManager->PhysicsComponents[i];

		// Skip components that are not simulating
		if (!PhysicsComponentA.bIsSimulating)
		{
			continue;
		}

		// Get the transform component for the first entity
		PKTransformComponent* TransformComponentA = ComponentManager->GetTransformComponent(PhysicsComponentA.EntityID);
		if (!TransformComponentA)
		{
			continue;
		}

		// Query the QuadTree for nearby entities
		TArray<int32> NearbyEntities;
		QuadTree->Query(
			FVector2D(TransformComponentA->Position.X - PhysicsComponentA.CollisionRadius, TransformComponentA->Position.Y - PhysicsComponentA.CollisionRadius),
			FVector2D(TransformComponentA->Position.X + PhysicsComponentA.CollisionRadius, TransformComponentA->Position.Y + PhysicsComponentA.CollisionRadius),
			NearbyEntities
		);

		for (int32 NearbyEntityID : NearbyEntities)
		{
			// Skip self-collision
			if (NearbyEntityID == PhysicsComponentA.EntityID)
			{
				continue;
			}

			// Get the physics and transform components for the nearby entity
			PKPhysicsComponent* PhysicsComponentB = ComponentManager->GetPhysicsComponent(NearbyEntityID);
			PKTransformComponent* TransformComponentB = ComponentManager->GetTransformComponent(NearbyEntityID);

			if (!PhysicsComponentB || !TransformComponentB || !PhysicsComponentB->bIsSimulating)
			{
				continue;
			}

			// Check for collision and resolve it if necessary
			FVector Delta = TransformComponentA->Position - TransformComponentB->Position;
			float DistanceSquared = Delta.SizeSquared();
			float MinDistance = PhysicsComponentA.CollisionRadius + PhysicsComponentB->CollisionRadius;

			if (DistanceSquared < MinDistance * MinDistance)
			{
				ResolveCollision(*TransformComponentA, PhysicsComponentA, *TransformComponentB, *PhysicsComponentB);
			}
		}
	}

	// Draw debug visualization of the QuadTree if debugging is enabled
	if (bShouldDebug)
	{
		QuadTree->DrawDebug(GetWorld(), 0.1f);
	}
}

TStatId UPKPhysicsSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(PKPhysicsSubsystem, STATGROUP_Tickables);
}

// Resolve a collision between two entities
void UPKPhysicsSubsystem::ResolveCollision(PKTransformComponent& TransformA, PKPhysicsComponent& PhysicsA, PKTransformComponent& TransformB, PKPhysicsComponent& PhysicsB)
{
	// Coefficient of restitution for collisions
	const float GlobalRestitution = 0.8f;

	FVector Delta = TransformA.Position - TransformB.Position;
	float Distance = Delta.Size();

	// Handle overlapping entities
	if (Distance == 0.0f)
	{
		Delta = FVector::UpVector;
		Distance = 1.0f;
	}

	// Calculate the collision normal
	FVector Normal = Delta / Distance;

	float MinDistance = PhysicsA.CollisionRadius + PhysicsB.CollisionRadius;
	float PenetrationDepth = MinDistance - Distance;

	// Resolve penetration by moving entities apart
	float TotalMass = PhysicsA.Mass + PhysicsB.Mass;
	float RatioA = PhysicsB.Mass / TotalMass;
	float RatioB = PhysicsA.Mass / TotalMass;

	TransformA.Position += Normal * PenetrationDepth * RatioA;
	TransformB.Position -= Normal * PenetrationDepth * RatioB;

	// Calculate the collision impulse
	FVector RelativeVelocity = PhysicsA.Velocity - PhysicsB.Velocity;
	float VelocityAlongNormal = FVector::DotProduct(RelativeVelocity, Normal);

	if (VelocityAlongNormal > 0)
	{
		return;
	}

	float ImpulseMagnitude = -(1.0f + GlobalRestitution) * VelocityAlongNormal;
	ImpulseMagnitude /= ((1.0f / PhysicsA.Mass) + (1.0f / PhysicsB.Mass));

	FVector Impulse = ImpulseMagnitude * Normal;

	// Apply the impulse to the entities
	PhysicsA.Velocity += Impulse / PhysicsA.Mass;
	PhysicsB.Velocity -= Impulse / PhysicsB.Mass;

	// Draw debug visualization if enabled
	if (bShouldDebug)
	{
		DrawDebugSphere(GetWorld(), TransformA.Position, PhysicsA.CollisionRadius, 12, FColor::Red, false, 1.0f);
		DrawDebugSphere(GetWorld(), TransformB.Position, PhysicsB.CollisionRadius, 12, FColor::Blue, false, 1.0f);
	}
}

void UPKPhysicsSubsystem::HandleBoundaryCollision(FVector& Position, FVector& Velocity, FVector& Acceleration, float CollisionRadius, const FVector2D& BoundsMin, const FVector2D& BoundsMax)
{
	const float Restitution = 0.8f;

	// Check and resolve collisions with the X-axis boundaries
	if (Position.X - CollisionRadius < BoundsMin.X)
	{
		Position.X = BoundsMin.X + CollisionRadius;
		Velocity.X *= -Restitution;
		Acceleration.X *= -Restitution;
	}
	else if (Position.X + CollisionRadius > BoundsMax.X)
	{
		Position.X = BoundsMax.X - CollisionRadius;
		Velocity.X *= -Restitution;
		Acceleration.X *= -Restitution;
	}

	// Check and resolve collisions with the Y-axis boundaries
	if (Position.Y - CollisionRadius < BoundsMin.Y)
	{
		Position.Y = BoundsMin.Y + CollisionRadius;
		Velocity.Y *= -Restitution;
		Acceleration.Y *= -Restitution;
	}
	else if (Position.Y + CollisionRadius > BoundsMax.Y)
	{
		Position.Y = BoundsMax.Y - CollisionRadius;
		Velocity.Y *= -Restitution;
		Acceleration.Y *= -Restitution;
	}

	// Clamp the velocity to a maximum value
	Velocity = Velocity.GetClampedToMaxSize(MaxVelocity);
}