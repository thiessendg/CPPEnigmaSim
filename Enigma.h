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
 * The commercial and M3 Enigma (army, air force, rail) had a fixed-position
 * reflector.  There were several rotors and reflectors to choose between.  The
 * exact rotation-point of individual rotors was controlled by rings.
 * The M4 Enigma (u-boats) was basically a M3 Enigma with a special two-part
 * reflector.  This configurable reflector made for a dramatic increase in
 * keyspace.
 */
struct Enigma {

    explicit Enigma(const reflector_t& ref, const rotor_t& l, const rotor_t& m,
            const rotor_t& r) : reflector(ref), left(l), middle(m), right(r) {
    };

    void init(int ofs_l, int rng_l, int ofs_m, int rng_m, int ofs_r, int rng_r) {
        left.ofs = SubMod(ofs_l, rng_l);
        middle.ofs = SubMod(ofs_m, rng_m);
        right.ofs = SubMod(ofs_r, rng_r);

        left.notches[0] = SubMod(left.rotor.notches[0], rng_l);
        left.notches[1] = left.rotor.notches[1] ?
                SubMod(left.rotor.notches[1], rng_l) : -1;

        middle.notches[0] = SubMod(middle.rotor.notches[0], rng_m);
        middle.notches[1] = middle.rotor.notches[1] ?
                SubMod(middle.rotor.notches[1], rng_m) : -1;

        right.notches[0] = SubMod(right.rotor.notches[0], rng_r);
        right.notches[1] = right.rotor.notches[1] ?
                SubMod(right.rotor.notches[1], rng_r) : -1;
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

    reflector_t reflector;
    wheel_t left, middle, right;
};

#endif /* ENIGMA_H */
