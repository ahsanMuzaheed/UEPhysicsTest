// Fill out your copyright notice in the Description page of Project Settings.

#include "PhysicsTest.h"
#include "PhysicsTestActorComponent.h"


// Sets default values for this component's properties
UPhysicsTestActorComponent::UPhysicsTestActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPhysicsTestActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPhysicsTestActorComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	UE_LOG(LogClass, Log, TEXT("TickComponent: %f"), DeltaTime);

	// Add custom physics forces each tick
	//GetBodyInstance()->AddCustomPhysics(OnCalculateCustomPhysics);
}
/*
void UPhysicsTestActorComponent::CustomPhysics(float DeltaTime, FBodyInstance* BodyInstance)
{
	//UE_LOG(LogClass, Log, TEXT("CustomPhysics: %s"), DeltaTime);

	URailroadWheelComponent* RailroadWheelComponent = Cast<URailroadWheelComponent>(BodyInstance->OwnerComponent.Get());
	if (!RailroadWheelComponent) return;

	// Calculate custom forces
	//.....

	// You will have to apply forces directly to PhysX body if you want to apply forces! If you want to read body coordinates, read them from PhysX bodies! This is important for sub-steps as transformations aren't updated until end of physics tick
	PxRigidBody* PRigidBody = BodyInstance->GetPxRigidBody();
	PxTransform PTransform = PRigidBody->getGlobalPose();
	PxVec3 PVelocity = PRigidBody->getLinearVelocity();
	PxVec3 PAngVelocity = PRigidBody->getAngularVelocity();

	//......

	PRigidBody->addForce(PForce, PxForceMode::eFORCE, true);
	PRigidBody->addTorque(PTorque, PxForceMode::eFORCE, true);
}
*/