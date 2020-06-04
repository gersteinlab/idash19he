#include <fstream>
#include <iostream>
#include <string>

using namespace std;

int main( int argc, char * argv[] )
{
    auto filename = string( argv[1] );

    ifstream fin(filename);
    string word;
    unsigned zero=0, one=0, two=0, others=0;
    while ( fin >> word )
    {
        if ( word == "0" ) zero++;
        else if ( word == "1" ) one++;
        else if ( word == "2" ) two++;
        else others++;
    }
    cout << " 0: " << zero << "\n";
    cout << " 1: " << one << "\n";
    cout << " 2: " << two << "\n";
    cout << " others: " << others << "\n";
}
