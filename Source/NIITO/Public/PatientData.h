// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PatientData.generated.h"

/**
 * 
 */
USTRUCT(Blueprintable)
struct FPatientData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString FirstName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString SecondName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Diagnose;
};

UCLASS(Blueprintable)
class NIITO_API UPatientDataLib : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal FPatientData", CompactNodeTitle = "==", Keywords = "== equal"), Category = "Math")
    static bool EqualEqual_FPatientDataFPatientData(const FPatientData& A, const FPatientData& B);
};
