#include <string>
#include <vector>
#include <cstdio>
#include <cassert>
#include <map>
#include <time.h>
#include "reflector.h"
#include "rotor.h"
#include "Enigma.h"
#include "plugboard.h"
#include "AddSubMod.h"
#include "constants.h"

int romanToInt(const std::string& s) {
    std::map<char, int> roman;
    //roman['M'] = 1000;
    //roman['D'] = 500;
    //roman['C'] = 100;
    //roman['L'] = 50;
    roman['X'] = 10; //all my numbers will be below 10
    roman['V'] = 5;
    roman['I'] = 1;

    int result = 0;
    for (int i = 0; i < s.size() - 1; ++i) { 
        if (roman[s[i]] < roman[s[i + 1]])
            result -= roman[s[i]];
        else
            result += roman[s[i]];
    }
    result += roman[s[s.size() - 1]];
    return result;
}

const rotor_t* assignRotor(const std::string& rotor) {
    const rotor_t* temp;
    switch (romanToInt(rotor)) {
        case 1:
            temp = &I;
            break;
        case 2:
            temp = &II;
            break;
        case 3:
            temp = &III;
            break;
        case 4:
            temp = &IV;
            break;
        case 5:
            temp = &V;
            break;
        case 6:
            temp = &VI;
            break;
        case 7:
            temp = &VII;
            break;
        case 8:
            temp = &VIII;
            break;
        default:
            printf("Error - Wheel notation invalid. Defaulting to I.\n");
            temp = &I;
            break;
    }
    return temp;
}

reflector_t make_M4_reflector(const reflector_t& thin, const reflector_t& greek,
        char ofs, int ring) {
    reflector_t reflector = {thin.name + ":" + greek.name + ":" + ofs};
    ofs -= 'A';
    ofs = SubMod(ofs, ring - 1);

    // make the inverse of the mappings
    int inverse_thin[26], inverse_greek[26];
    for (int i = 0; i < 26; i++) {
        for (int j = 0; j < 26; j++) {
            if (thin.map[j] == i)
                inverse_thin[i] = j;
            if (greek.map[j] == i)
                inverse_greek[i] = j;
        }
    }
    // work out effective mapping
    for (int i = 0; i < 26; i++) {
        int ch = greek.map[AddMod(i, ofs)]; // enter greek rotor
        ch = AddMod(inverse_thin[SubMod(ch, ofs)], ofs); // through the thin reflector
        reflector.map[i] = SubMod(inverse_greek[ch], ofs); // and back out the greek rotor
    }
    return reflector;
}

