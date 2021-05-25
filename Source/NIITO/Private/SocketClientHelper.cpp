// Fill out your copyright notice in the Description page of Project Settings.


#include "SocketClientHelper.h"

#include "Engine/Engine.h"

#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"

#include "Core/Public/HAL/Runnable.h"
#include "Core/Public/HAL/RunnableThread.h"
#include <thread>

static std::mutex _lock;
static std::condition_variable_any _cond;
static bool _socket_operation_finished = false;



//#pragma comment(lib, "sioclient.lib")




// Sets default values
ASocketClientHelper::ASocketClientHelper()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    m_client.set_open_listener(std::bind(&ASocketClientHelper::OnConnected, this));
    m_client.set_close_listener(std::bind(&ASocketClientHelper::OnClosed, this, std::placeholders::_1));
    m_client.set_fail_listener(std::bind(&ASocketClientHelper::OnFailed, this));
    m_client.set_reconnect_listener(std::bind(&ASocketClientHelper::OnReconnected, this));
}

ASocketClientHelper::~ASocketClientHelper()
{
}

void ASocketClientHelper::Connect(FString token, FSocketDelegate delegate)
{
    UE_LOG(LogTemp, Log, TEXT("sio: Call Connect"));
    if (m_bIsConnected)
        return;

    m_onConnectedDelegate = delegate;



    std::map<std::string, std::string> headers = {};
    headers["authorization"] = std::string(TCHAR_TO_UTF8(*token));
    const std::map<std::string, std::string> query = {};

    m_client.connect(TCHAR_TO_UTF8(*m_serverName), query, headers);
    _lock.lock();
    if (!_socket_operation_finished)
    {
        std::thread t([&]() { _sleep(2000); _cond.notify_all(); m_onConnectedDelegate.Execute(false); });
        _cond.wait(_lock);
        t.join();
    }
    _lock.unlock();

    if (m_bIsConnected)
        m_socket = m_client.socket();
    else
        m_client.close();
}

void ASocketClientHelper::Disconnect()
{
    _lock.lock();
    m_client.close();
    m_bIsConnected = false;
    _lock.unlock();
}

void ASocketClientHelper::EmitConnectToStream(FString token, FString pair_id, FString patient_id, FString phobia_id, FString device)
{
    UE_LOG(LogTemp, Log, TEXT("sio: EmitConnectToStream"));
    //m_onConnectedToStreamDelegate = delegate;


    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    JsonObject->SetStringField(TEXT("pair_id"),    pair_id);
    JsonObject->SetStringField(TEXT("patient_id"), patient_id);
    JsonObject->SetStringField(TEXT("phobia_id"),  phobia_id);
    JsonObject->SetStringField(TEXT("device"),     device);

    FString jsonOutputString;
    TSharedRef<TJsonWriter<TCHAR>> jsonWriter = TJsonWriterFactory<TCHAR>::Create(&jsonOutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), jsonWriter);


    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("sio: jsonOutputString: %s"), *jsonOutputString));
    UE_LOG(LogTemp, Log, TEXT("sio: EmitConnectToStream: jsonOutputString: %s"), *jsonOutputString);

    /*
    char buf[1000];
    std::string asd = std::string(TCHAR_TO_UTF8(*jsonOutputString));
    strncpy(buf, asd.c_str(), sizeof(buf) - 1);
    m_socket->emit("connect_to_stream", std::make_shared<std::string>(buf, sizeof(buf)));
    */

    sio::message::list li(std::string(TCHAR_TO_UTF8(*jsonOutputString)));
    //m_socket->emit("connect_to_stream", li);
    m_socket->emit("connect_to_stream", li, [&](sio::message::list const& msg) { /*m_onConnectedToStreamDelegate.Execute(true);*/ });
}

void ASocketClientHelper::SetStressListener(FOnStressDelegate delegate)
{
    m_onStressDelegate.Clear();
    m_onStressDelegate = delegate;
    m_client.socket()->on("unreal_stress_event", [&](sio::event& ev)
    {
            auto a = FDateTime::Now();
            ;

        bool isStress = ev.get_message()->get_map()["stress"]->get_bool();
        UE_LOG(LogTemp, Log, TEXT("sio: unreal_stress_event: %d"), isStress);
        if (isStress && (FDateTime::Now() - m_lastStressTime).GetDuration().GetTotalSeconds() > 60)
        {
            m_lastStressTime = FDateTime::Now();
            m_onStressDelegate.Execute();
        }
        //handle login message
        //post to UI thread if any UI updating.
    });
}

bool ASocketClientHelper::IsConnectedToStream()
{
    return m_bIsConnectedToStream;
}

// Called when the game starts or when spawned
void ASocketClientHelper::BeginPlay()
{
    Super::BeginPlay();
}

void ASocketClientHelper::OnConnected()
{
    _lock.lock();
    UE_LOG(LogTemp, Log, TEXT("sio: OnConnected"));
    m_bIsConnected = true;
    _cond.notify_all();
    _socket_operation_finished = true;
    _lock.unlock();
    m_onConnectedDelegate.Execute(true);
}

void ASocketClientHelper::OnReconnected()
{
    UE_LOG(LogTemp, Log, TEXT("sio: OnReconnected"));
}

void ASocketClientHelper::OnClosed(sio::client::close_reason const& reason)
{
    UE_LOG(LogTemp, Log, TEXT("sio: OnClosed"));
}

void ASocketClientHelper::OnFailed()
{
    UE_LOG(LogTemp, Log, TEXT("sio: OnFailed"));
}


