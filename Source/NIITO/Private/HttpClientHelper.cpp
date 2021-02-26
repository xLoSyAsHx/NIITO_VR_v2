// Fill out your copyright notice in the Description page of Project Settings.


#include "HttpClientHelper.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"


const FString AHttpClientHelper::NetPaths::Auth = "/auth";
const FString AHttpClientHelper::NetPaths::GetPatientList = "/get_pl";

void AHttpClientHelper::BeginPlay()
{
    Super::BeginPlay();
}

AHttpClientHelper::AHttpClientHelper()
{
    PrimaryActorTick.bCanEverTick = false;
    Root = CreateDefaultSubobject<USceneComponent>("Root");
    RootComponent = Root;

    //Mesh = CreateDefaultSubobject<UMeshComponent>("Mesh");
    //Root->AttachTo(Mesh);

    //Mesh->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
    //Mesh->SetMobility(EComponentMobility::Static);
}

void AHttpClientHelper::AuthRequest(FString name, FString password, FAuthDelegate delegate)
{
    if (!m_token.IsEmpty())
    {
        m_onAuthSuccessDelegate.Execute(true);
        return;
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Call AuthRequest"));
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Name/Pass  ->  %s:%s"), *name, *password));
    }
    m_onAuthSuccessDelegate = delegate;

    auto req = FHttpModule::Get().CreateRequest();
    req->OnProcessRequestComplete().BindUObject(this, &AHttpClientHelper::OnAuthResponse);

    //This is the url on which to process the request
    req->SetURL(FString::Printf(TEXT("%s%s"), *m_serverAddress, *NetPaths::Auth));
    req->SetVerb("GET");
    req->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
    req->SetHeader("authorization", FString::Printf(TEXT("%s:%s"), *name, *password));
    req->ProcessRequest();
}

void AHttpClientHelper::PatientListRequest(FPatientListDelegate delegate)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Call PatientListRequest"));
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Token  ->  %s"), *m_token));
    }
    m_onPLSuccessDelegate = delegate;

    auto req = FHttpModule::Get().CreateRequest();
    req->OnProcessRequestComplete().BindUObject(this, &AHttpClientHelper::OnPatientListResponse);

    //This is the url on which to process the request
    req->SetURL(FString::Printf(TEXT("%s%s"), *m_serverAddress, *NetPaths::GetPatientList));
    req->SetVerb("GET");
    req->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
    req->SetHeader("authorization", FString::Printf(TEXT("%s"), *m_token));
    req->ProcessRequest();
}

void AHttpClientHelper::OnAuthResponse(FHttpRequestPtr req, FHttpResponsePtr res, bool bWasSuccessful)
{
    if (bWasSuccessful && res->GetResponseCode() == EHttpResponseCodes::Ok
        && res->GetContentType() == "application/json")
    {
        TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
        TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(res->GetContentAsString());
        FJsonSerializer::Deserialize(JsonReader, JsonObject);

        FString token = JsonObject->GetStringField("token");
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Auth token: %s"), *token));

        m_token = token;
        m_onAuthSuccessDelegate.Execute(true);
    }
    else if (res->GetResponseCode() == EHttpResponseCodes::Denied)
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Failed to get auth token"));
        m_onAuthSuccessDelegate.Execute(false);
    }
    else
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Invalid response: %d"), res->GetResponseCode()));
        m_onAuthSuccessDelegate.Execute(false);
    }
}

void AHttpClientHelper::OnPatientListResponse(FHttpRequestPtr req, FHttpResponsePtr res, bool bWasSuccessful)
{
    TArray<FPatientData> patients;

    if (bWasSuccessful && res->GetResponseCode() == EHttpResponseCodes::Ok
        && res->GetContentType() == "application/json")
    {
        TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
        TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(res->GetContentAsString());
        FJsonSerializer::Deserialize(JsonReader, JsonObject);

        if (GEngine && !JsonObject.IsValid())
        {
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Failed to parse Json."));
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Content: %s"), *res->GetContentAsString()));
            return;
        }

        for (auto& el : JsonObject->GetArrayField("Patients"))
        {
            const auto& obj = el->AsObject();
            patients.Add({
                obj->GetStringField("FirstName"),
                obj->GetStringField("SecondName") });
        }

        m_onPLSuccessDelegate.Execute(true, patients);
    }
    else
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Failed to get PL"));
        m_onPLSuccessDelegate.Execute(false, patients);
    }
}
