#include <string>
#include <vector>
#include <cstdio>
#include <cassert>
#include <time.h>
#include "reflector.h"
#include "rotor.h"
#include "Enigma.h"
#include "plugboard.h"
#include "AddSubMod.h"
#include "constants.h"

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
    int machine, lt_r, mi_r, rt_r, gk_r;
    char pb[40];
    int lt, mi, rt;
    char reflektor, greek, lt_s, mi_s, rt_s, gk_s;
    printf("Enigma M3 or M4? Enter 3 or 4:\n");
    scanf("%d", &machine);
    getchar(); //pull one newline off the input buffer

    //prompt user for plug board settings
    printf("Enter plugboard wiring (if any) as space separated pairs. Ex: AN BY CX: \n");
    fgets(pb, sizeof (pb), stdin);
    plugboard_t myPlug(pb);
    const reflector_t* myRefl;
    const reflector_t* myM4Refl;
    const reflector_t* myGreek;
    const rotor_t* myLRot;
    const rotor_t* myMRot;
    const rotor_t* myRRot;
   
    if (machine == 3) {
        //prompt user for reflektor
        printf("Enter Reflector (A, B, C):\n");
        scanf("%c", &reflektor);
        getchar(); //pull one newline off the input buffer

        switch (reflektor) {
            case 'A':
                myRefl = &A;
                break;
            case 'B':
                myRefl = &B;
                break;
            case 'C':
                myRefl = &C;
                break;
            default:
                myRefl = &B;
                break;
        }
    }//endif machine I/M3
    
    if (machine == 4) {
        //prompt user for reflektor
        printf("Enter Reflector (B or C (thin)):\n");
        scanf("%c", &reflektor);
        getchar(); //pull one newline off the input buffer

        switch (reflektor) {
            case 'B':
                myM4Refl = &B_Thin;
                break;
            case 'C':
                myM4Refl = &C_Thin;
                break;
            default:
                myM4Refl = &B_Thin;
                break;
        }
        
        //prompt user for greek rotor beta or gamma
        printf("Enter Greek Rotor (B or G (Beta,Gamma):\n");
        scanf("%c", &greek);
        getchar(); //pull one newline off the input buffer
        switch (greek) {
            case 'B':
                myGreek = &Beta;
                break;
            case 'G':
                myGreek = &Gamma;
                break;
            default:
                myGreek = &Beta;
                break;
        }
        printf("Enter starting char of greek wheel (Ex. A):\n");
        scanf("%c", &gk_s);
        getchar(); //pull one newline off the input buffer

        printf("Enter greek wheel ring position (Ex. 1):\n");
        scanf("%d", &gk_r);
        getchar(); //pull one newline off the input buffer
        reflector_t temp = make_M4_reflector(*myM4Refl, *myGreek, gk_s, gk_r);
        myRefl = &temp;
    }//endif machine M4

        //prompt user for rotors, positions, rings
        printf("Enter left, middle, and right rotor numbers (1=I, 2=II, 3=III,etc): \n");
        printf("Example: 1 3 5 means I III V\n");
        scanf("%d %d %d", &lt, &mi, &rt);

        switch (lt) {
            case 1:
                myLRot = &I;
                break;
            case 2:
                myLRot = &II;
                break;
            case 3:
                myLRot = &III;
                break;
            case 4:
                myLRot = &IV;
                break;
            case 5:
                myLRot = &V;
                break;
            case 6:
                myLRot = &VI;
                break;
            case 7:
                myLRot = &VII;
                break;
            case 8:
                myLRot = &VIII;
                break;
            default:
                myLRot = &I;
                break;
        }
        switch (mi) {
            case 1:
                myMRot = &I;
                break;
            case 2:
                myMRot = &II;
                break;
            case 3:
                myMRot = &III;
                break;
            case 4:
                myMRot = &IV;
                break;
            case 5:
                myMRot = &V;
                break;
            case 6:
                myMRot = &VI;
                break;
            case 7:
                myMRot = &VII;
                break;
            case 8:
                myMRot = &VIII;
                break;
            default:
                myMRot = &I;
                break;
        }
        switch (rt) {
            case 1:
                myRRot = &I;
                break;
            case 2:
                myRRot = &II;
                break;
            case 3:
                myRRot = &III;
                break;
            case 4:
                myRRot = &IV;
                break;
            case 5:
                myRRot = &V;
                break;
            case 6:
                myRRot = &VI;
                break;
            case 7:
                myRRot = &VII;
                break;
            case 8:
                myRRot = &VIII;
                break;
            default:
                myRRot = &I;
                break;
        }
        getchar(); //pull one newline off the input buffer

        printf("Enter space separated starting char, left to right (Ex. A A A):\n");
        scanf("%c %c %c", &lt_s, &mi_s, &rt_s);
        getchar(); //pull one newline off the input buffer

        printf("Enter space separated ring position, left to right (Ex. 1 1 1):\n");
        scanf("%d %d %d", &lt_r, &mi_r, &rt_r);
        getchar(); //pull one newline off the input buffer
       
    Enigma enigma(*myRefl, *myLRot, *myMRot, *myRRot);
    enigma.init(lt_s - 'A', lt_r - 1, mi_s - 'A', mi_r - 1, rt_s - 'A', rt_r - 1);

    printf("Beginning display:\n");
    printf("%c %c %c\n", AddMod(enigma.getLeftOfs(), lt_r - 1) + 'A',
            AddMod(enigma.getMiddleOfs(), mi_r - 1) + 'A',
            AddMod(enigma.getRightOfs(), rt_r - 1) + 'A');

    char msg[250];
    printf("Enter your message (250 char limit):\n");
    fgets(msg, 250, stdin);

    //remove the null terminator
    for (int i = 0; i < sizeof (msg); ++i) {
        if (msg[i] == '\n') {
            msg[i] = '\0';
        }
    }
    //if we get all the way to here, there must not have been a newline!

    std::string out;
    for (int i = 0; msg[i]; i++) {
        enigma.advance();
        int ch = myPlug.map[msg[i] - 'A'];
        assert(ch >= 0 && ch < 26);
        ch = enigma.code(ch);
        assert(ch >= 0 && ch < 26);
        const int check = myPlug.map[enigma.code(ch)] + 'A';
        ch = myPlug.map[ch];
        out += ch + 'A';
    } //end for msg

    printf("message: \n\t%s\n", msg);

    printf("encrypt/decrypt: \n\t%s\n", out.c_str());

    printf("Ending display:\n");
    printf("%c %c %c\n", AddMod(enigma.getLeftOfs(), lt_r - 1) + 'A',
            AddMod(enigma.getMiddleOfs(), mi_r - 1) + 'A',
            AddMod(enigma.getRightOfs(), rt_r - 1) + 'A');

    return 0;
}
