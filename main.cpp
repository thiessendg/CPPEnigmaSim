#include <string>
#include <vector>
#include <cstdio>
#include <cassert>
#include <time.h>

#define INLINE __attribute__((always_inline)) inline

INLINE int AddMod(int a, int b)
{
    return (a + b) > 25 ? (a + b) - 26 : a + b;
}

INLINE int SubMod(int a, int b)
{
    return (a - b) < 0 ? (a - b) + 26 : a - b;
}

struct reflector_t
{
	const std::string name;
	char map[26];
};

struct rotor_t
{
	const std::string name;
	char enter[26];
	char exit[26];
	int notches[2];
};

const rotor_t I =	{"I",
                    {4,10,12,5,11,6,3,16,21,25,13,19,14,22,24,7,23,20,18,15,0,8,1,17,2,9},
                    {20,22,24,6,0,3,5,15,21,25,1,4,2,10,12,19,7,23,18,11,17,8,13,16,14,9},
                    {16}};
const rotor_t II =  {"II",
                    {0,9,3,10,18,8,17,20,23,1,11,7,22,19,12,2,16,6,25,13,15,24,5,21,14,4},
                    {0,9,15,2,25,22,17,11,5,1,3,10,14,19,24,20,16,6,4,13,7,23,12,8,21,18},
                    {4}};
const rotor_t III = {"III",
                    {1,3,5,7,9,11,2,15,17,19,23,21,25,13,24,4,8,22,6,0,10,12,20,18,16,14},
                    {19,0,6,1,15,2,18,3,16,4,20,5,21,13,25,7,24,8,23,9,22,11,17,10,14,12},
                    {21}};
const rotor_t IV =  {"IV",
                    {4,18,14,21,15,25,9,0,24,16,20,8,17,7,23,11,13,5,19,6,10,3,2,12,22,1},
                    {7,25,22,21,0,17,19,13,11,6,20,15,23,16,2,4,9,12,1,18,10,3,24,14,8,5},
                    {9}};
const rotor_t V =   {"V",
                    {21,25,1,17,6,8,19,24,20,15,18,3,13,7,11,23,0,22,12,9,16,14,5,4,2,10},
                    {16,2,24,11,23,22,4,13,5,19,25,14,18,12,21,9,20,3,10,6,8,0,17,15,7,1},
                    {25}};
const rotor_t VI =  {"VI",
                    {9,15,6,21,14,20,12,5,24,16,1,4,13,7,25,17,3,10,0,18,23,11,8,2,19,22},
                    {18,10,23,16,11,7,2,13,22,0,17,21,6,12,4,1,9,15,19,24,5,3,25,20,8,14},
                    {25,12}};
const rotor_t VII = {"VII",
                    {13,25,9,7,6,17,2,23,12,24,18,22,1,14,20,5,0,8,21,11,15,4,10,16,3,19},
                    {16,12,6,24,21,15,4,3,17,2,22,19,8,0,13,20,23,5,10,25,14,18,11,7,9,1},
                    {25,12}};
const rotor_t VIII ={"VIII",
                    {5,10,16,7,19,11,23,14,2,1,9,18,15,3,25,17,0,12,4,22,13,8,20,24,6,21},
                    {16,9,8,13,18,0,24,3,21,10,1,5,17,20,7,12,2,15,11,4,22,25,19,6,23,14},
                    {25,12}};

const reflector_t A =   {"A",
                        {4,9,12,25,0,11,24,23,21,1,22,5,2,17,16,20,14,13,19,18,15,8,10,7,6,3}};
const reflector_t B =   {"B",
                        {24,17,20,7,16,18,11,3,15,23,13,6,14,10,12,8,4,1,5,25,2,22,21,9,0,19}};
const reflector_t C =   {"C",
                        {5,21,15,9,8,0,14,24,4,3,17,25,23,22,6,2,19,10,20,16,18,1,13,12,7,11}};
const reflector_t Gamma =   {"Gamma",
                            {5,18,14,10,0,13,20,4,17,7,12,1,19,8,24,2,22,11,16,15,25,23,21,6,9,3}};
