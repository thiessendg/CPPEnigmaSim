/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Enigma.cpp
 * Author: dthiessen
 * 
 * Created on June 30, 2016, 7:34 AM
 */

#include "Enigma.h"

Enigma::Enigma(const reflector_t& ref, const rotor_t& l, const rotor_t& m, const rotor_t& r)
: reflector(ref), left(l), middle(m), right(r) {
}

void Enigma::init(int ofs_l, int ring_l, int ofs_m, int ring_m, int ofs_r, int ring_r) {
    left.ofs = SubMod(ofs_l, ring_l);
    middle.ofs = SubMod(ofs_m, ring_m);
    right.ofs = SubMod(ofs_r, ring_r);

    left.notches[0] = SubMod(left.rotor.notches[0], ring_l);
    left.notches[1] = left.rotor.notches[1] ? SubMod(left.rotor.notches[1], ring_l) : -1;

    middle.notches[0] = SubMod(middle.rotor.notches[0], ring_m);
    middle.notches[1] = middle.rotor.notches[1] ? SubMod(middle.rotor.notches[1], ring_m) : -1;

    right.notches[0] = SubMod(right.rotor.notches[0], ring_r);
    right.notches[1] = right.rotor.notches[1] ? SubMod(right.rotor.notches[1], ring_r) : -1;
}

void Enigma::advance() {
    if (middle.knocks()) {
        left.advance();
        middle.advance();
    } else if (right.knocks()) {
        middle.advance();
    }
    right.advance();
}

int Enigma::enter(int ch) const {
    ch = right.enter(ch, 0);
    ch = middle.enter(ch, right.ofs);
    ch = left.enter(ch, middle.ofs);
    return ch;
}

int Enigma::exit(int ch) const {
    ch = reflector.map[SubMod(ch, left.ofs)];
    ch = left.exit(ch, 0);
    ch = middle.exit(ch, left.ofs);
    ch = right.exit(ch, middle.ofs);
    ch = SubMod(ch, right.ofs);
    return ch;
}

int Enigma::code(int ch) const {
    return exit(enter(ch));
}

int Enigma::getLeftOfs() const {
    return left.ofs;
}

int Enigma::getMiddleOfs() const {
    return middle.ofs;
}

int Enigma::getRightOfs() const {
    return right.ofs;
}
reflector_t Enigma::getReflector() const {
    return reflector;
}
/*
Enigma::Enigma(const Enigma& orig) {
    //copy ctor
}
 */

Enigma::~Enigma() {
}