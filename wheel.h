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
    explicit wheel_t(const rotor_t&);
    bool knocks() const;
    void advance();
    int enter(int ch, int prev_ofs) const;
    int exit(int ch, int prev_ofs) const;
    const rotor_t rotor;
    int ofs;
    int notches[2] = {0,0};
};
#endif /* WHEEL_H */

