// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Player/Player_Base.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Game/System/LockOnInterface.h"
#include "Runtime/GameplayTags/Public/GameplayTags.h"

#define DEFAULT_TARGET_ARM_LENGTH	800.f			//デフォルトのプレイヤーまでのカメラの距離
#define MAX_TARGET_ARM_LENGTH		3000.f			//デフォルトのプレイヤーまでのカメラの距離

APlayer_Base::APlayer_Base()
{
	//毎フレーム呼び出す
	PrimaryActorTick.bCanEverTick = true;

	//カプセルコンポーネントのサイズの初期化
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	//コントローラーの回転を取るときは使わない
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	Tags.Add(FName(TEXT("Player")));

	//キャラクタームーブメントの設定
	GetCharacterMovement()->bOrientRotationToMovement = true;				//入力した方向へキャラクターを移動させる
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);	//回転の速さ

	GetCharacterMovement()->JumpZVelocity = 700.f;							//ジャンプ時の初速度
	GetCharacterMovement()->AirControl = 0.35f;								//空中での横方向の動きの制御量
	GetCharacterMovement()->MaxWalkSpeed = 500.f;							//最大歩行速度
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;						//アナログスティックの最小値での歩行速度
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;			//歩行時の減速度

	//CameraBoomを追加する
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);								//RootComponentにアタッチする
	CameraBoom->TargetArmLength = DEFAULT_TARGET_ARM_LENGTH;				//SpringArmの長さを調整する
	CameraBoom->bUsePawnControlRotation = true; 							//PawnのControllerRotationを使用する

	//SpringArmを追加する
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(CameraBoom);
	SpringArm->TargetArmLength = 0.f;
	SpringArm->bDoCollisionTest = false;

	//follow cameraを追加する
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);	//SpringArmにアタッチする
	FollowCamera->bUsePawnControlRotation = false;								//PawnのControllerRotationを使用する
	FollowCamera->PostProcessSettings.MotionBlurAmount = 0.f;					//MotoinBlurをオフにする

	//IM_Defaultを読みこむ
	DefaultMappingContext = LoadObject<UInputMappingContext>(nullptr, TEXT("/Game/ThirdPerson/Input/IMC_Default"));

	//IA_Moveを読み込む
	MoveAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/ThirdPerson/Input/Actions/IA_Move"));
	//IA_Lookを読み込む
	LookAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/ThirdPerson/Input/Actions/IA_Look"));

	//BoxComponentを追加する
	LockOnCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	LockOnCollision->SetupAttachment(RootComponent);																	//RootComponentにアタッチする
	LockOnCollision->SetBoxExtent(FVector(46000.f, 46000.f, 8000.f));													//ボックスのサイズの初期化
	LockOnCollision->SetCollisionProfileName(UCollisionProfile::CustomCollisionProfileName);							//コリジョンプリセットをカスタムに設定
	LockOnCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);												//コリジョンを無効にする
	LockOnCollision->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);									//コリジョンのオブジェクトタイプをLockOnにする
	LockOnCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);									//コリジョンに対する反応をすべてIgnoreにする
	LockOnCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);		//コリジョンに対する反応をPawnだけOverlapにする

	// AbilitySystemコンポーネントを追加する
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	// プレイヤー用AttributeSetを追加
	PlayerAttributeSet = CreateDefaultSubobject<UPlayerAttributeSet>(TEXT("AttributeSet"));

	//AActorの初期化
	LockOnTargetActor = nullptr;

	//FVectorの初期化
	CameraImpactPoint = FVector(0.f);

	//boolの初期化
	LockOnFlg = false;
}

void APlayer_Base::BeginPlay()
{
	Super::BeginPlay();

	//LockOnCollisionのBeginOverlapにイベントをバインドする
	LockOnCollision->OnComponentBeginOverlap.AddDynamic(this, &APlayer_Base::OnLockOnCollisionBeginOverlap);
	//LockOnCollisionのEndOverlapにイベントをバインドする
	LockOnCollision->OnComponentEndOverlap.AddDynamic(this, &APlayer_Base::OnLockOnCollisionEndOverlap);

	//InputMappingContextの追加
	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
}

