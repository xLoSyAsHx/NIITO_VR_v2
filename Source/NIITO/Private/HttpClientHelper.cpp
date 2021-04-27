// Fill out your copyright notice in the Description page of Project Settings.


#include "HttpClientHelper.h"
#include "NIITOGameInstance.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"

#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"

#include <string>
#include <fstream>

#include "LocalFileManager.h"


const FString AHttpClientHelper::NetPaths::POST_login = "/login";
const FString AHttpClientHelper::NetPaths::GET_Auth = "/auth";
const FString AHttpClientHelper::NetPaths::POST_PatientList = "/get_patients";
const FString AHttpClientHelper::NetPaths::POST_AddPatient = "/post_add_p";
const FString AHttpClientHelper::NetPaths::POST_DelPatient = "/post_del_p";

void AHttpClientHelper::BeginPlay()
{
    Super::BeginPlay();
    auto GI = Cast<UNIITOGameInstance> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (GI)
    {
        if (!GI->SessionToken.IsEmpty())
            m_token = GI->SessionToken;
    }
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

void AHttpClientHelper::AuthRequest(FAuthLoginDelegate delegate)
{
    m_onAuthLoginOKDelegate = delegate;

    if (!ALocalFileManager::LoadToken(m_token))
    {
        m_onAuthLoginOKDelegate.Execute(false, "No token found in cache");
        return;
    }

    auto req = FHttpModule::Get().CreateRequest();
    req->OnProcessRequestComplete().BindUObject(this, &AHttpClientHelper::OnAuthResponse);

    //This is the url on which to process the request
    req->SetURL(FString::Printf(TEXT("%s%s"), *m_serverAddress, *NetPaths::GET_Auth));
    req->SetVerb("GET");
    req->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
    req->SetHeader(TEXT("authorization"), FString::Printf(TEXT("%s"), *m_token));

    req->ProcessRequest();
}

void AHttpClientHelper::LoginRequest(FString email, FString password, FAuthLoginDelegate delegate)
{
    m_onAuthLoginOKDelegate = delegate;
    m_lastLoginEmail = email;

    auto req = FHttpModule::Get().CreateRequest();
    req->OnProcessRequestComplete().BindUObject(this, &AHttpClientHelper::OnLoginResponse);

    //This is the url on which to process the request
    req->SetURL(FString::Printf(TEXT("%s%s"), *m_serverAddress, *NetPaths::POST_login));
    req->SetVerb("POST");
    req->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
    req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    // Json content
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    JsonObject->SetStringField(TEXT("email"), email);
    JsonObject->SetStringField(TEXT("password"), password);

    FString jsonOutputString;
    TSharedRef<TJsonWriter<TCHAR>> jsonWriter = TJsonWriterFactory<TCHAR>::Create(&jsonOutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), jsonWriter);
    req->SetContentAsString(jsonOutputString);

    req->ProcessRequest();
}

void AHttpClientHelper::PatientListRequest(FPatientListDelegate delegate)
{
    m_onPLOKDelegate = delegate;

    auto req = FHttpModule::Get().CreateRequest();
    req->OnProcessRequestComplete().BindUObject(this, &AHttpClientHelper::OnPatientListResponse);

    //This is the url on which to process the request
    req->SetURL(FString::Printf(TEXT("%s%s"), *m_serverAddress, *NetPaths::POST_PatientList));
    req->SetVerb("POST");
    req->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
    req->SetHeader(TEXT("authorization"), FString::Printf(TEXT("%s"), *m_token));
    req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    // Json content
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    JsonObject->SetNumberField(TEXT("skip"), 0);

    FString jsonOutputString;
    TSharedRef<TJsonWriter<TCHAR>> jsonWriter = TJsonWriterFactory<TCHAR>::Create(&jsonOutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), jsonWriter);
    req->SetContentAsString(jsonOutputString);

    req->ProcessRequest();
}

void AHttpClientHelper::AddPatientRequest(FPatientData data, FAddPatientDelegate delegate)
{
    m_onAddPatientOKDelegate = delegate;

    auto req = FHttpModule::Get().CreateRequest();
    req->OnProcessRequestComplete().BindUObject(this, &AHttpClientHelper::OnAddPatientResponse);

    //This is the url on which to process the request
    req->SetURL(FString::Printf(TEXT("%s%s"), *m_serverAddress, *NetPaths::POST_AddPatient));
    req->SetVerb("POST");
    req->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
    req->SetHeader(TEXT("authorization"), FString::Printf(TEXT("%s"), *m_token));
    req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    JsonObject->SetStringField(TEXT("FirstName"), data.FirstName);
    JsonObject->SetStringField(TEXT("SecondName"), data.LastName);
    JsonObject->SetStringField(TEXT("Diagnose"), data.Diagnose);

    FString jsonOutputString;
    TSharedRef<TJsonWriter<TCHAR>> jsonWriter = TJsonWriterFactory<TCHAR>::Create(&jsonOutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), jsonWriter);
    req->SetContentAsString(jsonOutputString);
    req->ProcessRequest();
}

void AHttpClientHelper::DelPatientRequest(FPatientData data, FDelPatientDelegate delegate)
{
    if (!delegate.IsBound())
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Error. Invalid delegate."));
        return;
    }
    m_onDelPatientOKDelegate = delegate;

    auto req = FHttpModule::Get().CreateRequest();
    req->OnProcessRequestComplete().BindUObject(this, &AHttpClientHelper::OnDelPatientResponse);

    //This is the url on which to process the request
    req->SetURL(FString::Printf(TEXT("%s%s"), *m_serverAddress, *NetPaths::POST_DelPatient));
    req->SetVerb("POST");
    req->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
    req->SetHeader(TEXT("authorization"), FString::Printf(TEXT("%s"), *m_token));
    req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    JsonObject->SetStringField(TEXT("FirstName"), data.FirstName);
    JsonObject->SetStringField(TEXT("SecondName"), data.LastName);
    JsonObject->SetStringField(TEXT("Diagnose"), data.Diagnose);

    FString jsonOutputString;
    TSharedRef<TJsonWriter<TCHAR>> jsonWriter = TJsonWriterFactory<TCHAR>::Create(&jsonOutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), jsonWriter);
    req->SetContentAsString(jsonOutputString);
    req->ProcessRequest();
}

