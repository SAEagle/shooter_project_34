// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "Weapons/ShooterWeapon.h"
#include "Pickups/ShooterPickup_Weapon.h"
#include "OnlineSubsystemUtils.h"

AShooterPickup_Weapon::AShooterPickup_Weapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

bool AShooterPickup_Weapon::IsForWeapon(UClass* WeaponClass)
{
    return WeaponType->IsChildOf(WeaponClass);
}

bool AShooterPickup_Weapon::CanBePickedUp(AShooterCharacter* TestPawn) const
{
    AShooterWeapon* TestWeapon = (TestPawn ? TestPawn->FindWeapon(WeaponType) : NULL);
    if (bIsActive && TestWeapon)
    {
        return TestWeapon->GetCurrentAmmo() < TestWeapon->GetMaxAmmo();
    }

    return false;
}

void AShooterPickup_Weapon::GivePickupTo(class AShooterCharacter* Pawn)
{
    AShooterWeapon* Weapon = (Pawn ? Pawn->FindWeapon(WeaponType) : NULL);
    if (Weapon)
    {
        Weapon->GiveAmmo(AmmoToAdd);

        // Fire event for collected ammo
        if (Pawn)
        {
            const UWorld* World = GetWorld();
            const IOnlineEventsPtr Events = Online::GetEventsInterface(World);
            const IOnlineIdentityPtr Identity = Online::GetIdentityInterface(World);

            if (Events.IsValid() && Identity.IsValid())
            {
                AShooterPlayerController* PC = Cast<AShooterPlayerController>(Pawn->Controller);
                if (PC)
                {
                    ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->Player);

                    if (LocalPlayer)
                    {
                        const int32 UserIndex = LocalPlayer->GetControllerId();
                        TSharedPtr<const FUniqueNetId> UniqueID = Identity->GetUniquePlayerId(UserIndex);
                        if (UniqueID.IsValid())
                        {
                            FVector Location = Pawn->GetActorLocation();

                            FOnlineEventParms Params;

                            Params.Add(TEXT("SectionId"), FVariantData((int32)0));         // unused
                            Params.Add(TEXT("GameplayModeId"), FVariantData((int32)1));    // @todo determine game mode (ffa v tdm)
                            Params.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0)); // unused

                            Params.Add(TEXT("ItemId"),
                                FVariantData((int32)Weapon->GetAmmoType() +
                                             1)); // @todo come up with a better way to determine item id, currently health is 0 and ammo counts from 1
                            Params.Add(TEXT("AcquisitionMethodId"), FVariantData((int32)0)); // unused
                            Params.Add(TEXT("LocationX"), FVariantData(Location.X));
                            Params.Add(TEXT("LocationY"), FVariantData(Location.Y));
                            Params.Add(TEXT("LocationZ"), FVariantData(Location.Z));
                            Params.Add(TEXT("ItemQty"), FVariantData((int32)AmmoToAdd));

                            Events->TriggerEvent(*UniqueID, TEXT("CollectPowerup"), Params);
                        }
                    }
                }
            }
        }
    }
}

void AShooterPickup_Weapon::NotifyActorBeginOverlap(AActor* Other)
{
    // Super::NotifyActorBeginOverlap(Other);
    PickUp(Cast<AShooterCharacter>(Other));
}

void AShooterPickup_Weapon::PickUp(class AShooterCharacter* Pawn)
{
    const bool bJustSpawned = CreationTime <= (GetWorld()->GetTimeSeconds() + 3.0f);
    if (bJustSpawned)
    {
        UE_LOG(LogTemp, Display, TEXT("Cab be picked up"));
        if (bIsActive && Pawn && Pawn->IsAlive() && !IsPendingKill())
        {
            if (CanBePickedUp(Pawn))
            {
                GivePickupTo(Pawn);
                PickedUpBy = Pawn;

                if (!IsPendingKill())
                {
                    bIsActive = false;
                    OnPickedUp();

                    if (RespawnTime > 0.0f)
                    {
                        RemovePickUpFromScene();
                    }
                }
            }
        }
    }
}

void AShooterPickup_Weapon::RemovePickUpFromScene()
{
    UE_LOG(LogTemp, Display, TEXT("Wanted to Destroy"));
    // Destroy();
}

void AShooterPickup_Weapon::RespawnPickup()
{
    UE_LOG(LogTemp, Display, TEXT("Lifetime is over"));
    SetLifeSpan(15.0f);
    bIsActive = true;
    PickedUpBy = NULL;
    OnRespawned();

    TSet<AActor*> OverlappingPawns;
    GetOverlappingActors(OverlappingPawns, AShooterCharacter::StaticClass());

    for (AActor* OverlappingPawn : OverlappingPawns)
    {
        PickupOnTouch(CastChecked<AShooterCharacter>(OverlappingPawn));
    }
}

void AShooterPickup_Weapon::SetAmmoToDrop(int32 AmmoAmount)
{
    AmmoToAdd = AmmoAmount;
}
