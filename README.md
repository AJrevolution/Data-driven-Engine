# Data-driven-Engine
University project of engine creation

Code samples for the sake of portfolio, not entire project with any dlls or libs

elements is a lua file that handles the level creator for the project, i attatched documentation to show how to use it,
and explains how to create your own custom levels that you can browse over.

main file is of course the main.cpp, which contains the main game loop to execute each subsystem. 

I have not included the audio subsystem, nothing special, simply played the desired audio clip when an event required it to.

Event manager can be seen inside PhysicsManager and Renderer, the renderer will always act to draw the scene (hence not looking for specific events
like the physics manager), the only events inside the engine are movement based, such as LEFT, RIGHT etc as seen from physicsManager.cpp
