/*
 * lumen 끔
 * AI System forget state actors false
 * DefaultGame.ini 
	[/Script/AIModule.AISense_Sight]
	bAutoRegisterAllPawnsAsSources=false
 * TraceChannels Target
 * 에셋매니저 클래스 변경
 * 게임플레이큐 경로
 * 에셋매니저 프리머리 에셋 추가
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
		const FName CanCast("CanCast");
		const FName UpgradeAvailable("UpgradeAvailable");
	}
	namespace ConstValue
	{
		constexpr int32 MaxLevel= 4;
	}
	namespace TableRowName
	{
		const FName ExperienceRowName("ExperienceNeededToReachLevel");
	}
}
