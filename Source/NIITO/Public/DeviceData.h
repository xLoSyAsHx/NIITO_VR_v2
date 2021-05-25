// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DeviceData.generated.h"

/**
 *
 */
USTRUCT(Blueprintable)
struct FDeviceData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString ID;

    UPROPERTY(BlueprintReadOnly)
    FString DeviceUUID;
};

class APlayerController;

UCLASS(Blueprintable)
class NIITO_API UDeviceDataLib : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal FDeviceData", CompactNodeTitle = "==", Keywords = "== equal"), Category = "Math")
    static bool EqualEqual_FDeviceDataFDeviceData(const FDeviceData& A, const FDeviceData& B);


    UFUNCTION(BlueprintCallable, Category = "Runtime Inspector")
    static int GetCurrentViewMode(const APlayerController* PlayerController);
};



