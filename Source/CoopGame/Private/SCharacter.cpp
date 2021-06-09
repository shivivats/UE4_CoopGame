// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SWeapon.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "CoopGame/CoopGame.h"
#include "Components/SHealthComponent.h"
#include "Components/SInteractionComponent.h"
#include "Net/UnrealNetwork.h"
#include "Projectiles/Grenade.h"
#include "DrawDebugHelpers.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	ZoomedFOV = 65.f;

	bWantsToZoom = false;

	ZoomInterpSpeed = 20.0f;

	WeaponAttachSocketName = "WeaponSocket";

	GrenadeAttachSocketName = "GrenadeSocket";

	InteractionCheckFrequency = 0.f;
	InteractionCheckDistance = 1000.f; // in cms, so this is 10 meters max distance
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultFOV = CameraComp->FieldOfView;

	HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);

	if (HasAuthority())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		//SpawnParams.Owner = this;

		// Spawn a default weapon
		CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);

			WeaponMesh = Cast<USkeletalMeshComponent>(CurrentWeapon->GetComponentByClass(USkeletalMeshComponent::StaticClass()));

		}
	}
}

void ASCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ASCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void ASCharacter::BeginCrouch()
{
	Crouch();
}

void ASCharacter::EndCrouch()
{
	UnCrouch();
}

void ASCharacter::BeginZoom()
{
	bWantsToZoom = true;
}

void ASCharacter::EndZoom()
{
	bWantsToZoom = false;
}

void ASCharacter::PrepareThrow()
{
	if(!HasAuthority())
	{
		ServerPrepareThrow();
		return;
	}

	 if(CurrentWeapon)
	 {
		// if we have a weapon in hand, hide it
		if(WeaponMesh)
		{
			//WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			WeaponMesh->SetVisibility(false);

			// Now we spawn grenade in hand
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.Owner = this;
			//SpawnParams.Owner = this;

			UE_LOG(LogTemp, Warning, TEXT("try spawn grenade in hand"));

			// Spawn the grenade, for now in the weapon socket
			CurrentGrenade = GetWorld()->SpawnActor<AGrenade>(StarterGrenadeClass, GetMesh()->GetSocketLocation(WeaponAttachSocketName), FRotator::ZeroRotator, SpawnParams);
			if (CurrentGrenade)
			{
				UE_LOG(LogTemp, Warning, TEXT("spawned grenade in hand"));
				//CurrentGrenade->SetOwner(this);
				CurrentGrenade->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, GrenadeAttachSocketName);
				//CurrentGrenade->SetActorScale3D(FVector(0.1f));
			}
		}

	 }
}

void ASCharacter::ServerPrepareThrow_Implementation()
{
	PrepareThrow();
}

bool ASCharacter::ServerPrepareThrow_Validate()
{
	return true;
}
void ASCharacter::Throw()
{
	if (!HasAuthority())
	{
		ServerThrow();
		return;
	}

	if(CurrentGrenade)
	{
		// call the throw function here
		UE_LOG(LogTemp, Warning, TEXT("try throw grenade"));

		// we need to calc the direction in which the impulse needs to be added.

		FVector EyeLocation;
		FRotator EyeRotation;
		this->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ThrowDirection = EyeRotation.Vector();

		//FVector TraceEnd = EyeLocation + (ThrowDirection * 10000);

		// so basically if we get a vector in the direction of the eye rotation
		// we can add that to the socket location

		FVector HandLocation = GetMesh()->GetSocketLocation(WeaponAttachSocketName);

		CurrentGrenade->ThrowGrenade(EyeLocation, HandLocation, ThrowDirection);
		CurrentGrenade = nullptr;
	}

	if (CurrentWeapon)
	{
		// if we have a weapon in hand, show it now
		if (WeaponMesh)
		{
			WeaponMesh->SetVisibility(true);
		}
	}
}

void ASCharacter::ServerThrow_Implementation()
{
	Throw();
}

bool ASCharacter::ServerThrow_Validate()
{
	return true;
}

void ASCharacter::StartFire()
{
	if (CurrentWeapon && WeaponMesh->IsVisible())
	{
		CurrentWeapon->StartFire();
	}
}

void ASCharacter::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

bool ASCharacter::IsInteracting() const
{
	// check if the timer is active
	return GetWorldTimerManager().IsTimerActive(TimerHandle_Interact);
}

float ASCharacter::GetRemainingInteractTime() const
{
	// simply return the time remaining
	return GetWorldTimerManager().GetTimerRemaining(TimerHandle_Interact);
}

void ASCharacter::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta,
	const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.f && !bDied)
	{
		// we ded
		bDied = true;

		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		StopFire();

		DetachFromControllerPendingDestroy();

		// after 10 seconds, get destroyed, nerd
		SetLifeSpan(10.f);
	}
}


