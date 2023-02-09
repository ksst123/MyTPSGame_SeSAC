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

	// direction �������� �̵�
	FTransform trans(owner->GetControlRotation());
	FVector ResultDirection = trans.TransformVector(direction);
	// ���� ��ǲ ���� ī�޶� �������� ��� ������, ���� �Ʒ��� �� ���·� ���� �̵��� �ϸ� �۵����� �ʴ´�. �̸� �����ϱ� ���ؼ�
	ResultDirection.Z = 0;
	ResultDirection.Normalize();
	owner->AddMovementInput(ResultDirection);
	// Tick ���� ��ǲ �Լ��� �� ���� ȣ��� ��� ������ ������� �ֱ� ������ direction ���� 0���� �Ѵ�.
	direction = FVector::ZeroVector;

	// P = P0 + vt
	// SetActorLocation(GetActorLocation() + direction * WalkSpeed * DeltaTime);
}

void UTPSPlayerMoveComponent::SetupPlayerInput(UInputComponent* PlayerInputComponent)
{
	// �����¿� �̵� ����
	PlayerInputComponent->BindAxis(TEXT("MoveHorizontal"), this, &UTPSPlayerMoveComponent::OnAxisHorizontal);
	PlayerInputComponent->BindAxis(TEXT("MoveVertical"), this, &UTPSPlayerMoveComponent::OnAxisVertical);

	// �÷��̾� ���� ���� ����
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &UTPSPlayerMoveComponent::OnAxisLookUp);
	PlayerInputComponent->BindAxis(TEXT("TurnRight"), this, &UTPSPlayerMoveComponent::OnAxisTurnRight);

	// ���� �׼� ����
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &UTPSPlayerMoveComponent::OnActionJump);

	// �޸��� �׼� ����
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &UTPSPlayerMoveComponent::OnActionRunPressed);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &UTPSPlayerMoveComponent::OnActionRunReleased);

	// ��ݻ�� �ȱ� �׼� ����
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
	// Y��(Pitch) ȸ��
	owner->AddControllerPitchInput(value);
}

void UTPSPlayerMoveComponent::OnAxisTurnRight(float value)
{
	// Z��(Yaw) ȸ��
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