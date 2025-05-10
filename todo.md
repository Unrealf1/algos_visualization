+ Read algorithms code
+ Improve visuals
    + Change 'visited' color
    + Add visualization of maze tiles discovered by the algorithm
    + Fix streching when maze has different proportions from display
* Add more algorithms
    +   Add option to allow diagonals (with or without surrounding free paths)
    +   Random DFS
    -   ARA
    -   Another one from AI :)
    -   https://en.wikipedia.org/wiki/Poisson%27s_equation
* Add more maze generation options
    +   Add "slower" tiles - to show difference between Dijkstra and BFS
    *   Research
        * https://en.wikipedia.org/wiki/Maze_generation_algorithm
            + random dfs

    *   Load premade maze
        +   save/load functions
        +   extract save file to config parameter
        *   Interface to create maze?
            +   Save dialog
            +   Load existing maze to editor
            *   Parameters for generation algorithms
                +   In designer
                -   In config for algvis
* fix leaks on application close
    ?  Leaks are in allegro and various cleanup functions do nothing, abandon this for now
* Publish to web
  +   Combo app to design and pathfind in one place
  +   Sensible defaults(interesting maze, big but not huge field)
  -   Fix no touch input in web (https://allegro5.org/docs/html/refman/touch.html)
  +   Proper controls explanation(Q, brush, RMB, future pan&zoon)
  *   Better ui & langs
  +   Pan & zoom
  -   Support saving and loading in the browser
- Prove all the algorithms
- Algorithm descriptions
- Options to guarantee path for every algorithm?
- Compile a video(s) / posts
- Turn this into a screensaver!

