// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
//#include "C:\\Program Files (x86)\\sioclient\\include\\sio_client.h"
//#include "C:\\Program Files (x86)\\sioclient\\include\\sio_socket.h"

#include "sio_socket.h"
#include "sio_client.h"
#include "sio_message.h"
#include <thread>
#include <mutex>
#include <condition_variable>

#include "IPAddress.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SocketClientHelper.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnStressDelegate);
DECLARE_DYNAMIC_DELEGATE_OneParam(FSocketDelegate, bool, bSuccess);

UCLASS(Blueprintable)
class NIITO_API ASocketClientHelper : public AActor
{
    GENERATED_BODY()
    
public:
    // Sets default values for this actor's properties
    ASocketClientHelper();
    ~ASocketClientHelper();


    UFUNCTION(BlueprintCallable)
    void Connect(FString token, FSocketDelegate delegate);

    UFUNCTION(BlueprintCallable)
    void Disconnect();

    UFUNCTION(BlueprintCallable)
    void EmitConnectToStream(
        FString token,
        FString pair_id,
        FString patient_id,
        FString phobia_id,
        FString device);

    UFUNCTION(BlueprintCallable)
    void SetStressListener(FOnStressDelegate delegate);

    UFUNCTION(BlueprintPure)
    bool IsConnectedToStream();


    UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
    FString m_serverName = "http://patients-back.past-tech.ru";

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;


private:
    void OnConnected();
    void OnReconnected();
    void OnClosed(sio::client::close_reason const& reason);
    void OnFailed();

    bool m_bIsConnected = false;
    bool m_bIsConnectedToStream = false;

    sio::client      m_client;
    sio::client      m_clientv2;
    sio::socket::ptr m_socket;

    FSocketDelegate m_onConnectedDelegate;
    FOnStressDelegate m_onStressDelegate;

    FDateTime m_lastStressTime;
};