const reflector_t Beta =    {"Beta",
                            {11,4,24,9,21,2,13,8,23,22,15,1,16,12,3,17,19,0,10,25,6,5,20,7,14,18}};
const reflector_t B_Thin =  {"B_Thin",
                            {4,13,10,16,0,20,24,22,9,8,2,14,15,1,11,12,3,23,25,21,5,19,7,17,6,18}};
const reflector_t C_Thin =  {"C_Thin",
                            {17,3,14,1,9,13,19,10,21,4,7,12,11,5,2,22,25,0,23,6,24,8,15,18,20,16}};

struct Enigma
{
	/* An Enigma machine has 3 moving rotors and a reflector.
	The commercial and M3 Enigma (army, air force, railways) had a fixed-position reflector.
	There were several rotors and reflectors to choose between.  The exact rotation-point of individual rotors was controlled by rings.
	The M4 Enigma (u-boats) was basically a M3 Enigma with a special two-part reflector.  This configurable reflector made for
	a dramatic increase in keyspace. */

	Enigma(const reflector_t& ref, const rotor_t& l, const rotor_t& m, const rotor_t& r):
		reflector(ref), left(l), middle(m), right(r) {};

	INLINE void init(int ofs_l, int ring_l, int ofs_m, int ring_m, int ofs_r, int ring_r)
	{
		left.ofs = SubMod(ofs_l, ring_l);
		middle.ofs = SubMod(ofs_m, ring_m);
		right.ofs = SubMod(ofs_r, ring_r);

		left.notches[0] = SubMod(left.rotor.notches[0], ring_l);
		left.notches[1] = left.rotor.notches[1]? SubMod(left.rotor.notches[1], ring_l): -1;

		middle.notches[0] = SubMod(middle.rotor.notches[0], ring_m);
		middle.notches[1] = middle.rotor.notches[1]? SubMod(middle.rotor.notches[1], ring_m): -1;

		right.notches[0] = SubMod(right.rotor.notches[0], ring_r);
		right.notches[1] = right.rotor.notches[1]? SubMod(right.rotor.notches[1], ring_r): -1;
	};

	INLINE void advance()
	{
		if (middle.knocks())
        {
			left.advance();
			middle.advance();
		}
		else if (right.knocks())
        {
			middle.advance();
		}
		right.advance();
	};

	INLINE int enter(int ch) const
	{
		ch = right.enter(ch, 0);
		ch = middle.enter(ch, right.ofs);
		ch = left.enter(ch, middle.ofs);
		return ch;
	};

	INLINE int exit(int ch) const
	{
		ch = reflector.map[SubMod(ch, left.ofs)];
		ch = left.exit(ch, 0);
		ch = middle.exit(ch, left.ofs);
		ch = right.exit(ch, middle.ofs);
		ch = SubMod(ch, right.ofs);
		return ch;
	};

	INLINE int code(int ch) const
	{
		return exit(enter(ch));
	};

	reflector_t reflector;

	struct wheel_t
	{
		wheel_t(const rotor_t r): rotor(r) {};

		INLINE bool knocks() const
		{
		    return ofs == notches[0] || ofs == notches[1];
        };

		INLINE void advance()
		{
		    ofs = AddMod(ofs, 1);
        };

    	INLINE int enter(int ch, int prev_ofs) const
    	{
    	    return rotor.enter[SubMod(AddMod(ch, ofs), prev_ofs)];
        }

    	INLINE int exit(int ch, int prev_ofs) const
    	{
    	    return rotor.exit[SubMod(AddMod(ch, ofs), prev_ofs)];
        }

		const rotor_t rotor;
		int ofs;
		int notches[2];
	} left, middle, right;
};

reflector_t make_M4_relector(const reflector_t& thin, const reflector_t& greek, char ofs, int ring)
{
	reflector_t reflector = { thin.name + ":" + greek.name + ":" + ofs };
	ofs -= 'A';
	ofs = SubMod(ofs, ring-1);

	// make the inverse of the mappings
	int inverse_thin[26], inverse_greek[26];
	for (int i=0; i<26; i++)
    {
		for (int j=0; j<26; j++)
		{
			if (thin.map[j] == i)
				inverse_thin[i] = j;
			if (greek.map[j] == i)
				inverse_greek[i] = j;
		}
	}
	// work out effective mapping
	for (int i=0; i<26; i++)
	{
		int ch = greek.map[AddMod(i, ofs)]; // enter greek rotor
		ch = AddMod(inverse_thin[SubMod(ch, ofs)], ofs); // through the thin reflector
		reflector.map[i] = SubMod(inverse_greek[ch], ofs); // and back out the greek rotor
	}
	return reflector;
}

