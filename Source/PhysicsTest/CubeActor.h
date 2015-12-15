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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Harmonic Motion")
		float StartVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Harmonic Motion")
		float KElasticity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floater")
		bool bUseFloater;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floater")
		float Kp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floater")
		float Ki;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floater")
		float Kd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
		bool bSubstepEnabled;

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
	float lasterror, errorsum;

	void DoPhysics(float DeltaTime, bool InSubstep);
	void DoHarmonic(float DeltaTime, bool InSubstep);
	void DoFloater(float DeltaTime, bool InSubstep);

	UStaticMeshComponent *cube;

	int32 FrameCount;
};
