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

const reflector_t* M3Reflector(const char& reflector) {
    const reflector_t* temp;
    switch (reflector) {
        case 'A':
        case 'a':
            temp = &A;
            break;
        case 'B':
        case 'b':
            temp = &B;
            break;
        case 'C':
        case 'c':
            temp = &C;
            break;
        default:
            printf("Reflector value invalid; defaulting to B reflector.\n");
            temp = &B;
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

int main(int argc, char** args) {
    int myMachineType;
    printf("Enigma I/M3 or M4? Enter 3 or 4:\n");
    scanf("%d", &myMachineType);
    getchar(); //pull one newline off the input buffer
    if (myMachineType < 3 || myMachineType > 4) {
        printf("ERROR - machine type invalid; must be 3 for I/M3 or 4 for M4!\n");
        return -1;
    }

    char pb[40]; //max 13 pairs of letters + space 
    //prompt user for plug board settings
    printf("Enter plugboard wiring (if any) as space separated pairs. Ex: AN BY CX: \n");
    fgets(pb, sizeof (pb), stdin);
    plugboard_t myPlugboard(pb);

    const reflector_t* myReflector = nullptr;
    if (myMachineType == 3) {
        char reflektor;
        printf("Enter Reflector (A, B, C):\n");
        scanf("%c", &reflektor);
        getchar(); //pull one newline off the input buffer
        myReflector = M3Reflector(reflektor);
    }//endif machine I/M3

    if (myMachineType == 4) {
        char reflektor, greek, greekStart;
        printf("Enter thin reflector (B or C):\n");
        scanf("%c", &reflektor);
        getchar(); //pull one newline off the input buffer
        const reflector_t* thinReflector;
        switch (reflektor) {
            case 'B':
            case 'b':
                thinReflector = &B_Thin;
                break;
            case 'C':
            case 'c':
                thinReflector = &C_Thin;
                break;
            default:
                printf("Thin reflector value invalid; defaulting to thin B.\n");
                thinReflector = &B_Thin;
                break;
        }
        
        //prompt user for greek rotor beta or gamma
        printf("Enter Greek Rotor (B or G (Beta,Gamma):\n");
        scanf("%c", &greek);
        getchar(); //pull one newline off the input buffer
        const reflector_t* greekWheel;
        switch (greek) {
            case 'B':
            case 'b':
                greekWheel = &Beta;
                break;
            case 'G':
            case 'g':
                greekWheel = &Gamma;
                break;
            default:
                printf("Greek rotor value invalid; defaulting to Beta.\n");
                greekWheel = &Beta;
                break;
        }
        printf("Enter starting char of greek wheel (A-Z):\n");
        scanf("%c", &greekStart);
        getchar(); //pull one newline off the input buffer

        int greekRing;
        printf("Enter greek wheel ring position (Ex. 1):\n");
        scanf("%d", &greekRing);
        getchar(); //pull one newline off the input buffer

        reflector_t temp = make_M4_reflector(*thinReflector, *greekWheel, greekStart, greekRing);
        myReflector = &temp;
    }//endif machine M4

    //prompt user for rotors, positions, rings
    char leftRoman[5], middleRoman[5], rightRoman[5];
    printf("Enter left, middle, and right rotor roman numbers (I, II, III, etc): \n");
    scanf("%s %s %s", leftRoman, middleRoman, rightRoman);
    getchar(); //pull one newline off the input buffer
    const rotor_t* myLeftRotor = assignRotor(leftRoman);
    const rotor_t* myMiddleRotor = assignRotor(middleRoman);
    const rotor_t* myRightRotor = assignRotor(rightRoman);

    char leftStart, middleStart, rightStart;
    printf("Enter space separated starting char, left to right (Ex. A A A):\n");
    scanf("%c %c %c", &leftStart, &middleStart, &rightStart);
    getchar(); //pull one newline off the input buffer

    int leftRing, middleRing, rightRing;
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