struct plugboard_t
{
	plugboard_t(const char* p): pairs(p)
	{
		for (int i=0; i<26; i++)
			map[i] = i;
		while (*p)
        {
			int a = *p++ - 'A';
			int b = *p++ - 'A';
			map[a] = b;
			map[b] = a;
			if (!*p++) break;
		}
	};
	const std::string pairs;
	char map[26];
};

bool test(const char* test_name, const plugboard_t plugboard, const reflector_t& reflector,
        const rotor_t& left, char ofs_l, int ring_l, const rotor_t& middle, char ofs_m, int ring_m,
		const rotor_t& right, char ofs_r, int ring_r, const char* message, const char* expect)
{
	bool success = true;
	printf("%s: \"%s\" %s %s:%c:%d %s:%c:%d %s:%c:%d\n", test_name, plugboard.pairs.c_str(),
		reflector.name.c_str(),
		left.name.c_str(), ofs_l, ring_l,
		middle.name.c_str(), ofs_m, ring_m,
		right.name.c_str(), ofs_r, ring_r);
	Enigma enigma(reflector, left, middle, right);
	enigma.init(ofs_l-'A', ring_l-1, ofs_m-'A', ring_m-1, ofs_r-'A', ring_r-1);
	std::string out;
	int histogram_enter[26] = {0};
	int histogram_out[26] = {0};
	for (int i=0; message[i]; i++)
    {
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
	if (out != expect)
	{
		printf("expect:  %s\n", expect);
	}
	printf("got:     %s\n", out.c_str());
	int max = 0, sum = 0;
	printf("histogram_enter: [");
	for (int i=0; i<26; i++)
	{
		sum += histogram_enter[i];
		printf("%s%d", i? ",": "", histogram_enter[i]);
		if (histogram_enter[i] > max)
			max = histogram_enter[i];
	}
	printf("]=%d=%d\n", sum, max);
	max = sum = 0;
	printf("histogram_out:   [");
	for (int i=0; i<26; i++)
	{
		sum += histogram_out[i];
		printf("%s%d", i? ",": "", histogram_out[i]);
		if (histogram_out[i] > max)
			max = histogram_out[i];
	}
	printf("]=%d=%d\n", sum, max);
	return success;
}

void test()
{
    // from http://wiki.franklinheath.co.uk/index.php/Enigma/Paper_Enigma
    test("paper1", "", B, I, 'A', 1, II, 'B', 1, III, 'C', 1, "AEFAEJXXBNXYJTY", "CONGRATULATIONS");
    test("paper2", "", B, I, 'A', 1, II, 'B', 1, III, 'R', 1, "MABEKGZXSG", "TURNMIDDLE");
    test("paper3", "", B, I, 'A', 1, II, 'D', 1, III, 'S', 1, "RZFOGFYHPL", "TURNSTHREE");
    test("paper4", "", B, I, 'X', 10, II, 'Y', 14, III, 'Z', 21, "QKTPEBZIUK", "GOODRESULT");
    test("paper5", "AP BR CM FZ GJ IL NT OV QS WX", B, I, 'V', 10, II, 'Q', 14, III, 'Q', 21, "HABHVHLYDFNADZY","THATSITWELLDONE");
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

int main(int argc, char** args)
{
	//test();
	test("DGT", // testname
		"AT BL DF GJ HM NW OP QY RZ VX", //plugboard
        make_M4_relector(B_Thin, Beta, 'V', 5), //M4 reflector, greek wheel, start, ring
        II, 'J', 17, //left wheel type, start, ring
        IV, 'N', 6, //middle
        VI, 'H', 22, //right
		"DAVIDGTHIESSEN", //input
		"              ");//expected output
	return 0;
}
