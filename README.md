# Valentines_Windows

Gameplay: find hearts, you need a certain amount of hearts to progress to the next area 

The hearts fill up the sides of your screen 
You will receive status updates in the command prompt, such as things like requiring a certain number of hearts to enter a door

Monday, January 27 2025

I texted Claire and she'll get involved soon. 

I want the doors to slide upwards like they do in the old wolfenstein games

I want to make sure that text prompts only occur once and not spammy like now -- 
proposed solution is a variable passed into the pressing E function that -- I fixed it by commenting out the bit switch when the e key
is realeased, I guess that works for now

I want to change the audio system, look up some tutorials for sdl audio or something. Maybe finish the raycaster video, that might 
be helpful

System that continuously tracks your number of hearts, maybe something like a variable that will spawn in an enemy if you can't 
make progress otherwise

To replace the Leigha walls, I think there should be a hall/doorway looking texture, and Malane's face will be replaced by a door
that slides up as previously mentioned. 

There should be sound effects for all of these things

I need a design for the monsters, maybe different maps should have different colored hearts. Could use the item sound effect for when 
you have collected all the hearts for an area. In the text prompt it can tell you to press enter and then it will load you into a new map
that has different hearts and maybe enemies too -- Leigha suggests evil cupids which I like

Have an end screen, obviously it will say something along the lines of "Happy Valentines Malane" with all sorts of hearts. 
Idea that has just now struck me... the end screen will be just like the old game. 


Sprite : type, state, map, (x,y,z), health
define ENEMY_HEALTH 3
map defines the image that will be used
state will now define the enemy type / whether they are alive (can be rendered such as "if (state)")
type will be renamed to region and define where in the map the enemy will appear

levels [4(levels)][4(rooms)][3(sprites)][4(state, x, y, z)]

levelinfo = levels[LEVEL_NUMBER]
headSprite;
headS;
for (int a = 0; a < ROOM_CONST; a++) {
	for (int b = 0; b < SPRITE_CONST; b++) {
		attribute = levelinfo[a][b]
		if (b == 0) {
			headSprite = newSprite(b, attribute[0], attribute[1], attribute[2], attribute[3]);
		}
		else {
			addSprite(headSprite, b, attribute[0], attribute[1], attribute[2], attribute[3])
		}
	}
}

Saturday, Feb 1

For the door texture I will most likely cheap out for the animation

Tuesday, Feb 11

What textures do I want to work on: Walls, hallways, floor, ceiling
I've added the cupid guy and the gun item + animation

I also want to make sound effects: gun fire, enemy anger, enemy death

Needs a losing heart sound effect

Add music -- the one song, I'm sure I'll remember it -- TOMORROW
Music will play automatically, I'll have a mute button or smth, I'll pause while the main game is playing and 
resume when you hit escape. 

For February 12th:
	Enemy stealing heart sound
	Enemy getting hit sound
	Enemy anger sound
	New gun sound
	Enemy death sound

Right now:

Hex code for the blue is: #58a8cc
Hex code for the grey is: #2b3337
	
ASSETS CREATED USING PISKEL, (some) SOUNDS FROM PIXABAY
In the title screen have a button for you to add more hearts to the screen

February 12th:

I did not make those sound effects, I did, however, make a bunch of other stuff, so there!

Player move has become a really important function, it is going to dictate changing levels as well as game over stuff

Boss at the end of level 3
Different color hearts for different levels