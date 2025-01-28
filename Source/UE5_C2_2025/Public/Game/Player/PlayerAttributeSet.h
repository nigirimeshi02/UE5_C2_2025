// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "PlayerAttributeSet.generated.h"

//アクセサ(Setter、Getter)を生成するためのマクロ
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class UE5_C2_2025_API UPlayerAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	// コンストラクタ
	UPlayerAttributeSet();

	// GameplayEffect動作後の処理
	void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)override;

	//体力
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Health)

	//最大体力
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxHealth)

	//スタミナ
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Stamina)

	//最大スタミナ
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxStamina)

	//攻撃力
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, AttackPower)

	//防御力
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttributeData DefensePower;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, DefensePower)
	
	//クリティカル率
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttributeData CriticalRate;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, CriticalRate)

	//クリティカルダメージ
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayAttributeData CriticalDamage;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, CriticalDamage)

};
