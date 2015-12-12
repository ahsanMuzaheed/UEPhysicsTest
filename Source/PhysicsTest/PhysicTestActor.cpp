// Fill out your copyright notice in the Description page of Project Settings.

#include "PhysicsTest.h"
#include "PhysicTestActor.h"


// Sets default values
APhysicTestActor::APhysicTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APhysicTestActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APhysicTestActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

