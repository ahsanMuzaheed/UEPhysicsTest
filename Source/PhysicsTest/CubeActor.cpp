// Fill out your copyright notice in the Description page of Project Settings.

#include "PhysicsTest.h"

#include "PhysXIncludes.h"
#include "PhysicsPublic.h"
#include "Runtime/Engine/Private/PhysicsEngine/PhysXSupport.h"

#include "MyStaticMeshComponent.h"

#include "CubeActor.h"

#define LOCTEXT_NAMESPACE "foo"

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

	SecondaryActorTick.TickGroup = TG_PrePhysics;
	SecondaryActorTick.bCanEverTick = true;
	SecondaryActorTick.bStartWithTickEnabled = true;

	// Init parameters
	StartVelocity = 1000.0f;
	ForceApplied = 0.0f;
	LastError = 0;
	ErrorIntegration = 0;
	LastDeltaTime = 0;
	Kp = 100.0f;
	Ki = 0;
	Kd = 5;
	ForceVariationPeriod = 1;

	CurrentAppliedForce = 0;

	FrameCount = 0;
	bApplyForce = false;
	bEnableLogging = false;
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("StaticMesh'/Engine/BasicShapes/Cube'"));

	USceneComponent *root = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root"));
	SetRootComponent(root);

	Cube = PCIP.CreateDefaultSubobject <UMyStaticMeshComponent>(this, TEXT("Cube"));
	Cube->SetStaticMesh(CubeMesh.Object);
	Cube->AttachTo(root);

	DebugPanel = PCIP.CreateDefaultSubobject <UTextRenderComponent>(this, TEXT("Debug panel"));
	DebugPanel->SetRelativeLocation(FVector(0, -100, 0));
	DebugPanel->AttachTo(root);
	DebugPanel->SetXScale(2);
	DebugPanel->SetYScale(2);
	DebugPanel->SetText(FText::FromString(TEXT("")));
	DebugPanel->SetTextRenderColor(FColor(0, 0, 0));
}

#if WITH_EDITORONLY_DATA
void ACubeActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	UE_LOG(LogClass, Log, TEXT("ACubeActor::PostEditChangeProperty %s"), *PropertyName.ToString());

	if (PropertyName == TEXT("StartVelocity"))
	{
		bTest = StartVelocity;
	}
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

	if (Cube) {
		Cube->SetSimulatePhysics(true);
		Cube->SetEnableGravity(false);
		Cube->SetMassOverrideInKg();
		Cube->SetAngularDamping(0);
		Cube->SetLinearDamping(0);

		Cube->SetConstraintMode(EDOFMode::SixDOF);

		FBodyInstance *bi = Cube->GetBodyInstance();
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

	if(Cube)
		Cube->SetPhysicsLinearVelocity(FVector(0.0f, 0.0f, StartVelocity));
}

// Called every frame
void ACubeActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );	

	FrameCount++;

	if (GetGameInstance()->GetFirstLocalPlayerController()->WasInputKeyJustPressed(EKeys::SpaceBar)) {
		bApplyForce = !bApplyForce;

		UE_LOG(LogClass, Log, TEXT("Apply force: %s"), bApplyForce ? TEXT("True") : TEXT("False"));
	}

	if(bEnableLogging)
		UE_LOG(LogClass, Log, TEXT("%d ACubeActor::Tick DeltaTime: %f, Z: %f"), FrameCount, DeltaTime, Cube->GetComponentLocation().Z);

	if (!bSubstepEnabled)
		MainTick(DeltaTime);

	UpdateAnalysisValues(DeltaTime);
}

void ACubeActor::MainTick(float DeltaTime) 
{
	if (bEnableLogging)
		UE_LOG(LogClass, Log, TEXT("%d ACubeActor::MainTick DeltaTime: %f"), FrameCount, DeltaTime);

	DoPhysics(DeltaTime, false);
}

