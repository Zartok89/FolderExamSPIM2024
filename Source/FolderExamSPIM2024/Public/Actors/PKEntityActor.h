#pragma once

#include "CoreMinimal.h"
#include "PKBaseEntity.h"
#include "GameFramework/Actor.h"
#include "PKEntityActor.generated.h"

UCLASS()
class FOLDEREXAMSPIM2024_API APKEntityActor : public AActor, public PKBaseEntity
{
	GENERATED_BODY()

public:
	APKEntityActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	void SetTransformIndex(int32 Index) { TransformIndex = Index; }
	void SetPhysicsIndex(int32 Index) { PhysicsIndex = Index; }

	int32 GetTransformIndex() const { return TransformIndex; }
	int32 GetPhysicsIndex() const { return PhysicsIndex; }

	void SetEntityID(int32 ID) { EntityID = ID; }
	int32 GetEntityID() const { return EntityID; }
	/*
	 * Variables
	 */
	int32 TransformIndex;
	int32 PhysicsIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity")
	UStaticMeshComponent* EntityMesh;
};
