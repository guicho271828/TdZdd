
#include <string>
#include <fstream>
#include <chrono>

#include <tdzdd/DdStructure.hpp>
#include <tdzdd/DdSpecOp.hpp>

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

void usage(const char* programName) {
    cerr << "Usage: " << programName << " n k " << endl;
    cerr << "This program generates dot files for a ZDD containing all subsets of a set contianing 1..n whose size is less than k." << endl;
}


void dump(DdStructure<2> &dd, string filename){
    ofstream os(filename);
    dd.dumpDot(os);
}

int main(int argc, char *argv[]) {

    int n;
    int k;

    // Check if the correct number of arguments is provided
    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    // Parse the integers
    try {
        n = stoi(argv[1]);
        k = stoi(argv[2]);

        // Output the parsed integers
        cout << "First integer: " << n << endl;
        cout << "Second integer: " << k << endl;
    } catch (const invalid_argument& e) {
        cerr << "Error: Invalid integer input." << endl;
        usage(argv[0]);
        return 1;
    } catch (const out_of_range& e) {
        cerr << "Error: Integer out of range." << endl;
        usage(argv[0]);
        return 1;
    }

    PropositionalState s1(n, true);
    PropositionalState s2(n, true);
    PropositionalState s3(n, true);
    PropositionalState s4(n, true);
    s1[0] = false;
    s2[1] = false;
    s3[n-1] = false;
    s4[n-2] = false;

    Subsets spec1(s1, k);
    Subsets spec2(s2, k);
    Subsets spec3(s3, k);
    Subsets spec4(s4, k);

    DdStructure<2> dd1(spec1);
    DdStructure<2> dd2(spec2);
    DdStructure<2> dd3(spec3);
    DdStructure<2> dd4(spec4);

    dd1.zddReduce();
    dd2.zddReduce();
    dd3.zddReduce();
    dd4.zddReduce();

    cout << "computing the union between dd1 and dd2" << endl;

    auto start12 = std::chrono::high_resolution_clock::now();
    auto spec12 = zddUnion(dd1,dd2);
    DdStructure<2> dd12(spec12);
    dd12.zddReduce();
    auto end12 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration12 = end12 - start12;

    cout << "computing the union between dd3 and dd2" << endl;

    auto start34 = std::chrono::high_resolution_clock::now();
    auto spec34 = zddUnion(dd3,dd4);
    DdStructure<2> dd34(spec34);
    dd34.zddReduce();
    auto end34 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration34 = end34 - start34;

    std::cout << "Runtime [sec]: "
	      << duration12.count()
	      << " vs. "
	      << duration34.count()
	      << " "
	      << duration12.count() / duration34.count()
	      << " speedup on "
	      << " k = " << k
	      << " n = " << n
	      << std::endl;


    dump(dd1, to_string(n) + "-" + to_string(k) + "-dd1.dot");
    dump(dd2, to_string(n) + "-" + to_string(k) + "-dd2.dot");
    dump(dd3, to_string(n) + "-" + to_string(k) + "-dd3.dot");
    dump(dd4, to_string(n) + "-" + to_string(k) + "-dd4.dot");
    dump(dd12, to_string(n) + "-" + to_string(k) + "-dd12.dot");
    dump(dd34, to_string(n) + "-" + to_string(k) + "-dd34.dot");

    return 0;
}
