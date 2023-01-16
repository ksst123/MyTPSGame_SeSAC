// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"

// class USpringArmComponent; => 원래 정석의 전방선언(Forward Declaration) 방식

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

	UPROPERTY(EditAnywhere)
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UCameraComponent* CameraComponent;

	// 총알 공장
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABulletActor> BulletFactory;
	// UClass를 사용해도 위와 같은 구현이 가지하지만, UClass의 범위가 너무 넓어 문제가 발생할 수 있기 때문에 TSubobject<>로 정확히 범위를 정해준다.
	// class UClass* BulletFactory;

	// 총 스켈레탈 메시 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* GunMeshComponent;

	UPROPERTY(EditAnywhere)
	float FireInterval = 0.5f;

	void OnAxisHorizontal(float value);
	void OnAxisVertical(float value);
	void OnAxisLookUp(float value);
	void OnAxisTurnRight(float value);
	void OnActionJump();
	void OnActionFirePressed();
	void OnActionFireReleased();
	
	// 실제로 총알을 발사하는 함수
	void DoFire();

private:
	FVector direction;
	float WalkSpeed = 600.f;

	FTimerHandle FireTimerHandle;

};