void ASCharacter::PerformInteractionCheck()
{
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	FVector EyeLocation;
	FRotator EyeRotation;
	this->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	//UE_LOG(LogTemp, Warning, TEXT("eye location value before is: %s"), *EyeLocation.ToString());

	FName HeadBoneLocationName = "head";
	EyeLocation = GetMesh()->GetBoneLocation(HeadBoneLocationName, EBoneSpaces::WorldSpace);


	//UE_LOG(LogTemp, Warning, TEXT("eye location value after is: %s"), *EyeLocation.ToString());
	
	FVector TraceStart = EyeLocation + GetActorForwardVector() * 30.f;
	FVector TraceEnd = EyeLocation + (EyeRotation.Vector() * InteractionCheckDistance);
	FHitResult TraceHit;

	// Making sure the raycast doesnt hit the player themselves
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(true);

	// Run a line trace where we are looking
	if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, COLLISION_INTERACTACTION, QueryParams))
	{
		// Check if we hit something
		if (TraceHit.GetActor())
		{
			UE_LOG(LogTemp, Warning, TEXT("saw an object"));
			// Check if the thing we hit is an interactable
			if (USInteractionComponent* InteractionComponent = Cast<USInteractionComponent>(TraceHit.GetActor()->GetComponentByClass(USInteractionComponent::StaticClass())))
			{
				UE_LOG(LogTemp, Warning, TEXT("saw an interactable object"));

				// Get how far away we are from the object
				float Distance = (TraceStart - TraceHit.ImpactPoint).Size();

				// we found interactable and we are within distance
				if (InteractionComponent != GetInteractable() && Distance <= InteractionComponent->InteractionDistance)
				{
					FoundNewInteractable(InteractionComponent);
				}
				// we found interactable but we are not within distance
				else if (GetInteractable() && Distance > InteractionComponent->InteractionDistance)
				{
					CouldntFindInteractable();
				}

				return;
			}
		}
	}

	// if we get here then we either didnt hit an actor or we didnt hit an interactable
	CouldntFindInteractable();
}

void ASCharacter::CouldntFindInteractable()
{
	// We've lost the interactable. Clear the timer.
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_Interact))
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Interact);
	}

	// Tell the interactable we've stopped focusing on it and clear the current interactable
	if (USInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->EndFocus(this);

		if (InteractionData.bInteractHeld)
		{
			EndInteract();
		}
	}

	// arent viewing any interactable anymore
	InteractionData.ViewedInteractionComponent = nullptr;
}

void ASCharacter::FoundNewInteractable(USInteractionComponent* Interactable)
{
	// when we find a new interactable, first stop interacting with any current interactable
	EndInteract();

	// remove focus from the old interactable if any
	if (USInteractionComponent* OldInteractable = GetInteractable())
	{
		OldInteractable->EndFocus(this);
	}

	// then update the interaction data to have the new interactable
	InteractionData.ViewedInteractionComponent = Interactable;
	Interactable->BeginFocus(this);
}

void ASCharacter::BeginInteract()
{
	// if we're not the server
	if (!HasAuthority())
	{
		// we wanna call begin interact on the server
		ServerBeginInteract();
	}

	InteractionData.bInteractHeld = true;

	// if we have an interactable component
	if (USInteractionComponent* Interactable = GetInteractable())
	{
		UE_LOG(LogTemp, Warning, TEXT("try to interact with an object"));

		Interactable->BeginInteract(this);

		// if interaction time is smol
		if (FMath::IsNearlyZero(Interactable->InteractionTime))
		{
			Interact();
		}
		else
		{
			// now we gotta wait to interact. set a timer to call interact function after the interaction time has elapsed
			GetWorldTimerManager().SetTimer(TimerHandle_Interact, this, &ASCharacter::Interact, Interactable->InteractionTime, false);
		}
	}
}

void ASCharacter::EndInteract()
{
	// if we're not the server
	if (!HasAuthority())
	{
		// we wanna call end interact on the server
		ServerEndInteract();
	}

	InteractionData.bInteractHeld = false;

	// clear the interaction duration timer if the timer was running
	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);

	if (USInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->EndInteract(this);
	}
}

void ASCharacter::ServerBeginInteract_Implementation()
{
	// we just call the begin interact function on the server
	BeginInteract();
}

bool ASCharacter::ServerBeginInteract_Validate()
{
	return true;
}

void ASCharacter::ServerEndInteract_Implementation()
{
	// we just call the end interact function on the server
	EndInteract();
}

bool ASCharacter::ServerEndInteract_Validate()
{
	return true;
}

void ASCharacter::Interact()
{
	// clear the interaction duration timer if the timer was running
	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);

	// we call the interactable's interact function
	if (USInteractionComponent* Interactable = GetInteractable())
	{
		UE_LOG(LogTemp, Warning, TEXT("INTERACTED WITH AN OBJECT"));
		Interactable->Interact(this);
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;

	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	CameraComp->SetFieldOfView(NewFOV);

	// if we are the server and we are interacting
	//const bool bIsInteractingOnServer = (HasAuthority() && IsInteracting());


	// only perform interaction check according to the interaction frequency, for optimisation
	if (GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) >= InteractionCheckFrequency)
	{
		//UE_LOG(LogTemp, Warning, TEXT("performing interaction check"));
		PerformInteractionCheck();
	}
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ASCharacter::BeginInteract);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &ASCharacter::EndInteract);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopFire);

	PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &ASCharacter::PrepareThrow);
	PlayerInputComponent->BindAction("Throw", IE_Released, this, &ASCharacter::Throw);
}

FVector ASCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, CurrentWeapon);
	DOREPLIFETIME(ASCharacter, WeaponMesh);
	DOREPLIFETIME(ASCharacter, bDied);
}