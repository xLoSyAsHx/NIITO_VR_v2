// Fill out your copyright notice in the Description page of Project Settings.


#include "HttpClientHelper.h"
#include "NIITOGameInstance.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"

#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"


const FString AHttpClientHelper::NetPaths::GET_Auth = "/auth";
const FString AHttpClientHelper::NetPaths::GET_PatientList = "/get_pl";
const FString AHttpClientHelper::NetPaths::POST_AddPatient = "/post_add_p";

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

void AHttpClientHelper::AuthRequest(FString name, FString password, FAuthDelegate delegate)
{
    if (!m_token.IsEmpty())
    {
        m_onAuthOKDelegate.Execute(true);
        return;
    }
    m_onAuthOKDelegate = delegate;

    auto req = FHttpModule::Get().CreateRequest();
    req->OnProcessRequestComplete().BindUObject(this, &AHttpClientHelper::OnAuthResponse);

    //This is the url on which to process the request
    req->SetURL(FString::Printf(TEXT("%s%s"), *m_serverAddress, *NetPaths::GET_Auth));
    req->SetVerb("GET");
    req->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
    req->SetHeader(TEXT("authorization"), FString::Printf(TEXT("%s:%s"), *name, *password));
    req->ProcessRequest();
}

void AHttpClientHelper::PatientListRequest(FPatientListDelegate delegate)
{
    m_onPLOKDelegate = delegate;

    auto req = FHttpModule::Get().CreateRequest();
    req->OnProcessRequestComplete().BindUObject(this, &AHttpClientHelper::OnPatientListResponse);

    //This is the url on which to process the request
    req->SetURL(FString::Printf(TEXT("%s%s"), *m_serverAddress, *NetPaths::GET_PatientList));
    req->SetVerb("GET");
    req->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
    req->SetHeader(TEXT("authorization"), FString::Printf(TEXT("%s"), *m_token));
    req->ProcessRequest();
}

void AHttpClientHelper::AddPatientRequest(FString firstName, FString secondName, FString diagnose, FAddPatientDelegate delegate)
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
    JsonObject->SetStringField(TEXT("FirstName"), firstName);
    JsonObject->SetStringField(TEXT("SecondName"), secondName);
    JsonObject->SetStringField(TEXT("Diagnose"), diagnose);

    FString jsonOutputString;
    TSharedRef<TJsonWriter<TCHAR>> jsonWriter = TJsonWriterFactory<TCHAR>::Create(&jsonOutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), jsonWriter);
    req->SetContentAsString(jsonOutputString);
    req->ProcessRequest();
}

void AHttpClientHelper::Logout()
{
    m_token.Reset();
    m_onAuthOKDelegate.Clear();
    m_onPLOKDelegate.Clear();
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
        m_token = token;
        auto GI = Cast<UNIITOGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
        if (GI)
            GI->SessionToken = m_token;
        m_onAuthOKDelegate.Execute(true);
    }
    else if (res->GetResponseCode() == EHttpResponseCodes::Denied)
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Failed to get auth token"));
        m_onAuthOKDelegate.Execute(false);
    }
    else
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Invalid response: %d"), res->GetResponseCode()));
        m_onAuthOKDelegate.Execute(false);
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
                obj->GetStringField("SecondName"),
                obj->GetStringField("Diagnose") });
        }

        m_onPLOKDelegate.Execute(true, patients);
    }
    else
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Failed to get PL"));
        m_onPLOKDelegate.Execute(false, patients);
    }
}

void AHttpClientHelper::OnAddPatientResponse(FHttpRequestPtr req, FHttpResponsePtr res, bool bWasSuccessful)
{
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

        FString errMsg = JsonObject->GetStringField("ErrorMsg");
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, errMsg);
        m_onAddPatientOKDelegate.Execute(true, errMsg);
    }
    else
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Server error. Failed to add patient"));
        m_onAddPatientOKDelegate.Execute(false, TEXT("Server error. Failed to add patient"));
    }
}
