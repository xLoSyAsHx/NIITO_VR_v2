// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PatientData.h"
#include "PhobiaData.h"
#include "DeviceData.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Templates/SharedPointer.h"
#include "HttpModule.h"
#include "Dom/JsonObject.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpClientHelper.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FAuthLoginDelegate,   bool, bSuccess, FString, errorMsg);
DECLARE_DYNAMIC_DELEGATE_FourParams(FPatientListDelegate, bool, bSuccess, const TArray<FPatientData>&, patients, const TArray<FPhobiaData>&, phobias, const TArray<FDeviceData>&, devices);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FAddPatientDelegate,  bool, bSuccess, FString, errorMsg);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FDelPatientDelegate,  bool, bSuccess, FString, errorMsg);

UCLASS(Blueprintable)
class NIITO_API AHttpClientHelper : public AActor
{
	GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AHttpClientHelper();

    UFUNCTION(BlueprintCallable)
    void AuthRequest(FAuthLoginDelegate delegate);

    UFUNCTION(BlueprintCallable)
    void LoginRequest(FString email, FString password, FAuthLoginDelegate delegate);

    UFUNCTION(BlueprintCallable)
    void PatientListRequest(FPatientListDelegate delegate);

    UFUNCTION(BlueprintCallable)
    void AddPatientRequest(FPatientData data, FAddPatientDelegate delegate);

    UFUNCTION(BlueprintCallable)
    void DelPatientRequest(FPatientData data, FDelPatientDelegate delegate);

    UFUNCTION(BlueprintCallable)
    void Logout();

    UPROPERTY()
    USceneComponent* Root;

    UPROPERTY(EditAnywhere)
    USceneComponent* Mesh;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
    FString m_serverAddress = "http://patients-back.past-tech.ru";

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

private:
    void ExtractAndSaveToken(TSharedPtr<FJsonObject> jsonObject);

    void OnAuthResponse(FHttpRequestPtr req, FHttpResponsePtr res, bool bWasSuccessful);

    void OnLoginResponse(FHttpRequestPtr req, FHttpResponsePtr res, bool bWasSuccessful);

    void OnPatientListResponse(FHttpRequestPtr req, FHttpResponsePtr res, bool bWasSuccessful);

    void OnAddPatientResponse(FHttpRequestPtr req, FHttpResponsePtr res, bool bWasSuccessful);

    void OnDelPatientResponse(FHttpRequestPtr req, FHttpResponsePtr res, bool bWasSuccessful);

    FAuthLoginDelegate m_onAuthLoginOKDelegate;
    FPatientListDelegate m_onPLOKDelegate;
    FAddPatientDelegate m_onAddPatientOKDelegate;
    FDelPatientDelegate m_onDelPatientOKDelegate;
    FString m_token;
    FString m_lastLoginEmail;

    struct NetPaths
    {
        static const FString POST_login;
        static const FString GET_Auth;
        static const FString POST_PatientList;
        static const FString POST_AddPatient;
        static const FString POST_DelPatient;
    };
};

