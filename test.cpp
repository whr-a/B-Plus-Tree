#include "utility.hpp"
#include <iostream>
int main ()
{
    int a[]={11,12};
    std::cout<<sjtu::upper_bound(a,a+2,12)-a;
}