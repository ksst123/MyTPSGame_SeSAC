// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerAnim.h"
#include "TPSPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

void UTPSPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// Owner의 Velocity를 가져와서 
	ATPSPlayer* owner = Cast<ATPSPlayer>(TryGetPawnOwner());

	if (owner != nullptr)
	{
		// RightVelocity와 ForwardVelocity를 구한다.
		// Vector의 내적을 이용해서 방향값을 구하고 싶다.
		FVector velocity = owner->GetVelocity();
		RightVelocity = FVector::DotProduct(owner->GetActorRightVector(), velocity);
		ForwardVelocity = FVector::DotProduct(owner->GetActorForwardVector(), velocity);

		// IsJumping을 알고 싶다.
		IsJumping = owner->GetCharacterMovement()->IsFalling();
	}
}

void UTPSPlayerAnim::OnFire(FName sectionName)
{
	ATPSPlayer* owner = Cast<ATPSPlayer>(TryGetPawnOwner());
	owner->PlayAnimMontage(FireMontage, 1, sectionName);
	// Montage_Play(FireMontage);
}

void UTPSPlayerAnim::OnGunReload()
{
	ATPSPlayer* owner = Cast<ATPSPlayer>(TryGetPawnOwner());
	owner->OnMyGunReload();
}

void UTPSPlayerAnim::OnSniperReload()
{
	ATPSPlayer* owner = Cast<ATPSPlayer>(TryGetPawnOwner());
	owner->OnMySniperReload();
}
