
// prime numbers and hashes used by perlin noise

int primes[] = {11807, 11813, 11821, 11827, 11831, 11833, 11839, 11863, 11867, 11887, 11897, 11903, 11909, 11923,
11927, 11933, 11939, 11941, 11953, 11959, 11969, 11971, 11981, 11987, 12007, 12011, 12037, 12041,
12043, 12049, 12071, 12073, 12097, 12101, 12107, 12109, 12113, 12119, 12143, 12149, 12157, 12161,
12163, 12197, 12203, 12211, 12227, 12239, 12241, 12251, 12253, 12263, 12269, 12277, 12281, 12289,
12301, 12323, 12329, 12343, 12347, 12373, 12377, 12379, 12391, 12401, 12409, 12413, 12421, 12433,
12437, 12451, 12457, 12473, 12479, 12487, 12491, 12497, 12503, 12511, 12517, 12527, 12539, 12541,
12547, 12553, 12569, 12577, 12583, 12589, 12601, 12611, 12613, 12619, 12637, 12641, 12647, 12653,
12659, 12671, 12689, 12697, 12703, 12713, 12721, 12739, 12743, 12757, 12763, 12781, 12791, 12799,
12809, 12821, 12823, 12829, 12841, 12853, 12889, 12893, 12899, 12907, 12911, 12917, 12919, 12923,
12941, 12953, 12959, 12967, 12973, 12979, 12983, 13001, 13003, 13007, 13009, 13033, 13037, 13043,
13049, 13063, 13093, 13099, 13103, 13109, 13121, 13127, 13147, 13151, 13159, 13163, 13171, 13177,
13183, 13187, 13217, 13219, 13229, 13241, 13249, 13259, 13267, 13291, 13297, 13309, 13313, 13327,
13331, 13337, 13339, 13367, 13381, 13397, 13399, 13411, 13417, 13421, 13441, 13451, 13457, 13463,
13469, 13477, 13487, 13499, 13513, 13523, 13537, 13553, 13567, 13577, 13591, 13597, 13613, 13619,
13627, 13633, 13649, 13669, 13679, 13681, 13687, 13691, 13693, 13697, 13709, 13711, 13721, 13723,
13729, 13751, 13757, 13759, 13763, 13781, 13789, 13799, 13807, 13829, 13831, 13841, 13859, 13873,
13877, 13879, 13883, 13901, 13903, 13907, 13913, 13921, 13931, 13933, 13963, 13967, 13997, 13999,
14009, 14011, 14029, 14033, 14051, 14057, 14071, 14081, 14083, 14087, 14107, 14143, 14149, 14153,
14159, 14173, 14177, 14197, 14207, 14221, 14243, 14249, 14251, 14281, 14293, 14303, 14321, 14323,
18719,  18731,  18743,  18749,  18757,  18773,  18787,  18793,  18797,  18803,  18839,  18859,
 18869,  18899,  18911,  18913,  18917,  18919,  18947,  18959,  18973,  18979,  19001,  19009,
 19013,  19031,  19037,  19051,  19069,  19073,  19079,  19081,  19087,  19121,  19139,  19141,
 19157,  19163,  19181,  19183,  19207,  19211,  19213,  19219,  19231,  19237,  19249,  19259,
 19267,  19273,  19289,  19301,  19309,  19319,  19333,  19373,  19379,  19381,  19387,  19391,
 19403,  19417,  19421,  19423,  19427,  19429,  19433,  19441,  19447,  19457,  19463,  19469,
 19471,  19477,  19483,  19489,  19501,  19507,  19531,  19541,  19543,  19553,  19559,  19571,
 19577,  19583,  19597,  19603,  19609,  19661,  19681,  19687,  19697,  19699,  19709,  19717,
 19727,  19739,  19751,  19753,  19759,  19763,  19777,  19793,  19801,  19813,  19819,  19841,
 19843,  19853,  19861,  19867,  19889,  19891,  19913,  19919,  19927,  19937,  19949,  19961,
 19963,  19973,  19979,  19991,  19993,  19997,  20011,  20021,  20023,  20029,  20047,  20051,
 20063,  20071,  20089,  20101,  20107,  20113,  20117,  20123,  20129,  20143,  20147,  20149,
 20161,  20173,  20177,  20183,  20201,  20219,  20231,  20233,  20249,  20261,  20269,  20287,
 20297,  20323,  20327,  20333,  20341,  20347,  20353,  20357,  20359,  20369,  20389,  20393,
 20399,  20407,  20411,  20431,  20441,  20443,  20477,  20479,  20483,  20507,  20509,  20521,
 20533,  20543,  20549,  20551,  20563,  20593,  20599,  20611,  20627,  20639,  20641,  20663,
 20681,  20693,  20707,  20717,  20719,  20731,  20743,  20747,  20749,  20753,  20759,  20771,
 20773,  20789,  20807,  20809,  20849,  20857,  20873,  20879,  20887,  20897,  20899,  20903,
 20921,  20929,  20939,  20947,  20959,  20963,  20981,  20983,  21001,  21011,  21013,  21017,
 21019,  21023,  21031,  21059,  21061,  21067,  21089,  21101,  21107,  21121,  21139,  21143,
 21149,  21157,  21163,  21169,  21179,  21187,  21191,  21193,  21211,  21221,  21227,  21247,
 21269,  21277,  21283,  21313,  21317,  21319,  21323,  21341,  21347,  21377,  21379,  21383,
 21391,  21397,  21401,  21407,  21419,  21433,  21467,  21481,  21487,  21491,  21493,  21499,
 21503,  21517,  21521,  21523,  21529,  21557,  21559,  21563,  21569,  21577,  21587,  21589,
 21599,  21601,  21611,  21613,  21617,  21647,  21649,  21661,  21673,  21683,  21701,  21713,
 21727,  21737,  21739,  21751,  21757,  21767,  21773,  21787,  21799,  21803,  21817,  21821,
 21839,  21841,  21851,  21859,  21863,  21871,  21881,  21893,  21911,  21929,  21937,  21943,
 21961,  21977,  21991,  21997,  22003,  22013,  22027,  22031,  22037,  22039,  22051,  22063,
 22067,  22073,  22079,  22091,  22093,  22109,  22111,  22123,  22129,  22133,  22147,  22153,
 22157,  22159,  22171,  22189,  22193,  22229,  22247,  22259,  22271,  22273,  22277,  22279,
 22283,  22291,  22303,  22307,  22343,  22349,  22367,  22369,  22381,  22391,  22397,  22409,
 22433,  22441,  22447,  22453,  22469,  22481,  22483,  22501,  22511,  22531,  22541,  22543,
 22549,  22567,  22571,  22573,  22613,  22619,  22621,  22637,  22639,  22643,  22651,  22669,
 22679,  22691,  22697,  22699,  22709,  22717,  22721,  22727,  22739,  22741,  22751,  22769,
 22777,  22783,  22787,  22807,  22811,  22817,  22853,  22859,  22861,  22871,  22877,  22901,
 22907,  22921,  22937,  22943,  22961,  22963,  22973,  22993,  23003,  23011,  23017,  23021,
 23027,  23029,  23039,  23041,  23053,  23057,  23059,  23063,  23071,  23081,  23087,  23099,
 23117,  23131,  23143,  23159,  23167,  23173,  23189,  23197,  23201,  23203,  23209,  23227,
 23251,  23269,  23279,  23291,  23293,  23297,  23311,  23321,  23327,  23333,  23339,  23357,
 23369,  23371,  23399,  23417,  23431,  23447,  23459,  23473,  23497,  23509,  23531,  23537,
 23539,  23549,  23557,  23561,  23563,  23567,  23581,  23593,  23599,  23603,  23609,  23623,
 23627,  23629,  23633,  23663,  23669,  23671,  23677,  23687,  23689,  23719,  23741,  23743,
 23747,  23753,  23761,  23767,  23773,  23789,  23801,  23813,  23819,  23827,  23831,  23833,
 23857,  23869,  23873,  23879,  23887,  23893,  23899,  23909,  23911,  23917,  23929,  23957,
 23971,  23977,  23981,  23993,  24001,  24007,  24019,  24023,  24029,  24043,  24049,  24061,
 24071,  24077,  24083,  24091,  24097,  24103,  24107,  24109,  24113,  24121,  24133,  24137,
 24151,  24169,  24179,  24181,  24197,  24203,  24223,  24229,  24239,  24247,  24251,  24281,
 24317,  24329,  24337,  24359,  24371,  24373,  24379,  24391,  24407,  24413,  24419,  24421,
 24439,  24443,  24469,  24473,  24481,  24499,  24509,  24517,  24527,  24533,  24547,  24551,
 24571,  24593,  24611,  24623,  24631,  24659,  24671,  24677,  24683,  24691,  24697,  24709,
 24733,  24749,  24763,  24767,  24781,  24793,  24799,  24809,  24821,  24841,  24847,  24851,
 24859,  24877,  24889,  24907,  24917,  24919,  24923,  24943,  24953,  24967,  24971,  24977,
 24979,  24989,  25013,  25031,  25033,  25037,  25057,  25073,  25087,  25097,  25111,  25117,
 25121,  25127,  25147,  25153,  25163,  25169,  25171,  25183,  25189,  25219,  25229,  25237,
 25243,  25247,  25253,  25261,  25301,  25303,  25307,  25309,  25321,  25339,  25343,  25349,
 25357,  25367,  25373,  25391,  25409,  25411,  25423,  25439,  25447,  25453,  25457,  25463,
 25469,  25471,  25523,  25537,  25541,  25561,  25577,  25579,  25583,  25589,  25601,  25603,
 25609,  25621,  25633,  25639,  25643,  25657,  25667,  25673,  25679,  25693,  25703,  25717,
 25733,  25741,  25747,  25759,  25763,  25771,  25793,  25799,  25801,  25819,  25841,  25847,
 25849,  25867,  25873,  25889,  25903,  25913,  25919,  25931,  25933,  25939,  25943,  25951,
 25969,  25981,  25997,  25999,  26003,  26017,  26021,  26029,  26041,  26053,  26083,  26099,
 26107,  26111,  26113,  26119,  26141,  26153,  26161,  26171,  26177,  26183,  26189,  26203,
 26209,  26227,  26237,  26249,  26251,  26261,  26263,  26267,  26293,  26297,  26309,  26317,
 26321,  26339,  26347,  26357,  26371,  26387,  26393,  26399,  26407,  26417,  26423,  26431,
 26437,  26449,  26459,  26479,  26489,  26497,  26501,  26513,  26539,  26557,  26561,  26573,
 26591,  26597,  26627,  26633,  26641,  26647,  26669,  26681,  26683,  26687,  26693,  26699,
 26701,  26711,  26713,  26717,  26723,  26729,  26731,  26737,  26759,  26777,  26783,  26801,
 26813,  26821,  26833,  26839,  26849,  26861,  26863,  26879,  26881,  26891,  26893,  26903,
 26921,  26927,  26947,  26951,  26953,  26959,  26981,  26987,  26993,  27011,  27017,  27031,
 27043,  27059,  27061,  27067,  27073,  27077,  27091,  27103,  27107,  27109,  27127,  27143,
 27179,  27191,  27197,  27211,  27239,  27241,  27253,  27259,  27271,  27277,  27281,  27283,
 27299,  27329,  27337,  27361,  27367,  27397,  27407,  27409,  27427,  27431,  27437,  27449,
 27457,  27479,  27481,  27487,  27509,  27527,  27529,  27539,  27541,  27551,  27581,  27583,
 27611,  27617,  27631,  27647,  27653,  27673,  27689,  27691,  27697,  27701,  27733,  27737,
 27739,  27743,  27749,  27751,  27763,  27767,  27773,  27779,  27791,  27793,  27799,  27803,
 27809,  27817,  27823,  27827,  27847,  27851,  27883,  27893,  27901,  27917,  27919,  27941,
 27943,  27947,  27953,  27961,  27967,  27983,  27997,  28001,  28019,  28027,  28031,  28051,
 28057,  28069,  28081,  28087,  28097,  28099,  28109,  28111,  28123,  28151,  28163,  28181,
 28183,  28201,  28211,  28219,  28229,  28277,  28279,  28283,  28289,  28297,  28307,  28309,
 28319,  28349,  28351,  28387,  28393,  28403,  28409,  28411,  28429,  28433,  28439,  28447,
 28463,  28477,  28493,  28499,  28513,  28517,  28537,  28541,  28547,  28549,  28559,  28571,
 28573,  28579,  28591,  28597,  28603,  28607,  28619,  28621,  28627,  28631,  28643,  28649,
 28657,  28661,  28663,  28669,  28687,  28697,  28703,  28711,  28723,  28729,  28751,  28753,
 28759,  28771,  28789,  28793,  28807,  28813,  28817,  28837,  28843,  28859,  28867,  28871,
 28879,  28901,  28909,  28921,  28927,  28933,  28949,  28961,  28979,  29009,  29017,  29021,
 29023,  29027,  29033,  29059,  29063,  29077,  29101,  29123,  29129,  29131,  29137,  29147,
 29153,  29167,  29173,  29179,  29191,  29201,  29207,  29209,  29221,  29231,  29243,  29251,
 29269,  29287,  29297,  29303,  29311,  29327,  29333,  29339,  29347,  29363,  29383,  29387,
 29389,  29399,  29401,  29411,  29423,  29429,  29437,  29443,  29453,  29473,  29483,  29501,
 29527,  29531,  29537,  29567,  29569,  29573,  29581,  29587,  29599,  29611,  29629,  29633,
 29641,  29663,  29669,  29671,  29683,  29717,  29723,  29741,  29753,  29759,  29761,  29789,
 29803,  29819,  29833,  29837,  29851,  29863,  29867,  29873,  29879,  29881,  29917,  29921,
 29927,  29947,  29959,  29983,  29989,  30011,  30013,  30029,  30047,  30059,  30071,  30089,
 30091,  30097,  30103,  30109,  30113,  30119,  30133,  30137,  30139,  30161,  30169,  30181,
 30187,  30197,  30203,  30211,  30223,  30241,  30253,  30259,  30269,  30271,  30293,  30307,
 30313,  30319,  30323,  30341,  30347,  30367,  30389,  30391,  30403,  30427,  30431,  30449,
 30467,  30469,  30491,  30493,  30497,  30509,  30517,  30529,  30539,  30553,  30557,  30559,
 30577,  30593,  30631,  30637,  30643,  30649,  30661,  30671,  30677,  30689,  30697,  30703,
 30707,  30713,  30727,  30757,  30763,  30773,  30781,  30803,  30809,  30817,  30829,  30839,
 30841,  30851,  30853,  30859,  30869,  30871,  30881,  30893,  30911,  30931,  30937,  30941,
 30949,  30971,  30977,  30983,  31013,  31019,  31033,  31039,  31051,  31063,  31069,  31079,
 31081,  31091,  31121,  31123,  31139,  31147,  31151,  31153,  31159,  31177,  31181,  31183,
 31189,  31193,  31219,  31223,  31231,  31237,  31247,  31249,  31253,  31259,  31267,  31271,
 31277,  31307,  31319,  31321,  31327,  31333,  31337,  31357,  31379,  31387,  31391,  31393,
 31397,  31469,  31477,  31481,  31489,  31511,  31513,  31517,  31531,  31541,  31543,  31547,
 31567,  31573,  31583,  31601,  31607,  31627,  31643,  31649,  31657,  31663,  31667,  31687,
 31699,  31721,  31723,  31727,  31729,  31741,  31751,  31769,  31771,  31793,  31799,  31817,
 31847,  31849,  31859,  31873,  31883,  31891,  31907,  31957,  31963,  31973,  31981,  31991,
 32003,  32009,  32027,  32029,  32051,  32057,  32059,  32063,  32069,  32077,  32083,  32089,
 32099,  32117,  32119,  32141,  32143,  32159,  32173,  32183,  32189,  32191,  32203,  32213,
 32233,  32237,  32251,  32257,  32261,  32297,  32299,  32303,  32309,  32321,  32323,  32327,
 32341,  32353,  32359,  32363,  32369,  32371,  32377,  32381,  32401,  32411,  32413,  32423,
 32429,  32441,  32443,  32467,  32479,  32491,  32497,  32503,  32507,  32531,  32533,  32537,
 32561,  32563,  32569,  32573,  32579,  32587,  32603,  32609,  32611,  32621,  32633,  32647,
 32653,  32687,  32693,  32707,  32713,  32717,  32719,  32749,  32771,  32779,  32783,  32789,
 32797,  32801,  32803,  32831,  32833,  32839,  32843,  32869,  32887,  32909,  32911,  32917,
 32933,  32939,  32941,  32957,  32969,  32971,  32983,  32987,  32993,  32999,  33013,  33023,
 33029,  33037,  33049,  33053,  33071,  33073,  33083,  33091,  33107,  33113,  33119,  33149,
 33151,  33161,  33179,  33181,  33191,  33199,  33203,  33211,  33223,  33247,  33287,  33289,
 33301,  33311,  33317,  33329,  33331,  33343,  33347,  33349,  33353,  33359,  33377,  33391,
 33403,  33409,  33413,  33427,  33457,  33461,  33469,  33479,  33487,  33493,  33503,  33521,
 33529,  33533,  33547,  33563,  33569,  33577,  33581,  33587,  33589,  33599,  33601,  33613,
 33617,  33619,  33623,  33629,  33637,  33641,  33647,  33679,  33703,  33713,  33721,  33739,
 33749,  33751,  33757,  33767,  33769,  33773,  33791,  33797,  33809,  33811,  33827,  33829,
 33851,  33857,  33863,  33871,  33889,  33893,  33911,  33923,  33931,  33937,  33941,  33961,
 33967,  33997,  34019,  34031,  34033,  34039,  34057,  34061,  34123,  34127,  34129,  34141,
 34147,  34157,  34159,  34171,  34183,  34211,  34213,  34217,  34231,  34253,  34259,  34261,
 34267,  34273,  34283,  34297,  34301,  34303,  34313,  34319,  34327,  34337,  34351,  34361,
 34367,  34369,  34381,  34403,  34421,  34429,  34439,  34457,  34469,  34471,  34483,  34487,
 34499,  34501,  34511,  34513,  34519,  34537,  34543,  34549,  34583,  34589,  34591,  34603,
 34607,  34613,  34631,  34649,  34651,  34667,  34673,  34679,  34687,  34693,  34703,  34721,
 34729,  34739,  34747,  34757,  34759,  34763,  34781,  34807,  34819,  34841,  34843,  34847,
 34849,  34871,  34877,  34883,  34897,  34913,  34919,  34939,  34949,  34961,  34963,  34981,
 35023,  35027,  35051,  35053,  35059,  35069,  35081,  35083,  35089,  35099,  35107,  35111,
 35117,  35129,  35141,  35149,  35153,  35159,  35171,  35201,  35221,  35227,  35251,  35257};

