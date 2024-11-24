// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PKComponentManager.h"
#include "PKEntityManagerSubsystem.h"
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

	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime);

	virtual TStatId GetStatId() const override;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	void ResolveCollision(PKTransformComponent& TransformA, PKPhysicsComponent& PhysicsA, PKTransformComponent& TransformB, PKPhysicsComponent& PhysicsB);

	/*
	 * Variables
	 */
	UPROPERTY()
	UPKEntityManagerSubsystem* EntityManagerSubsystem;
	PKComponentManager* ComponentManager;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bShouldDebug;
};
