// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyFSM.h"

#include "EnemyAnim.generated.h"


/**
 * 
 */
UCLASS()
class MYTPSGAME_API UEnemyAnim : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeBeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEnemyState state;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAttackPlay;

	/*UFUNCTION(BlueprintCallable)
	void OnHitEvent();*/

	UPROPERTY()
	class AEnemy* owner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnemyDieEnd;

	UFUNCTION()
	void AnimNotify_OnHit();
};
