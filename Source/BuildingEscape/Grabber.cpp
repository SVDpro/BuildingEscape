// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

    FindPhysicsHandle();
    SetupInputComponent();
}

void UGrabber::Grab()
{
    FHitResult HitResult = GetFirstPhysicsBodyInReach();
    UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
    AActor* ActorHit = HitResult.GetActor();

    // If we hit something then attach the physics handle.
    if (ActorHit != nullptr && PhysicsHandle != nullptr)
    {
        PhysicsHandle->GrabComponentAtLocation(ComponentToGrab, NAME_None, GetPlayersReach());
    }
}

void UGrabber::Release()
{
    if (PhysicsHandle != nullptr)
        PhysicsHandle->ReleaseComponent();
}

// Check for the Physics Handle Component
void UGrabber::FindPhysicsHandle()
{
    PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

    if (PhysicsHandle == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("No physics handle component found on %s!"), *GetOwner()->GetName());
    }
}

void UGrabber::SetupInputComponent()
{
    InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();

    if (InputComponent != nullptr)
    {
        // bind actions
        InputComponent->BindAction("Grab", EInputEvent::IE_Pressed, this, &UGrabber::Grab);
        InputComponent->BindAction("Grab", EInputEvent::IE_Released, this, &UGrabber::Release);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("No input component found on %s!"), *GetOwner()->GetName());
    }
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // If the physics handle attached.
    if (PhysicsHandle != nullptr && PhysicsHandle->GrabbedComponent != nullptr)
    {
        // Move the object we are holding.
        PhysicsHandle->SetTargetLocation(GetPlayersReach());
    }
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
    // Ray-cast out to a certain distance (Reach)
    FHitResult Hit;
    FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

    GetWorld()->LineTraceSingleByObjectType(OUT Hit,
        GetPlayersWorldPos(),
        GetPlayersReach(),
        FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
        TraceParams);

    return Hit;
}

FVector UGrabber::GetPlayersReach() const
{
    FVector PlayerViewPointLocation;
    FRotator PlayerViewPointRotation;

    GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);

    return PlayerViewPointLocation + (PlayerViewPointRotation.Vector() * Reach);
}

FVector UGrabber::GetPlayersWorldPos() const
{
    FVector PlayerViewPointLocation;
    FRotator PlayerViewPointRotation;

    GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);

    return PlayerViewPointLocation;
}
