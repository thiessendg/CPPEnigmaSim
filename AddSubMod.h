/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AddSubMod.h
 * Author: dthiessen
 *
 * Created on June 30, 2016, 8:03 AM
 */
#ifndef ADDSUBMOD_H
#define ADDSUBMOD_H

inline int AddMod(int a, int b) {
    return (a + b) > 25 ? (a + b) - 26 : a + b;
}

inline int SubMod(int a, int b) {
    return (a - b) < 0 ? (a - b) + 26 : a - b;
}

#endif /* ADDSUBMOD_H */

