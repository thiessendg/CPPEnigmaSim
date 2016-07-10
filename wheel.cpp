/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "wheel.h"

wheel_t::wheel_t(const rotor_t& r) : rotor(r), ofs(0){
	notches[0] = 0;
	notches[1] = 0;
}

bool wheel_t::knocks() const {
    return ofs == notches[0] || ofs == notches[1];
}

void wheel_t::advance() {
    ofs = AddMod(ofs, 1);
}

int wheel_t::enter(int ch, int prev_ofs) const {
    return rotor.enter[SubMod(AddMod(ch, ofs), prev_ofs)];
}

int wheel_t::exit(int ch, int prev_ofs) const {
    return rotor.exit[SubMod(AddMod(ch, ofs), prev_ofs)];
}
