/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   wheel.h
 * Author: dthiessen
 *
 * Created on June 30, 2016, 9:52 AM
 */

#ifndef WHEEL_H
#define WHEEL_H

#include "rotor.h"
#include "AddSubMod.h"

struct wheel_t {

    explicit wheel_t(const rotor_t& r) : rotor(r), ofs(0) {
        notches[0] = 0;
        notches[1] = 0;
    };

    bool knocks() const {
        return ofs == notches[0] || ofs == notches[1];
    };

    void advance() {
        ofs = AddMod(ofs, 1);
    };

    int enter(int ch, int prev_ofs) const {
        return rotor.enter[SubMod(AddMod(ch, ofs), prev_ofs)];
    };

    int exit(int ch, int prev_ofs) const {
        return rotor.exit[SubMod(AddMod(ch, ofs), prev_ofs)];
    };
    const rotor_t rotor;
    int ofs;
    int notches[2];
};
#endif /* WHEEL_H */

