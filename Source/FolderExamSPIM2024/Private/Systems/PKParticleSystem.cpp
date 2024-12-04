#include "Systems/PKParticleSystem.h"

APKParticleSystem::APKParticleSystem()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APKParticleSystem::BeginPlay()
{
	Super::BeginPlay();

	Positions.SetNum(MaxParticles);
	Velocities.SetNum(MaxParticles);
	Lifetimes.SetNum(MaxParticles);
	Colors.SetNum(MaxParticles);
}

void APKParticleSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bEmitParticles)
	{
		TimeSinceLastEmission += DeltaTime;
		if (TimeSinceLastEmission >= EmissionRate)
		{
			TimeSinceLastEmission = 0.0f;

			EmitParticles(GetActorLocation());
		}
	}

	UpdatePositions(DeltaTime);
	UpdateVelocities(DeltaTime);
	UpdateLifetimes(DeltaTime);
	CleaningUpParticles();

	for (int32 i = 0; i < ParticleCount; ++i)
	{
		DrawDebugPoint(GetWorld(), Positions[i], 5.0f, Colors[i], false, -1.0f);
	}
}

void APKParticleSystem::EmitParticles(const FVector& SpawnPosition)
{
	for (int32 i = 0; i < ParticlesPerEmission && ParticleCount < MaxParticles; ++i)
	{
		float ParticleAngleSpread = FMath::RandRange(0.f, 2.f * PI);
		float ParticleVelocitySpread = FMath::RandRange(ParticleMinSpread, ParticleMaxSpread);

		FVector Velocity = FVector(FMath::Cos(ParticleAngleSpread) * ParticleVelocitySpread, FMath::Sin(ParticleAngleSpread) * ParticleVelocitySpread, 0.0f);

		Velocity += Wind;

		Positions[ParticleCount] = SpawnPosition;
		Velocities[ParticleCount] = Velocity;
		Lifetimes[ParticleCount] = ParticleLifetime;

		if (bRandomParticleColor)
		{
			FColor RandColor = FColor::MakeRandomColor();
			Colors[ParticleCount] = RandColor;
		}
		else
		{
			Colors[ParticleCount] = ParticleColor;
		}

		++ParticleCount;
	}
}

void APKParticleSystem::UpdatePositions(float DeltaTime)
{
	for (int32 i = 0; i < ParticleCount; ++i)
	{
		Positions[i] += Velocities[i] * DeltaTime;
	}
}

void APKParticleSystem::UpdateVelocities(float DeltaTime)
{
	for (int32 i = 0; i < ParticleCount; ++i)
	{
		Velocities[i] += Gravity * DeltaTime;

		Velocities[i] += Wind * DeltaTime;
	}
}

void APKParticleSystem::UpdateLifetimes(float DeltaTime)
{
	for (int32 i = 0; i < ParticleCount; ++i)
	{
		Lifetimes[i] -= DeltaTime;

		if (Lifetimes[i] > 0.0f)
		{
			float Alpha = FMath::Clamp(Lifetimes[i] / ParticleLifetime, 0.0f, 1.0f);
			Colors[i].A = static_cast<uint8>(Alpha * 255);
		}
	}
}

void APKParticleSystem::CleaningUpParticles()
{
	int32 WriteIndex = 0;

	for (int32 ReadIndex = 0; ReadIndex < ParticleCount; ++ReadIndex)
	{
		if (Lifetimes[ReadIndex] > 0.0f)
		{
			Positions[WriteIndex] = Positions[ReadIndex];
			Velocities[WriteIndex] = Velocities[ReadIndex];
			Lifetimes[WriteIndex] = Lifetimes[ReadIndex];
			Colors[WriteIndex] = Colors[ReadIndex];
			++WriteIndex;
		}
	}

	ParticleCount = WriteIndex;
}