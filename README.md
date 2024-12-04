The project is divided into two versions: Process-Based Version and Thread-Based Version.

Process-Based Version
Architecture:
The architecture of the process-based version is composed of a single pipe for communication between producers and the consumer.

Producers: All objects on the screen (spaceship, enemy ships, missiles, bombs).
Consumer: A process responsible for managing the rendering of objects and handling collisions.
Collision Handling:
The screen is divided into sections to manage enemy ship collisions. Each section size is proportional to 1 / number_of_ships_per_column.
Two macros, defined in the file globale.h, allow dynamic adjustment of the number of enemy ships, up to a maximum of 5 ships per column and 3 columns in total.
When enemy ships reach the boundary of the screen, they reverse their direction.

Thread-Based Version
Architecture:
The thread-based version uses a shared buffer in memory to handle communication between producers and consumers.

Producers write to the buffer.
Consumers read from the buffer.
This process is controlled to prevent:

Producers from writing to a full buffer.
Consumers from reading from an empty buffer.
Access to the buffer is regulated so that only one thread at a time can access it. Similarly, the output buffer is managed using mutexes, ensuring exclusive access to shared resources.

Collision Handling:
The screen is divided into sections to manage enemy ship collisions, just as in the process-based version.
Section sizes are proportional to 1 / number_of_ships_per_column, with macros in globale.h enabling dynamic adjustments. The maximum configuration supports 5 ships per column and 3 columns in total.
When enemy ships reach the screen boundaries, they reverse their direction.

Design Choices
The spaceship can fire two missiles at a time (with a timer controlling the rate of fire).
Enemy ships change color when hit:
Green → Yellow → Red.
Level 1 enemy ships, when hit, evolve into Level 2 ships with a different sprite shape.
If the terminal window is smaller than 90x26, the user will be notified to maximize the terminal for the best experience. Otherwise, the "Space Defender" text won't be displayed.
The terminal size can be adjusted during the introduction phase without needing to restart the program.
To provide a sense of horizontal motion, the top and bottom borders scroll horizontally.
