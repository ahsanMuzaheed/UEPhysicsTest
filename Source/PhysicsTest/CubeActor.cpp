// Fill out your copyright notice in the Description page of Project Settings.

#include "PhysicsTest.h"

#include "PhysXIncludes.h"
#include "PhysicsPublic.h"
#include "Runtime/Engine/Private/PhysicsEngine/PhysXSupport.h"

#include "MyStaticMeshComponent.h"

#include "CubeActor.h"

void FMySecondaryTickFunction::ExecuteTick(
	float DeltaTime,
	ELevelTick TickType,
	ENamedThreads::Type CurrentThread,
	const FGraphEventRef& MyCompletionGraphEvent)
{
	if (Target && !Target->HasAnyFlags(RF_PendingKill | RF_Unreachable))
	{
		FScopeCycleCounterUObject ActorScope(Target);
		Target->TickPostPhysics(DeltaTime*Target->CustomTimeDilation, TickType, *this);
	}
}

FString FMySecondaryTickFunction::DiagnosticMessage()
{
	return Target->GetFullName() + TEXT("[TickActor2]");
}

void ACubeActor::TickPostPhysics(
	float DeltaSeconds,
	ELevelTick TickType,
	FMySecondaryTickFunction& ThisTickFunction
	)
{
	// Non-player update.
	const bool bShouldTick =
		((TickType != LEVELTICK_ViewportsOnly) || ShouldTickIfViewportsOnly());
	if (bShouldTick)
	{
		if (!IsPendingKill() && GetWorld())
		{
			if (GetWorldSettings() != NULL &&
				(bAllowReceiveTickEventOnDedicatedServer || !IsRunningDedicatedServer()))
			{
				//My cool post physics tick stuff
				UE_LOG(LogClass, Log, TEXT("%d ACubeActor::TickActor2 DeltaSeconds: %f"), FrameCount, DeltaSeconds);
			}
		}
	}
}

// Sets default values
ACubeActor::ACubeActor(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	UE_LOG(LogClass, Log, TEXT("ACubeActor::ACubeActor Constructor"));

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//SetTickGroup(ETickingGroup::TG_PrePhysics);
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	SecondaryActorTick.TickGroup = TG_PostPhysics;
	SecondaryActorTick.bCanEverTick = true;
	SecondaryActorTick.bStartWithTickEnabled = true;

	// Harmonic
	StartVelocity = 1000.0f;
	KElasticity = 100.0f;

	// Floater 
	lasterror = 0;
	errorsum = 0;
	Kp = 100.0f;
	Ki = 0;
	Kd = 5;

	FrameCount = 0;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("StaticMesh'/Engine/BasicShapes/Cube'"));

	cube = Cast<UStaticMeshComponent>(PCIP.CreateDefaultSubobject <UMyStaticMeshComponent>(this, TEXT("Cube")));
	cube->SetStaticMesh(CubeMesh.Object);
}

#if WITH_EDITORONLY_DATA
void ACubeActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	UE_LOG(LogClass, Log, TEXT("ACubeActor::PostEditChangeProperty %s"), *PropertyName.ToString());
}
#endif

void ACubeActor::PostInitProperties()
{
	Super::PostInitProperties();
	if (!IsTemplate() && SecondaryActorTick.bCanEverTick)
	{
		SecondaryActorTick.Target = this;
		SecondaryActorTick.SetTickFunctionEnable(SecondaryActorTick.bStartWithTickEnabled);
		SecondaryActorTick.RegisterTickFunction(GetLevel());
	}
}

void ACubeActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UE_LOG(LogClass, Log, TEXT("ACubeActor::PostInitializeComponents"));

	if (cube) {
		cube->SetSimulatePhysics(true);
		cube->SetEnableGravity(false);
		cube->SetMassOverrideInKg();
		cube->SetAngularDamping(0);
		cube->SetLinearDamping(0);

		cube->SetConstraintMode(EDOFMode::SixDOF);

		FBodyInstance *bi = cube->GetBodyInstance();
		if (bi) {
			bi->bLockXTranslation = true;
			bi->bLockYTranslation = true;
			bi->bLockZTranslation = false;

			bi->bLockXRotation = true;
			bi->bLockYRotation = true;
			bi->bLockZRotation = true;
		}
	}
}

