// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ShooterDamageType.generated.h"

// DamageType class that specifies an icon to display
UCLASS(const, Blueprintable, BlueprintType)
class UShooterDamageType : public UDamageType
{
    GENERATED_UCLASS_BODY()

    /** icon displayed in death messages log when killed with this weapon */
    UPROPERTY(EditDefaultsOnly, Category = HUD)
    FCanvasIcon KillIcon;

    /** force feedback effect to play on a player hit by this damage type */
    UPROPERTY(EditDefaultsOnly, Category = Effects)
    UForceFeedbackEffect* HitForceFeedback;

    /** force feedback effect to play on a player killed by this damage type */
    UPROPERTY(EditDefaultsOnly, Category = Effects)
    UForceFeedbackEffect* KilledForceFeedback;

    /** allow player to freeze  */
    UPROPERTY(EditDefaultsOnly, Category = DamageType)
    uint32 bFreeze : 1;

    /** freeze time amount */
    UPROPERTY(EditDefaultsOnly, Category = DamageType)
    float FreezeTime;
};
