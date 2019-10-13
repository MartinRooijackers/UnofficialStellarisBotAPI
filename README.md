# Unofficial Stellaris Bot API
This is the github page for the unofficial Stellaris bot API. An API which will allow anyone to create bots for stellaris that have full control over their galactic empire, as opposed to the scripted bots on the workshop which are mostly confined to changing AI weights. Unlike those AI mods, this API will allow a bot to do anything a human player can.

This bot API is machine vision based, because paradox probably won't like it if I or others started to reverse engineer their game(s).

Example of OpenCV text detection on Stellaris:<br>
<img width="60%" src ="https://i.imgur.com/FwcMKOA.png"/><br>

Contact: <br>
Reddit for Stellaris AI: https://www.reddit.com/r/StellarisAI/ <br>
My reddit account (for PM): https://www.reddit.com/user/LetaBot/ <br>
My email:   letabot  (a) gmx  (.) com <br>

If you want to contribute, feel free to use any of the above ways to get in contact

# TODO

A lot, after all I just started. Most of it is computer vision, for which I am currently using OpenCV and Tesseract OCR.
But there are other things which don't require any computer vision knowledge.
If you want to contribute to any of these below, let me know:

- Detecting star systems and their connections. Turn it into a node graph ( https://en.wikipedia.org/wiki/Graph_(discrete_mathematics) )
- Cross platform way to do keyboard/mouse actions in the game
- Linux/Mac port (especially Mac, since I don't have one)
- Object detection (for things like fleet design, research options etc)



# FAQ

\# <b>Why not reverse engineering</b>

Beside paradox probably not wanting people to reverse engineer their game, it would also require the offsets to be updated each patch. Whereas a vision based approach only needs an update for new features.

\# <b>Is this cheating? Will I get a VAC ban? Can this be used to get achievements?</b>

Paradox development studio games don't use VAC, so you won't get a VAC ban. You can thus run this in single player, and in multiplayer with just you and the bot without any trouble. In single player, you can set iron man to true and still get achievements because this isn't a mod, but an external program, and thus won't change the checksum (and any other check for a modded game). But if you want an achievement without playing yourself you can just use a steam achievement manager anyway.

In multiplayer it depends on whether or not the host will allow it. The bot could technially be used to substitute a player who is afk, but it is up to the host to determine if that is allowed. If you use it even though it isn't allowed, you could get banned from that specific server. But you can still host/join other games.

\# <b>How will multiplayer work? can you do a compstomp (human vs bot)?</b>

The easiest way to play with/against bots with this bot API is to buy another copy of the base game of stellaris and let the bot run on that one. The host is the only one who needs the DLC. You could set the bots into a team (for compstomp), though how effective they will be together depends on how the bot is programmed. This bot API only gives a framework so more sophisticated bots can be created. Weather or not they actually will be depends on the bot author.

\# <b>Will it work with mods? Do I need any DLC?</b>

Every mod will most likely require some additional vision module, just like a new DLC feature/UI change would. My plan for now is to make it work on the base game and every DLC. Some small mods which only change the numbers/calculations should still work. But the bots created with this API would have to take those changes into account (the bot API only gives the bots information and handles their action decisions)

\# <b>Will you make a bot API for other PDS games</b>

For now I will focus on Stellaris. 

\# <b>Will this work on Linux/Mac/Console? (Mobile?) </b>

This bot API will only work on PC. My main focus is Windows, but a Linux build is planned. I don't have a Mac, so if you do and want to help me port it to Mac, feel free to contact me (see above).
