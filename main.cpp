#include <string>
#include <cassert>
#include <map>
#include "reflector.h"
#include "rotor.h"
#include "Enigma.h"
#include "plugboard.h"
#include "constants.h"

void strToUpper(char* strInput) {
	while (*strInput) {
		*strInput = toupper(*strInput);
		strInput++;
	}
}

reflector_t makeM3Reflector(const char& reflector) {
	switch (reflector) {
	case 'A':
		return A;
	case 'B':
		return B;
	case 'C':
		return C;
	default:
		printf("Reflector value invalid; defaulting to B reflector.\n");
		return B;
	}
}

reflector_t makeM4Reflector(const reflector_t& thin, const reflector_t& greek,
	char offset, int ring) {
	reflector_t reflector = { thin.name + ":" + greek.name + ":" + offset };
	offset -= 'A';
	offset = static_cast<char> (SubMod(offset, ring - 1));

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
		int ch = greek.map[AddMod(i, offset)];
		// through the thin reflector
		ch = AddMod(inverse_thin[SubMod(ch, offset)], offset);
		// and back out the greek rotor
		reflector.map[i] = static_cast<char> (SubMod(inverse_greek[ch], offset));
	}
	return reflector;
}

rotor_t assignRotor(const std::string& rotor) {
	//map the roman numeral string to an int
	std::map<char, int> roman;
	//roman['M'] = 1000;
	//roman['D'] = 500;
	//roman['C'] = 100;
	//roman['L'] = 50;
	//roman['X'] = 10;
	roman['V'] = 5;
	roman['I'] = 1;
	int result = 0;
	for (unsigned int i = 0; i < rotor.size() - 1; ++i) {
		if (roman[rotor[i]] < roman[rotor[i + 1]])
			result -= roman[rotor[i]];
		else
			result += roman[rotor[i]];
	}
	result += roman[rotor[rotor.size() - 1]];

	/* I normally prefer one return from a function but this makes more sense */
	switch (result) {
	case 1:
		return I;
	case 2:
		return II;
	case 3:
		return III;
	case 4:
		return IV;
	case 5:
		return V;
	case 6:
		return VI;
	case 7:
		return VII;
	case 8:
		return VIII;
	default:
		printf("Error - Wheel notation invalid. Defaulting to I.\n");
		return I;
	}
}

