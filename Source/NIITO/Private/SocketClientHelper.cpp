// Fill out your copyright notice in the Description page of Project Settings.


#include "SocketClientHelper.h"

#include "SocketIOClientComponent.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"


static std::mutex _lock;
static std::condition_variable_any _cond;
static bool _socket_operation_finished = false;


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

void ASocketClientHelper::Connect(FString token)
{
    if (m_bIsConnected)
        return;

    std::map<std::string, std::string> headers = {};
    headers["authorization"] = std::string(TCHAR_TO_UTF8(*token));
    const std::map<std::string, std::string> query = {};

    m_client.connect(TCHAR_TO_UTF8(*m_serverName), query, headers);
    _lock.lock();
    if (!_socket_operation_finished)
    {
        _cond.wait(_lock);
    }
    _lock.unlock();
    m_socket = m_client.socket();

    m_bIsConnected = true;
}

void ASocketClientHelper::Disconnect()
{
    _lock.lock();
    m_client.close();
    m_bIsConnected = false;
    _lock.unlock();
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
    _cond.notify_all();
    _socket_operation_finished = true;
    _lock.unlock();
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


