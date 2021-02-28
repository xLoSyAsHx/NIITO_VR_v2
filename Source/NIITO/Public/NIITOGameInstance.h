// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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

    UFUNCTION(BlueprintPure)
    bool HasLevel(FName LevelName) const;

    UPROPERTY(BlueprintReadOnly)
    FString SessionToken;

private:
    TSet<FString> m_knownLevels;
};
