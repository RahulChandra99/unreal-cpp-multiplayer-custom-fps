# Unreal Cpp FPS Multiplayer Game  
Made using Unreal Engine 5 (Blueprints and C++)

## Project Overview  
This project has first person shooter mechanics with smooth control, full weapon logic, drag and drop inventory, teams, UI and multiplayer support.  

## Features  

### Player Control and Movement  
- Uses CharacterMovementComponent for walk, run, jump, crouch and air control  
- AimOffset asset for smooth aim blend during fire  
- BlendSpace for idle and movement animation  
- Server RPCs for sprint, jump, crouch and aim to keep clients in sync  
- CameraComponent for clean first person view with arms mesh  
- Separate FP mesh and full body mesh with proper visibility rules  

### Weapon System  
- Base weapon class made in Cpp with modular fire logic  
- Data assets for weapon stats such as damage, fire rate, ammo, reload time  
- Server RPC for firing, with client side cosmetic updates  
- Line traces from camera for bullet hits  
- Muzzle flash with Niagara  
- Impact effect based on surface using PhysicalMaterial  
- Reload animation with Montage notify events  
- RepNotify for ammo count so HUD updates on all clients  
- Attachment system using sockets on the FP mesh  
- Pickup actors with collision triggers and replication  

### Inventory System  
- Inventory built in Cpp with TArray of item entries  
- Drag and drop UI made in UMG  
- Items stored as data assets to support easy expand  
- Server RPC for moving, swapping and dropping items  
- Replicated inventory so all changes stay correct in multiplayer  
- UI updates through OnRep callbacks  

### Health Armor and Damage  
- HealthComponent written in Cpp  
- ApplyDamage and TakeDamage pipeline  
- Armor reduces incoming value before health  
- Hit direction passed to UI for feedback  
- Headshot checks using bone names  
- Death state replicated through OnRep  
- Damage events reach both attacker and victim  

### Death and Respawn  
- Server side death handle  
- Drop weapons with spawn parameters  
- Respawn after delay using GameMode rules  
- PlayerState stores kills and deaths  
- RestartPlayer selects correct spawn point  

### Spawn System  
- Team based spawn using tags  
- Spawn selection checks if area is safe  
- GameMode handles first spawn and respawn  
- PlayerController receives spawn info for HUD  

### Team Mode  
- Team choice menu with UMG  
- PlayerState stores team index  
- GameState stores team scores  
- Server increments score when kill happens  
- Match end triggers UI on all clients  

### User Interface  
- HUD made with UMG and Cpp bridges  
- Ammo count, health bar, armor bar and weapon name  
- Crosshair spread driven by velocity and fire events  
- Inventory grid with drag and drop  
- Hit marker triggered from damage event  
- Kill feed with replicated event list  
- Team select UI on match start  

### Visual and Audio Effects  
- Niagara for muzzle flash, impact sparks and cartridge ejection  
- Sound cues for fire, reload, hit, damage and steps  
- Camera shake for rifle fire  
- UI sound for inventory move  