int Prims2[] = {1376312627,1376312629,1376312657,1376312687,1376312689,1376312753,1376312783,1376312789,1376312813,1376312857,1376312879,1376312881,1376312897,1376312909,1376312929,
1376312953,1376312989,1376313007,1376313011,1376313017,1376313053,1376313067,1376313101,1376313131,1376313137,1376313143,1376313151,1376313167,1376313179,1376313199,1376313209,
1376313223,1376313229,1376313277,1376313313,1376313343,1376313347,1376313377,1376313409,1376313481,1376313493,1376313517,1376313563,1376313577,1376313611,1376313613,1376313619,
1376313623,1376313647,1376313649,1376313691,1376313703,1376313713,1376313733,1376313751,1376313779,1376313787,1376313793,1376313823,1376313839,1376313907,1376313941,1376313959,
1376313977,1376314007,1376314013,1376314087,1376314139,1376314141,1376314189,1376314211,1376314231,1376314249,1376314253,1376314259,1376314297,1376314307,1376314327,1376314337,
1376314361,1376314483,1376314487,1376314501,1376314561,1376314579,1376314613,1376314619,1376314627,1376314633,1376314637,1376314649,1376314663,1376314673,1376314703,1376314729,
1376314739,1376314763,1376314777,1376314783,1376314811,1376314873,1376314903,1376314949,1376314991,1376314993,1376315041,1376315071,1376315081,1376315093,1376315099,1376315107,
1376315111,1376315119,1376315197,1376315201,1376315207,1376315221,1376315267,1376315279,1376315299,1376315309,1376315357,1376315387,1376315411,1376315417,1376315431,1376315449,
1376315459,1376315467,1376315477,1376315503,1376315509,1376315519,1376315527,1376315531,1376315581,1376315657,1376315683,1376315747,1376315753,1376315761,1376315777,1376315797,
1376315827,1376315861,1376315879,1376315887,1376315893,1376315909,1376315917,1376315947,1376315953,1376315989,1376316041,1376316197,1376316223,1376316239,1376316241,1376316247,
1376316257,1376316301,1376316317,1376316341,1376316367,1376316371,1376316391,1376316397,1376316407,1376316419,1376316457,1376316467,1376316533,1376316553,1376316563,1376316569,
1376316581,1376316593,1376316629,1376316637,1376316649,1376316671,1376316673,1376316677,1376316679,1376316691,1376316727,1376316751,1376316761,1376316763,1376316769,1376316779,
1376316791,1376316841,1376316853,1376316899,1376316901,1376316911,1376316917,1376316943,1376316989,1376317051,1376317087,1376317123,1376317153,1376317183,1376317219,1376317289,
1376317333,1376317339,1376317361,1376317387,1376317391,1376317427,1376317477,1376317483,1376317511,1376317517,1376317519,1376317549,1376317559,1376317609,1376317667,1376317693,
1376317711,1376317717,1376317729,1376317751,1376317753,1376317759,1376317793,1376317799,1376317843,1376317871,1376317879,1376317913,1376317919,1376317927,1376317937,1376317973,
1376318017,1376318023,1376318063,1376318101,1376318117,1376318123,1376318131,1376318189,1376318197,1376318227,1376318231,1376318233,1376318261,1376318263,1376318287,1376318323,
1376318387,1376318393,1376318401,1376318483,1376318491,1376318497,1376318527,1376318549,1376318557,1376318579,1376318621,1376318639,1376318641,1376318683,1376318707,1376318717,
1376318729,1376318731,1376318737,1376318747,1376318753,1376318767,1376318777,1376318807,1376318809,1376318821,1376318831,1376318837,1376318897,1376318921,1376318953,1376318959,
1376318981,1376318989,1376319041,1376319047,1376319053,1376319079,1376319089,1376319101,1376319107,1376319187,1376319221,1376319253,1376319257,1376319331,1376319341,1376319349,
1376319403,1376319409,1376319419,1376319433,1376319437,1376319467,1376319479,1376319499,1376319509,1376319559,1376319569,1376319577,1376319631,1376319641,1376319647,1376319661,
1376319713,1376319719,1376319757,1376319761,1376319767,1376319781,1376319799,1376319811,1376319823,1376319827,1376319899,1376319907,1376319913,40015373,40015403,40015421,40015429,40015441,40015483,40015489,40015519,40015543,40015559,40015571,40015579,40015601,40015609,40015627,40015643,
40015649,40015667,40015669,40015681,40015687,40015709,40015721,40015769,40015771,40015783,40015793,40015799,40015849,40015867,40015873,40015879,
40015903,40015961,40015973,40015979,40015999,40016051,40016077,40016083,40016087,40016149,40016153,40016161,40016173,40016213,40016231,40016237,
40016243,40016281,40016309,40016311,40016321,40016323,40016329,40016357,40016393,40016429,40016441,40016461,40016497,40016507,40016563,40016569,
40016591,40016593,40016611,40016651,40016687,40016719,40016731,40016761,40016773,40016777,40016833,40016857,40016863,40016887,40016891,40016903,
40016923,40016953,40016959,40016971,40017007,40017011,40017101,40017113,40017119,40017121,40017137,40017161,40017169,40017209,40017221,40017223,
40017227,40017283,40017293,40017301,40017317,40017347,40017407,40017409,40017413,40017421,40017449,40017451,40017479,40017503,40017511,40017533,
40017557,40017577,40017583,40017611,40017617,40017619,40017623,40017661,40017671,40017701,40017713,40017721,40017751,40017773,40017793,40017829,
40017839,40017853,40017863,40017877,40017889,40017899,40017947,40017959,40018001,40018031,40018037,40018039,40018061,40018067,40018093,40018109,
40018157,40018169,40018189,40018193,40018211,40018213,40018217,40018219,40018243,40018247,40018249,40018259,40018261,40018283,40018327,40018361,
40018373,40018393,40018403,40018409,40018411,40018421,40018439,40018441,40018453,40018457,40018499,40018507,40018513,40018519,40018543,40018567,
40018577,40018607,40018609,40018621,40018639,40018661,40018669,40018679,40018691,40018703,40018717,40018721,40018729,40018733,40018739,40018751,
40018757,40018859,40018907,40018931,40018943,40018961,40018981,40018987,40018991,40019017,40019041,40019123,40019131,40019141,40019179,40019219,
40019227,40019249,40019251,40019261,40019279,40019299,40019303,40019323,40019327,40019341,40019359,40019407,40019417,40019431,40019443,40019467,
40019489,40019509,40019521,40019527,40019537,40019557,40019579,40019591,40019621,40019633,40019659,40019671,40019687,40019689,40019699,40019743,
40019753,40019767,40019773,40019801,40019809,40019821,40019831,40019849,40019851,40019869,40019891,40019897,40019911,40019933,40019939,40019953,
40019957,40019981,40020011,40020017,40020037,40020041,40020067,40020083,40020089,40020103,40020119,40020131,40020151,40020157,40020173,40020181,
40020187,40020191,40020221,40020257,40020271,40020307,40020317,40020371,40020389,40020433,40020443,40020451,40020457,40020481,40020503,40020521,
40020523,40020527,40020599,40020611,40020637,40020649,40020677,40020731,40020733,40020749,40020751,40020763,40020769,40020787,40020809,40020817,
40020823,40020833,40020853,40020913,40020949,40020971,40020991,40020997,40021001,40021021,40021061,40021117,40021123,40021147,40021151,40021159,
40021169,40021171,40021181,40021199,40021211,40021231,40021243,40021249,40021259,40021271,40021273,40021313,40021337,40021339,40021361,40021369,
40021381,40021409,40021427,40021459,40021463,40021469,40021477,40021481,40021483,40021489,40021507,40021517,40021547,40021561,40021573,40021591,
40021603,40021607,40021613,40021627,40021661,40021703,40021727,40021747,40021759,40021763,40021789,40021819,40021841,40021843,40021867,40021903,
40021913,40021931,40021937,40021951,40021963,40021973,40021997,40022011,40022041,40022053,40022069,40022071,40022083,40022107,40022137,40022149,
40022153,40022219,40022251,40022263,40022287,40022299,40022309,40022329,40022347,40022357,40022377,40022413,40022429,40022441,40022447,40022459,
40022467,40022483,40022501,40022503,40022509,40022537,40022557,40022611,40022621,40022657,40022659,40022693,40022707,40022711,40022753,40022819,
40022821,40022833,40022837,40022839,40022861,40022869,40022881,40022887,40022903,40022923,40022951,40022959,40022977,40022993,40022999,40023007,
40023041,40023089,40023097,40023119,40023127,40023133,40023143,40023149,40023157,40023167,40023173,40023209,40023223,40023229,40023311,40023323,
40023341,40023353,40023359,40023397,40023407,40023413,40023419,40023433,40023437,40023463,40023499,40023551,40023569,40023587,40023617,40023619,
40023629,40023631,40023689,40023701,40023733,40023757,40023769,40023773,40023787,40023791,40023793,40023811,40023839,40023871,40023901,40023911,
40023917,40023941,40023943,40023947,40023989,40024009,40024057,40024073,40024079,40024081,40024087,40024091,40024097,40024109,40024133,40024151,
40024181,40024183,40024247,40024267,40024297,40024339,40024351,40024363,40024373,40024399,40024421,40024423,40024433,40024447,40024463,40024489,
40024493,40024499,40024519,40024541,40024547,40024549,40024559,40024591,40024603,40024619,40024631,40024637,40024657,40024693,40024703,40024729,
40024757,40024763,40024771,40024781,40024877,40024883,40024903,40024921,40024969,40024981,40024993,40025023,40025033,40025071,40025087,40025099,
40025101,40025119,40025147,40025171,40025179,40025201,40025203,40025239,40025261,40025263,40025281,40025299,40025311,40025351,40025353,40025399 };



