// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class FOLDEREXAMSPIM2024_API PKBaseEntity
{
public:
	PKBaseEntity() : EntityID(-1) {}  
    virtual ~PKBaseEntity() {}  

    virtual int32 GetEntityID() const { return EntityID; }  
    virtual void SetEntityID(int32 NewID) { EntityID = NewID; }

    /*
     * Variables
     */
    int32 EntityID;
};
