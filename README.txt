=====================================================================
Drifter v3.0a by Eric Burgess aka RamenAndVitamins (ericdb@gmail.com)
=====================================================================


========
OVERVIEW
========
In Drifter, you pilot your ship at high speed through a winding tunnel. 
Unlike most games of this kind however, your ship has inertia, and will 
not stop moving on its own once you give it a nudge. If you want it to 
stop, you have to nudge it the other way. This may sound difficult but 
as you will see, it makes the game MUCH easier to control, as well as 
adding depth to the scoring.


=======
CONTROL
=======
Pilot your ship with the LEFT ARROW and RIGHT ARROW buttons. Each time 
you tap the button, you give your ship a nudge in that direction. 
Tapping a button repeatedly will cause your ship to move faster (side 
to side that is; you have no control over your forward speed). If you 
really need to move fast, you can hold down the button. But don't lose 
control!

By getting just the right sideways speed, you can drift down a slanting 
corridor without touching the controls at all!  In fact, this is what 
you should always try to do, since by performing long drifts, you'll earn
big bonuses. (see DRIFTS, below)

At regular intervals you will see a horizontal line across the course. 
These lines are mile markers. The number of miles you've covered so 
far is shown on the marker as well. Every two miles, the tunnel width 
will decrease by one pixel, so that the difficulty slowly increases as 
you go.

Crashing into the wall will end your game, and display your score, 
along with the number of miles you covered, and the high score.


======================
SCORING (CLASSIC MODE)
======================

Every frame (one pixel of forward distance) your score will increase by 
a number of points equal to the mile you're on. For instance, during
the first mile, you get one point per frame, and during the tenth mile,
you'll be earning ten points each frame. Obviously, making it many miles 
into the tunnel is a key to huge scores!  However, that's not the only 
thing...


======
DRIFTS
======
Drifts are the single biggest key to maximizing your score. A drift 
begins when you release the left or right arrow key, and lasts until the
next time you press a button. Your ship will "drift" at a constant 
speed during this time, hence the name. If you execute a long enough
drift, you will see a "C" (Crazy Drift) icon appear at the left edge of 
the screen. Drift even longer and you'll see an "S" (Super Drift). 
Perform a truly huge drift and you'll see an "M" (MEGA Drift!)  Each time
you perform a drift, you get a bonus to your score:

	Crazy Drift	-	+10%
	Super Drift	-	+25%
	MEGA  Drift	-	+100%  (Double!)

(Legend has it that some have attained a level beyond MEGA...)

Since these bonuses are cumulative, you can really rake in the points
with a MEGA Drift. For example, if you had 10,000 points when you 
started a MEGA Drift, you would have 27,500 at the end of it!  This is
because you also get the Crazy and Super bonuses, too.

The distance you must cover to get a Crazy Drift is equal to 1.2 screens.
The Super Drift is awarded when the "C" icon from the Crazy Drift reaches
the bottom of the screen. Similarly, the MEGA Drift is awarded when the 
"S" icon hits the bottom. By watching the icons, you'll know when to cut 
it close and when to play it safe!


==========
COMBO MODE
==========
Combo Mode is new in Drifter 3.0. Combo Mode is where the REAL skill is at.
Think of Classic Mode as training for Combo Mode. In Combo Mode, you don't
gain any points for moving forward. Drifts are worth a flat number of points,
in the following sequence: 100, 200, 300, 500, 800, 1300, and so on. To get
big points, you've got to link your drifts into "combos". 

A combo is a set of drifts with only one thrust in between them. This means
you only get one press of the button (regardless of how long you hold it) to
set up your next drift. This takes careful planning, anticipation, and some
luck to pull off. Sometimes you'll want to sacrifice a long drift if it means 
you can continue your combo. 

Getting combos is difficult, but well worth it. The second drift in a combo is
worth double its normal point value. The third is quadruple, and the fourth is
worth eight times as much. If you could get a 10x combo, then the last drift
would be worth 512 times its normal value!  

Combo Mode is far more challenging than Classic Mode, but you do have a couple
of things going for you. The tunnel starts out wider than in Classic Mode, and
high scores are maintained separately for the two modes. 


======================
CHANGES IN VERSION 3.0
======================
Many new features were added in version 3.0:
- Status bar added with current score, mile, and combo status
- Combo Mode added
- High scores added
- Menu system added
- Online instructions added
- Game speed is now adjustable
- There is now a slight delay before starting the game ("Ready, Set, Go!")

I consider this the "final" version of the game, in that I have implemented all
of the good ideas that I have. Feedback is still welcome as always.

VERSION 2.01 CHANGES:
---------------------
- Fixed a bug where the tunnel became narrower much faster then it should have.

VERSION 2.0 CHANGES:
--------------------
- Drift levels beyond MEGA have been added. Each level beyond MEGA adds another
  x2 multiplier to your score. Get one of these toward the end of a long run
  and watch your score explode!

- The horizontal lines marking the beginning of each mile are now three pixels
  wide instead of one. This should make them easier to see. 

VERSION 1.2 CHANGES:
--------------------
- Fuel consumption penalty was removed. Its purpose was to encourage drifting,
  but the addition of the drift system provided a much better way to achieve
  that goal.

- Narrow-tunnel score bonus was removed. Scores would have inflated out of 
  control, plus the player doesn't have any control over the shape of the course,
  so why reward them for the narrow parts?  Surviving to perform more drifts
  is its own reward!


==========
KNOWN BUGS
==========
- If you leave the game on the title screen until the auto-powerdown
  kicks in, you may have to reset the calculator. It happened to me
  once, and I think that's why.

====================
BUILDING FROM SOURCE
====================
I have included the source code in case anyone is interested. It's not 
pretty, however, nor well-documented. Don't say I didn't warn you!  You
will need to download the ExtGraph library and add the files extgraph.h 
and extgraph.a to the project in order to compile the source. 

Get ExtGraph from https://github.com/debrouxl/ExtGraph

Drifter is written for the TI-89 calculator. It runs fine on the 
hardware version 2, and probably on version 1 as well, although I don't 
have access to one of those, so it hasn't been tested. If you do have 
a version 1 model, I would like to hear how it works (or doesn't).

===========
DEVELOPMENT
===========
Drifter is my first calculator game. I wrote it using TIGCC, which I 
have found to be an excellent development environment, and I highly 
recommend it. For fast graphics routines I used the ExtGraph Library 
version 0.86 beta. Testing was done on my calculator, and on the 
VirtualTI emulator, which is also a fine piece of software. TIGCC, 
ExtGraph, and VTI are all free. Many thanks to the authors who made 
that possible!


==============
(no) COPYRIGHT
==============
I hereby release this game into the Public Domain. This means I waive 
all rights to the code. You may do anything whatsoever with it, 
without asking for my permission. You may distribute or modify it, or 
derive your own game from it.
where appropriate :)

