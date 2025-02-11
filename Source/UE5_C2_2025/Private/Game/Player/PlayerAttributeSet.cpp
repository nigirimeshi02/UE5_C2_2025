// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Player/PlayerAttributeSet.h"

UPlayerAttributeSet::UPlayerAttributeSet() :
	Health(300.f),
	MaxHealth(300.f),
	Stamina(100.f),
	MaxStamina(100.f),
	Energy(0.f),
	MaxEnergy(100.f),
	AttackPower(128.f),
	DefensePower(88.f),
	CriticalRate(36.f),
	CriticalDamage(150.f),
	AttackSpeed(1.2f),
	SkillCoolTime(5.0f),
	UltimateSkillCoolTime(30.0f)
{

}

void UPlayerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// スタミナ値の更新が走った場合はスタミナ値をMaxとMinの間に収まるようにする
	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, 100.f));
	}

	if (Data.EvaluatedData.Attribute == GetSkillCoolTimeAttribute())
	{
		if (GetSkillCoolTime() < 0)
		{
			SetSkillCoolTime(5.0f);
		}
	}

	if (Data.EvaluatedData.Attribute == GetUltimateSkillCoolTimeAttribute())
	{
		if (GetUltimateSkillCoolTime() < 0)
		{
			SetUltimateSkillCoolTime(30.0f);
		}
	}
}