void AHttpClientHelper::Logout()
{
    m_token.Reset();
    m_onAuthLoginOKDelegate.Clear();
    m_onPLOKDelegate.Clear();
}

void AHttpClientHelper::ExtractAndSaveToken(TSharedPtr<FJsonObject> jsonObject)
{
    m_token = jsonObject->GetStringField("token");
    auto GI = Cast<UNIITOGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (GI)
        GI->SessionToken = m_token;

    ALocalFileManager::SaveToken(m_token);
}

void AHttpClientHelper::OnAuthResponse(FHttpRequestPtr req, FHttpResponsePtr res, bool bWasSuccessful)
{
    if (bWasSuccessful && res->GetResponseCode() == EHttpResponseCodes::Ok)
    {
        TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
        TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(res->GetContentAsString());
        FJsonSerializer::Deserialize(JsonReader, JsonObject);

        if (!JsonObject->GetBoolField("access"))
        {
            m_onAuthLoginOKDelegate.Execute(false, "Invalid token");
            return;
        }

        ExtractAndSaveToken(JsonObject);
        m_onAuthLoginOKDelegate.Execute(true, "");
    }
    else
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Invalid response: %d"), res->GetResponseCode()));
        m_onAuthLoginOKDelegate.Execute(false, "");
    }
}

void AHttpClientHelper::OnLoginResponse(FHttpRequestPtr req, FHttpResponsePtr res, bool bWasSuccessful)
{
    if (bWasSuccessful && res->GetResponseCode() == EHttpResponseCodes::Ok)
    {
        TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
        TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(res->GetContentAsString());
        FJsonSerializer::Deserialize(JsonReader, JsonObject);

        
        if (JsonObject->HasField("error"))
        {
            if (GEngine)
                GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Error msg: %s"), *JsonObject->GetStringField("error")));

            m_onAuthLoginOKDelegate.Execute(false, JsonObject->GetStringField("error"));
            return;
        }

        ALocalFileManager::SaveLoginUser(m_lastLoginEmail);
        ExtractAndSaveToken(JsonObject);
        m_onAuthLoginOKDelegate.Execute(true, "");
    }
    else if (res->GetResponseCode() == EHttpResponseCodes::Denied)
    {
        m_onAuthLoginOKDelegate.Execute(false, "Failed to get auth token. Response code == Access denied");
    }
    else
    {
        m_onAuthLoginOKDelegate.Execute(false, FString::Printf(TEXT("Invalid response code: %d"), res->GetResponseCode()));
    }
}

void AHttpClientHelper::OnPatientListResponse(FHttpRequestPtr req, FHttpResponsePtr res, bool bWasSuccessful)
{
    TArray<FPatientData> patients;

    if (bWasSuccessful && res->GetResponseCode() == EHttpResponseCodes::Ok)
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


        for (auto& el : JsonObject->GetArrayField("data"))
        {
            const auto& obj = el->AsObject();
            FDateTime birthday;
            FDateTime::ParseIso8601(*obj->GetStringField("birthday"), birthday);
            patients.Add({
                obj->GetStringField("_id"),
                obj->GetStringField("firstname"),
                obj->GetStringField("surname"),
                obj->GetStringField("lastname"),
                birthday,
                // obj->GetStringField("phobia") });
                "Acrophobia" });
        }

        m_onPLOKDelegate.Execute(true, patients);
    }
    else
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Failed to get PL. Error code: %d"), res->GetResponseCode()));
        m_onPLOKDelegate.Execute(false, patients);
    }
}

void AHttpClientHelper::OnAddPatientResponse(FHttpRequestPtr req, FHttpResponsePtr res, bool bWasSuccessful)
{
    if (bWasSuccessful && res->GetResponseCode() == EHttpResponseCodes::Ok && res->GetContentLength() == 0)
    {
        m_onAddPatientOKDelegate.Execute(true, "");
    }
    else if (bWasSuccessful && res->GetResponseCode() == EHttpResponseCodes::Ok)
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

        FString errMsg = JsonObject->GetStringField("ErrorMsg");
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, errMsg);
        m_onAddPatientOKDelegate.Execute(false, errMsg);
    }
    else
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Server error. Failed to add patient"));
        m_onAddPatientOKDelegate.Execute(false, TEXT("Server error. Failed to add patient"));
    }
}

void AHttpClientHelper::OnDelPatientResponse(FHttpRequestPtr req, FHttpResponsePtr res, bool bWasSuccessful)
{
    if (bWasSuccessful && res->GetResponseCode() == EHttpResponseCodes::Ok && res->GetContentLength() == 0)
    {
        m_onDelPatientOKDelegate.Execute(true, "");
    }
    else if (bWasSuccessful && res->GetResponseCode() == EHttpResponseCodes::Ok)
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

        FString errMsg = JsonObject->GetStringField("ErrorMsg");
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, errMsg);
        m_onAddPatientOKDelegate.Execute(false, errMsg);
    }
    else
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Server error. Failed to del patient"));
        m_onDelPatientOKDelegate.Execute(false, TEXT("Server error. Failed to del patient"));
    }
}
