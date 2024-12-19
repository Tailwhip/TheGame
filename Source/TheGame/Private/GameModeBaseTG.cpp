// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeBaseTG.h"

#include "TheGame/TheGame.h"

AGameModeBaseTG::AGameModeBaseTG()
{
}

void AGameModeBaseTG::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	UE_LOG(LogTheGame, Log, TEXT("Game is running: %s %s"), *MapName, *Options);
	
}
