#include <string>
//#include <vector>
#include <cassert>
#include <map>
//#include <cctype>
#include "reflector.h"
#include "rotor.h"
#include "Enigma.h"
#include "plugboard.h"
#include "constants.h"

const reflector_t* make_M3_Reflector(const char& reflector) {
    const reflector_t* temp;
    switch (reflector) {
        case 'A':
            temp = &A;
            break;
        case 'B':
            temp = &B;
            break;
        case 'C':
            temp = &C;
            break;
        default:
            printf("Reflector value invalid; defaulting to B reflector.\n");
            temp = &B;
            break;
    }
    return temp;
}

void make_M4_Reflector(reflector_t* reflector, const reflector_t& thin, const reflector_t& greek, char ofs, int ring) {
    *reflector = {thin.name + ":" + greek.name + ":" + ofs};
    ofs -= 'A';
    ofs = static_cast<char> (SubMod(ofs, ring - 1));

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
        // enter greek rotor
        int ch = greek.map[AddMod(i, ofs)];
        // through the thin reflector
        ch = AddMod(inverse_thin[SubMod(ch, ofs)], ofs);
        // and back out the greek rotor
        reflector->map[i] = static_cast<char> (SubMod(inverse_greek[ch], ofs));
    }
    //return &reflector;
}

