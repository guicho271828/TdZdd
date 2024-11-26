
#include <string>
#include <fstream>
#include <chrono>
#include <random>

#include <tdzdd/DdStructure.hpp>
#include <tdzdd/DdSpecOp.hpp>
// #include <tdzdd/DdEval.hpp>

using namespace tdzdd;
using namespace std;

using TupleSize = int;
using Level = int;
using PropositionalState = vector<bool>;


template<class T>
ostream& operator<<(ostream& os, const vector<T> &v){
    os << "[";
    bool first = true;
    for (const T &elem : v){
	if (first) {
	    first = false;
	} else {
	    os << ", ";
	}
	os << elem;
    }
    os << "]";
    return os;
}

class Subsets: public DdSpec<Subsets,TupleSize,2> {
    PropositionalState const s;
    int const k;

public:
    Subsets(PropositionalState s, int k)
	: s(s), k(k) {
	// cout << "instantiating spec" << endl;
	// cout << s << endl;
    }

    Level getRoot(TupleSize& size) const {
	// cout << "generating dd for s = " << s << ", k = " << k << endl;
        size = 0;
        return s.size();
    }

    // level -1 : terminal 1
    // level 0 : terminal 0
    Level getChild(TupleSize& size, Level plevel, int value) const {

	// cout << "  parent l = " << plevel << ", parent s = " << size << ", value = " << value << endl;

	if (s[plevel-1] == false){
	    // the proposition is false.
	    if (value == 1){
		// not possible to add.
		return 0;
	    } else {		// value = 0
		Level level = plevel - 1;
		if (level == 0) {
		    if (size > k) {
			// if it has more than k elements, prune.
			return 0;
		    } else {
			return -1;
		    }
		} else {
		    if (size > k) {
			// if it has more than k elements, prune.
			return 0;
		    } else {
			return level;
		    }
		}
	    }
	} else {		// s[plevel-1] == true
	    // since the proposition does exist, we can choose to add it or ignore it
	    size += value;
	    Level level = plevel - 1;
	    if (level == 0) {
		if (size > k) {
		    // if it has more than k elements, prune.
		    return 0;
		} else {
		    return -1;
		}
	    } else {
		if (size > k) {
		    // if it has more than k elements, prune.
		    return 0;
		} else {
		    return level;
		}
	    }
	}
    }
};



// class Size: public DdEval<E,int> {
//
// };

void dump(DdStructure<2> &dd, string filename){
    ofstream os(filename);
    dd.dumpDot(os);
}

chrono::duration<double> measure(int n, int k, int m, int from, int SAMPLE_SIZE){

    mt19937 gen(42); // Use the Mersenne Twister engine
    bernoulli_distribution dist(0.1); // 50% chance for true/false

    vector<PropositionalState> ss;
    vector<Subsets> specs;
    vector<DdStructure<2>> dds;
    ss.reserve(SAMPLE_SIZE);
    specs.reserve(SAMPLE_SIZE);
    dds.reserve(SAMPLE_SIZE);

    for (int i = 0; i < SAMPLE_SIZE; i++){
	PropositionalState s(n, true);
	for (int j = from ; j < from + m ; j++) {
	    s[j] = dist(gen);
	}
	// cout << s << endl;
	ss.push_back(s);
	Subsets spec(s,k);
	specs.push_back(spec);
	DdStructure<2> dd(spec);
	dd.zddReduce();
	dds.push_back(dd);
    }

    auto start = chrono::high_resolution_clock::now();

    DdStructure<2> dd = dds[0];
    for (int i = 1; i < SAMPLE_SIZE; i++){
	auto spec = zddUnion(dd, dds[i]);
	dd = DdStructure<2>(spec);
	dd.zddReduce();
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    dump(dd,
	 to_string(n) + "-" +
	 to_string(k) + "-" +
	 to_string(m) + "-" +
	 to_string(SAMPLE_SIZE) + "-" +
	 (from == 0 ? "bot" : "top") + ".dot");
    return duration;
}


void usage(const char* programName) {
    cerr << "Usage: " << programName << " n k m" << endl;
    cerr << "This program generates dot files for a ZDD containing all subsets of a set contianing 1..n whose size is less than k." << endl;
}

int main(int argc, char *argv[]) {

    int n;
    int k;
    int m;
    int SAMPLE_SIZE;

    // Check if the correct number of arguments is provided
    if (argc != 5) {
        usage(argv[0]);
        return 1;
    }

    // Parse the integers
    try {
        n = stoi(argv[1]);
        k = stoi(argv[2]);
        m = stoi(argv[3]);
        SAMPLE_SIZE = stoi(argv[4]);

        // Output the parsed integers
        cout << "First integer: " << n << endl;
        cout << "Second integer: " << k << endl;
        cout << "Third integer: " << m << endl;
        cout << "sample size: " << SAMPLE_SIZE << endl;

	if (n < m){
	    throw invalid_argument("m should be smaller than n");
	}
    } catch (const invalid_argument& e) {
        cerr << "Error: Invalid integer input." << endl;
        usage(argv[0]);
        return 1;
    } catch (const out_of_range& e) {
        cerr << "Error: Integer out of range." << endl;
        usage(argv[0]);
        return 1;
    }

    chrono::duration<double> duration1 = measure(n, k, m, 0, SAMPLE_SIZE);
    chrono::duration<double> duration2 = measure(n, k, m, n-m, SAMPLE_SIZE);

    cout << "Runtime [sec]: "
	 << duration1.count()
	 << " (change near bottom) vs. "
	 << duration2.count()
	 << " (change near top). " << endl
	 << "bottom/top = "
	 << duration1.count() / duration2.count()
	 << " on "
	 << " k = " << k
	 << " n = " << n
	 << endl;

    return 0;
}

