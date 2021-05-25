#include <iostream>
#include "Text_Iter.h"
#include "TextNode.h"
#include "Text.cpp"
#include "TextIter.cpp"
#include "TextNode.cpp"

 using namespace std;

int main()
{

	char* temp = "Qwerty. \n Asd fgh zxc.\n";
	Text A(temp);
	TextIter B = A.Find("y");

	A.InsertLine("lol", B);

	cout << endl << A;
	return 0;

}