int main(int argc, char** args) {
    int myMachine, leftRing, middleRing, rightRing, greekRing;
    char pb[40];
    char leftRoman[5], middleRoman[5], rightRoman[5];
    char reflektor, greek, leftStart, middleStart, rightStart, greekStart;

    printf("Enigma I/M3 or M4? Enter 3 or 4:\n");
    scanf("%d", &myMachine);
    getchar(); //pull one newline off the input buffer
    if (myMachine < 3 || myMachine > 4) {
        printf("ERROR - machine type invalid; must be 3 for I/M3 or 4 for M4!\n");
        return -1;
    }
    //prompt user for plug board settings
    printf("Enter plugboard wiring (if any) as space separated pairs. Ex: AN BY CX: \n");
    fgets(pb, sizeof (pb), stdin);
    plugboard_t myPlugboard(pb);

    const reflector_t* myReflector;
    const reflector_t* myThinReflector;
    const reflector_t* myGreekRotor;
    const rotor_t* myLeftRotor;
    const rotor_t* myMiddleRotor;
    const rotor_t* myRightRotor;

    if (myMachine == 3) {
        //prompt user for reflektor
        printf("Enter Reflector (A, B, C):\n");
        scanf("%c", &reflektor);
        getchar(); //pull one newline off the input buffer

        switch (reflektor) {
            case 'A':
                myReflector = &A;
                break;
            case 'B':
                myReflector = &B;
                break;
            case 'C':
                myReflector = &C;
                break;
            default:
                printf("Reflector value invalid; defaulting to B reflector.\n");
                myReflector = &B;
                break;
        }
    }//endif machine I/M3

    if (myMachine == 4) {
        //prompt user for reflektor
        printf("Enter thin reflector (B or C):\n");
        scanf("%c", &reflektor);
        getchar(); //pull one newline off the input buffer

        switch (reflektor) {
            case 'B':
            case 'b':
                myThinReflector = &B_Thin;
                break;
            case 'C':
            case 'c':
                myThinReflector = &C_Thin;
                break;
            default:
                printf("Thin reflector value invalid; defaulting to thin B.\n");
                myThinReflector = &B_Thin;
                break;
        }

        //prompt user for greek rotor beta or gamma
        printf("Enter Greek Rotor (B or G (Beta,Gamma):\n");
        scanf("%c", &greek);
        getchar(); //pull one newline off the input buffer
        switch (greek) {
            case 'B':
            case 'b':
                myGreekRotor = &Beta;
                break;
            case 'G':
            case 'g':
                myGreekRotor = &Gamma;
                break;
            default:
                printf("Greek rotor value invalid; defaulting to Beta.\n");
                myGreekRotor = &Beta;
                break;
        }
        printf("Enter starting char of greek wheel (A-Z):\n");
        scanf("%c", &greekStart);
        getchar(); //pull one newline off the input buffer

        printf("Enter greek wheel ring position (Ex. 1):\n");
        scanf("%d", &greekRing);
        getchar(); //pull one newline off the input buffer
        reflector_t temp = make_M4_reflector(*myThinReflector, *myGreekRotor, greekStart, greekRing);
        myReflector = &temp;
    }//endif machine M4

    //prompt user for rotors, positions, rings
    printf("Enter left, middle, and right rotor roman numbers (I, II, III, etc): \n");
    //printf("Example: 1 3 5 means I III V\n");
    scanf("%s %s %s", leftRoman, middleRoman, rightRoman);
    myLeftRotor = assignRotor(leftRoman);
    myMiddleRotor = assignRotor(middleRoman);
    myRightRotor = assignRotor(rightRoman);
    getchar(); //pull one newline off the input buffer

    printf("Enter space separated starting char, left to right (Ex. A A A):\n");
    scanf("%c %c %c", &leftStart, &middleStart, &rightStart);
    getchar(); //pull one newline off the input buffer

    printf("Enter space separated ring position, left to right (Ex. 1 1 1):\n");
    scanf("%d %d %d", &leftRing, &middleRing, &rightRing);
    getchar(); //pull one newline off the input buffer

    Enigma myEnigma(*myReflector, *myLeftRotor, *myMiddleRotor, *myRightRotor);
    myEnigma.init(leftStart - 'A', leftRing - 1, middleStart - 'A', middleRing - 1, rightStart - 'A', rightRing - 1);

    printf("Beginning display:\n");
    printf("%c %c %c\n", AddMod(myEnigma.getLeftOfs(), leftRing - 1) + 'A',
            AddMod(myEnigma.getMiddleOfs(), middleRing - 1) + 'A',
            AddMod(myEnigma.getRightOfs(), rightRing - 1) + 'A');

    char myMessage[256]; //read somewhere messages were <= 250
    printf("Enter your message (256 char limit):\n");
    fgets(myMessage, 256, stdin);

    //remove the null terminator
    for (int i = 0; i < sizeof (myMessage); ++i) {
        if (myMessage[i] == '\n') {
            myMessage[i] = '\0';
        }
    }

    std::string output;
    for (int i = 0; myMessage[i]; i++) {
        myEnigma.advance();
        int ch = myPlugboard.map[myMessage[i] - 'A'];
        assert(ch >= 0 && ch < 26);
        ch = myEnigma.code(ch);
        assert(ch >= 0 && ch < 26);
        const int check = myPlugboard.map[myEnigma.code(ch)] + 'A';
        ch = myPlugboard.map[ch];
        output += ch + 'A';
    } //end for msg

    printf("message: \n\t%s\n", myMessage);

    printf("encoded:  \n\t%s\n", output.c_str());

    printf("Ending display:\n");
    printf("%c %c %c\n", AddMod(myEnigma.getLeftOfs(), leftRing - 1) + 'A',
            AddMod(myEnigma.getMiddleOfs(), middleRing - 1) + 'A',
            AddMod(myEnigma.getRightOfs(), rightRing - 1) + 'A');

    return 0;
}
