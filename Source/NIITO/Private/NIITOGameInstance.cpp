// Fill out your copyright notice in the Description page of Project Settings.


#include "NIITOGameInstance.h"

#include "HAL/FileManager.h"
#include "Misc/Paths.h"

void UNIITOGameInstance::Init()
{
    UGameInstance::Init();

    TArray<FString> fPathes;
    IFileManager::Get().FindFilesRecursive(fPathes, *FPaths::GameContentDir(), TEXT("*.umap"), true, false, false);
    for (int i = 0; i < fPathes.Num(); i++)
    {
        TArray<FString> temp;
        int size = fPathes[i].ParseIntoArray(temp, TEXT("Maps/"), true);
        if (size == 2)
        {
            fPathes[i] = temp[1];
            fPathes[i].ParseIntoArray(temp, TEXT("."), true);
            m_knownLevels.Add(temp[0]);
        }
    }
}

bool UNIITOGameInstance::HasLevel(FName LevelName) const
{
    return m_knownLevels.Contains(LevelName.ToString());
}
