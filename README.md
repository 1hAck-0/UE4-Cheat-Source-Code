# UE4 Cheat Source Code | Sword With Sauce
This a small cheat menu for Sword With Sauce (a UE4 game) that I made in a week or so. I started this project with 1 main goal in mind which is to learn the basics of Unreal Engine game hacking. You will find many comments and the purpose is to easily teach the basics of UE4 game hacking.

**If you find this repository useful in any way, please star it so it reachs out to more people!**

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
I wasn't able to make a few options/features in the menu myself so I hope some of you can help with that if they have experience:
* Silent Aim
* Use the Enemy velocity for more accurate Aimbot calculations (I was able to get the velocity of the enemies but somewhy when I tried to implement it in my calculations, my aimbot was very laggy)
* No Recoil/Spread
* View the first comment in FHitResult::GetActor() in UnrealEngine.cpp; why do we have to add 3 to get the actual serial number!?
If you know how to make any of these, please make a pull request!

## UE Guide
I had previously experience with Unity game hacking which seems definitely a lot easier than Unreal Engine game hacking. So if you are a beginner I highly recommend to start out with an easier engine to hack.

Anyway, this is how my "journey" went: At first I didn't even have an idea whether Unreal Engine games need a **special approach** to hack and as I learned, they definitely do. I read the forum about Unreal Engine Game Hacking on guidedhacking.com and honestly, it wasn't very good. Most of the stuff that I know now I taught myself because there really aren't many great tutorials for people starting out with Unreal Engine. As you read down below, I used a cheat engine table to find most of the structure offsets. It is more like a dumper because it automatically dumps the most important offsets like UWorld/GWorld and gives you a script with which you can easily reverse any class related to the game just like you can with the mono dissector in cheat engine. To enable this option, first run the "Unreal Engine" script in the cheat table, then turn on "UE Structure Lookup".

I had to download a few different versions of the **cheat table** until I found one that worked for my target game. Also it should be obvious but, **don't learn UE game hacking on a game with an anti-cheat**. You are setting up yourself for failure and hair loss. I attached the cheat table in the Tools directory if you want to download it. Here is the forum and downloads for the dumper though: https://fearlessrevolution.com/viewtopic.php?f=23&t=14414

You can **genrate an SDK** for your target game which will have almost all of the structures/classes that the game uses. It is very useful and time-saving but I personally didn't do it. It also seemt quite hard to be honest. The offsets you will need to do so are **UWorld/GWorld, GNames, GObjects and UObject::ProcessEvent()**.

## Tools Used
* Visual Studio 2022 Preview
* **Unreal Finder Tool**: https://github.com/CorrM/Unreal-Finder-Tool
* **Cheat Engine** + **UE4 Win64 Base Table**: https://fearlessrevolution.com/viewtopic.php?f=23&t=14414
* **IDA Pro** (a must have for hacking UE games)

Needless to say, you need an IDE to compile your project (hack) so I used VS22. I personally didn't use **Unreal Finder Tool** all that much but you can use it for **finding GNames, GObjects** and generating an SDK as well as dumping GNames, GObjects (dumping in the sense of getting all of the names and objects, not the offset of GNames and GObjects).

**Cheat Engine** is a classic no matter what game you are hacking. The thing that helped me by far the most is the UE4 Win64 Base Table for Cheat Engine though. It lets you find your local player and UWorld literally within a single click. The other thing that's really useful is the UE Structurue Lookup script in the table. It lets you view the game/unreal engine structures in a really nice format just like Mono Dissector. I used the latest version of the tool right now which is v6.9.

**IDA Pro**, also another classic in game hacking. What you will use it for is finding offsets + functions and reversing game functions. I personally had no problem finding ANY Unreal Engine function with IDA Pro. Like for example UObject::ProcessEvent(). The best way to find Unreal Engine functions/offsets is to download the Unreal Engine version your game uses and then build a simple game with it. The magic is in the .pdb which Unreal Engine can generate. If you open the generated game exe with its linked pdb, all the Unreal Engine functions will be named. Then you can simply go to the function you want to find, copy the opcodes and signature scan them in IDA in the other game. As you will see in this project, I used patterns/sigantures too. Here is a specific tutorial on that: https://guidedhacking.com/threads/how-to-easily-make-signatures-for-unreal-engine-4-games.15529/
