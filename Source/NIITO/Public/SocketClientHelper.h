// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
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

UCLASS(Blueprintable)
class NIITO_API ASocketClientHelper : public AActor
{
    GENERATED_BODY()
    
public:
    // Sets default values for this actor's properties
    ASocketClientHelper();
    ~ASocketClientHelper();


    UFUNCTION(BlueprintCallable)
    void Connect(FString token);

    UFUNCTION(BlueprintCallable)
    void Disconnect();


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
    
    sio::client      m_client;
    sio::socket::ptr m_socket;
};
