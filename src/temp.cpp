#include <iostream>
#include <fstream>
using namespace std;

int main()
{
	fstream file("C://Workspace//2021-12//1.txt");
	char *line = new char[1000];
	file.getline(line, 1000);
	cout << line << endl;

	return 0;
}