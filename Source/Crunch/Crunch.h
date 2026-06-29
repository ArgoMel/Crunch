/*
 * lumen 끔
 * AI System forget stale actors true
 * DefaultGame.ini 
	[/Script/AIModule.AISense_Sight]
	bAutoRegisterAllPawnsAsSources=false
	
	[Crunch.Net]
	CoordinatorURL="3.143.228.73"
	
 * DefaultEngine.ini 
	[OnlineSubsystemEOS]
	bEnabled=true
	
	[OnlineSubsystem]
	DefaultPlatformService=EOS
	
	[/Script/Engine.Engine]
	!NetDriverDefinitions=ClearArray
	+NetDriverDefinitions=(DefName="GameNetDriver",DriverClassName="/Script/SocketSubsystemEOS.NetDriverEOSBase",DriverClassNameFallback="/Script/OnlineSubsystemUtils.IpNetDriver")
	+NetDriverDefinitions=(DefName="DemoNetDriver",DriverClassName="/Script/Engine.DemoNetDriver",DriverClassNameFallback="/Script/Engine.DemoNetDriver")
	
 * TraceChannels Target
 * 에셋매니저 클래스 변경
 * 게임플레이큐 경로
 * 에셋매니저 프리머리 에셋 추가
 * 패키징 쿠킹항 추가 에셋 다랙터리에 파티클폴더,게임플레이큐 폴더,/NNEDenoiser 추가
 * 플래폼-윈도우 스플래시 이미지 2장, 아이콘 이미지 1장 변경
 * 
 * aws 서버 할떄
 * eos overlay부터 online subsystem eos까지 6개 플러그인과 socket subsystem 플러그인 켜기
 * 플러그인 online subsystem EOS artifacts,default artifact name 세팅
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
		const FName RenderTarget("RenderTarget");
		const FName Progress("Progress");
		const FName Saturation("Saturation");
		const FName Empty("Empty");
	}
	namespace VFXParam
	{
		const FName Length("Length");
		const FName Origin("Origin");
	}
	namespace ConstValue
	{
		constexpr int32 MaxLevel= 4;
		constexpr uint8 MaxPlayerCount= 5;
	}
	namespace TableRowName
	{
		const FName ExperienceRowName("ExperienceNeededToReachLevel");
	}
	namespace SocketName
	{
		const FName Root("root");
		const FName Lazer("Lazer");
		const FName TargetDashCenter("TargetDashCenter");
	}
	namespace Session
	{
		const FName SessionName("SESSION_NAME");
		const FName SessionSearchID("SESSION_SEARCH_ID");
		const FName PortKey("PORT");
		const FName CoordinatorURLKey("COORDINATOR_URL");
		const FName TestingURLKey("TESTING_URL");
	}
}
