#include <string>
#include <vector>
#include <cstdio>
#include <cassert>
#include <time.h>
#include "CppEnigmaSim/reflector.h"
#include "CppEnigmaSim/rotor.h"
#include "CppEnigmaSim/Enigma.h"
#include "CppEnigmaSim/plugboard.h"
#include "CppEnigmaSim/AddSubMod.h"
#include "CppEnigmaSim/constants.h"

reflector_t make_M4_relector(const reflector_t& thin, const reflector_t& greek,
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
    return success;
}

void test() {
    // from http://wiki.franklinheath.co.uk/index.php/Enigma/Paper_Enigma
    test("paper1", "", B, I, 'A', 1, II, 'B', 1, III, 'C', 1, "AEFAEJXXBNXYJTY", "CONGRATULATIONS");
    test("paper2", "", B, I, 'A', 1, II, 'B', 1, III, 'R', 1, "MABEKGZXSG", "TURNMIDDLE");
    test("paper3", "", B, I, 'A', 1, II, 'D', 1, III, 'S', 1, "RZFOGFYHPL", "TURNSTHREE");
    test("paper4", "", B, I, 'X', 10, II, 'Y', 14, III, 'Z', 21, "QKTPEBZIUK", "GOODRESULT");
    test("paper5", "AP BR CM FZ GJ IL NT OV QS WX", B, I, 'V', 10, II, 'Q', 14, III, 'Q', 21, "HABHVHLYDFNADZY", "THATSITWELLDONE");
    // from http://wiki.franklinheath.co.uk/index.php/Enigma/Sample_Messages
    test("Enigma Instruction Manual, 1930",
            "AM FI NV PS TU WZ", A, II, 'A', 24, I, 'B', 13, III, 'L', 22,
            "GCDSEAHUGWTQGRKVLFGXUCALXVYMIGMMNMFDXTGNVHVRMMEVOUYFZSLRHDRRXFJWCFHUHMUNZEFRDISIKBGPMYVXUZ",
            "FEINDLIQEINFANTERIEKOLONNEBEOBAQTETXANFANGSUEDAUSGANGBAERWALDEXENDEDREIKMOSTWAERTSNEUSTADT");
    test("Operation Barbarossa, 1941 #1",
            "AV BS CG DL FU HZ IN KM OW RX", B, II, 'B', 2, IV, 'L', 21, V, 'A', 12,
            "EDPUDNRGYSZRCXNUYTPOMRMBOFKTBZREZKMLXLVEFGUEYSIOZVEQMIKUBPMMYLKLTTDEISMDICAGYKUACTCDOMOHWXMUUIAUBSTSLRNBZSZWNRFXWFYSSXJZVIJHIDISHPRKLKAYUPADTXQSPINQMATLPIFSVKDASCTACDPBOPVHJK",
            "AUFKLXABTEILUNGXVONXKURTINOWAXKURTINOWAXNORDWESTLXSEBEZXSEBEZXUAFFLIEGERSTRASZERIQTUNGXDUBROWKIXDUBROWKIXOPOTSCHKAXOPOTSCHKAXUMXEINSAQTDREINULLXUHRANGETRETENXANGRIFFXINFXRGTX");
    test("Operation Barbarossa, 1941 #2",
            "AV BS CG DL FU HZ IN KM OW RX", B, II, 'L', 2, IV, 'S', 21, V, 'D', 12,
            "SFBWDNJUSEGQOBHKRTAREEZMWKPPRBXOHDROEQGBBGTQVPGVKBVVGBIMHUSZYDAJQIROAXSSSNREHYGGRPISEZBOVMQIEMMZCYSGQDGRERVBILEKXYQIRGIRQNRDNVRXCYYTNJR",
            "DREIGEHTLANGSAMABERSIQERVORWAERTSXEINSSIEBENNULLSEQSXUHRXROEMXEINSXINFRGTXDREIXAUFFLIEGERSTRASZEMITANFANGXEINSSEQSXKMXKMXOSTWXKAMENECXK");
    test("U-264 (Kapitänleutnant Hartwig Looks), 1942",
            "AT BL DF GJ HM NW OP QY RZ VX", make_M4_relector(B_Thin, Beta, 'V', 1), II, 'J', 1, IV, 'N', 1, I, 'A', 22,
            "NCZWVUSXPNYMINHZXMQXSFWXWLKJAHSHNMCOCCAKUQPMKCSMHKSEINJUSBLKIOSXCKUBHMLLXCSJUSRRDVKOHULXWCCBGVLIYXEOAHXRHKKFVDREWEZLXOBAFGYUJQUKGRTVUKAMEURBVEKSUHHVOYHABCJWMAKLFKLMYFVNRIZRVVRTKOFDANJMOLBGFFLEOPRGTFLVRHOWOPBEKVWMUQFMPWPARMFHAGKXIIBG",
            "VONVONJLOOKSJHFFTTTEINSEINSDREIZWOYYQNNSNEUNINHALTXXBEIANGRIFFUNTERWASSERGEDRUECKTYWABOSXLETZTERGEGNERSTANDNULACHTDREINULUHRMARQUANTONJOTANEUNACHTSEYHSDREIYZWOZWONULGRADYACHTSMYSTOSSENACHXEKNSVIERMBFAELLTYNNNNNNOOOVIERYSICHTEINSNULL");
    test("Scharnhorst (Konteradmiral Erich Bey), 1943",
            "AN EZ HK IJ LR MQ OT PV SW UX", B, III, 'U', 1, VI, 'Z', 8, VIII, 'V', 13,
            "YKAENZAPMSCHZBFOCUVMRMDPYCOFHADZIZMEFXTHFLOLPZLFGGBOTGOXGRETDWTJIQHLMXVJWKZUASTR",
            "STEUEREJTANAFJORDJANSTANDORTQUAAACCCVIERNEUNNEUNZWOFAHRTZWONULSMXXSCHARNHORSTHCO");
    // other
    test("singh cipher challenge stage 8",
            "EI AS JN KL MU OT", B, III, 'A', 1, II, 'F', 26, I, 'L', 1,
            "KJQPWCAISRXWQMASEUPFOCZOQZVGZGWWKYEZVTEMTPZHVNOTKZHRCCFQLVRPCCWLWPUYONFHOGDDMOJXGGBHWWUXNJEZAXFUMEYSECSMAZFXNNASSZGWRBDDMAPGMRWT"
            "GXXZAXLBXCPHZBOUYVRRVFDKHXMQOGYLYYCUWQBTADRLBOZKYXQPWUUAFMIZTCEAXBCREDHZJDOPSQTNLIHIQHNMJZUHSMVAHHQJLIJRRXQZNFKHUIINZPMPAFLHYONM"
            "RMDADFOXTYOPEWEJGECAHPHSDQIENAYUUBAGTBHYBGDDWQCPGUELZZWDKJYAAAGATKFNSIYFJTTEHHFSGBRRDFDIXCPAHVOELLSTDXUAQUWNAPH",
            "DASXLOESUNGSWORTXISTXPLUTOXXSTUFEXNEUNXENTHAELTXEINEXMITTEILUNGXDIEXMITXDESXENTKODIERTXISTXXICHXHABEXDASXLINKSSTEHENDEXBYTEXDESX"
            "SCHLUESSELSXENTDECKTXXESXISTXEINSXEINSXZEROXEINSXZEROXZEROXEINSXEINSXEINSXXICHXPROGRAMMIERTEXDESXUNDXENTDECKTEXDASSXDASXWORTXDEB"
            "UGGERXWENNXESXMITXDEMXZUGRUNDELIEGENDENXSCHLUESSELXENTKODIERTXWIRDXALSXRESULTATXDIEXSCHRIFTZEICHENXUNTENXERGIBT");
}

