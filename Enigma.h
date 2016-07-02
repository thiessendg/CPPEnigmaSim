/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Enigma.h
 * Author: dthiessen
 *
 * Created on June 30, 2016, 7:34 AM
 */
#ifndef ENIGMA_H
#define ENIGMA_H

#include "reflector.h"
#include "rotor.h"
#include "wheel.h"
#include "AddSubMod.h"

 /* An Enigma machine has 3 moving rotors and a reflector.
    The commercial and M3 Enigma (army, air force, railways) had a fixed-position reflector.
    There were several rotors and reflectors to choose between.  The exact rotation-point of individual rotors was controlled by rings.
    The M4 Enigma (u-boats) was basically a M3 Enigma with a special two-part reflector.  This configurable reflector made for
    a dramatic increase in keyspace. */
class Enigma {
public:
    Enigma(const reflector_t&, const rotor_t&, const rotor_t&, const rotor_t&);
    void init(int, int, int, int, int, int);
    void advance();
    int enter(int) const;
    int exit(int) const;
    int code(int) const;
    //Enigma(const Enigma& orig);
    int getLeftOfs() const;
    int getMiddleOfs() const;
    int getRightOfs() const;
    virtual ~Enigma();
private:
    reflector_t reflector;
    wheel_t left, middle, right;
};

#endif /* ENIGMA_H */