static float hashpntf[768] = {0.536902, 0.020915, 0.501445, 0.216316, 0.517036, 0.822466, 0.965315,
0.377313, 0.678764, 0.744545, 0.097731, 0.396357, 0.247202, 0.520897,
0.613396, 0.542124, 0.146813, 0.255489, 0.810868, 0.638641, 0.980742,
0.292316, 0.357948, 0.114382, 0.861377, 0.629634, 0.722530, 0.714103,
0.048549, 0.075668, 0.564920, 0.162026, 0.054466, 0.411738, 0.156897,
0.887657, 0.599368, 0.074249, 0.170277, 0.225799, 0.393154, 0.301348,
0.057434, 0.293849, 0.442745, 0.150002, 0.398732, 0.184582, 0.915200,
0.630984, 0.974040, 0.117228, 0.795520, 0.763238, 0.158982, 0.616211,
0.250825, 0.906539, 0.316874, 0.676205, 0.234720, 0.667673, 0.792225,
0.273671, 0.119363, 0.199131, 0.856716, 0.828554, 0.900718, 0.705960,
0.635923, 0.989433, 0.027261, 0.283507, 0.113426, 0.388115, 0.900176,
0.637741, 0.438802, 0.715490, 0.043692, 0.202640, 0.378325, 0.450325,
0.471832, 0.147803, 0.906899, 0.524178, 0.784981, 0.051483, 0.893369,
0.596895, 0.275635, 0.391483, 0.844673, 0.103061, 0.257322, 0.708390,
0.504091, 0.199517, 0.660339, 0.376071, 0.038880, 0.531293, 0.216116,
0.138672, 0.907737, 0.807994, 0.659582, 0.915264, 0.449075, 0.627128,
0.480173, 0.380942, 0.018843, 0.211808, 0.569701, 0.082294, 0.689488, 
0.573060, 0.593859, 0.216080, 0.373159, 0.108117, 0.595539, 0.021768, 
0.380297, 0.948125, 0.377833, 0.319699, 0.315249, 0.972805, 0.792270, 
0.445396, 0.845323, 0.372186, 0.096147, 0.689405, 0.423958, 0.055675, 
0.117940, 0.328456, 0.605808, 0.631768, 0.372170, 0.213723, 0.032700, 
0.447257, 0.440661, 0.728488, 0.299853, 0.148599, 0.649212, 0.498381,
0.049921, 0.496112, 0.607142, 0.562595, 0.990246, 0.739659, 0.108633, 
0.978156, 0.209814, 0.258436, 0.876021, 0.309260, 0.600673, 0.713597, 
0.576967, 0.641402, 0.853930, 0.029173, 0.418111, 0.581593, 0.008394, 
0.589904, 0.661574, 0.979326, 0.275724, 0.111109, 0.440472, 0.120839, 
0.521602, 0.648308, 0.284575, 0.204501, 0.153286, 0.822444, 0.300786, 
0.303906, 0.364717, 0.209038, 0.916831, 0.900245, 0.600685, 0.890002, 
0.581660, 0.431154, 0.705569, 0.551250, 0.417075, 0.403749, 0.696652, 
0.292652, 0.911372, 0.690922, 0.323718, 0.036773, 0.258976, 0.274265, 
0.225076, 0.628965, 0.351644, 0.065158, 0.080340, 0.467271, 0.130643,
0.385914, 0.919315, 0.253821, 0.966163, 0.017439, 0.392610, 0.478792, 
0.978185, 0.072691, 0.982009, 0.097987, 0.731533, 0.401233, 0.107570, 
0.349587, 0.479122, 0.700598, 0.481751, 0.788429, 0.706864, 0.120086, 
0.562691, 0.981797, 0.001223, 0.192120, 0.451543, 0.173092, 0.108960,
0.549594, 0.587892, 0.657534, 0.396365, 0.125153, 0.666420, 0.385823, 
0.890916, 0.436729, 0.128114, 0.369598, 0.759096, 0.044677, 0.904752, 
0.088052, 0.621148, 0.005047, 0.452331, 0.162032, 0.494238, 0.523349, 
0.741829, 0.698450, 0.452316, 0.563487, 0.819776, 0.492160, 0.004210, 
0.647158, 0.551475, 0.362995, 0.177937, 0.814722, 0.727729, 0.867126, 
0.997157, 0.108149, 0.085726, 0.796024, 0.665075, 0.362462, 0.323124,
0.043718, 0.042357, 0.315030, 0.328954, 0.870845, 0.683186, 0.467922, 
0.514894, 0.809971, 0.631979, 0.176571, 0.366320, 0.850621, 0.505555, 
0.749551, 0.750830, 0.401714, 0.481216, 0.438393, 0.508832, 0.867971, 
0.654581, 0.058204, 0.566454, 0.084124, 0.548539, 0.902690, 0.779571, 
0.562058, 0.048082, 0.863109, 0.079290, 0.713559, 0.783496, 0.265266, 
0.672089, 0.786939, 0.143048, 0.086196, 0.876129, 0.408708, 0.229312, 
0.629995, 0.206665, 0.207308, 0.710079, 0.341704, 0.264921, 0.028748, 
0.629222, 0.470173, 0.726228, 0.125243, 0.328249, 0.794187, 0.741340, 
0.489895, 0.189396, 0.724654, 0.092841, 0.039809, 0.860126, 0.247701, 
0.655331, 0.964121, 0.672536, 0.044522, 0.690567, 0.837238, 0.631520, 
0.953734, 0.352484, 0.289026, 0.034152, 0.852575, 0.098454, 0.795529, 
0.452181, 0.826159, 0.186993, 0.820725, 0.440328, 0.922137, 0.704592,
0.915437, 0.738183, 0.733461, 0.193798, 0.929213, 0.161390, 0.318547,
0.888751, 0.430968, 0.740837, 0.193544, 0.872253, 0.563074, 0.274598, 
0.347805, 0.666176, 0.449831, 0.800991, 0.588727, 0.052296, 0.714761, 
0.420620, 0.570325, 0.057550, 0.210888, 0.407312, 0.662848, 0.924382, 
0.895958, 0.775198, 0.688605, 0.025721, 0.301913, 0.791408, 0.500602, 
0.831984, 0.828509, 0.642093, 0.494174, 0.525880, 0.446365, 0.440063, 
0.763114, 0.630358, 0.223943, 0.333806, 0.906033, 0.498306, 0.241278,
0.427640, 0.772683, 0.198082, 0.225379, 0.503894, 0.436599, 0.016503, 
0.803725, 0.189878, 0.291095, 0.499114, 0.151573, 0.079031, 0.904618, 
0.708535, 0.273900, 0.067419, 0.317124, 0.936499, 0.716511, 0.543845, 
0.939909, 0.826574, 0.715090, 0.154864, 0.750150, 0.845808, 0.648108, 
0.556564, 0.644757, 0.140873, 0.799167, 0.632989, 0.444245, 0.471978, 
0.435910, 0.359793, 0.216241, 0.007633, 0.337236, 0.857863, 0.380247, 
0.092517, 0.799973, 0.919000, 0.296798, 0.096989, 0.854831, 0.165369, 
0.568475, 0.216855, 0.020457, 0.835511, 0.538039, 0.999742, 0.620226, 
0.244053, 0.060399, 0.323007, 0.294874, 0.988899, 0.384919, 0.735655, 
0.773428, 0.549776, 0.292882, 0.660611, 0.593507, 0.621118, 0.175269, 
0.682119, 0.794493, 0.868197, 0.632150, 0.807823, 0.509656, 0.482035, 
0.001780, 0.259126, 0.358002, 0.280263, 0.192985, 0.290367, 0.208111, 
0.917633, 0.114422, 0.925491, 0.981110, 0.255570, 0.974862, 0.016629,
0.552599, 0.575741, 0.612978, 0.615965, 0.803615, 0.772334, 0.089745, 
0.838812, 0.634542, 0.113709, 0.755832, 0.577589, 0.667489, 0.529834,
0.325660, 0.817597, 0.316557, 0.335093, 0.737363, 0.260951, 0.737073, 
0.049540, 0.735541, 0.988891, 0.299116, 0.147695, 0.417271, 0.940811, 
0.524160, 0.857968, 0.176403, 0.244835, 0.485759, 0.033353, 0.280319, 
0.750688, 0.755809, 0.924208, 0.095956, 0.962504, 0.275584, 0.173715,
0.942716, 0.706721, 0.078464, 0.576716, 0.804667, 0.559249, 0.900611, 
0.646904, 0.432111, 0.927885, 0.383277, 0.269973, 0.114244, 0.574867, 
0.150703, 0.241855, 0.272871, 0.199950, 0.079719, 0.868566, 0.962833, 
0.789122, 0.320025, 0.905554, 0.234876, 0.991356, 0.061913, 0.732911, 
0.785960, 0.874074, 0.069035, 0.658632, 0.309901, 0.023676, 0.791603, 
0.764661, 0.661278, 0.319583, 0.829650, 0.117091, 0.903124, 0.982098, 
0.161631, 0.193576, 0.670428, 0.857390, 0.003760, 0.572578, 0.222162, 
0.114551, 0.420118, 0.530404, 0.470682, 0.525527, 0.764281, 0.040596, 
0.443275, 0.501124, 0.816161, 0.417467, 0.332172, 0.447565, 0.614591, 
0.559246, 0.805295, 0.226342, 0.155065, 0.714630, 0.160925, 0.760001, 
0.453456, 0.093869, 0.406092, 0.264801, 0.720370, 0.743388, 0.373269, 
0.403098, 0.911923, 0.897249, 0.147038, 0.753037, 0.516093, 0.739257, 
0.175018, 0.045768, 0.735857, 0.801330, 0.927708, 0.240977, 0.591870,
0.921831, 0.540733, 0.149100, 0.423152, 0.806876, 0.397081, 0.061100, 
0.811630, 0.044899, 0.460915, 0.961202, 0.822098, 0.971524, 0.867608, 
0.773604, 0.226616, 0.686286, 0.926972, 0.411613, 0.267873, 0.081937, 
0.226124, 0.295664, 0.374594, 0.533240, 0.237876, 0.669629, 0.599083, 
0.513081, 0.878719, 0.201577, 0.721296, 0.495038, 0.079760, 0.965959,
0.233090, 0.052496, 0.714748, 0.887844, 0.308724, 0.972885, 0.723337,
0.453089, 0.914474, 0.704063, 0.823198, 0.834769, 0.906561, 0.919600,
0.100601, 0.307564, 0.901977, 0.468879, 0.265376, 0.885188, 0.683875,
0.868623, 0.081032, 0.466835, 0.199087, 0.663437, 0.812241, 0.311337,
0.821361, 0.356628, 0.898054, 0.160781, 0.222539, 0.714889, 0.490287,
0.984915, 0.951755, 0.964097, 0.641795, 0.815472, 0.852732, 0.862074,
0.051108, 0.440139, 0.323207, 0.517171, 0.562984, 0.115295, 0.743103,
0.977914, 0.337596, 0.440694, 0.535879, 0.959427, 0.351427, 0.704361,
0.010826, 0.131162, 0.577080, 0.349572, 0.774892, 0.425796, 0.072697,
0.500001, 0.267322, 0.909654, 0.206176, 0.223987, 0.937698, 0.323423,
0.117501, 0.490308, 0.474372, 0.689943, 0.168671, 0.719417, 0.188928,
0.330464, 0.265273, 0.446271, 0.171933, 0.176133, 0.474616, 0.140182,
0.114246, 0.905043, 0.713870, 0.555261, 0.951333};

