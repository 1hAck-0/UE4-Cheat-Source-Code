# UE4 Cheat Source Code and Guide
**FOR EDUCATIONAL PURPOSES ONLY**

This a small cheat menu for Sword With Sauce (a UE4 game) that I made in a week or so. I started this project with 1 main goal in mind which is to learn the basics of Unreal Engine game hacking.

**If you find this repository useful in any way, please star it so it reachs out to more people!**

What you will find in this repository:
* Source Code to a UE4 Multi-Hack with comments and common game hacking techniques like calling, hooking functions and self-updating cheat through pattern scanning
* Introduction and in-depth Guide to hacking UE4 games

![alt text](https://github.com/1hAck-0/UE4-Cheat-Source-Code/raw/master/Media/Menu.png?raw=true)

## Features
* ESP
  * Boxes
    * 2D Boxes
    * 3D Boxes
    * Corner Boxes
  * Snaplines
  * Skeletons

* Aimbot
  * Visibile Check
  * FOV + FOV Circle
  * Smoothness
  * Custom Hotkey

* Triggerbot
  * Custom Hotkey
  * Delay
    * Specific Custom Delay
    * Random Delay (you choose the range)

* Exploits
  * Infinite Health
  * Infinite Ammo
  * No Spread (doesn't work well)
  * No Clip (with custom toggle key option)
  * Flyhack (with custom toggle key option)
  * Speedhack + Speed slider
  * Super Jump + Jump Height slider
  * Modify Gravity + Gravity slider

## To do
I wasn't able to make a few options/features in the menu myself so I hope someone can help with those:
* Silent Aim (scroll down to the Silent Aim part in the Itroduction on why I wasn't able to do it)
* Use the Enemy velocity for more accurate Aimbot calculations (I was able to get the velocity of the enemies but somewhy when I tried to implement it in my calculations, my aimbot was very laggy)
* No Recoil/Spread
* View the first comment in FHitResult::GetActor() in UnrealEngine.cpp: Why do we have to add 3 to get the actual serial number!?

If you know how to make any of these, please make a pull request!

## Build and Compile
* You can only compile in VS22 or higher since the project was created with this version of Visual Studio. For ya all lazy asses who haven't updated since VS19, it's worth it anyway
* Build and compile in x64 architecture since Sword With Suace is 64-bit
* If you are still not able to compile, please put the correct path to the DirectX SDK in project settings->Configuration Properties->VC++ Directories->Include Directories and Library Directories

## Introduction to UE
You should take a special approach to hacking UE games. As you will read in many other tutorials, you will need to find most importantly **UWorld AKA GWorld** which has all of the game information you could ever need. This is often referred to as UWolrd but it's actually called GWorld, UWorld is the class name in the UE Source Code. You will also often stumble across **GNames and GObjects**, GNames contains all of the class names basically, you definetly need **GNames** if you want to make something more than just a simple value hack. You will find a function in my source called mem::GetGName() which will show you how to use GNames, it is located in UnrealEngine/Utils.cpp. **GObjects** is an array just like GNames, it also can be accessed similarly to GNames, however I don't have a function for that in my project. GObjects holds the addresses to all types that the game uses at least as far as I understood it.

The **WorldToScreen** function as well as many other methods used are universal meaning they will be the **same or almost the same across all Unreal Engine games** so you don't need to fully understand some of the things, you can just copy paste them no matter how cheap that sounds. Where the **entity list or bone matrix** is located in what struct isn't game specific, for example the entity list is in `UWorld->PersistentLevel->Actors` (only the offsets will sometimes differ between Unreal Engine versions but the path to the entity list will be the same). I found the BoneMatrix in `EntityClassObject->Mesh->CachedBoneArray`.
Another useful path is `UWorld->GameInstance->LocalPlayers[0]`, this will ALWAYS be the path to your LocalPlayer.

I wasn't able to implement **Silent Aim** in this project, because it's an offline game which means that you have hook the trace line function and modify its output of it in order to have an actual silent aim, however in all modern online games that's not done on the player's client meaning that the only way to have silent aim in an online game is through sending/spoofing the view angles that are sent to the server. The server then will traceline based on those angles meaning you will hit the enemy on the server although you really shouldn't if done properly. The 2 functions you want to look at for Silent Aim are **ULocalPlayer::GetViewPoint()** and **APlayerController::GetPlayerViewPoint()**. Those functions return information about where the local player is looking and by hooking them you can make silent aim in most online games. However there is no guarantee it will work for your target game. Here is some [more information](https://github.com/RiceCum1/UE4-Silent-Aim?ref=https://githubhelp.com) about that. So, back to offline games, you can't simply hook a traceline function first of all because there are many different Unreal Engine functions that your game might be using so you will actually need to reverse the game and find the ShootGun function where you will see what trace line function the game uses. Then you have to hook the ShootGun function and set a boolean in the program to true. In your trace line hook you then check if that boolean is true, if it is true you know that the function was called by the ShootGun function and from there you can return a false hit to simulate silent aim. Here is a [related tutorial](https://www.youtube.com/watch?v=paWftS5yhKU&t).

## UE Guide
I had previously experience with Unity game hacking which seems definitely a lot easier than Unreal Engine game hacking. So if you are a beginner I highly recommend to start out with an easier engine to hack.

Anyway, this is how my *journey* went: At first I didn't even have an idea whether Unreal Engine games need a **special approach** to hack and as I learned, they definitely do. I read the forum about Unreal Engine Game Hacking on guidedhacking.com and honestly, it wasn't very good. Most of the stuff that I know now I taught myself because there really aren't many great tutorials for people starting out with Unreal Engine. As you read down below, I used a cheat engine table to find most of the structure offsets. It is more like a dumper because it automatically dumps the most important offsets like UWorld/GWorld and gives you a script with which you can easily reverse any class related to the game just like you can with the mono dissector in cheat engine. To enable this option, first run the *Unreal Engine* script in the cheat table, then turn on *UE Structure Lookup*. I personally wasn't very determined to find the ShootGun function, that's why the cheat doesn't have this option.

I had to download a few different versions of the **cheat table** until I found one that worked for my target game. Also it should be obvious but, **don't learn UE game hacking on a game with an anti-cheat**. You are setting up yourself for failure and hair loss. I attached the cheat table in the Tools directory if you want to download it. Here is the [forum and downloads](https://fearlessrevolution.com/viewtopic.php?f=23&t=14414) for the dumper. An awesome [tutorial](https://www.youtube.com/watch?v=iYPl9OOMack&t) on the cheat table.

You can **genrate an SDK** for your target game which will have almost all of the structures/classes that the game uses. It is very useful and time-saving but I personally didn't do it. It also seemt quite hard to be honest. The offsets you will need to do so are **UWorld/GWorld, GNames, GObjects and UObject::ProcessEvent()**.

## Tools Used
* Visual Studio 2022 Preview
* [**Unreal Finder Tool**](https://github.com/CorrM/Unreal-Finder-Tool)
* [**Cheat Engine** + **UE4 Win64 Base Table**](https://fearlessrevolution.com/viewtopic.php?f=23&t=14414)
* **IDA Pro** (a must have for hacking UE games)

Needless to say, you need an IDE to compile your project (hack) so I used VS22. I personally didn't use **Unreal Finder Tool** all that much but you can use it for **finding GNames, GObjects** and generating an SDK as well as dumping GNames, GObjects (dumping in the sense of getting all of the names and objects, not the offset of GNames and GObjects).

**Cheat Engine** is a classic no matter what game you are hacking. The thing that helped me by far the most is the UE4 Win64 Base Table for Cheat Engine though. It lets you find your local player and UWorld literally within a single click. The other thing that's really useful is the UE Structurue Lookup script in the table. It lets you view the game/unreal engine structures in a really nice format just like Mono Dissector. I used the latest version of the tool right now which is v6.9.

**NOTE:** *UE Structure Lookup* won't name of the variables inside of a struct even if they are not garbage. For example the CachedBoneArray inside of EntityClassObject->Mesh as well as the entity list(actors) in UWorld->PersistentLevel weren't named by the dumper.

**IDA Pro**, also another classic in game hacking. What you will use it for is finding offsets + functions and reversing game functions. I personally had no problem finding ANY Unreal Engine function with IDA Pro. Like for example **UObject::ProcessEvent()**. The best way to find Unreal Engine functions/offsets is to download the Unreal Engine version your game uses and then build a simple game with it. The magic is in the .pdb which Unreal Engine can generate. If you open the generated game exe with its linked pdb, all the Unreal Engine functions will be named. Then you can simply go to the function you want to find, copy the opcodes and signature scan them in IDA in the other game. As you will see in this project, I used patterns/sigantures too. Here is a [specific tutorial](https://guidedhacking.com/threads/how-to-easily-make-signatures-for-unreal-engine-4-games.15529/) on that.

## Credits
> Thanks to:
* [omar](https://github.com/ocornut) for the awsome ImGui library
* [rdbo](https://github.com/rdbo/ImGui-DirectX-11-Kiero-Hook) for the project template
* [Guided Hacking](https://guidedhacking.com)
