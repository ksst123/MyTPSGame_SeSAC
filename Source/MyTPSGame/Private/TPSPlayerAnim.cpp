// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerAnim.h"
#include "TPSPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

void UTPSPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// Owner�� Velocity�� �����ͼ� 
	ATPSPlayer* owner = Cast<ATPSPlayer>(TryGetPawnOwner());

	if (owner != nullptr)
	{
		// RightVelocity�� ForwardVelocity�� ���Ѵ�.
		// Vector�� ������ �̿��ؼ� ���Ⱚ�� ���ϰ� �ʹ�.
		FVector velocity = owner->GetVelocity();
		RightVelocity = FVector::DotProduct(owner->GetActorRightVector(), velocity);
		ForwardVelocity = FVector::DotProduct(owner->GetActorForwardVector(), velocity);

		// IsJumping�� �˰� �ʹ�.
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
