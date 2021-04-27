// Fill out your copyright notice in the Description page of Project Settings.


#include "LocalFileManager.h"

#include "HAL/FileManager.h"
#include "HAL/PlatformFilemanager.h"

#include "Misc/Paths.h"

#include <fstream>

bool ALocalFileManager::m_initialized = false;
std::wstring ALocalFileManager::m_appDir;
std::wstring ALocalFileManager::m_tokenName = L"\\session.tkn";
std::wstring ALocalFileManager::m_loginUserName = L"\\last_login_usr.name";

void ALocalFileManager::Init()
{
    if (m_initialized)
        return;

    m_appDir = std::wstring(_wgetenv(L"APPDATA")) + L"\\NIITO";
    m_initialized = true;
}

void ALocalFileManager::GetKnownLevels(TSet<FString> &knownLevels)
{
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
            knownLevels.Add(temp[0]);
        }
    }
}

void ALocalFileManager::SaveToken(FString token)
{
    WriteToFile(m_tokenName, token);
}

bool ALocalFileManager::LoadToken(FString & token)
{
    return ReadFile(m_tokenName, token);
}

void ALocalFileManager::SaveLoginUser(FString loginUser)
{
    WriteToFile(m_loginUserName, loginUser);
}

FString ALocalFileManager::LoadLoginUser()
{
    FString loginUser;
    ReadFile(ALocalFileManager::m_loginUserName, loginUser);
    return loginUser;
}

bool ALocalFileManager::ReadFile(std::wstring path, FString& content)
{
    FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(m_appDir.c_str());
    std::ifstream file;
    file.open(m_appDir + path);

    if (!file.is_open())
        return false;

    std::string data;
    data.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    content = data.c_str();
    file.close();
    return true;
}

void ALocalFileManager::WriteToFile(std::wstring path, FString& content)
{
    FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(m_appDir.c_str());
    std::ofstream file;
    file.open(m_appDir + path, std::ios_base::trunc);
    if (file.is_open())
    {
        file << std::string(TCHAR_TO_UTF8(*content));
        file.close();
    }
}
