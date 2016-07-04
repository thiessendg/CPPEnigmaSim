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

int romanToInt(std::string s) {
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

bool test(const char* test_name, const plugboard_t plugboard, const reflector_t& reflector,
        const rotor_t& left, char ofs_l, int ring_l, const rotor_t& middle, char ofs_m, int ring_m,
        const rotor_t& right, char ofs_r, int ring_r, const char* message, const char* expect) {
    bool success = true;
    printf("%s: \"%s\" %s %s:%c:%d %s:%c:%d %s:%c:%d\n", test_name, plugboard.pairs.c_str(),
            reflector.name.c_str(),
            left.name.c_str(), ofs_l, ring_l,
            middle.name.c_str(), ofs_m, ring_m,
            right.name.c_str(), ofs_r, ring_r);
    Enigma enigma(reflector, left, middle, right);
    enigma.init(ofs_l - 'A', ring_l - 1, ofs_m - 'A', ring_m - 1, ofs_r - 'A', ring_r - 1);

    printf("Beginning display:\n");
    printf("%c %c %c\n", AddMod(enigma.getLeftOfs(), ring_l - 1) + 'A',
            AddMod(enigma.getMiddleOfs(), ring_m - 1) + 'A',
            AddMod(enigma.getRightOfs(), ring_r - 1) + 'A');
    std::string out;
    int histogram_enter[26] = {0};
    int histogram_out[26] = {0};
    for (int i = 0; message[i]; i++) {
        enigma.advance();
        int ch = plugboard.map[message[i] - 'A'];
        assert(ch >= 0 && ch < 26);
        ch = enigma.code(ch);
        assert(ch >= 0 && ch < 26);
        const int check = plugboard.map[enigma.code(ch)] + 'A';
        ch = plugboard.map[ch];
        histogram_enter[enigma.code(message[i] - 'A')]++;
        histogram_out[ch]++;
        out += ch + 'A';
        success &= message[i] == check;
    }
    printf("message: %s\n", message);
    if (out != expect) {
        printf("expect:  %s\n", expect);
    }
    printf("got:     %s\n", out.c_str());
    int max = 0, sum = 0;
    printf("histogram_enter: [");
    for (int i = 0; i < 26; i++) {
        sum += histogram_enter[i];
        printf("%s%d", i ? "," : "", histogram_enter[i]);
        if (histogram_enter[i] > max)
            max = histogram_enter[i];
    }
    printf("]=%d=%d\n", sum, max);
    max = sum = 0;
    printf("histogram_out:   [");
    for (int i = 0; i < 26; i++) {
        sum += histogram_out[i];
        printf("%s%d", i ? "," : "", histogram_out[i]);
        if (histogram_out[i] > max)
            max = histogram_out[i];
    }
    printf("]=%d=%d\n", sum, max);
    //printf("%d %d %d\n", enigma.left.ofs, enigma.middle.ofs, enigma.right.ofs);
    printf("Ending display:\n");
    printf("%c %c %c\n", AddMod(enigma.getLeftOfs(), ring_l - 1) + 'A',
            AddMod(enigma.getMiddleOfs(), ring_m - 1) + 'A',
            AddMod(enigma.getRightOfs(), ring_r - 1) + 'A');
    return success;
}

void test() {
    // from http://wiki.franklinheath.co.uk/index.php/Enigma/Paper_Enigma
    /*
    test("paper1", "", B, I, 'A', 1, II, 'B', 1, III, 'C', 1, "AEFAEJXXBNXYJTY", "CONGRATULATIONS");
    test("paper2", "", B, I, 'A', 1, II, 'B', 1, III, 'R', 1, "MABEKGZXSG", "TURNMIDDLE");
    test("paper3", "", B, I, 'A', 1, II, 'D', 1, III, 'S', 1, "RZFOGFYHPL", "TURNSTHREE");
    test("paper4", "", B, I, 'X', 10, II, 'Y', 14, III, 'Z', 21, "QKTPEBZIUK", "GOODRESULT");
    test("paper5", "AP BR CM FZ GJ IL NT OV QS WX", B, I, 'V', 10, II, 'Q', 14, III, 'Q', 21, "HABHVHLYDFNADZY", "THATSITWELLDONE");
     */

    // from http://wiki.franklinheath.co.uk/index.php/Enigma/Sample_Messages
    test("Enigma Instruction Manual, 1930 Decrypt",
            "AM FI NV PS TU WZ", A, II, 'A', 24, I, 'B', 13, III, 'L', 22,
            "GCDSEAHUGWTQGRKVLFGXUCALXVYMIGMMNMFDXTGNVHVRMMEVOUYFZSLRHDRRXFJWCFHUHMUNZEFRDISIKBGPMYVXUZ",
            "FEINDLIQEINFANTERIEKOLONNEBEOBAQTETXANFANGSUEDAUSGANGBAERWALDEXENDEDREIKMOSTWAERTSNEUSTADT");
    test("Enigma Instruction Manual, 1930 Encrypt",
            "AM FI NV PS TU WZ", A, II, 'A', 24, I, 'B', 13, III, 'L', 22,
            "FEINDLIQEINFANTERIEKOLONNEBEOBAQTETXANFANGSUEDAUSGANGBAERWALDEXENDEDREIKMOSTWAERTSNEUSTADT",
            "GCDSEAHUGWTQGRKVLFGXUCALXVYMIGMMNMFDXTGNVHVRMMEVOUYFZSLRHDRRXFJWCFHUHMUNZEFRDISIKBGPMYVXUZ");

    test("Operation Barbarossa, 1941 #1 Decrypt",
            "AV BS CG DL FU HZ IN KM OW RX", B, II, 'B', 2, IV, 'L', 21, V, 'A', 12,
            "EDPUDNRGYSZRCXNUYTPOMRMBOFKTBZREZKMLXLVEFGUEYSIOZVEQMIKUBPMMYLKLTTDEISMDICAGYKUACTCDOMOHWXMUUIAUBSTSLRNBZSZWNRFXWFYSSXJZVIJHIDISHPRKLKAYUPADTXQSPINQMATLPIFSVKDASCTACDPBOPVHJK",
            "AUFKLXABTEILUNGXVONXKURTINOWAXKURTINOWAXNORDWESTLXSEBEZXSEBEZXUAFFLIEGERSTRASZERIQTUNGXDUBROWKIXDUBROWKIXOPOTSCHKAXOPOTSCHKAXUMXEINSAQTDREINULLXUHRANGETRETENXANGRIFFXINFXRGTX");
    test("Operation Barbarossa, 1941 #1 Encrypt",
            "AV BS CG DL FU HZ IN KM OW RX", B, II, 'B', 2, IV, 'L', 21, V, 'A', 12,
            "AUFKLXABTEILUNGXVONXKURTINOWAXKURTINOWAXNORDWESTLXSEBEZXSEBEZXUAFFLIEGERSTRASZERIQTUNGXDUBROWKIXDUBROWKIXOPOTSCHKAXOPOTSCHKAXUMXEINSAQTDREINULLXUHRANGETRETENXANGRIFFXINFXRGTX",
            "EDPUDNRGYSZRCXNUYTPOMRMBOFKTBZREZKMLXLVEFGUEYSIOZVEQMIKUBPMMYLKLTTDEISMDICAGYKUACTCDOMOHWXMUUIAUBSTSLRNBZSZWNRFXWFYSSXJZVIJHIDISHPRKLKAYUPADTXQSPINQMATLPIFSVKDASCTACDPBOPVHJK");

    test("Operation Barbarossa, 1941 #2 Decrypt",
            "AV BS CG DL FU HZ IN KM OW RX", B, II, 'L', 2, IV, 'S', 21, V, 'D', 12,
            "SFBWDNJUSEGQOBHKRTAREEZMWKPPRBXOHDROEQGBBGTQVPGVKBVVGBIMHUSZYDAJQIROAXSSSNREHYGGRPISEZBOVMQIEMMZCYSGQDGRERVBILEKXYQIRGIRQNRDNVRXCYYTNJR",
            "DREIGEHTLANGSAMABERSIQERVORWAERTSXEINSSIEBENNULLSEQSXUHRXROEMXEINSXINFRGTXDREIXAUFFLIEGERSTRASZEMITANFANGXEINSSEQSXKMXKMXOSTWXKAMENECXK");
    test("Operation Barbarossa, 1941 #2 Encrypt",
            "AV BS CG DL FU HZ IN KM OW RX", B, II, 'L', 2, IV, 'S', 21, V, 'D', 12,
            "DREIGEHTLANGSAMABERSIQERVORWAERTSXEINSSIEBENNULLSEQSXUHRXROEMXEINSXINFRGTXDREIXAUFFLIEGERSTRASZEMITANFANGXEINSSEQSXKMXKMXOSTWXKAMENECXK",
            "SFBWDNJUSEGQOBHKRTAREEZMWKPPRBXOHDROEQGBBGTQVPGVKBVVGBIMHUSZYDAJQIROAXSSSNREHYGGRPISEZBOVMQIEMMZCYSGQDGRERVBILEKXYQIRGIRQNRDNVRXCYYTNJR");

    test("U-264 (Kapitänleutnant Hartwig Looks), 1942 Decrypt",
            "AT BL DF GJ HM NW OP QY RZ VX", make_M4_reflector(B_Thin, Beta, 'V', 1), II, 'J', 1, IV, 'N', 1, I, 'A', 22,
            "NCZWVUSXPNYMINHZXMQXSFWXWLKJAHSHNMCOCCAKUQPMKCSMHKSEINJUSBLKIOSXCKUBHMLLXCSJUSRRDVKOHULXWCCBGVLIYXEOAHXRHKKFVDREWEZLXOBAFGYUJQUKGRTVUKAMEURBVEKSUHHVOYHABCJWMAKLFKLMYFVNRIZRVVRTKOFDANJMOLBGFFLEOPRGTFLVRHOWOPBEKVWMUQFMPWPARMFHAGKXIIBG",
            "VONVONJLOOKSJHFFTTTEINSEINSDREIZWOYYQNNSNEUNINHALTXXBEIANGRIFFUNTERWASSERGEDRUECKTYWABOSXLETZTERGEGNERSTANDNULACHTDREINULUHRMARQUANTONJOTANEUNACHTSEYHSDREIYZWOZWONULGRADYACHTSMYSTOSSENACHXEKNSVIERMBFAELLTYNNNNNNOOOVIERYSICHTEINSNULL");
    test("U-264 (Kapitänleutnant Hartwig Looks), 1942 Encrypt",
            "AT BL DF GJ HM NW OP QY RZ VX", make_M4_reflector(B_Thin, Beta, 'V', 1), II, 'J', 1, IV, 'N', 1, I, 'A', 22,
            "VONVONJLOOKSJHFFTTTEINSEINSDREIZWOYYQNNSNEUNINHALTXXBEIANGRIFFUNTERWASSERGEDRUECKTYWABOSXLETZTERGEGNERSTANDNULACHTDREINULUHRMARQUANTONJOTANEUNACHTSEYHSDREIYZWOZWONULGRADYACHTSMYSTOSSENACHXEKNSVIERMBFAELLTYNNNNNNOOOVIERYSICHTEINSNULL",
            "NCZWVUSXPNYMINHZXMQXSFWXWLKJAHSHNMCOCCAKUQPMKCSMHKSEINJUSBLKIOSXCKUBHMLLXCSJUSRRDVKOHULXWCCBGVLIYXEOAHXRHKKFVDREWEZLXOBAFGYUJQUKGRTVUKAMEURBVEKSUHHVOYHABCJWMAKLFKLMYFVNRIZRVVRTKOFDANJMOLBGFFLEOPRGTFLVRHOWOPBEKVWMUQFMPWPARMFHAGKXIIBG");

    test("Scharnhorst (Konteradmiral Erich Bey), 1943 Decrypt",
            "AN EZ HK IJ LR MQ OT PV SW UX", B, III, 'U', 1, VI, 'Z', 8, VIII, 'V', 13,
            "YKAENZAPMSCHZBFOCUVMRMDPYCOFHADZIZMEFXTHFLOLPZLFGGBOTGOXGRETDWTJIQHLMXVJWKZUASTR",
            "STEUEREJTANAFJORDJANSTANDORTQUAAACCCVIERNEUNNEUNZWOFAHRTZWONULSMXXSCHARNHORSTHCO");
    test("Scharnhorst (Konteradmiral Erich Bey), 1943 Encrypt",
            "AN EZ HK IJ LR MQ OT PV SW UX", B, III, 'U', 1, VI, 'Z', 8, VIII, 'V', 13,
            "STEUEREJTANAFJORDJANSTANDORTQUAAACCCVIERNEUNNEUNZWOFAHRTZWONULSMXXSCHARNHORSTHCO",
            "YKAENZAPMSCHZBFOCUVMRMDPYCOFHADZIZMEFXTHFLOLPZLFGGBOTGOXGRETDWTJIQHLMXVJWKZUASTR");
    // other
    /*
    test("singh cipher challenge stage 8",
            "EI AS JN KL MU OT", B, III, 'A', 1, II, 'F', 26, I, 'L', 1,
            "KJQPWCAISRXWQMASEUPFOCZOQZVGZGWWKYEZVTEMTPZHVNOTKZHRCCFQLVRPCCWLWPUYONFHOGDDMOJXGGBHWWUXNJEZAXFUMEYSECSMAZFXNNASSZGWRBDDMAPGMRWT"
            "GXXZAXLBXCPHZBOUYVRRVFDKHXMQOGYLYYCUWQBTADRLBOZKYXQPWUUAFMIZTCEAXBCREDHZJDOPSQTNLIHIQHNMJZUHSMVAHHQJLIJRRXQZNFKHUIINZPMPAFLHYONM"
            "RMDADFOXTYOPEWEJGECAHPHSDQIENAYUUBAGTBHYBGDDWQCPGUELZZWDKJYAAAGATKFNSIYFJTTEHHFSGBRRDFDIXCPAHVOELLSTDXUAQUWNAPH",
            "DASXLOESUNGSWORTXISTXPLUTOXXSTUFEXNEUNXENTHAELTXEINEXMITTEILUNGXDIEXMITXDESXENTKODIERTXISTXXICHXHABEXDASXLINKSSTEHENDEXBYTEXDESX"
            "SCHLUESSELSXENTDECKTXXESXISTXEINSXEINSXZEROXEINSXZEROXZEROXEINSXEINSXEINSXXICHXPROGRAMMIERTEXDESXUNDXENTDECKTEXDASSXDASXWORTXDEB"
            "UGGERXWENNXESXMITXDEMXZUGRUNDELIEGENDENXSCHLUESSELXENTKODIERTXWIRDXALSXRESULTATXDIEXSCHRIFTZEICHENXUNTENXERGIBT");

     */
}

int main(int argc, char** args) {
    //test();
    /*
    test("DGT I", // testname
            "AT BL DF GJ HM NW OP QY RZ VX", //plugboard
            make_M4_relector(B_Thin, Beta, 'V', 5), //M4 reflector, greek wheel, start, ring
            II, 'J', 17, //left wheel type, start, ring
            IV, 'N', 6, //middle
            VI, 'H', 22, //right
            "DAVIDGTHIESSEN", //input
            "OJSCLJMOWJNCYI"); //expected output
    
    test("DGT II", // testname
            "", //plugboard
            B,
            III, 'A', 1, //left wheel type, start, ring
            II, 'A', 1, //middle
            I, 'A', 1, //right
            "AAAAA", //input
            "FTZMG"); //expected output
    
    test("DGT III", // testname
            "", //plugboard
            B,
            III, 'A', 2, //left wheel type, start, ring
            II, 'A', 2, //middle
            I, 'A', 2, //right
            "AAAAA", //input
            "TTKXE"); //expected output
    
    test("DGT IV", // testname
            "", //plugboard
            B,
            I, 'A', 1, //left wheel type, start, ring
            II, 'A', 1, //middle
            III, 'A', 1, //right
            "AAAAA", //input
            "BDZGO"); //expected output
    
    test("DGT V", // testname
            "", //plugboard
            B,
            I, 'A', 2, //left wheel type, start, ring
            II, 'A', 2, //middle
            III, 'A', 2, //right
            "AAAAA", //input
            "EWTYX"); //expected output
     */
    //prompt user for I/M3 or M4 Enigma
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
