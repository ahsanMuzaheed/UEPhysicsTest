// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CubeActor.generated.h"

class UStaticMeshComponent;

USTRUCT()
struct FMySecondaryTickFunction : public FTickFunction
{
	GENERATED_USTRUCT_BODY()

	class ACubeActor* Target;

	PHYSICSTEST_API virtual void ExecuteTick(
		float DeltaTime,
		ELevelTick TickType,
		ENamedThreads::Type CurrentThread,
		const FGraphEventRef& MyCompletionGraphEvent) override;

	PHYSICSTEST_API virtual FString DiagnosticMessage() override;
};

UCLASS()
class PHYSICSTEST_API ACubeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACubeActor(const class FObjectInitializer& PCIP);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Parameters")
		float StartVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Parameters")
		float ForceApplied;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Parameters")
		float ForceVariationPeriod;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Parameters")
		float Kp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Parameters")
		float Ki;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Parameters")
		float Kd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Parameters")
		bool bSubstepEnabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics Parameters")
		bool bEnableLogging;

	UPROPERTY(VisibleAnywhere, Category = "Linear Damping Analysis", meta = (ToolTip = "Natural frequency (Hz)"))
		float W0;

	UPROPERTY(VisibleAnywhere, Category = "Linear Damping Analysis", meta = (ToolTip = "Natural period (ms)"))
		float Period;

	UPROPERTY(VisibleAnywhere, Category = "Linear Damping Analysis", meta = (ToolTip = "Damping Ratio (Z=1: Critical, Z>1: Over, 0<=Z<1: Under)"))
		float Z;

	UPROPERTY(VisibleAnywhere, Category = "Linear Damping Analysis", meta = (ToolTip = "Damping Frequency (Hz)"))
		float Wd;

	UPROPERTY(VisibleAnywhere, Category = "Linear Damping Analysis", meta = (ToolTip = "Peak Frequency (Hz)"))
		float WPeak;

	UPROPERTY(VisibleAnywhere, Category = "Linear Damping Analysis", meta = (ToolTip = "Max Gain at Peak Frequency"))
		float MaxGain;

	UPROPERTY(EditDefaultsOnly, Category = "Tick")
	FMySecondaryTickFunction SecondaryActorTick;

#if WITH_EDITORONLY_DATA
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void PostInitializeComponents() override;
	virtual void PostInitProperties() override;

	void MainTick(float DeltaTime);
	void SubstepTick(float DeltaTime);

	void TickPostPhysics(float DeltaSeconds, ELevelTick TickType, FMySecondaryTickFunction& ThisTickFunction);

private:
	float StartH;
	float LastError, ErrorIntegration;
	float LastDeltaTime;

	void DoPhysics(float DeltaTime, bool InSubstep);
	void DoFloater(float DeltaTime, bool InSubstep);

	void UpdateMotionAnalysis(float DeltaTime);
	void UpdateLinearDampingDescriptors();

	float ClampForce(float Force, float DeltaTime);
	float GetAppliedforce(float DeltaTime);

	UStaticMeshComponent *Cube;
	UTextRenderComponent *DebugPanel;

	int32 FrameCount;

	bool bApplyForce;

	float CurrentAppliedForce;
};
