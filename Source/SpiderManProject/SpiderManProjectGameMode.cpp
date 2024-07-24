// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpiderManProjectGameMode.h"
#include "SpiderManProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASpiderManProjectGameMode::ASpiderManProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