int main(int argc, char** args) {
    //test();
    test("DGT I", // testname
            "AT BL DF GJ HM NW OP QY RZ VX", //plugboard
            make_M4_relector(B_Thin, Beta, 'V', 5), //M4 reflector, greek wheel, start, ring
            II, 'J', 17, //left wheel type, start, ring
            IV, 'N', 6, //middle
            VI, 'H', 22, //right
            "DAVIDGTHIESSEN", //input
            ""); //expected output
    
    test("DGT II", // testname
            "", //plugboard
            B,
            III, 'A', 1, //left wheel type, start, ring
            II, 'A', 1, //middle
            I, 'A', 1, //right
            "AAAAA", //input
            ""); //expected output
    
    test("DGT III", // testname
            "", //plugboard
            B,
            III, 'A', 2, //left wheel type, start, ring
            II, 'A', 2, //middle
            I, 'A', 2, //right
            "AAAAA", //input
            ""); //expected output
    
    test("DGT IV", // testname
            "", //plugboard
            B,
            I, 'A', 1, //left wheel type, start, ring
            II, 'A', 1, //middle
            III, 'A', 1, //right
            "AAAAA", //input
            ""); //expected output
    
    test("DGT V", // testname
            "", //plugboard
            B,
            I, 'A', 2, //left wheel type, start, ring
            II, 'A', 2, //middle
            III, 'A', 2, //right
            "AAAAA", //input
            ""); //expected output
    
    return 0;
}
