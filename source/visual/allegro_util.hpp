#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>


namespace visual {
    void initialize() {
        al_init();
        al_install_keyboard();
        al_init_primitives_addon();
    }
}
