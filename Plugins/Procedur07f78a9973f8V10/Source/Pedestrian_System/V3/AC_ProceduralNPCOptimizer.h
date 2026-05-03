//  Copyright (c) 2025 KomodoBit Games. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SkinnedMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "AC_ProceduralNPCOptimizer.generated.h"

USTRUCT(BlueprintType)
struct FProceduralNPCOptimizationLOD
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Skeletal Mesh Optimizations")
	bool UseBoneMotionBlur = true;

	UPROPERTY(EditAnywhere, Category = "Skeletal Mesh Optimizations")
	bool DeferKinematicBonesUpdate = false;

	UPROPERTY(EditAnywhere, Category = "Skeletal Mesh Optimizations")
	EVisibilityBasedAnimTickOption VisibilityAnimationTick = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;

	UPROPERTY(EditAnywhere, Category = "Skeletal Mesh Optimizations")
	bool EnableUpdateRateOptimizations = true;
	
	UPROPERTY(EditAnywhere, Category = "Skeletal Mesh Optimizations")
	bool RenderSkeletalMeshAsStatic = false;

	UPROPERTY(EditAnywhere, Category = "Skeletal Mesh Optimizations")
	bool SkipBoundsWhenInterpolating = false;

	UPROPERTY(EditAnywhere, Category = "Skeletal Mesh Optimizations")
	bool SkipKinematicUpdateWhenInterpolating = false;

	UPROPERTY(EditAnywhere, Category = "Skeletal Mesh Optimizations")
	bool SkipSkeletalMeshUpdate = false;

	UPROPERTY(EditAnywhere, Category = "Skeletal Mesh Optimizations")
	int SkeletalMeshUpdateRate = 0;

	UPROPERTY(EditAnywhere, Category = "Skeletal Mesh Optimizations")
	bool CastShadow = true;

	UPROPERTY(EditAnywhere, Category = "Behavior Optimizations")
	bool DisableBehaviorTree = false;

	UPROPERTY(EditAnywhere, Category = "Character Movement Optimizations")
	float CharMovementTickRate = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Tick Optimizations")
	float OwnerTickRate = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Optimization Settings")
	float OptimizeTierStartDistance = 0.0;

	UPROPERTY(EditAnywhere, Category = "Optimization Settings")
	float OptimizeTierStopDistance = 0.0;
	
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PEDESTRIAN_SYSTEM_API UAC_ProceduralNPCOptimizer : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAC_ProceduralNPCOptimizer();

	UPROPERTY(EditDefaultsOnly, Category = "Optimizations")
	TArray<FProceduralNPCOptimizationLOD> ProceduralNPCOptimizations;

	UPROPERTY(EditDefaultsOnly, Category= "Optimizations")
	float TimerTickRate;

	
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void OptimizeTimer();

	UFUNCTION()
	void OptimizeNPCLoop(int Index);

	UFUNCTION()
	void ToggleBehaviors(int Index);
	

	


	

		
	
};