const rotor_t* assignRotor(const std::string& rotor) {
    //map the roman numeral string to an int
    std::map<char, int> roman;
    //roman['M'] = 1000;
    //roman['D'] = 500;
    //roman['C'] = 100;
    //roman['L'] = 50;
    roman['X'] = 10; //all my numbers will be below 10
    roman['V'] = 5;
    roman['I'] = 1;
    int result = 0;
    for (unsigned int i = 0; i < rotor.size() - 1; ++i) {
        if (roman[rotor[i]] < roman[rotor[i+1]])
            result -= roman[rotor[i]];
        else
            result += roman[rotor[i]];
    }
    result += roman[rotor[rotor.size()-1]];

    const rotor_t* temp;

    switch (result) {
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
    int myMachineType = 0;
    printf("Enigma I/M3 or M4? Enter 3 or 4:\n");
    scanf("%d", &myMachineType);
    getchar(); //pull one newline off the input buffer
    //check
    if (myMachineType < 3 || myMachineType > 4) {
        printf("ERROR: machine type invalid; must be 3: I/M3 or 4: M4!\n");
        return -1;
    }

    char pb[40]; //max 13 pairs of letters + space
    //prompt user for plug board settings
    printf("Enter plugboard wiring (if any) as space separated pairs.\n");
    printf("(Ex. AN BY CX )\n");
    fgets(pb, sizeof pb, stdin);
    plugboard_t myPlugboard(pb);

    const reflector_t* myReflector = nullptr;
	char reflektor;
    if (myMachineType == 3) {
        printf("Enter Reflector (A, B, or C):\n");
        scanf("%c", &reflektor);
        getchar(); //pull one newline off the input buffer
        reflektor = static_cast<char> (toupper(reflektor));
        if (reflektor != 'A' && reflektor != 'B' && reflektor != 'C') {
            printf("Error - reflector must be A, B, or C.\n");
            return -1;
        }
        myReflector = make_M3_Reflector(reflektor);
    }//endif machine I/M3

    if (myMachineType == 4) {
        printf("Enter thin reflector (B or C):\n");
        scanf("%c", &reflektor);
        getchar(); //pull one newline off the input buffer
        reflektor = static_cast<char> (toupper(reflektor));
        if (reflektor != 'B' && reflektor != 'C') {
            printf("Error - thin reflector must be B or C.\n");
            return -1;
        }
        const reflector_t* thinReflector;
        switch (reflektor) {
            case 'B':
                thinReflector = &B_Thin;
                break;
            case 'C':
                thinReflector = &C_Thin;
                break;
            default:
                printf("Thin reflector value invalid; defaulting to thin B.\n");
                thinReflector = &B_Thin;
                break;
        }
		char greek, greekStart;
        //prompt user for greek rotor beta or gamma
        printf("Enter Greek Rotor (B or G (Beta,Gamma):\n");
        scanf("%c", &greek);
        getchar(); //pull one newline off the input buffer
        greek = static_cast<char> (toupper(greek));
        //check
        if (greek != 'B' && greek != 'G') {
            printf("Error - Greek Rotor must be B/G for Beta/Gamma.\n");
            return -1;
        }
        const reflector_t* greekWheel;
        switch (greek) {
            case 'B':
                greekWheel = &Beta;
                break;
            case 'G':
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
        greekStart = static_cast<char> (toupper(greekStart));
        //check
        if (greekStart < 'A' || greekStart > 'Z') {
            printf("Error - Start of Greek wheel must be A-Z.\n");
            return -1;
        }

        int greekRing;
        printf("Enter greek wheel ring position (1-26):\n");
        scanf("%d", &greekRing);
        getchar(); //pull one newline off the input buffer
        //check
        if (greekRing < 1 || greekRing > 26) {
            printf("Error - Greek ring must be 1-26.\n");
            return -1;
        }

        //make sure ptrs arent nullptr
        if (thinReflector == nullptr || greekWheel == nullptr) {
            printf("Error: thinReflector and/or greekWheel are nullptr.\n");
            return -1;
        }

		make_M4_Reflector(const_cast<reflector_t*>(myReflector), *thinReflector, *greekWheel, greekStart, greekRing);
    }//endif machine M4

    //prompt user for rotors, positions, rings
    char leftRoman[5], middleRoman[5], rightRoman[5];
    printf("Enter left, middle, right rotor numbers (I-VIII) (Ex. I II IV: \n");
    scanf("%4s %4s %4s", leftRoman, middleRoman, rightRoman);
    getchar(); //pull one newline off the input buffer
    const rotor_t* myLeftRotor = assignRotor(leftRoman);
    const rotor_t* myMiddleRotor = assignRotor(middleRoman);
    const rotor_t* myRightRotor = assignRotor(rightRoman);

    char leftStart, middleStart, rightStart;
    printf("Enter starting chars (A-Z) left to right (Ex. A A A):\n");
    scanf("%c %c %c", &leftStart, &middleStart, &rightStart);
    getchar(); //pull one newline off the input buffer
    leftStart = static_cast<char> (toupper(leftStart));
    middleStart = static_cast<char> (toupper(middleStart));
    rightStart = static_cast<char> (toupper(rightStart));
    if (leftStart < 'A' || leftStart > 'Z') {
        printf("Error - Start of left rotor must be A-Z.\n");
        return -1;
    }
    if (middleStart < 'A' || middleStart > 'Z') {
        printf("Error - Start of middle must be A-Z.\n");
        return -1;
    }
    if (rightStart < 'A' || rightStart > 'Z') {
        printf("Error - Start of right rotor must be A-Z.\n");
        return -1;
    }

    int leftRing, middleRing, rightRing;
    printf("Enter ring positions (1-26) left to right (Ex. 1 1 1):\n");
    scanf("%d %d %d", &leftRing, &middleRing, &rightRing);
    getchar(); //pull one newline off the input buffer
    //check
    if (leftRing < 1 || leftRing > 26) {
        printf("Error - left ring must be 1-26.\n");
        return -1;
    }
    if (middleRing < 1 || middleRing > 26) {
        printf("Error - middle ring must be 1-26.\n");
        return -1;
    }
    if (rightRing < 1 || rightRing > 26) {
        printf("Error - right ring must be 1-26.\n");
        return -1;
    }

    if (myReflector == nullptr || myLeftRotor == nullptr ||
            myMiddleRotor == nullptr || myRightRotor == nullptr) {
        printf("Error: Reflector and/or Rotor(s) are nullptr!\n");
        return -1;
    }

    Enigma myEnigma(*myReflector, *myLeftRotor, *myMiddleRotor, *myRightRotor);
    myEnigma.init(leftStart - 'A', leftRing - 1,
                  middleStart - 'A', middleRing - 1,
                  rightStart - 'A', rightRing - 1);

    printf("Beginning display:\n");
    //if (myMachineType == 4) {
    //    printf("%c ", myEnigma.getReflector().name.back());
    //}
    printf("%c %c %c\n", AddMod(myEnigma.getLeftOfs(), leftRing - 1) + 'A',
            AddMod(myEnigma.getMiddleOfs(), middleRing - 1) + 'A',
            AddMod(myEnigma.getRightOfs(), rightRing - 1) + 'A');

    char myMessage[256]; //read somewhere messages were <= 250
    printf("Enter your message (256 char limit):\n");
    fgets(myMessage, 256, stdin);

    //remove the null terminator and make upper case
    for (unsigned int i = 0; i < sizeof (myMessage); ++i) {
        if (myMessage[i] == '\n') {
            myMessage[i] = '\0';
        }
        myMessage[i] = static_cast<char> (toupper(myMessage[i]));
    }

    std::string output;
    for (int i = 0; myMessage[i]; i++) {
        myEnigma.advance();
        int ch = myPlugboard.map[myMessage[i] - 'A'];
        assert(ch >= 0 && ch < 26);
        ch = myEnigma.code(ch);
        assert(ch >= 0 && ch < 26);
        //const int check = myPlugboard.map[myEnigma.code(ch)] + 'A';
        ch = myPlugboard.map[ch];
        output += ch + 'A';
    } //end for msg

    printf("message: \n\t%s\n", myMessage);
    printf("encoded:  \n\t%s\n", output.c_str());

    printf("Ending display:\n");
    //if (myMachineType == 4) {
    //    printf("%c ", myEnigma.getReflector().name.back());
    //}
    printf("%c %c %c\n", AddMod(myEnigma.getLeftOfs(), leftRing - 1) + 'A',
            AddMod(myEnigma.getMiddleOfs(), middleRing - 1) + 'A',
            AddMod(myEnigma.getRightOfs(), rightRing - 1) + 'A');

    return 0;
}
