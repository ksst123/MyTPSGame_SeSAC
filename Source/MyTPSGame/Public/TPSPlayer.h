// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"

// class USpringArmComponent; => 원래 정석의 전방선언(Forward Declaration) 방식

#define GRENADE_GUN true
#define SNIPER_GUN false

DECLARE_MULTICAST_DELEGATE_OneParam(FSetupInputDelegate, class UInputComponent*);

UCLASS()
class MYTPSGAME_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;




	// =================================================
	UPROPERTY(EditAnywhere)
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UCameraComponent* CameraComponent;

	// 총 스켈레탈 메시 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* GunMeshComponent;

	// 저격총 스태틱 메시 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* SniperMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UTPSPlayerMoveComponent* MoveComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UTPSPlayerFireComponent* FireComponent;

	UPROPERTY(EditAnywhere)
	class USoundWave* FireSound;

	FSetupInputDelegate SetupInputDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 CurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxHP = 100;

	UFUNCTION(BlueprintNativeEvent)
	void OnMyHit(int damage);

	UFUNCTION(BlueprintImplementableEvent)
	void OnMyGameOver();

	UFUNCTION(BlueprintImplementableEvent)
	void OnMyChooseGun(bool IsGrenadeGun);

	UFUNCTION(BlueprintImplementableEvent)
	void OnMyGrenadeGunAmmoUpdate(int32 current, int32 max);
	UFUNCTION(BlueprintImplementableEvent)
	void OnMySniperGunAmmoUpdate(int32 current, int32 max);
	// =================================================

};
