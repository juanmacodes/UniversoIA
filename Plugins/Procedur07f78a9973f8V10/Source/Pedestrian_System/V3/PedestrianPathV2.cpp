//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#include "PedestrianPathV2.h"

#include "Components/SplineComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"


// Sets default values
APedestrianPathV2::APedestrianPathV2()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
}

// Called when the game starts or when spawned
void APedestrianPathV2::BeginPlay()
{
	Super::BeginPlay();
	PlaceNPCSpawners();
	PlaceNPCPathPoints();
	
}

void APedestrianPathV2::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

// Called every frame
void APedestrianPathV2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APedestrianPathV2::PlaceNPCSpawners()
{
	if (SplineComponent)
	{
		float SplineLength = SplineComponent->GetSplineLength();

		for (float Distance = DistanceBetweenSpawners; Distance < SplineLength; Distance += DistanceBetweenSpawners)
		{
			FVector SpawnLocation = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
			FRotator SpawnRotation = SplineComponent->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
			FActorSpawnParameters SpawnParameters;
			GetWorld()->SpawnActor<AActor>(SpawnerActor, SpawnLocation, SpawnRotation, SpawnParameters);
		}
	}
}

void APedestrianPathV2::PlaceNPCPathPoints()
{
	if (SplineComponent)
	{
		float SplineLength = SplineComponent->GetSplineLength();

		for (float Distance = DistanceBetweenPathPoints; Distance < SplineLength; Distance += DistanceBetweenPathPoints)
		{
			FVector SpawnLocation = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
			FRotator SpawnRotation = SplineComponent->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
			FActorSpawnParameters SpawnParameters;

			SpawnParameters.Owner = this;
			GetWorld()->SpawnActor<AActor>(PathPointActor, SpawnLocation, SpawnRotation, SpawnParameters);
		}
	}
}

void APedestrianPathV2::CopySelectedSpline()
{
	if (!SelectedActorWithSpline || !SplineComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected actor or local spline is invalid."));
		return;
	}

	if (USplineComponent* SelectedSplineComponent = SelectedActorWithSpline->FindComponentByClass<USplineComponent>())
	{
	

		this->SetActorLocation(SelectedActorWithSpline->GetActorLocation());
		this->SetActorRotation(SelectedActorWithSpline->GetActorRotation());

		SplineComponent->ClearSplinePoints(false);
		
		int32 NumberOfPoints = SelectedSplineComponent->GetNumberOfSplinePoints();

		for (int32 i = 0; i < NumberOfPoints; i++)
		{
			FVector WorldLocation = SelectedSplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
			FVector WorldTangent = SelectedSplineComponent->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::World);
			ESplinePointType::Type PointType = SelectedSplineComponent->GetSplinePointType(i);

			WorldLocation.Z += CopyZOffset;

			// Convert to local space for this spline
			FVector LocalLocation = SplineComponent->GetComponentTransform().InverseTransformPosition(WorldLocation);
			FVector LocalTangent = SplineComponent->GetComponentTransform().InverseTransformVectorNoScale(WorldTangent);

			FSplinePoint NewPoint(i, LocalLocation);
			NewPoint.ArriveTangent = LocalTangent;
			NewPoint.LeaveTangent = LocalTangent;
			NewPoint.Type = PointType;

			// Add the point to the spline component
			SplineComponent->AddPoint(NewPoint, false);
		}

		SplineComponent->UpdateSpline();
		
	}
}

