// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PatientData.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Templates/SharedPointer.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpClientHelper.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FAuthDelegate, bool, bAuthSuccess);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FPatientListDelegate, bool, bSuccess, const TArray<FPatientData>&, patients);

UCLASS(Blueprintable)
class NIITO_API AHttpClientHelper : public AActor
{
	GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AHttpClientHelper();

    UFUNCTION(BlueprintCallable)
    void AuthRequest(FString name, FString password, FAuthDelegate delegate);

    UFUNCTION(BlueprintCallable)
    void PatientListRequest(FPatientListDelegate delegate);

    UPROPERTY()
    USceneComponent* Root;

    UPROPERTY(EditAnywhere)
    USceneComponent* Mesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FString m_serverAddress = "http://localhost:8000";

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

private:
    void OnAuthResponse(FHttpRequestPtr req, FHttpResponsePtr res, bool bWasSuccessful);

    void OnPatientListResponse(FHttpRequestPtr req, FHttpResponsePtr res, bool bWasSuccessful);

    FAuthDelegate m_onAuthSuccessDelegate;
    FPatientListDelegate m_onPLSuccessDelegate;
    FString m_token;

    struct NetPaths
    {
        static const FString Auth;
        static const FString GetPatientList;
    };
};

