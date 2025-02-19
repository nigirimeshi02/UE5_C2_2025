// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Game/System/LockOnInterface.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "EnemyBaseAttributeSet.h"
#include "Enemy_Base.generated.h"

UCLASS()
class UE5_C2_2025_API AEnemy_Base : public ACharacter, public ILockOnInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	//ロックオンのウィジェット
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widget, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* LockOnMarkerWidget;

	//AbilitySystemコンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	class UAbilitySystemComponent* AbilitySystemComponent;

	//エネミー用AttributeSet
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UEnemyBaseAttributeSet* EnemyBaseAttributeSet;


public:
	// Sets default values for this character's properties
	AEnemy_Base();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//ロックオンの有効フラグを設定する
	virtual void SetLockOnEnable_Implementation(bool LockOnFlg)override;

protected:
	// AbilitySystemコンポーネントの取得
	UAbilitySystemComponent* GetAbilitySystemComponent() const
	{
		return AbilitySystemComponent;
	};

};
