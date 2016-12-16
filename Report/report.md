# Yasir Mohamed - 13318246
## Meshes
4 different mesh objects were used in this game:
* City (Textured)
* Plane (Textured)
* Bananaman (Textured)
* Monkey

### City
![images/city.png](City Model)

* File: baseCity.obj
* Root of Hierarchy

This is the main playing area of the game. The roads and buildings together were developed as a single mesh object, so as a result collision detection does not actually work between the player and the buildings of the city since the player technically has to walk through on the city's mesh via the roads.

### Plane
![images/plane.png](Plane Model)

* File: plane2.obj
* Child of Hierarchy

The plane is placed above the city, and it was originally intended to have the plane flying above the city and have it capable of doing bomb runs on the city. I didn't get a chance to implement this feature, so I just kept the plane hovering above the city in my game.

### Bananaman

* File: banananaman4.obj
* Child of Hierarchy

The bananamen represent enemies in the game and spawn randomly on one of the four edges of the city and travel straight through the city to the other side. A new bananaman is spawned every 5 seconds, each has a random speed, and each has its own hitbox to detect collisions. The texture for the bananaman was created by me and is found in the `bananaTex.png` file. The game stops spawning new bananamen when there is a total of 50 active bananamen in the game.

### Bullet

* File: Monkeyhead.dae
* Spawns from player position

The monkey mesh is used as the mesh for the Bullet Class, is untextured, and translates across the map at a constant speed. Bullets come in two sizes, with the bigger one being a Bazooka. A maximum of 30 bullets can be in the world at once, and bullets are fired from the players position, in the direction of the camera. Normal bullets can be fired once every 0.1 seconds, bazookas once every 3 seconds. Bullets also have hitboxes in order to calculate when a collision occurs between a bullet and a bananaman.

An important detail about the bazookas is that bazookas have a damage radius of 5 units in the world, but it's collision radius is only 1 unit. When a collision occurs, damage is then done to bananamen within a 5 unit radius.

## Game Features
### User Interaction
The user can move using the WASD keys, and can control the camera using the mouse. Camera control is locked to horizontal movement only to make gameplay easier, the player can shoot normal bullets by holding down the left mouse button, and shoot their bazooka using the right mouse button. The `x` key is used to exit the game.

### Scoring & Losing
Scoring in the game is measured by counting the number of bananamen dodged or killed. A dodged bananaman is a bananaman that reaches the end of the road without colliding with a player so it respawns. Since the aim of the game is to survive for as long as possible, the player loses when he takes enough damage from bananamen that he runs out of health.

### Phong lighting
Phong illumination was implemented in the vertex shader with one light source placed in (0.0, 10.0, 0.0). Ambient, specular and diffuse lighting were implemented.

### Textures
There are three texture mapped objects in my game; the city, the plane and the bananamen. The City and bananamen were textured myself, the plane had a texture available to download.

Texture loading was done using the SOIL library, and all textures were loaded into memory in the `init` function of `main.cpp`, and the textures bound to the texture buffers when they were required.

### Narrative
The player stumbles across the abandoned city of Monotropolis and ventures into the centre of the city. Suddenly, the player hears the war cry of the cyborg banana army - "BANANA". An ambush is sprung as hordes of cyborg bananamen enter the city. The player must survive for as long as they can.

### Collision Detection
Collision detection is implemented in the game between the user and bananaman objects, and also between bananamen and bullets. Collision is detected by using the object's size (scale) and its position vector, where it's position is checked along the X and Z axes. The Y axis is not checked since there is no movement along the Y axis. There is no collision detection between any objects and the City, since the city was developed entirely as one mesh, including the roads on which the player traverses.

### Player Stats
The suggested text library is used to display player details including health, score, and final score when the game is over.

### Sound Effects
Using the irrklang library, the game has background music and appropriate sound effects that play during gameplay. These sound effects are:

* Game Over
* Fire Bullet
* Bazooka Explosion
* Player Taking Damage
* Cyborg banana voice when bananaman spawns

