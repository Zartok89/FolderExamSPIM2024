#include "Systems/PKParticleSystem.h"

APKParticleSystem::APKParticleSystem()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APKParticleSystem::BeginPlay()
{
	Super::BeginPlay();

	// Initialize particle arrays to the maximum number of particles
	Positions.SetNum(MaxParticles);
	Velocities.SetNum(MaxParticles);
	Lifetimes.SetNum(MaxParticles);
	Colors.SetNum(MaxParticles);
}

void APKParticleSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Emit new particles if particle emission is enabled
	if (bEmitParticles)
	{
		// Emission timer
		TimeSinceLastEmission += DeltaTime;
		if (TimeSinceLastEmission >= EmissionRate)
		{
			TimeSinceLastEmission = 0.0f;

			EmitParticles(GetActorLocation());
		}
	}

	// Update particle properties
	UpdatePositions(DeltaTime);
	UpdateVelocities(DeltaTime);
	UpdateLifetimes(DeltaTime);
	CleaningUpParticles();

	// Draw each particle as a debug point in the world
	for (int32 i = 0; i < ParticleCount; ++i)
	{
		DrawDebugPoint(GetWorld(), Positions[i], 5.0f, Colors[i], false, -1.0f);
	}
}

// Emit new particles at the specified spawn position
void APKParticleSystem::EmitParticles(const FVector& SpawnPosition)
{
	for (int32 i = 0; i < ParticlesPerEmission && ParticleCount < MaxParticles; ++i)
	{
		// Generate random velocity for the particle
		float ParticleAngleSpread = FMath::RandRange(0.f, 2.f * PI);
		float ParticleVelocitySpread = FMath::RandRange(ParticleMinSpread, ParticleMaxSpread);

		FVector Velocity = FVector(FMath::Cos(ParticleAngleSpread) * ParticleVelocitySpread, FMath::Sin(ParticleAngleSpread) * ParticleVelocitySpread, 0.0f);

		// Add wind effect to the velocity
		Velocity += Wind;

		// Initialize particle properties
		Positions[ParticleCount] = SpawnPosition;
		Velocities[ParticleCount] = Velocity;
		Lifetimes[ParticleCount] = ParticleLifetime;

		// Assign a color to the particle
		if (bRandomParticleColor)
		{
			FColor RandColor = FColor::MakeRandomColor();
			Colors[ParticleCount] = RandColor;
		}
		else
		{
			Colors[ParticleCount] = ParticleColor;
		}

		// Increment the particle count
		++ParticleCount;
	}
}

// Update the positions of all particles based on their velocities
void APKParticleSystem::UpdatePositions(float DeltaTime)
{
	for (int32 i = 0; i < ParticleCount; ++i)
	{
		Positions[i] += Velocities[i] * DeltaTime;
	}
}

// Update the velocities of all particles by applying forces like gravity and wind
void APKParticleSystem::UpdateVelocities(float DeltaTime)
{
	for (int32 i = 0; i < ParticleCount; ++i)
	{
		Velocities[i] += Gravity * DeltaTime;

		Velocities[i] += Wind * DeltaTime;
	}
}

// Update the lifetimes of all particles and adjust their alpha values
void APKParticleSystem::UpdateLifetimes(float DeltaTime)
{
	for (int32 i = 0; i < ParticleCount; ++i)
	{
		Lifetimes[i] -= DeltaTime;

		if (Lifetimes[i] > 0.0f)
		{
			// Calculate the alpha value based on the remaining lifetime
			float Alpha = FMath::Clamp(Lifetimes[i] / ParticleLifetime, 0.0f, 1.0f);
			Colors[i].A = static_cast<uint8>(Alpha * 255);
		}
	}
}

// Remove particles that have expired (lifetime <= 0)
void APKParticleSystem::CleaningUpParticles()
{
	// Index for writing valid particles
	int32 WriteIndex = 0;

	for (int32 ReadIndex = 0; ReadIndex < ParticleCount; ++ReadIndex)
	{
		// If the particle is still alive
		if (Lifetimes[ReadIndex] > 0.0f)
		{
			// Copy data from the current index to the write index
			Positions[WriteIndex] = Positions[ReadIndex];
			Velocities[WriteIndex] = Velocities[ReadIndex];
			Lifetimes[WriteIndex] = Lifetimes[ReadIndex];
			Colors[WriteIndex] = Colors[ReadIndex];

			// Move to the next write index
			++WriteIndex;
		}
	}

	// Update the particle count to the number of valid particles
	ParticleCount = WriteIndex;
}