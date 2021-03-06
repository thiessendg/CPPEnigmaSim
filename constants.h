/*
 * File:   constants.h
 * Author: dthiessen
 *
 * Created on June 30, 2016, 8:35 PM
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

/* Alphabet A-Z corresponds to positions 0-25 */

/* I 	EKMFLGDQVZNTOWYHXUSPAIBRCJ  Notch = Q 	If rotor steps from Q to R, the next rotor is advanced */
const rotor_t I = {"I",
    {4, 10, 12, 5, 11, 6, 3, 16, 21, 25, 13, 19, 14, 22, 24, 7, 23, 20, 18, 15, 0, 8, 1, 17, 2, 9},
    {20, 22, 24, 6, 0, 3, 5, 15, 21, 25, 1, 4, 2, 10, 12, 19, 7, 23, 18, 11, 17, 8, 13, 16, 14, 9},
    {16}};

/* II 	AJDKSIRUXBLHWTMCQGZNPYFVOE  Notch = E 	If rotor steps from E to F, the next rotor is advanced */
const rotor_t II = {"II",
    {0, 9, 3, 10, 18, 8, 17, 20, 23, 1, 11, 7, 22, 19, 12, 2, 16, 6, 25, 13, 15, 24, 5, 21, 14, 4},
    {0, 9, 15, 2, 25, 22, 17, 11, 5, 1, 3, 10, 14, 19, 24, 20, 16, 6, 4, 13, 7, 23, 12, 8, 21, 18},
    {4}};

/* III 	BDFHJLCPRTXVZNYEIWGAKMUSQO  Notch = V 	If rotor steps from V to W, the next rotor is advanced */
const rotor_t III = {"III",
    {1, 3, 5, 7, 9, 11, 2, 15, 17, 19, 23, 21, 25, 13, 24, 4, 8, 22, 6, 0, 10, 12, 20, 18, 16, 14},
    {19, 0, 6, 1, 15, 2, 18, 3, 16, 4, 20, 5, 21, 13, 25, 7, 24, 8, 23, 9, 22, 11, 17, 10, 14, 12},
    {21}};

/* IV 	ESOVPZJAYQUIRHXLNFTGKDCMWB  Notch = J 	If rotor steps from J to K, the next rotor is advanced */
const rotor_t IV = {"IV",
    {4, 18, 14, 21, 15, 25, 9, 0, 24, 16, 20, 8, 17, 7, 23, 11, 13, 5, 19, 6, 10, 3, 2, 12, 22, 1},
    {7, 25, 22, 21, 0, 17, 19, 13, 11, 6, 20, 15, 23, 16, 2, 4, 9, 12, 1, 18, 10, 3, 24, 14, 8, 5},
    {9}};

/* V 	VZBRGITYUPSDNHLXAWMJQOFECK  Notch = Z 	If rotor steps from Z to A, the next rotor is advanced */
const rotor_t V = {"V",
    {21, 25, 1, 17, 6, 8, 19, 24, 20, 15, 18, 3, 13, 7, 11, 23, 0, 22, 12, 9, 16, 14, 5, 4, 2, 10},
    {16, 2, 24, 11, 23, 22, 4, 13, 5, 19, 25, 14, 18, 12, 21, 9, 20, 3, 10, 6, 8, 0, 17, 15, 7, 1},
    {25}};

/* VI 	JPGVOUMFYQBENHZRDKASXLICTW  Notch = Z+M If rotor steps from Z to A, or from M to N the next rotor is advanced */
const rotor_t VI = {"VI",
    {9, 15, 6, 21, 14, 20, 12, 5, 24, 16, 1, 4, 13, 7, 25, 17, 3, 10, 0, 18, 23, 11, 8, 2, 19, 22},
    {18, 10, 23, 16, 11, 7, 2, 13, 22, 0, 17, 21, 6, 12, 4, 1, 9, 15, 19, 24, 5, 3, 25, 20, 8, 14},
    {25, 12}};

