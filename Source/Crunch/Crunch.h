/*
 * lumen 끔
 * AI System forget state actors false
 * DefaultGame.ini 
	[/Script/AIModule.AISense_Sight]
	bAutoRegisterAllPawnsAsSources=false
 * TraceChannels Target
 */

#pragma once

#include "CoreMinimal.h"

#define ECC_Target ECC_GameTraceChannel1

namespace Crunch
{
	namespace AIKey
	{
		const FName Target("Target");
		const FName Goal("Goal");
	}
	namespace MatParam
	{
		const FName Icon("Icon");
		const FName Level("Level");
		const FName Percent("Percent");
	}
	namespace ConstValue
	{
		constexpr int32 MaxLevel= 4;
	}
}
