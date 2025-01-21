// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE5_C2_2025GameMode.h"
#include "UE5_C2_2025Character.h"
#include "UObject/ConstructorHelpers.h"

AUE5_C2_2025GameMode::AUE5_C2_2025GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
