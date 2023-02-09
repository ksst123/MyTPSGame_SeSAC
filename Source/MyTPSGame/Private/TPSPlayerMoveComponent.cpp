// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerMoveComponent.h"
#include "TPSPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UTPSPlayerMoveComponent::UTPSPlayerMoveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTPSPlayerMoveComponent::BeginPlay()
{
	Super::BeginPlay();

	owner->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}


// Called every frame
void UTPSPlayerMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// direction 방향으로 이동
	FTransform trans(owner->GetControlRotation());
	FVector ResultDirection = trans.TransformVector(direction);
	// 현재 인풋 값은 카메라를 기준으로 잡기 때문에, 위나 아래를 본 상태로 전후 이동을 하면 작동되지 않는다. 이를 수정하기 위해서
	ResultDirection.Z = 0;
	ResultDirection.Normalize();
	owner->AddMovementInput(ResultDirection);
	// Tick 보다 인풋 함수가 더 적게 호출될 경우 오차가 생길수도 있기 때문에 direction 값을 0으로 한다.
	direction = FVector::ZeroVector;

	// P = P0 + vt
	// SetActorLocation(GetActorLocation() + direction * WalkSpeed * DeltaTime);
}

void UTPSPlayerMoveComponent::SetupPlayerInput(UInputComponent* PlayerInputComponent)
{
	// 상하좌우 이동 연결
	PlayerInputComponent->BindAxis(TEXT("MoveHorizontal"), this, &UTPSPlayerMoveComponent::OnAxisHorizontal);
	PlayerInputComponent->BindAxis(TEXT("MoveVertical"), this, &UTPSPlayerMoveComponent::OnAxisVertical);

	// 플레이어 시점 변경 연결
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &UTPSPlayerMoveComponent::OnAxisLookUp);
	PlayerInputComponent->BindAxis(TEXT("TurnRight"), this, &UTPSPlayerMoveComponent::OnAxisTurnRight);

	// 점프 액션 연결
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &UTPSPlayerMoveComponent::OnActionJump);

	// 달리기 액션 연결
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &UTPSPlayerMoveComponent::OnActionRunPressed);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &UTPSPlayerMoveComponent::OnActionRunReleased);

	// 살금살금 걷기 액션 연결
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &UTPSPlayerMoveComponent::OnActionCrouchPressed);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Released, this, &UTPSPlayerMoveComponent::OnActionCrouchReleased);
}

void UTPSPlayerMoveComponent::OnAxisHorizontal(float value)
{
	direction.Y = value;
}

void UTPSPlayerMoveComponent::OnAxisVertical(float value)
{
	direction.X = value;
}

void UTPSPlayerMoveComponent::OnAxisLookUp(float value)
{
	// Y축(Pitch) 회전
	owner->AddControllerPitchInput(value);
}

void UTPSPlayerMoveComponent::OnAxisTurnRight(float value)
{
	// Z축(Yaw) 회전
	owner->AddControllerYawInput(value);
}

void UTPSPlayerMoveComponent::OnActionRunPressed()
{
	owner->GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void UTPSPlayerMoveComponent::OnActionRunReleased()
{
	owner->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void UTPSPlayerMoveComponent::OnActionCrouchPressed()
{
	owner->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
	owner->Crouch();
}

void UTPSPlayerMoveComponent::OnActionCrouchReleased()
{
	owner->GetCharacterMovement()->MaxWalkSpeedCrouched = WalkSpeed;
	owner->UnCrouch();
}

void UTPSPlayerMoveComponent::OnActionJump()
{
	owner->Jump();
}