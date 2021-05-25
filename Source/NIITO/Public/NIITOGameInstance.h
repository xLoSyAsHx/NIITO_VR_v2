// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DeviceData.h"
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Containers/UnrealString.h"
#include "Containers/Set.h"
#include "NIITOGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class NIITO_API UNIITOGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
    void Init() override;

    void SetKnownDevices(const TArray<FDeviceData>& pairs);

    UFUNCTION(BlueprintPure)
    bool HasLevel(FName LevelName) const;

    UFUNCTION(BlueprintPure)
    const TArray<FDeviceData>& GetKnownDevices() const;

    UPROPERTY(BlueprintReadOnly)
    FString SessionToken;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString DeviceID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString PatientID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString PhobiaID;

private:
    TSet<FString> m_knownLevels;
    TArray<FDeviceData> m_knownDevices;
};
