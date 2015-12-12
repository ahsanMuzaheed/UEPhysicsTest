// Fill out your copyright notice in the Description page of Project Settings.

#include "PhysicsTest.h"
#include "CubeActor.h"


// Sets default values
ACubeActor::ACubeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StartVelocity = 100.0f;
	KElasticity = 1.0f;

	SetTickGroup(ETickingGroup::TG_PrePhysics);
}

// Called when the game starts or when spawned
void ACubeActor::BeginPlay()
{
	Super::BeginPlay();
	
	StartH = GetActorLocation().Z;

	mesh = Cast<UStaticMeshComponent>(GetRootComponent());

	if(mesh)
		mesh->SetPhysicsLinearVelocity(FVector(0.0f, 0.0f, StartVelocity));
}

// Called every frame
void ACubeActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );	

	DoPhysics(DeltaTime);
}

void ACubeActor::DoPhysics(float DeltaTime)
{
	float DeltaH = GetActorLocation().Z - StartH;

	if (mesh)
		mesh->AddForce(FVector(0.0f, 0.0f, -DeltaH * KElasticity));

	/*UE_LOG(LogClass, Log, TEXT("ACubeActor::DoPhysics - deltaH %f, dt %f, F %f"),
		GetActorLocation().Z - StartH,
		DeltaTime,
		-DeltaH * KElasticity
		);*/
}
