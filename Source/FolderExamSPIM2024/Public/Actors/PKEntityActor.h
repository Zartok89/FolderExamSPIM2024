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
	int32 GetEntityID() const { return EntityID; }
	void SetEntityID(int32 NewID) { EntityID = NewID; }

	/*
	 * Variables
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity")
	UStaticMeshComponent* EntityMesh;

};
