// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PKComponentManager.h"
#include "PKEntityManagerSubsystem.h"
#include "UPKQuadTree.h"
#include "Subsystems/WorldSubsystem.h"
#include "PKPhysicsSubsystem.generated.h"

/**
 *
 */
UCLASS()
class FOLDEREXAMSPIM2024_API UPKPhysicsSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable)
	void UpdateQuadTree();

	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	void ResolveCollision(PKTransformComponent& TransformA, PKPhysicsComponent& PhysicsA, PKTransformComponent& TransformB, PKPhysicsComponent& PhysicsB);

	void HandleBoundaryCollision(FVector& Position, FVector& Velocity, FVector& Acceleration, float CollisionRadius, const FVector2D& BoundsMin, const FVector2D& BoundsMax);



	/*
	 * Variables
	 */
	UPROPERTY()
	UPKEntityManagerSubsystem* EntityManagerSubsystem;

	PKComponentManager* ComponentManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bShouldDebug;

	/*QuadTree*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	UPKQuadTree* QuadTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
	FVector2D WorldBoundsMin = FVector2D(-10000.0f, -5000.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
	FVector2D WorldBoundsMax = FVector2D(10000.0f, 5000.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
	int MaxDepth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
	int MaxEntitiesPerNode;

	float MaxVelocity = 1000.0f;
};