void APlayer_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LockOnTarget();

	SmoothCameraCollision();
}

void APlayer_Base::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//アクションをバインドする
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		//移動
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayer_Base::Move);

		//視点操作
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayer_Base::Look);

	}
}

void APlayer_Base::Move(const FInputActionValue& Value)
{
	//入力をVector2Dで受け取る
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		//進行方向を探す
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		//前方向を取得
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		//右方向を取得
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		//動きを加える
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APlayer_Base::Look(const FInputActionValue& Value)
{
	//入力をVector2Dで受け取る
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		//ヨーとピッチの入力を加える
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayer_Base::SmoothCameraCollision()
{
	//始点座標
	FVector StartLocation = CameraBoom->GetComponentLocation();
	//カメラの方向ベクトル
	FVector CameraVec = FollowCamera->GetComponentLocation() - StartLocation;
	//正規化
	UKismetMathLibrary::Vector_Normalize(CameraVec, 0.00001);
	//終点座標
	FVector EndLocation = StartLocation + (CameraVec * DEFAULT_TARGET_ARM_LENGTH);
	//半径
	float SphereRadius = 50.f;
	//無視したいアクター
	TArray<AActor*> ActorToIgnore{ this };
	//結果
	FHitResult OutHit;
	//スフィアトレース
	UKismetSystemLibrary::SphereTraceSingle(this, StartLocation, EndLocation, SphereRadius, UEngineTypes::ConvertToTraceType(ECC_Camera), false, ActorToIgnore, EDrawDebugTrace::None, OutHit, true);

	//当たったなら
	if (OutHit.bBlockingHit)
	{
		//ヒットした座標を取得
		CameraImpactPoint = OutHit.ImpactPoint;

		//ヒットしない座標を取得
		double TargetPoint = UKismetMathLibrary::Vector_Distance(StartLocation, CameraImpactPoint) - SphereRadius;

		//カメラの長さを調節
		CameraBoom->TargetArmLength = UKismetMathLibrary::FInterpTo(CameraBoom->TargetArmLength, TargetPoint, GetWorld()->GetDeltaSeconds(), 3);
	}
	//当たっていないなら
	else
	{
		//カメラの長さがデフォルトの長さではないなら
		if (!UKismetMathLibrary::NearlyEqual_FloatFloat(CameraBoom->TargetArmLength, DEFAULT_TARGET_ARM_LENGTH, 1.0))
		{
			//カメラの長さを調節
			CameraBoom->TargetArmLength = UKismetMathLibrary::FInterpTo(CameraBoom->TargetArmLength, DEFAULT_TARGET_ARM_LENGTH, GetWorld()->GetDeltaSeconds(), 3);
		}
	}
}

void APlayer_Base::LockOnTarget()
{
	if (LockOnFlg && 			//ロックオンしているなら
		LockOnTargetActor)		//ロックオンの対象がいるなら
	{
		//向きたい方向へのプレイヤーの回転値を取得
		FRotator FindActorRotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), LockOnTargetActor->GetActorLocation());
		//向きたい方向へのプレイヤーの回転値の補間
		FRotator InterpActorRotarion = UKismetMathLibrary::RInterpTo(this->GetActorRotation(), FindActorRotation, GetWorld()->GetDeltaSeconds(), 10.f);

		//アクターを回転させる
		SetActorRotation(FRotator(this->GetActorRotation().Pitch, InterpActorRotarion.Yaw, this->GetActorRotation().Roll));

		//コントローラーが有効なら
		if (Controller)
		{
			//向きたい方向へのコントローラーの回転値の補間
			FRotator InterpControlRotation = UKismetMathLibrary::RInterpTo(Controller->GetControlRotation(), FindActorRotation, GetWorld()->GetDeltaSeconds(), 3.f);
			//微調整用
			float Adjustment = 1.5f;
			InterpControlRotation.Pitch -= Adjustment;
			////ControllerのPitchの角度を制限する
			double LimitPitchAngle = FMath::ClampAngle(InterpControlRotation.Pitch, -30.f, -5.f);

			//コントローラーを回転させる
			Controller->SetControlRotation(FRotator(LimitPitchAngle, InterpControlRotation.Yaw, Controller->GetControlRotation().Roll));
		}

		//Z軸のみの距離を取得
		double Distance = UKismetMathLibrary::Vector_Distance(FVector(0.f, 0.f, this->GetActorLocation().Z), FVector(0.f, 0.f, LockOnTargetActor->GetActorLocation().Z));
		//Z軸の距離が700以上なら
		if (Distance >= 700)
		{
			//補間させた値
			float ArmLength = UKismetMathLibrary::FInterpTo(CameraBoom->TargetArmLength, MAX_TARGET_ARM_LENGTH, GetWorld()->GetDeltaSeconds(), 3.f);
			CameraBoom->TargetArmLength = ArmLength;
		}
		//Z軸の距離が700以上ではないなら
		else
		{
			//補間させた値
			float ArmLength = UKismetMathLibrary::FInterpTo(CameraBoom->TargetArmLength, DEFAULT_TARGET_ARM_LENGTH, GetWorld()->GetDeltaSeconds(), 3.f);
			CameraBoom->TargetArmLength = ArmLength;
		}
	}
	//ロックオンしていないなら
	else
	{
		//ロックオンの候補がいるか調べる
		if (LockOnCandidates.IsValidIndex(0))
		{
			//コリジョンを無効にする
			LockOnCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		else
		{
			//ロックオンされるまで繰り返す
			LockOnCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			LockOnCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		//ロックオンしていないときにカメラの距離がデフォルトの長さより大きいなら元に戻す
		if (CameraBoom->TargetArmLength > DEFAULT_TARGET_ARM_LENGTH)
		{
			//補間させた値
			float ArmLength = UKismetMathLibrary::FInterpTo(CameraBoom->TargetArmLength, DEFAULT_TARGET_ARM_LENGTH, GetWorld()->GetDeltaSeconds(), 5.f);
			CameraBoom->TargetArmLength = ArmLength;
		}
	}
}

void APlayer_Base::OnLockOnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//ロックオンしていないなら
	if (!LockOnFlg)		
	{
		//配列に追加
		LockOnCandidates.AddUnique(OtherActor);
		//ロックオンの候補がいるか調べる
		if (LockOnCandidates.IsValidIndex(0))
		{
			//プレイヤーの向きを固定する
			GetCharacterMovement()->bOrientRotationToMovement = false;
			//強制的にカメラの長さをもとに戻す
			CameraBoom->TargetArmLength = DEFAULT_TARGET_ARM_LENGTH;
			//ロックオンする
			LockOnFlg = true;
			//ロックオンの対象のアクターを取得
			LockOnTargetActor = GetArraySortingFirstElement(LockOnCandidates);
			//ロックオンの対象がロックオンのインターフェースを持っているならロックオンのマーカーを表示する
			if (LockOnTargetActor->GetClass()->ImplementsInterface(ULockOnInterface::StaticClass()))
			{
				ILockOnInterface::Execute_SetLockOnEnable(LockOnTargetActor, true);
			}
		}
	}
}

void APlayer_Base::OnLockOnCollisionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void APlayer_Base::AddAbility(TSubclassOf<class UGameplayAbility> Ability, int32 AbilityLevel)
{
	if (AbilitySystemComponent && Ability)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability.GetDefaultObject(), AbilityLevel, -1));
	}
}

bool APlayer_Base::ActivateAbilitiesWithTags(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation)
{
	if (AbilitySystemComponent) 
	{
		return AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTags, bAllowRemoteActivation);
	}

	return false;
}

AActor* APlayer_Base::GetArraySortingFirstElement(TArray<AActor*> Array)
{
	TArray<AActor*> SortArray = Array;

	//最後の配列番号を取得
	int32 LastIndex = SortArray.Num() - 1;

	for (int32 I = 0; I < LastIndex; I++)
	{
		for (int32 J = I + 1; J < LastIndex; J++)
		{
			//配列のI番目よりJ番目の方が距離が小さいならスワップさせる
			if (GetDistanceTo(SortArray[I]) > GetDistanceTo(SortArray[J]))
			{
				SortArray.Swap(I, J);
			}
		}
	}

	//最初の要素を返す
	return SortArray[0];
}

