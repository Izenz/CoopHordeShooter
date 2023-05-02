# Multiplayer Third-person Horde Shooter made with Unreal Engine 4.
Personal project developed for educational purpouses with Unreal Engine 4 that features:
 - Logic implemented through Blueprint and C++ code.
 - Hitscan and projectile weapons.
 - Basic AI in C++.
 - Advanced AI using behavior trees and EQS.
 - Horde game-mode spawning enemy AI.
 - Authority handling and replication to support multiplayer gameplay.
 - UMG UI
 
 # Game Info
 
 ## Playable Character
 
 Character controlled by every activate player. It has a base health 100 points and is able to walk, shoot, crouch and jump.
 
 ### Rifle Weapon
 
 Currently the only available weapon for the player. It has unlimited ammunition.
 There is no recoil but there is minor **bullet spread**.
 Players can fire it with left mouse button and aim with right mouse button.
 
 ### Crouching
 
 Reduces weapon's bullet spread and cancels walking noise at the cost of movement speed.
 Players can crouch while holding left control button.
 
 ### Health Indicator
 
 After taking considerable damage this indicator will appear on the screen of the player indicating that it has sustained considerable damage. The image gets more and more visible as the player gets closer to death.
 
 ## Enemies
 
 Every enemy awards 20 score points to the player killing them.
 
 ### Tracker Ball
 
 This enemy will roam around the map and chase any nearby players to try and self-destruct near them.
 It has a small area of effect when it explodes and its base damage is low. However, when near other balls its power escalates according to the number of nearby Tracker Balls, up to 4 times, dealing devastating damage to any players caught in the explosion.
 It awards no score points if it self destructs. 
 
 ### Rifle Soldier
 
 Less common enemy. This enemy will chase the player and try to find cover to shoot the player from a distance. When its hp is low, it will try to flee and find a safe spot to recover its health.
 
 ## Power Ups
 
 ### Speed-up
 
 Enhances the movement speed of the player activating it for a moderate amount of time.
 
 ### Healing
 
 Provides a healing buff to the player who activates it. This healing buff will recover a set amount of missing health points after a small delay, twice, before expiring.
 
 # How to play with friends
 
 - Start the game and hit play on the menu.
 - Open up the console command with `~` key.
 - The person hosting the match (server) should type `open Blockout_P?listen`.
 - The person joining an existing match should type `open SERVER_IP:7777`. Whereas `SERVER_IP` is the IP address of whoever is hosting the match.
 - For testing purpouses it is possible to open two instances of the game on the same machine and join using `open 127.0.0.1`. No need to specify port in this case.
 
 # Engine Version
 Project developed and tested with version: 4.18.3
