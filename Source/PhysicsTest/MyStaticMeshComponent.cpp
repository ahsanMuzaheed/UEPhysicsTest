// Fill out your copyright notice in the Description page of Project Settings.

#include "PhysicsTest.h"
#include "CubeActor.h"
#include "MyStaticMeshComponent.h"

UMyStaticMeshComponent::UMyStaticMeshComponent()
{
	bWantsBeginPlay = false;

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	OnCalculateCustomPhysics.BindUObject(this, &UMyStaticMeshComponent::CustomPhysics);

	SetTickGroup(ETickingGroup::TG_PrePhysics);

	owner = Cast<ACubeActor>(GetOwner());
}

void UMyStaticMeshComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMyStaticMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//UE_LOG(LogClass, Log, TEXT("UMyStaticMeshComponent::TickComponent - DeltaTime: %f"), DeltaTime);

	if(!owner->bSubstepEnabled)
		owner->MainTick(DeltaTime);
	else 
	{
		// Add custom physics forces each tick
		GetBodyInstance()->AddCustomPhysics(OnCalculateCustomPhysics);
	}
}

void UMyStaticMeshComponent::CustomPhysics(float DeltaTime, FBodyInstance* BodyInstance)
{
	//UE_LOG(LogClass, Log, TEXT("CustomPhysics: %f"), DeltaTime);

	owner->SubstepTick(DeltaTime);
}