// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Agents/MLAdapterAgent.h"
#include "MLAdapterComponentTG.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEGAME_API UMLAdapterComponentTG : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMLAdapterComponentTG();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
								FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(EditAnywhere, meta=(TitleProperty="Name"), Category = "MLAgentTG")
	FMLAdapterAgentConfig Config;

	UPROPERTY(EditAnywhere, meta=(TitleProperty="Name"), Category = "MLAgentTG")
	TMap<FName, FMLAdapterParameterMap> Sensors;
	
	// UPROPERTY(EditAnywhere, Category = "MLAgentTG")
	// void AddActuator(const FName ActuatorName,
	// 				FMLAdapterParameterMap&& Parameters = FMLAdapterParameterMap());

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
// private:
	// TObjectPtr<UMLAdapterLocalDataCollectionSession> Session;
};
