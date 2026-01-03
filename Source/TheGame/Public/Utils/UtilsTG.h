// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "TheGame/TheGame.h"


namespace UtilsTG
{

template<class TReturnType, class TClassToConstructByDefault>
TReturnType* CreateDefaultSubobject(UObject* Owner, FName SubobjectName, TSubclassOf<TReturnType> CustomObjectType, bool bTransient = false)
{
	TRACE("")
	// Cast required so we can call protected CreateDefaultSubobject
	auto TypedOwner = CastChecked<AActor>(Owner);
	// If no subclass is assigned, use default
	TSubclassOf<TReturnType> ObjectType = CustomObjectType
		? CustomObjectType
		: TSubclassOf<TReturnType>(TClassToConstructByDefault::StaticClass());
	return static_cast<TReturnType*>(
		TypedOwner->CreateDefaultSubobject(SubobjectName, ObjectType, ObjectType, /*bIsRequired =*/ true, false)
	);
}


} // namespace UtilsTG

