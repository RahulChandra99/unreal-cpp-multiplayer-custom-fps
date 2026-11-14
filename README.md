<h2 align="center">Multiplayer FPS</h2>
<p align="center">Built using Unreal Engine 5.5, Blueprints and a little C++</p>

<p align="center">
  <a href="https://www.youtube.com/watch?v=wd9-LOUOrDo" target="_blank" rel="noopener noreferrer">
    <img alt="Gameplay Demo" width="520" src="https://github.com/user-attachments/assets/bb0fcd5a-91f0-4f21-a847-88551d22dc9f"/>
  </a>
</p>
<p align="center">Click the image to watch the demo.</p>

A Counter-Strike style team shooter. Two teams, three weapons, a server browser and a match timer, all replicated.

The gameplay is written in Blueprint. The C++ side is small: the match timer in the game mode and the replicated time in the game state. I've said so up front because the repo name has "cpp" in it.

| Platform | Engine | Duration | Team Size | Roles |
| --- | --- | --- | --- | --- |
| PC | Unreal Engine 5.5 | Over a month | Solo | Gameplay Programmer, Systems Designer |

## What's in it

**Hosting and joining.** `GI_ServerSessions` creates and finds sessions through the Advanced Sessions plugin. The main menu has a create-server screen and a server browser (`WB_CreateServer`, `WB_ServerBrowser`).

**Teams.** Players pick Terrorist or Counter-Terrorist on `WB_TeamSelection` and spawn at their team's spawn points. `BP_Terrorist` and `BP_CounterTerrorist` both extend `BP_PlayerBase`. There's a scoreboard.

**Weapons.** An AK47, an M4A1 and a pistol, all children of `BP_WeaponBase`, with their stats in a data table (`DT_WeaponInfo`). Primary and secondary slots, reload, drop, and picking weapons up off the ground.

**Movement and animation.** Walk, run, crouch and jump on Enhanced Input. `AnimBP_Player` has separate blend spaces and aim offsets for unarmed, pistol and rifle.

**Networking.** The server owns gameplay state. Clients ask through server RPCs, state comes back through replicated variables and RepNotifies, and cosmetic effects go out as multicasts.

## Where things are

| Path | What |
| --- | --- |
| `Content/Blueprints/Player` | Player base class, team characters, input, animation |
| `Content/Blueprints/Weapons` | Weapon base class and the three weapons |
| `Content/Blueprints/GM_CustomFPS`, `GS_CustomFPS`, `PC_CustomFPS` | Game mode, game state, player controller |
| `Content/Blueprints/GI_ServerSessions` | Session hosting and joining |
| `Content/UI/Widgets_MainMenu` | Menus, server browser, team select, scoreboard, crosshair |
| `Source/CustomFPS/Game` | C++ game mode and game state |

## The C++

`ACustomGameMode` starts a one-second timer when the match starts and ends the match when it runs out. `ACustomGameState::RemainingTime` is replicated so every client's HUD can show the clock.

```cpp
void ACustomGameMode::StartMatch()
{
	Super::StartMatch();

	ACustomGameState* GS = GetGameState<ACustomGameState>();
	if (GS)
	{
		GS->RemainingTime = MatchLength;
	}

	GetWorld()->GetTimerManager().SetTimer(
		MatchTimerHandle,
		this,
		&ACustomGameMode::TickMatchTimer,
		1.0f,
		true
	);
}

void ACustomGameMode::TickMatchTimer()
{
	ACustomGameState* GS = GetGameState<ACustomGameState>();
	if (!GS) return;

	GS->RemainingTime--;

	if (GS->RemainingTime <= 0)
	{
		EndMatch();
		GetWorld()->GetTimerManager().ClearTimer(MatchTimerHandle);
	}
}
```
