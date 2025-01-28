// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "PlayerAttributeSet.h"
#include "Player_Base.generated.h"

//継承してブループリントで使うためのクラス
UCLASS()
class UE5_C2_2025_API APlayer_Base : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:
	//スプリングアーム
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	//カメラをスムーズにするためのスプリングアーム
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	//カメラ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	//マッピングコンテキスト
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	//移動のインプットアクション
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;
	//視点操作のインプットアクション
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	//ロックオンのボックスコンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* LockOnCollision;

	// AbilitySystemコンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	class UAbilitySystemComponent* AbilitySystemComponent;

	// プレイヤー用AttributeSet
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPlayerAttributeSet* PlayerAttributeSet;

protected:
	//ロックオンの候補
	TArray<class AActor*> LockOnCandidates;

	//ロックオンの対象
	class AActor* LockOnTargetActor;

	//カメラの衝突の絶対座標
	FVector CameraImpactPoint;

	//ロックオンのフラグ
	bool LockOnFlg;

public:
	//コンストラクタ
	APlayer_Base();

protected:
	//ゲーム開始時に一度だけ呼ばれる関数
	virtual void BeginPlay() override;

public:	
	//毎フレーム呼ばれる関数
	virtual void Tick(float DeltaTime) override;

	//関数をバインドするために呼ばれる関数
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	//移動処理
	void Move(const FInputActionValue& Value);
	//視点操作処理
	void Look(const FInputActionValue& Value);

protected:
	//カメラのコリジョンの処理
	void SmoothCameraCollision();
	//ロックオン後のカメラやプレイヤーの処理
	void LockOnTarget();

protected:
	//ロックオンのコリジョンに入ったときの処理
	UFUNCTION()
	void OnLockOnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//ロックオンのコリジョンから出たときの処理
	UFUNCTION()
	void OnLockOnCollisionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Abilityの登録
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "Ability"))
	void AddAbility(TSubclassOf<class UGameplayAbility> Ability, int32 AbilityLevel);

	//Abilityの発動
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "Ability"))
	bool ActivateAbilitiesWithTags(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation);

	//体力を設定
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "AttributeSet"))
	void SetHealth(float NewHealth);

	//最大体力を設定
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "AttributeSet"))
	void SetMaxHealth(float NewMaxHealth);

	//スタミナを設定
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "AttributeSet"))
	void SetStamina(float NewStamina);

	//最大スタミナを設定
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "AttributeSet"))
	void SetMaxStamina(float NewMaxStamina);

	//攻撃力を設定
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "AttributeSet"))
	void SetAttackPower(float NewAttackPower);

	//防御力を設定
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "AttributeSet"))
	void SetDefensePower(float NewDefensePower);

	//クリティカル率を設定
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "AttributeSet"))
	void SetCriticalRate(float NewCriticalRate);

	//クリティカルダメージを設定
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "AttributeSet"))
	void SetCriticalDamage(float NewCriticalDamage);

protected:
	//ソートされた配列の最初の要素を取得
	AActor* GetArraySortingFirstElement(TArray<AActor*> Array);

	// AbilitySystemコンポーネントの取得
	UAbilitySystemComponent* GetAbilitySystemComponent() const 
	{
		return AbilitySystemComponent;
	};

};
