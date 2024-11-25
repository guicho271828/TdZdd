
#include <string>
#include <sstream>
#include <fstream>

#include <tdzdd/DdStructure.hpp>

using namespace tdzdd;
using namespace std;

using State = int;
using Level = int;

class Subsets: public DdSpec<Subsets,State,2> {
    int const n;
    int const k;

public:
    Subsets(int n, int k)
            : n(n), k(k) {
    }

    Level getRoot(State& state) const {
        state = 0;
        return n;
    }

    // level -1 : terminal 1
    // level 0 : terminal 0
    Level getChild(State& state, Level level, int value) const {
	cout << "  parent l = " << level << ", parent s = " << state << ", value = " << value << endl;
        state += value;
	level--;

        if (level == 0) {
	    // terminal 1 if there are exactly k elements; 0 if less.
	    return (state == k) ? -1 : 0;
	} else if (state > k) {
	    // if it has more than k elements, prune.
	    return 0;
	} else if (state + level < k) {
	    // if there would not be enough remaining elements, it would not each k elements in the end, so prune.
	    return 0;
	} else {
	    return level;
	}
    }
};

void usage(const char* programName) {
    cerr << "Usage: " << programName << " n k " << endl;
    cerr << "This program generates dot files for a ZDD containing all subsets of a set contianing 1..n whose size is less than k." << endl;
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

    cout << "generating spec" << endl;
    Subsets spec(n, k);
    ofstream os1(to_string(n) + "-" + to_string(k) + "-spec.dot");
    spec.dumpDot(os1);

    cout << "generating dd" << endl;
    DdStructure<2> dd(spec);
    ofstream os2(to_string(n) + "-" + to_string(k) + "-dd-before-reduced.dot");
    dd.dumpDot(os2);

    cout << "reducing dd" << endl;
    dd.zddReduce();
    ofstream os3(to_string(n) + "-" + to_string(k) + "-dd-after-reduced.dot");
    dd.dumpDot(os3);

    return 0;
}
