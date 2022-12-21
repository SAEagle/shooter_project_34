
#pragma once

#include "CoreMinimal.h"
#include "Pickups/ShooterPickup.h"
#include "ShooterPickup_Weapon.generated.h"

class AShooterCharacter;
class AShooterWeapon;

UCLASS()
class SHOOTERGAME_API AShooterPickup_Weapon : public AShooterPickup
{
    GENERATED_UCLASS_BODY()
    /** pickup on touch */
    virtual void NotifyActorBeginOverlap(class AActor* Other) override;

    /** check if pawn can use this pickup */
    virtual bool CanBePickedUp(AShooterCharacter* TestPawn) const override;

    bool IsForWeapon(UClass* WeaponClass);

    /** set ammo amount */
    void SetAmmoToDrop(int32 AmmoAmount);

protected:
    /** how much ammo does it give? in custom variant */
    int32 AmmoToAdd;

    /** which weapon we want to drop */
    UPROPERTY(EditDefaultsOnly, Category = Pickup)
    TSubclassOf<AShooterWeapon> WeaponType;

    /** give pickup */
    virtual void GivePickupTo(AShooterCharacter* Pawn) override;

    /** give pickup */
    void PickUp(class AShooterCharacter* Pawn);

    /** custom respawn override */
    virtual void RespawnPickup() override;

    /** remove object from scene */
    void RemovePickUpFromScene();
};
