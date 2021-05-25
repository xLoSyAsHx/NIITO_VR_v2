#include "DeviceData.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Engine/GameViewportClient.h"

bool UDeviceDataLib::EqualEqual_FDeviceDataFDeviceData(const FDeviceData& A, const FDeviceData& B)
{
    return A.ID == B.ID && A.DeviceUUID == B.DeviceUUID;
}

int UDeviceDataLib::GetCurrentViewMode(const APlayerController* PlayerController)
{
    if (IsValid(PlayerController))
    {
        UGameViewportClient* GameViewportClient = PlayerController->GetWorld()->GetGameViewport();
        ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();

        bool ignore = GameViewportClient->IgnoreInput();
        EMouseCaptureMode capt = GameViewportClient->CaptureMouseOnClick();

        if (ignore == false && capt == EMouseCaptureMode::CaptureDuringMouseDown)
        {
            return 0;  // Game And UI
        }
        else if (ignore == true && capt == EMouseCaptureMode::NoCapture)
        {
            return 1;  // UI Only
        }
        else
        {
            return 2;  // Game Only
        }
    }

    return -1;
}
