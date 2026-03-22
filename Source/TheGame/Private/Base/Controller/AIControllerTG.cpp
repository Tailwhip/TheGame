// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/Controller/AIControllerTG.h"


void AAIControllerTG::HandleLook(const FInputActionValue& InputActionValue)
{
    // Input is a Vector2D
    const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();

    FRotator ControlRot = this->GetControlRotation();

    ControlRot.Yaw += LookAxisVector.X * Sensitivity;
    ControlRot.Pitch += LookAxisVector.Y * Sensitivity;

    // Clamp pitch (important)
    ControlRot.Pitch = FMath::Clamp(ControlRot.Pitch, -80.f, 80.f);

    this->SetControlRotation(ControlRot);
}
