// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/PKMeshComponent.h"

UPKMeshComponent::UPKMeshComponent()  
{  
    // Create the static mesh component  
    MeshComponent = NewObject<UStaticMeshComponent>();  

    // Set a default sphere mesh  
    UStaticMesh* DefaultMesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Engine/BasicShapes/Sphere.Sphere")).Object;  
    if (DefaultMesh)  
    {  
        MeshComponent->SetStaticMesh(DefaultMesh);  
    }  

    // Set a default material  
    UMaterialInterface* DefaultMaterial = ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("/Game/Materials/M_EntityMaterial")).Object;  
    if (DefaultMaterial)  
    {  
        MeshComponent->SetMaterial(0, DefaultMaterial);  
        DynamicMaterial = MeshComponent->CreateAndSetMaterialInstanceDynamic(0);  
    }  
}  

void UPKMeshComponent::SetMesh(UStaticMesh* NewMesh)  
{  
    if (MeshComponent && NewMesh)  
    {  
        MeshComponent->SetStaticMesh(NewMesh);  
    }  
}  

void UPKMeshComponent::SetColor(const FLinearColor& NewColor)  
{  
    if (DynamicMaterial)  
    {  
        DynamicMaterial->SetVectorParameterValue("BaseColor", NewColor);  
    }  
}  