unsigned char hash[512]= {
0xA2,0xA0,0x19,0x3B,0xF8,0xEB,0xAA,0xEE,0xF3,0x1C,0x67,0x28,0x1D,0xED,0x0,0xDE,0x95,0x2E,0xDC,0x3F,0x3A,0x82,0x35,0x4D,0x6C,0xBA,0x36,0xD0,0xF6,0xC,0x79,0x32,0xD1,0x59,0xF4,0x8,0x8B,0x63,0x89,0x2F,0xB8,0xB4,0x97,0x83,0xF2,0x8F,0x18,0xC7,0x51,0x14,0x65,0x87,0x48,0x20,0x42,0xA8,0x80,0xB5,0x40,0x13,0xB2,0x22,0x7E,0x57,
0xBC,0x7F,0x6B,0x9D,0x86,0x4C,0xC8,0xDB,0x7C,0xD5,0x25,0x4E,0x5A,0x55,0x74,0x50,0xCD,0xB3,0x7A,0xBB,0xC3,0xCB,0xB6,0xE2,0xE4,0xEC,0xFD,0x98,0xB,0x96,0xD3,0x9E,0x5C,0xA1,0x64,0xF1,0x81,0x61,0xE1,0xC4,0x24,0x72,0x49,0x8C,0x90,0x4B,0x84,0x34,0x38,0xAB,0x78,0xCA,0x1F,0x1,0xD7,0x93,0x11,0xC1,0x58,0xA9,0x31,0xF9,0x44,0x6D,
0xBF,0x33,0x9C,0x5F,0x9,0x94,0xA3,0x85,0x6,0xC6,0x9A,0x1E,0x7B,0x46,0x15,0x30,0x27,0x2B,0x1B,0x71,0x3C,0x5B,0xD6,0x6F,0x62,0xAC,0x4F,0xC2,0xC0,0xE,0xB1,0x23,0xA7,0xDF,0x47,0xB0,0x77,0x69,0x5,0xE9,0xE6,0xE7,0x76,0x73,0xF,0xFE,0x6E,0x9B,0x56,0xEF,0x12,0xA5,0x37,0xFC,0xAE,0xD9,0x3,0x8E,0xDD,0x10,0xB9,0xCE,0xC9,0x8D,
0xDA,0x2A,0xBD,0x68,0x17,0x9F,0xBE,0xD4,0xA,0xCC,0xD2,0xE8,0x43,0x3D,0x70,0xB7,0x2,0x7D,0x99,0xD8,0xD,0x60,0x8A,0x4,0x2C,0x3E,0x92,0xE5,0xAF,0x53,0x7,0xE0,0x29,0xA6,0xC5,0xE3,0xF5,0xF7,0x4A,0x41,0x26,0x6A,0x16,0x5E,0x52,0x2D,0x21,0xAD,0xF0,0x91,0xFF,0xEA,0x54,0xFA,0x66,0x1A,0x45,0x39,0xCF,0x75,0xA4,0x88,0xFB,0x5D,
0xA2,0xA0,0x19,0x3B,0xF8,0xEB,0xAA,0xEE,0xF3,0x1C,0x67,0x28,0x1D,0xED,0x0,0xDE,0x95,0x2E,0xDC,0x3F,0x3A,0x82,0x35,0x4D,0x6C,0xBA,0x36,0xD0,0xF6,0xC,0x79,0x32,0xD1,0x59,0xF4,0x8,0x8B,0x63,0x89,0x2F,0xB8,0xB4,0x97,0x83,0xF2,0x8F,0x18,0xC7,0x51,0x14,0x65,0x87,0x48,0x20,0x42,0xA8,0x80,0xB5,0x40,0x13,0xB2,0x22,0x7E,0x57,
0xBC,0x7F,0x6B,0x9D,0x86,0x4C,0xC8,0xDB,0x7C,0xD5,0x25,0x4E,0x5A,0x55,0x74,0x50,0xCD,0xB3,0x7A,0xBB,0xC3,0xCB,0xB6,0xE2,0xE4,0xEC,0xFD,0x98,0xB,0x96,0xD3,0x9E,0x5C,0xA1,0x64,0xF1,0x81,0x61,0xE1,0xC4,0x24,0x72,0x49,0x8C,0x90,0x4B,0x84,0x34,0x38,0xAB,0x78,0xCA,0x1F,0x1,0xD7,0x93,0x11,0xC1,0x58,0xA9,0x31,0xF9,0x44,0x6D,
0xBF,0x33,0x9C,0x5F,0x9,0x94,0xA3,0x85,0x6,0xC6,0x9A,0x1E,0x7B,0x46,0x15,0x30,0x27,0x2B,0x1B,0x71,0x3C,0x5B,0xD6,0x6F,0x62,0xAC,0x4F,0xC2,0xC0,0xE,0xB1,0x23,0xA7,0xDF,0x47,0xB0,0x77,0x69,0x5,0xE9,0xE6,0xE7,0x76,0x73,0xF,0xFE,0x6E,0x9B,0x56,0xEF,0x12,0xA5,0x37,0xFC,0xAE,0xD9,0x3,0x8E,0xDD,0x10,0xB9,0xCE,0xC9,0x8D,
0xDA,0x2A,0xBD,0x68,0x17,0x9F,0xBE,0xD4,0xA,0xCC,0xD2,0xE8,0x43,0x3D,0x70,0xB7,0x2,0x7D,0x99,0xD8,0xD,0x60,0x8A,0x4,0x2C,0x3E,0x92,0xE5,0xAF,0x53,0x7,0xE0,0x29,0xA6,0xC5,0xE3,0xF5,0xF7,0x4A,0x41,0x26,0x6A,0x16,0x5E,0x52,0x2D,0x21,0xAD,0xF0,0x91,0xFF,0xEA,0x54,0xFA,0x66,0x1A,0x45,0x39,0xCF,0x75,0xA4,0x88,0xFB,0x5D,
};