/* VII 	NZJHGRCXMYSWBOUFAIVLPEKQDT Notch = Z+M If rotor steps from Z to A, or from M to N the next rotor is advanced */
const rotor_t VII = {"VII",
    {13, 25, 9, 7, 6, 17, 2, 23, 12, 24, 18, 22, 1, 14, 20, 5, 0, 8, 21, 11, 15, 4, 10, 16, 3, 19},
    {16, 12, 6, 24, 21, 15, 4, 3, 17, 2, 22, 19, 8, 0, 13, 20, 23, 5, 10, 25, 14, 18, 11, 7, 9, 1},
    {25, 12}};

/* VIII FKQHTLXOCBJSPDZRAMEWNIUYGV Notch = Z+M If rotor steps from Z to A, or from M to N the next rotor is advanced */
const rotor_t VIII = {"VIII",
    {5, 10, 16, 7, 19, 11, 23, 14, 2, 1, 9, 18, 15, 3, 25, 17, 0, 12, 4, 22, 13, 8, 20, 24, 6, 21},
    {16, 9, 8, 13, 18, 0, 24, 3, 21, 10, 1, 5, 17, 20, 7, 12, 2, 15, 11, 4, 22, 25, 19, 6, 23, 14},
    {25, 12}};

/* Reflector A 	EJMZALYXVBWFCRQUONTSPIKHGD */
const reflector_t A = {"A",
    {4, 9, 12, 25, 0, 11, 24, 23, 21, 1, 22, 5, 2, 17, 16, 20, 14, 13, 19, 18, 15, 8, 10, 7, 6, 3}};

/* Reflector B 	YRUHQSLDPXNGOKMIEBFZCWVJAT */
const reflector_t B = {"B",
    {24, 17, 20, 7, 16, 18, 11, 3, 15, 23, 13, 6, 14, 10, 12, 8, 4, 1, 5, 25, 2, 22, 21, 9, 0, 19}};

/* Reflector C 	FVPJIAOYEDRZXWGCTKUQSBNMHL */
const reflector_t C = {"C",
    {5, 21, 15, 9, 8, 0, 14, 24, 4, 3, 17, 25, 23, 22, 6, 2, 19, 10, 20, 16, 18, 1, 13, 12, 7, 11}};

/* Reflector B Thin ENKQAUYWJICOPBLMDXZVFTHRGS 	1940 	M4 R1 (M3 + Thin) */
const reflector_t B_Thin = {"B_Thin",
    {4, 13, 10, 16, 0, 20, 24, 22, 9, 8, 2, 14, 15, 1, 11, 12, 3, 23, 25, 21, 5, 19, 7, 17, 6, 18}};

/* Reflector C Thin RDOBJNTKVEHMLFCWZAXGYIPSUQ 	1940 	M4 R1 (M3 + Thin) */
const reflector_t C_Thin = {"C_Thin",
    {17, 3, 14, 1, 9, 13, 19, 10, 21, 4, 7, 12, 11, 5, 2, 22, 25, 0, 23, 6, 24, 8, 15, 18, 20, 16}};

/* Beta LEYJVCNIXWPBQMDRTAKZGFUHOS 	Spring 1941 	M4 R2 */
const reflector_t Beta = {"Beta",
    {11, 4, 24, 9, 21, 2, 13, 8, 23, 22, 15, 1, 16, 12, 3, 17, 19, 0, 10, 25, 6, 5, 20, 7, 14, 18}};

/* Gamma FSOKANUERHMBTIYCWLQPZXVGJD 	Spring 1942 	M4 R2 */
const reflector_t Gamma = {"Gamma",
    {5, 18, 14, 10, 0, 13, 20, 4, 17, 7, 12, 1, 19, 8, 24, 2, 22, 11, 16, 15, 25, 23, 21, 6, 9, 3}};

#endif /* CONSTANTS_H */