// Called when the game starts or when spawned
void ACubeActor::BeginPlay()
{
	Super::BeginPlay();
	
	StartH = GetActorLocation().Z;

	if(cube)
		cube->SetPhysicsLinearVelocity(FVector(0.0f, 0.0f, StartVelocity));
}

// Called every frame
void ACubeActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );	

	FrameCount++;

	UE_LOG(LogClass, Log, TEXT("%d ACubeActor::Tick DeltaTime: %f"), FrameCount, DeltaTime);
}

void ACubeActor::MainTick(float DeltaTime) 
{
	UE_LOG(LogClass, Log, TEXT("%d ACubeActor::MainTick DeltaTime: %f"), FrameCount, DeltaTime);

	DoPhysics(DeltaTime, false);
}

void ACubeActor::SubstepTick(float DeltaTime)
{
	UE_LOG(LogClass, Log, TEXT("%d ACubeActor::SubstepTick DeltaTime: %f"), FrameCount, DeltaTime);

	DoPhysics(DeltaTime, true);
}

void ACubeActor::DoPhysics(float DeltaTime, bool InSubstep)
{
	if (!cube) return;

	if (bUseFloater)
		DoFloater(DeltaTime, InSubstep);
	else
		DoHarmonic(DeltaTime, InSubstep);
}

void ACubeActor::DoHarmonic(float DeltaTime, bool InSubstep)
{
	float DeltaH = cube->GetComponentLocation().Z - StartH;

	if (InSubstep) {
		physx::PxRigidBody* PRigidBody = cube->GetBodyInstance()->GetPxRigidBody_AssumesLocked();
		PRigidBody->addForce(PxVec3(0.0f, 0.0f, -DeltaH * KElasticity), physx::PxForceMode::eFORCE, true);
	}
	else {
		cube->AddForce(FVector(0.0f, 0.0f, -DeltaH * KElasticity));
	}

	/*UE_LOG(LogClass, Log, TEXT("UMyStaticMeshComponent::DoPhysics - deltaH %f, dt %f, mass %f, F %f"),
	GetComponentLocation().Z - StartH,
	DeltaTime,
	GetMass(),
	-DeltaH * KElasticity
	);*/
}

void ACubeActor::DoFloater(float DeltaTime, bool InSubstep)
{
	float prevError = lasterror;
	lasterror = cube->GetComponentLocation().Z - StartH;
	errorsum += lasterror;

	float force = -(lasterror * Kp + +errorsum * Ki + (lasterror - prevError) * Kd / DeltaTime);

	if (InSubstep) {
		physx::PxRigidBody* PRigidBody = cube->GetBodyInstance()->GetPxRigidBody_AssumesLocked();
		PRigidBody->addForce(PxVec3(0.0f, 0.0f, force), physx::PxForceMode::eFORCE, true);
	}
	else {
		cube->AddForce(FVector(0.0f, 0.0f, force));
	}

	/* REDOUT PID Controller Code

	double prevError = RayHistory->LastError;
	RayHistory->LastError = TargetDistanceFromGround - RayHistory->TrackDistance;
	RayHistory->LastErrorF = RayHistory->LastError;
	RayHistory->ErrorSum = RayHistory->ErrorSum + RayHistory->LastError;

	floatSys.PrevForce = floatSys.Force;

	floatSys.Force = ((RayHistory->LastError - prevError) * floatSys.Kd / DeltaTick +
	RayHistory->LastError * floatSys.Kp +
	RayHistory->ErrorSum * floatSys.Ki)
	* floatSys.Power;
	*/
}