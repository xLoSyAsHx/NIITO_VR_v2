// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include <string>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LocalFileManager.generated.h"

/**
 * 
 */
UCLASS()
class NIITO_API ALocalFileManager : public AActor
{
    GENERATED_BODY()

public:

    static void Init();
    static void GetKnownLevels(TSet<FString>& knownLevels);

    static void SaveToken(FString token);
    static bool LoadToken(FString &token);

    static void SaveLoginUser(FString loginUser);

    UFUNCTION(BlueprintCallable)
    static FString LoadLoginUser();

private:
    static bool ReadFile(std::wstring path, FString& content);
    static void WriteToFile(std::wstring path, FString& content);

    static bool m_initialized;
    static std::wstring m_appDir;
    static std::wstring m_tokenName;
    static std::wstring m_loginUserName;
};
