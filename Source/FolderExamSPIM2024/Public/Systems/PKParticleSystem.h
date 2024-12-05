#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PKParticleSystem.generated.h"

UCLASS()
class FOLDEREXAMSPIM2024_API APKParticleSystem : public AActor
{
	GENERATED_BODY()

public:
	APKParticleSystem();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	void EmitParticles(const FVector& SpawnPosition);
	void UpdatePositions(float DeltaTime);
	void UpdateVelocities(float DeltaTime);
	void UpdateLifetimes(float DeltaTime);
	void CleaningUpParticles();

	/*
	 * Variables
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle System")
	bool bRandomParticleColor = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle System")
	FColor ParticleColor = FColor::Emerald;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle System")
	int32 MaxParticles = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle System")
	float ParticleLifetime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle System")
	float ParticleMinSpread = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle System")
	float ParticleMaxSpread = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle System")
	int32 ParticlesPerEmission = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle System")
	bool bEmitParticles = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle System")
	float EmissionRate = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle System", meta = (MakeEditWidget = "true"))
	FVector Gravity = FVector(0.0f, 0.0f, -980.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle System", meta = (MakeEditWidget = "true"))
	FVector Wind = FVector(200.0f, 0.0f, 0.0f);

	/*Structures of Arrays*/
	TArray<FVector> Positions;
	TArray<FVector> Velocities;
	TArray<float> Lifetimes;
	TArray<FColor> Colors;

	int32 ParticleCount = 0;
	float TimeSinceLastEmission = 0.0f;
};