void ACubeActor::SubstepTick(float DeltaTime)
{
	physx::PxRigidBody* PRigidBody = Cube->GetBodyInstance()->GetPxRigidBody_AssumesLocked();
	PxTransform PTransform = PRigidBody->getGlobalPose();

	if (bEnableLogging)
		UE_LOG(LogClass, Log, TEXT("%d ACubeActor::SubstepTick DeltaTime: %f, Z: %f"), FrameCount, DeltaTime, PTransform.p.z);

	DoPhysics(DeltaTime, true);
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
				if (bEnableLogging)
					UE_LOG(LogClass, Log, TEXT("%d ACubeActor::TickPostPhysics DeltaTime: %f, Z: %f"), FrameCount, DeltaSeconds, Cube->GetComponentLocation().Z);
			}
		}
	}
}

void ACubeActor::DoPhysics(float DeltaTime, bool InSubstep)
{
	if (!Cube) return;

	DoFloater(DeltaTime, InSubstep);
}

void ACubeActor::DoFloater(float DeltaTime, bool InSubstep)
{
	float CurrError = 0;

	if (InSubstep) {
		physx::PxRigidBody* PRigidBody = Cube->GetBodyInstance()->GetPxRigidBody_AssumesLocked();
		PxTransform PTransform = PRigidBody->getGlobalPose();

		CurrError = PTransform.p.z - StartH;
	}
	else {
		CurrError = Cube->GetComponentLocation().Z - StartH;
	}

	ErrorIntegration += CurrError * DeltaTime;
	float ErrorDerivative = LastDeltaTime == 0 ? 0 : (CurrError - LastError) / LastDeltaTime;

	float force = -(CurrError * Kp + ErrorIntegration * Ki + ErrorDerivative * Kd);

	force = ClampForce(force, DeltaTime) + GetAppliedforce(DeltaTime);

	if (InSubstep) {
		physx::PxRigidBody* PRigidBody = Cube->GetBodyInstance()->GetPxRigidBody_AssumesLocked();
		PRigidBody->addForce(PxVec3(0.0f, 0.0f, force), physx::PxForceMode::eFORCE, true);
	}
	else {
		Cube->AddForce(FVector(0.0f, 0.0f, force));
	}

	LastDeltaTime = DeltaTime;
	LastError = CurrError;

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

float ACubeActor::ClampForce(float Force, float DeltaTime) {
	return Force;
}

float ACubeActor::GetAppliedforce(float DeltaTime) {
	float TargetAppliedforce = 0;

	if (bApplyForce) TargetAppliedforce = ForceApplied;
	if (ForceVariationPeriod == 0) ForceVariationPeriod = 1;

	int d = CurrentAppliedForce < TargetAppliedforce ? 1 : -1;
	float dt = FMath::Abs(DeltaTime * ForceApplied / ForceVariationPeriod);

	if (FMath::Abs(TargetAppliedforce - CurrentAppliedForce) < dt) {
		CurrentAppliedForce = TargetAppliedforce;
	}
	else {
		CurrentAppliedForce += d * dt;
		//UE_LOG(LogClass, Log, TEXT("Applied force: %f, Target: %f"), CurrentAppliedForce, TargetAppliedforce);
	}

	return CurrentAppliedForce;
}

void ACubeActor::UpdateAnalysisValues(float DeltaTime)
{
	static int32 lastDir = 0, lastH = 0;
	static double lastPeriodStart = 0, currPeriod = 0, currAmplitude = 0;

	float currH = Cube->GetComponentLocation().Z - StartH;
	int currDir = currH > lastH ? 1 : -1;

	int32 Seconds;
	float PartialSeconds;
	UGameplayStatics::GetAccurateRealTime(GetWorld(), Seconds, PartialSeconds);
	double Time = (double)Seconds + PartialSeconds;

	if (lastPeriodStart == 0) lastPeriodStart = Time;
	else {
		if (lastDir != currDir) {
			currPeriod = Time - lastPeriodStart;
			currAmplitude = FMath::Abs(currH);

			lastPeriodStart = Time;
		}
	}

	int32 fps = FMath::RoundToInt(1 / DeltaTime);
	DebugPanel->SetText(FText::Format(LOCTEXT("Debug Panel", "{0} FPS\n{1} sec\n{2} Amplitude"), FText::AsNumber(fps), FText::AsNumber(currPeriod), FText::AsNumber(currAmplitude)));

	lastDir = currDir;
	lastH = currH;
}

#undef LOCTEXT_NAMESPACE