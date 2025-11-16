#include "GAS/AttributeSets/KREnemyAttributeSet.h"

UKREnemyAttributeSet::UKREnemyAttributeSet()
{
	InitCurrentPoise(1.f);
	InitMaxPoise(1.f);
	InitPoiseRecoveryRate(1.f);
	InitPoiseRecoveryDelay(1.f);
}
