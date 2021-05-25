// Fill out your copyright notice in the Description page of Project Settings.


#include "NIITOGameInstance.h"

#include "HAL/FileManager.h"
#include "Misc/Paths.h"

#include "LocalFileManager.h"

void UNIITOGameInstance::Init()
{
    UGameInstance::Init();

    ALocalFileManager::Init();
    ALocalFileManager::GetKnownLevels(m_knownLevels);

    // Tokens

}

void UNIITOGameInstance::SetKnownDevices(const TArray<FDeviceData>& devices)
{
    m_knownDevices.Empty();
    m_knownDevices = devices;
}

bool UNIITOGameInstance::HasLevel(FName LevelName) const
{
    return m_knownLevels.Contains(LevelName.ToString());
}

const TArray<FDeviceData>& UNIITOGameInstance::GetKnownDevices() const
{
    return m_knownDevices;
}
