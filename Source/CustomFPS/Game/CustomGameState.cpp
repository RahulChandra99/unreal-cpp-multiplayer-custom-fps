#include "CustomGameState.h"
#include "Net/UnrealNetwork.h"

void ACustomGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACustomGameState, RemainingTime);
}
