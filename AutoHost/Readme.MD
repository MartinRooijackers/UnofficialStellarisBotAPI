Auto host for Stellaris

.exe files and DLL can be gotten from: http://www.filedropper.com/stuffyouneedtorunthis_2


NOTE: This program will essentially take over your mouse and keyboard to perform actions.
You can hold down right control for about 10 seconds to close the program.
Alternatively you can open task manager and close it there (this program can't grab the mouse/keyboard in the task manager)



1. Install VC_redist.x64.exe 

2. Install tesseract-ocr-w64-setup-v5.0.0-alpha.20200328.exe
Preferably on D:\Tesseract-OCR\ 
If you install it elsewhere you have to change that in the settings.txt


3. Change the Paradox documents folder in settings.txt to the propper location

(it looks something like: C:/Users/YourUserName/Documents/Paradox Interactive/Stellaris/ )


4. Set resolution to 1920 x 1680 and UI/text scaling to 100%

5. Run StellarisAI.exe in the same folder as opencv_world411.dll and all the other text files.
At the moment this should already be the case, 
but if you want to run the exe from elsewhere you will need to copy those files as well



Other text files to change:

In settings.txt:
"Username to ignore": set this to the username of the Stellaris account that you use to run the auto-host
Otherwise it will grab commands from its own output as well


message.txt: is the message that is send to the lobby

rulesetcreate.txt: ruleset that is printed which indicated the rules when creating an empire (which origins/civics are banned etc)

rulesetingame.txt: the in-game ruleset. Includes things like the number of years in which no war is allowed.


