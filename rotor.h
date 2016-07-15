/*
 * File:   rotor.h
 * Author: dthiessen
 *
 * Created on June 30, 2016, 7:32 AM
 */

#ifndef ROTOR_H
#define ROTOR_H

#include <string>

struct rotor_t {
    const std::string name;
    char enter[26];
    char exit[26];
    int notches[2];
};

#endif /* ROTOR_H */

