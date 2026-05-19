// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/PA_AbilitySystemGenerics.h"

#include "Crunch/Crunch.h"

const FRealCurve* UPA_AbilitySystemGenerics::GetExperienceCurve() const
{
	return ExperienceCurveTable->FindCurve(Crunch::TableRowName::ExperienceRowName, "");
}
