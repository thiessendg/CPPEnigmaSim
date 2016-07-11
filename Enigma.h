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
struct Enigma {

    explicit Enigma(const reflector_t& ref, const rotor_t& l, const rotor_t& m, const rotor_t& r)
    : reflector(ref), left(l), middle(m), right(r) {
    };

    void init(int ofs_l, int ring_l, int ofs_m, int ring_m, int ofs_r, int ring_r) {

        left.ofs = SubMod(ofs_l, ring_l);
        middle.ofs = SubMod(ofs_m, ring_m);
        right.ofs = SubMod(ofs_r, ring_r);

        left.notches[0] = SubMod(left.rotor.notches[0], ring_l);
        left.notches[1] = left.rotor.notches[1] ? SubMod(left.rotor.notches[1], ring_l) : -1;

        middle.notches[0] = SubMod(middle.rotor.notches[0], ring_m);
        middle.notches[1] = middle.rotor.notches[1] ? SubMod(middle.rotor.notches[1], ring_m) : -1;

        right.notches[0] = SubMod(right.rotor.notches[0], ring_r);
        right.notches[1] = right.rotor.notches[1] ? SubMod(right.rotor.notches[1], ring_r) : -1;
    };

    void advance() {
        if (middle.knocks()) {
            left.advance();
            middle.advance();
        } else if (right.knocks()) {

            middle.advance();
        }
        right.advance();
    };

    int enter(int ch) const {
        ch = right.enter(ch, 0);
        ch = middle.enter(ch, right.ofs);
        ch = left.enter(ch, middle.ofs);

        return ch;
    };

    int exit(int ch) const {
        ch = reflector.map[SubMod(ch, left.ofs)];
        ch = left.exit(ch, 0);
        ch = middle.exit(ch, left.ofs);
        ch = right.exit(ch, middle.ofs);
        ch = SubMod(ch, right.ofs);

        return ch;
    };

    int code(int ch) const {
        return exit(enter(ch));
    };
    //Enigma(const Enigma& orig);
    //int getLeftOfs() const;
    //int getMiddleOfs() const;
    //int getRightOfs() const;
    //reflector_t getReflector() const;
    //virtual ~Enigma();
    //private:
    reflector_t reflector;
    wheel_t left, middle, right;
};

#endif /* ENIGMA_H */
