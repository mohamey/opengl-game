# Yasir Mohamed - 13318246
## Introduction
The player stumbles across the abandoned city of Monotropolis and ventures into the centre of the city. Suddenly, the player hears the war cry of the cyborg banana army - "BANANA". An ambush is sprung as hordes of cyborg bananamen enter the city. The player must survive for as long as he can.

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
