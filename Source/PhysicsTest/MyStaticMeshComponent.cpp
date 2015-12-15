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

	//SetTickGroup(ETickingGroup::TG_PrePhysics);
	SetTickGroup(ETickingGroup::TG_PostPhysics);

	owner = Cast<ACubeActor>(GetOwner());

	FrameCount = 0;
}

void UMyStaticMeshComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMyStaticMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FrameCount++;

	//UE_LOG(LogClass, Log, TEXT("%d UMyStaticMeshComponent::TickComponent - DeltaTime: %f, Z: %f"), FrameCount, DeltaTime, GetComponentLocation().Z);

	if(owner->bSubstepEnabled)
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

void UMyStaticMeshComponent::PostPhysicsTick(FPrimitiveComponentPostPhysicsTickFunction &ThisTickFunction) {
	UE_LOG(LogClass, Log, TEXT("UMyStaticMeshComponent::PostPhysicsTick: %f"), ThisTickFunction.TickInterval);
}