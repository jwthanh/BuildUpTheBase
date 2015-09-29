#pragma once
#ifndef ELEMENTS_H
#define ELEMENTS_H

enum elements_t {
    FireElement,
    WaterElement,
    DeathElement,
    LifeElement,
    CrystalElement,
    SpectreElement
};

TCODColor get_element_color(elements_t element);

#endif
