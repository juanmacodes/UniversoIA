//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "PedestrianPathV2.generated.h"

UCLASS()
class PEDESTRIAN_SYSTEM_API APedestrianPathV2 : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APedestrianPathV2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//----------------------------------------------------------------------------------------------------------------------
	//Spline functions
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pedestrian Path")
	USplineComponent* SplineComponent;

	UFUNCTION()
	void PlaceNPCSpawners();

	UFUNCTION()
	void PlaceNPCPathPoints();


//----------------------------------------------------------------------------------------------------------------------
//Spawner and Pathpoint loop variables

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pedestrian Path")
	float DistanceBetweenSpawners = 2000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pedestrian Path")
	float DistanceBetweenPathPoints = 600;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pedestrian Path")
	TSubclassOf<AActor> SpawnerActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pedestrian Path")
	TSubclassOf<AActor> PathPointActor;

	//----------------------------------------------------------------------------------------------------------------------
	//Snap to other spline Variables and functions

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pedestrian Path")
	AActor* SelectedActorWithSpline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pedestrian Path")
	float CopyZOffset = 0.0f;

	UFUNCTION(CallInEditor, Category = "Spline Copy")
	void CopySelectedSpline();

	
	
};
