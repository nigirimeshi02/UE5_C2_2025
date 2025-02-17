// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Enemy/Enemy_Base.h"
#include "Components/WidgetComponent.h"

// Sets default values
AEnemy_Base::AEnemy_Base()
{
 	//Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//WidgetComponentを追加する
	LockOnMarkerWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	LockOnMarkerWidget->SetupAttachment(RootComponent);
	LockOnMarkerWidget->SetWidgetSpace(EWidgetSpace::Screen);
	//設定したいウィジェットのクラスを作成
	FString LockOnMarkerWidgetPath = TEXT("/Game/Game/UI/WBP_LockOn.WBP_LockOn_C");
	TSubclassOf<UUserWidget>LockOnMarkerWidgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(*LockOnMarkerWidgetPath)).LoadSynchronous();
	//SetWidgetでnullptrを入れないとクラッシュする
	LockOnMarkerWidget->SetWidget(nullptr);
	//WidgetClassを設定
	LockOnMarkerWidget->SetWidgetClass(LockOnMarkerWidgetClass);
	//非表示にする
	LockOnMarkerWidget->SetVisibility(false);

	//AbilitySystemコンポーネントを追加する
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	//エネミー用AttributeSetを追加
	EnemyBaseAttributeSet = CreateDefaultSubobject<UEnemyBaseAttributeSet>(TEXT("AttributeSet"));
}

// Called when the game starts or when spawned
void AEnemy_Base::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemy_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy_Base::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy_Base::SetLockOnEnable_Implementation(bool LockOnFlg)
{
	if (LockOnFlg)
	{
		LockOnMarkerWidget->SetVisibility(true);
	}
	else
	{
		LockOnMarkerWidget->SetVisibility(false);
	}
}