int main(int argc, char* args[]) {
	int myMachineType = 0;
	printf("Enigma I/M3 or M4? Enter 3 or 4:\n");
	scanf("%d", &myMachineType);
	getchar(); //pull one newline off the input buffer
	if (myMachineType < 3 || myMachineType > 4) {
		printf("ERROR: machine type invalid; must be 3: I/M3 or 4: M4!\n");
		return -1;
	}

	char plugPairs[40]; //max 13 pairs of letters + space
	//prompt user for plug board settings
	printf("Enter plugboard wiring (if any) as space separated pairs");
	printf(" (Ex. AN BY CX ):\n");
	fgets(plugPairs, sizeof plugPairs, stdin);
	plugboard_t myPlugboard(plugPairs);

	reflector_t myReflector, thinReflector, grWheel;
	char reflektor, grLetter, grStart;
	int grRing;
	if (myMachineType == 3) {
		printf("Enter Reflector (A, B, or C):\n");
		scanf("%c", &reflektor);
		getchar(); //pull one newline off the input buffer
		reflektor = static_cast<char> (toupper(reflektor));
		if (reflektor != 'A' && reflektor != 'B' && reflektor != 'C') {
			printf("Error - reflector must be A, B, or C.\n");
			return -1;
		}
		myReflector = makeM3Reflector(reflektor);
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
		switch (reflektor) {
		case 'B':
			thinReflector = B_Thin;
			break;
		case 'C':
			thinReflector = C_Thin;
			break;
		default:
			printf("Thin reflector value invalid; defaulting to thin B.\n");
			thinReflector = B_Thin;
			break;
		}
		//prompt user for greek rotor beta or gamma
		printf("Enter Greek Rotor ([B]eta or [G]amma):\n");
		scanf("%c", &grLetter);
		getchar(); //pull one newline off the input buffer
		grLetter = static_cast<char> (toupper(grLetter));
		if (grLetter != 'B' && grLetter != 'G') {
			printf("Error - Greek Rotor must be B/G for Beta/Gamma.\n");
			return -1;
		}
		switch (grLetter) {
		case 'B':
			grWheel = Beta;
			break;
		case 'G':
			grWheel = Gamma;
			break;
		default:
			printf("Greek rotor value invalid; defaulting to Beta.\n");
			grWheel = Beta;
			break;
		}
		printf("Enter starting char of greek wheel (A-Z):\n");
		scanf("%c", &grStart);
		getchar(); //pull one newline off the input buffer
		grStart = static_cast<char> (toupper(grStart));
		if (grStart < 'A' || grStart > 'Z') {
			printf("Error - Start of Greek wheel must be A-Z.\n");
			return -1;
		}
		printf("Enter greek wheel ring position (1-26):\n");
		scanf("%d", &grRing);
		getchar(); //pull one newline off the input buffer
		if (grRing < 1 || grRing > 26) {
			printf("Error - Greek ring must be 1-26.\n");
			return -1;
		}
		myReflector = makeM4Reflector(thinReflector, grWheel, grStart, grRing);
	}//endif machine M4

	//prompt user for rotors, positions, rings
	char leftRoman[5], middleRoman[5], rightRoman[5];
	printf("Enter left, middle, right rotor numbers (I-VIII) (Ex. I II IV: \n");
	scanf("%4s %4s %4s", leftRoman, middleRoman, rightRoman);
	getchar(); //pull one newline off the input buffer
	strToUpper(leftRoman);
	strToUpper(middleRoman);
	strToUpper(rightRoman);
	rotor_t myLeftRotor = assignRotor(leftRoman);
	rotor_t myMiddleRotor = assignRotor(middleRoman);
	rotor_t myRightRotor = assignRotor(rightRoman);

	char lStart, mStart, rStart;
	printf("Enter starting chars, msg key, (A-Z) left to right (Ex. A A A):\n");
	scanf("%c %c %c", &lStart, &mStart, &rStart);
	getchar(); //pull one newline off the input buffer
	lStart = static_cast<char> (toupper(lStart));
	mStart = static_cast<char> (toupper(mStart));
	rStart = static_cast<char> (toupper(rStart));
	if (lStart < 'A' || lStart > 'Z') {
		printf("Error - Start of left rotor must be A-Z.\n");
		return -1;
	}
	if (mStart < 'A' || mStart > 'Z') {
		printf("Error - Start of middle must be A-Z.\n");
		return -1;
	}
	if (rStart < 'A' || rStart > 'Z') {
		printf("Error - Start of right rotor must be A-Z.\n");
		return -1;
	}

	int lRing, mRing, rRing;
	printf("Enter ring positions (1-26) left to right (Ex. 1 1 1):\n");
	scanf("%d %d %d", &lRing, &mRing, &rRing);
	getchar(); //pull one newline off the input buffer
	//check
	if (lRing < 1 || lRing > 26) {
		printf("Error - left ring must be 1-26.\n");
		return -1;
	}
	if (mRing < 1 || mRing > 26) {
		printf("Error - middle ring must be 1-26.\n");
		return -1;
	}
	if (rRing < 1 || rRing > 26) {
		printf("Error - right ring must be 1-26.\n");
		return -1;
	}

	Enigma myEnigma(myReflector, myLeftRotor, myMiddleRotor, myRightRotor);
	myEnigma.init(lStart - 'A', lRing - 1, mStart - 'A', mRing - 1, rStart - 'A',
		rRing - 1);

	printf("Beginning display:\n");
	if (myMachineType == 4) {
		//printf("%c ", myEnigma.reflector.name.back());//c++11 only
		printf("%c ", myEnigma.reflector.name.at(myEnigma.reflector.name.length() - 1));

	}
	printf("%c %c %c\n", AddMod(myEnigma.left.ofs, lRing - 1) + 'A',
		AddMod(myEnigma.middle.ofs, mRing - 1) + 'A',
		AddMod(myEnigma.right.ofs, rRing - 1) + 'A');

	char myMessage[256]; //read somewhere messages were <= 250
	printf("Enter your message (256 char limit):\n");
	fgets(myMessage, sizeof myMessage, stdin);

	//remove the null terminator and make upper case
	for (unsigned int i = 0; i < sizeof(myMessage); ++i) {
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
		ch = myPlugboard.map[ch];
		output += ch + 'A';
	} //end for msg

	printf("message: \n\t%s\n", myMessage);
	printf("encoded:  \n\t%s\n", output.c_str());

	printf("Ending display:\n");
	if (myMachineType == 4) {
		//printf("%c ", myEnigma.reflector.name.back());//c++11 only
		printf("%c ", myEnigma.reflector.name.at(myEnigma.reflector.name.length() - 1));
	}
	printf("%c %c %c\n", AddMod(myEnigma.left.ofs, lRing - 1) + 'A',
		AddMod(myEnigma.middle.ofs, mRing - 1) + 'A',
		AddMod(myEnigma.right.ofs, rRing - 1) + 'A');

	return 0;
}
