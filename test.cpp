#include <iostream>
#include "literal.h"


int main()
{
    String str = "~~~";
    str = "Zzz: " + str + '\n';
    str += Literal("Next line") + '\n';
    str += "0 " + Literal("1 ") + "2 " + "3 " + "3 " + "5 " + "6 " + "7 " + "8 " + "9 " + "10 " + '\n';
    str += '\n';

    std::cout << str.data();

    String str1 = "333";
    if(str1 > "444")std::cout << ">>>\n";
    else std::cout << "<<<\n";
    swap(str1, str);

    std::cout << str.data();
    return 0;
}
