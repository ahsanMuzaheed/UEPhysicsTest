// Fill out your copyright notice in the Description page of Project Settings.

#include "PhysicsTest.h"
#include "MyGameMode.h"



void AMyGameMode::StartPlay()
{	
	Super::StartPlay();

	//get main player controller
	APlayerController* controller = UGameplayStatics::GetPlayerController(this, 0);
	if (controller)
	{
		controller->ConsoleCommand(FString(TEXT("t.MaxFPS 30")), true);
		controller->ConsoleCommand(FString(TEXT("r.Vsync true")), true);
	}
}
