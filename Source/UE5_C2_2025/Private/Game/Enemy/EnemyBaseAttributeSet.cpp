// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Enemy/EnemyBaseAttributeSet.h"

UEnemyBaseAttributeSet::UEnemyBaseAttributeSet() :
	Health(50000.f),
	MaxHealth(50000.f),
	Stamina(100.f),
	MaxStamina(100.f),
	Energy(0.f),
	MaxEnergy(100.f),
	AttackPower(18.f),
	DefensePower(500.f),
	CriticalRate(5.f),
	CriticalDamage(150.f),
	AttackSpeed(1.2f)
{

}

void UEnemyBaseAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{

}
