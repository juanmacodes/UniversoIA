// MetaHumanFaceDriverAnimInstance.h

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MetaHumanFaceDriverAnimInstance.generated.h"

UCLASS(Blueprintable, BlueprintType, Transient)
class UNIVERSOIA26_API UMetaHumanFaceDriverAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LipSync")
	float TalkAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LipSync")
	float JawOpenAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LipSync")
	float MouthOpenAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LipSync")
	float MouthNarrowAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LipSync")
	float MouthWideAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LipSync")
	float HeadBobAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LipSync|Visemes")
	float VisemeMBP = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LipSync|Visemes")
	float VisemeFV = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LipSync|Visemes")
	float VisemeOO = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LipSync|Visemes")
	float VisemeEE = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LipSync|Visemes")
	float VisemeAA = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LipSync|Visemes")
	float VisemeSH = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LipSync")
	bool bIsTalking = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LipSync")
	float BlinkAlpha = 0.0f;
};