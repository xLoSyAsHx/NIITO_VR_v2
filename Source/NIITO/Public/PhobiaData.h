// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhobiaData.generated.h"

/**
 *
 */
USTRUCT(Blueprintable)
struct FPhobiaData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString ID;

    UPROPERTY(BlueprintReadOnly)
    FString NameToDisplay;
};

UCLASS(Blueprintable)
class NIITO_API UPhobiaDataLib : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal FPhobiaData", CompactNodeTitle = "==", Keywords = "== equal"), Category = "Math")
    static bool EqualEqual_FPhobiaDataFPhobiaData(const FPhobiaData& A, const FPhobiaData& B);
};
