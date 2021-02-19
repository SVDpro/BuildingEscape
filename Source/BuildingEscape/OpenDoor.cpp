// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDoor.h"
#include "Engine/World.h"

#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"

#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

    InitialYaw = GetOwner()->GetActorRotation().Yaw;
    CurrentYaw = InitialYaw;
    OpenAngle += InitialYaw;

    FindPressurePlate();
    FindAudioComponent();
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (TotalMassOfActors() > MassToOpenDoors)
    {
        OpenDoor(DeltaTime);
        DoorLastOpened = GetWorld()->GetTimeSeconds();
    }
    else
    {
        if ((GetWorld()->GetTimeSeconds() - DoorLastOpened) > DoorCloseDelay)
        {
            CloseDoor(DeltaTime);
        }
    }
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
    HandleDoor(DeltaTime * DoorOpenSpeed, OpenAngle);

    if (AudioComponent != nullptr)
    {
        if (! bOpenDoorSound)
        {
            AudioComponent->Play();
            bOpenDoorSound = true;
            bCloseDoorSound = false;
        }
    }
}

void UOpenDoor::CloseDoor(float DeltaTime)
{
    HandleDoor(DeltaTime * DoorCloseSpeed, InitialYaw);

    if (AudioComponent != nullptr)
    {
        if (! bCloseDoorSound)
        {
            AudioComponent->Play();
            bCloseDoorSound = true;
            bOpenDoorSound = false;
        }
    }
}

void UOpenDoor::HandleDoor(float DeltaTime, float Yaw)
{
    CurrentYaw = FMath::Lerp(CurrentYaw, Yaw, DeltaTime);

    FRotator DoorRotation = GetOwner()->GetActorRotation();
    DoorRotation.Yaw = CurrentYaw;

    GetOwner()->SetActorRotation(DoorRotation);
}

float UOpenDoor::TotalMassOfActors() const
{
    float TotalMass = 0.0f;

    // Find All Overlapping Actors.
    if (PressurePlate != nullptr)
    {
        TArray<AActor*> OverlappingActors;
        PressurePlate->GetOverlappingActors(OUT OverlappingActors);

        // Add Up Their Masses.
        for (auto* Actor : OverlappingActors)
        {
            TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
        }
    }

    return TotalMass;
}

void UOpenDoor::FindPressurePlate() const
{
    if (PressurePlate == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("%s missing pressure plate (trigger volume)!"), *GetOwner()->GetName());
    }
}

void UOpenDoor::FindAudioComponent()
{
    AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();

    if (AudioComponent == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("%s missing audio component!"), *GetOwner()->GetName());
    }
}