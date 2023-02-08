// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TPSPlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class MYTPSGAME_API UTPSPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	void NativeUpdateAnimation(float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ForwardVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RightVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsJumping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsCrouching;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* FireMontage;

	UFUNCTION(BlueprintCallable)
	void OnFire(FName sectionName);

	UFUNCTION(BlueprintCallable)
	void OnGunReload();

	UFUNCTION(BlueprintCallable)
	void OnSniperReload();
};
