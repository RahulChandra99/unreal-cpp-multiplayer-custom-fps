<h2><p align="center">FPS Multiplayer Game</p></h2>
<p align="center">Built using C++, Blueprints and Unreal Engine 5</p>

---

## Table of Contents
* [Project Overview](#projectoverview)
* [Tech Details](#techdetails)
* [Gameplay Demo](#demo)
* [Code Implementation](#code-implementation)
  * [Player Control and Movement](#movement)
  * [Weapon System](#weapons)
  * [Inventory System](#inventory)
  * [Health Armor and Damage](#health)
  * [Death and Respawn](#respawn)
  * [Spawn System](#spawns)
  * [Team Mode](#teams)
  * [User Interface](#ui)
  * [Visual and Audio Effects](#vfx)
  * [Multiplayer Architecture](#network)


---

<a name="projectoverview"></a>

## Project Overview

Multiplayer first person shooter framework with movement, weapons, inventory, teams, UI, and full replication.

<a name="techdetails"></a>

## Tech Details

| Platform | Engine | Duration | Team Size | Roles |
| --- | --- | --- | --- | --- |
| PC | Unreal Engine 5/C++ | >1 month | Solo | Gameplay Programmer, Systems Designer |

---

<a name="demo"></a>

## Gameplay Demo

<p align="center">
  <b>Click the image to open the video in a new tab.</b>
</p>
<p align="center">
  <a href="https://www.youtube.com/watch?v=wd9-LOUOrDo" target="_blank" rel="noopener noreferrer">
    <img alt="Gameplay Demo" width="520" src="https://github.com/user-attachments/assets/bb0fcd5a-91f0-4f21-a847-88551d22dc9f"/>
  </a>
</p>

<a name="code-implementation"></a>

## Code Implementation


<a name="movement"></a>

### Player Control and Movement

#### What this covers
* CharacterMovementComponent based movement
* First person camera, FP arms mesh, full body mesh
* AimOffset and BlendSpace animation setup
* Server RPC and replicated state for movement actions

#### Why it matters
Movement actions are server authoritative, clients send intent, server validates and replicates state to keep everyone in sync.

<details>
<summary><b><i>View Code</b></summary></i>

```cpp
// Header
UPROPERTY(ReplicatedUsing=OnRep_IsSprinting)
bool bIsSprinting = false;

UFUNCTION(Server, Reliable)
void ServerSetSprinting(bool bNewSprinting);

UFUNCTION()
void OnRep_IsSprinting();

// Source
void AMyCharacter::ServerSetSprinting_Implementation(bool bNewSprinting)
{
    bIsSprinting = bNewSprinting;
    OnRep_IsSprinting();
}

void AMyCharacter::OnRep_IsSprinting()
{
    GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
}
```

</details>

---

<a name="weapons"></a>

### Weapon System

#### What this covers
* Base weapon class in C++
* Data Asset driven stats
* Server authoritative firing
* Line trace hit detection
* RepNotify ammo updates
* Attachments via sockets

#### How it works
Clients request fire, server performs trace and applies damage, clients play cosmetic effects.

<details>
<summary><b><i>View Code</b></summary></i>

```cpp
UFUNCTION(Server, Reliable)
void ServerFire(const FVector& TraceStart, const FVector& TraceEnd);

UFUNCTION(NetMulticast, Unreliable)
void MulticastFireFX();

void AWeaponBase::ServerFire_Implementation(const FVector& TraceStart, const FVector& TraceEnd)
{
    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    const bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit, TraceStart, TraceEnd, ECC_Visibility, Params
    );

    if (bHit)
    {
        UGameplayStatics::ApplyPointDamage(
            Hit.GetActor(), Damage, (TraceEnd - TraceStart).GetSafeNormal(),
            Hit, GetInstigatorController(), this, DamageType
        );
    }

    ConsumeAmmoOnServer();
    MulticastFireFX();
}

void AWeaponBase::MulticastFireFX_Implementation()
{
    PlayMuzzleFlash();
    PlayFireSound();
    PlayCameraShakeIfLocal();
}
```

</details>

---

<a name="inventory"></a>

### Inventory System

#### What this covers
* Inventory in C++ using TArray
* Items as Data Assets
* Drag and drop UI with UMG
* Server RPC for move, swap, drop
* Replicated inventory with OnRep for UI refresh

#### How it works
Inventory data lives on the server, clients request changes, server replicates the updated array, UI rebuilds from OnRep.


<details>
<summary><b><i>View Code</b></summary></i>

```cpp
USTRUCT(BlueprintType)
struct FInventoryEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UItemDataAsset> Item = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Count = 1;
};

UPROPERTY(ReplicatedUsing=OnRep_Inventory)
TArray<FInventoryEntry> Inventory;

UFUNCTION()
void OnRep_Inventory();

void AMyPlayerState::OnRep_Inventory()
{
    OnInventoryChanged.Broadcast(Inventory);
}
```

</details>

---

<a name="health"></a>

### Health Armor and Damage

#### What this covers
* HealthComponent in C++
* Armor reduces damage first
* Headshot detection using bone name
* Hit direction feedback for UI
* Replicated death state


<details>
<summary><b><i>View Code</b></summary></i>

```cpp
float UHealthComponent::ApplyDamageWithArmor(float IncomingDamage)
{
    float Remaining = IncomingDamage;

    if (Armor > 0.0f)
    {
        const float Absorb = FMath::Min(Armor, Remaining);
        Armor -= Absorb;
        Remaining -= Absorb;
    }

    Health = FMath::Clamp(Health - Remaining, 0.0f, MaxHealth);
    return Remaining;
}
```

</details>

---

<a name="respawn"></a>

### Death and Respawn

#### What this covers
* Server handles death
* Drops weapon on death
* Respawn after delay using GameMode
* PlayerState stores kills and deaths

<details>
<summary><b><i>View Code</b></summary></i>

```cpp
void AMyGameMode::HandlePlayerDeath(AController* DeadController)
{
    FTimerHandle RespawnTimer;

    GetWorldTimerManager().SetTimer(RespawnTimer, FTimerDelegate::CreateLambda([this, DeadController]()
    {
        RestartPlayer(DeadController);
    }), RespawnDelay, false);
}
```

</details>

---

<a name="spawns"></a>

### Spawn System

#### What this covers
* Team based spawn points using tags
* Safety check before spawning
* GameMode selects spawn, Controller receives result

<details>
<summary><b><i>View Code</b></summary></i>

```cpp
bool AMyGameMode::IsSpawnSafe(const FVector& Location) const
{
    FCollisionShape Shape = FCollisionShape::MakeSphere(SafetyRadius);
    return !GetWorld()->OverlapAnyTestByChannel(
        Location, FQuat::Identity, ECC_Pawn, Shape
    );
}
```

</details>

---

<a name="teams"></a>

### Team Mode

#### What this covers
* Team select UI
* PlayerState stores team index
* GameState stores team scores
* Server updates scores on kill

<details>
<summary><b><i>View Code</b></summary></i>


```cpp
void AMyGameState::AddTeamScore(int32 TeamIndex, int32 Amount)
{
    if (!TeamScores.IsValidIndex(TeamIndex)) return;
    TeamScores[TeamIndex] += Amount;
    OnRep_TeamScores();
}
```

</details>

---

<a name="ui"></a>

### User Interface

#### What this covers
* HUD ammo, health, armor, weapon name
* Crosshair spread from movement and firing
* Inventory grid drag and drop
* Hit marker and kill feed events

<details>
<summary><b><i>View Code</b></summary></i>

```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAmmoChanged, int32, NewAmmo);

UPROPERTY(BlueprintAssignable)
FOnAmmoChanged OnAmmoChanged;

void AWeaponBase::SetAmmo(int32 NewAmmo)
{
    Ammo = NewAmmo;
    OnAmmoChanged.Broadcast(Ammo);
}
```

</details>

---

<a name="vfx"></a>

### Visual and Audio Effects

#### What this covers
* Niagara muzzle flash and impacts
* Surface based impacts using Physical Material
* Camera shake on fire
* Sounds for fire, reload, damage, footsteps, UI


---

<a name="network"></a>

### Multiplayer Architecture

#### Core rules
* Server owns gameplay state
* Clients send requests using Server RPC
* Server replicates state using Replicated and RepNotify
* Cosmetics use Multicast and local checks

<details>
<summary><b><i>View Code</b></summary></i>

```cpp
void AMyCharacter::TryFire()
{
    if (!IsLocallyControlled()) return;

    FVector Start, End;
    BuildFireTrace(Start, End);

    ServerTryFire(Start, End);
}

UFUNCTION(Server, Reliable)
void AMyCharacter::ServerTryFire(const FVector& Start, const FVector& End)
{
    Weapon->ServerFire(Start, End);
}
```

</details>


