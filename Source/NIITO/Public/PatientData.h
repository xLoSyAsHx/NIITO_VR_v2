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
};
