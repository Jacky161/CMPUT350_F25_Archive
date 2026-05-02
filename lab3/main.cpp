#include "UniquePtr.h"
#include <iostream>

int main()
{
    UniquePtr<int> uniquePtr1 = makeUnique<int>(5); // copy is elided
    {
        // or alternatively
        auto uniquePtr2 = makeUnique<int>(5);
        // Both UniquePtr<int>s own a heap-allocated int storing 5.

        std::cout << "&uniquePtr1 = " << uniquePtr1.get() << '\n';
        std::cout << "&uniquePtr1 = " << uniquePtr2.get() << '\n';

        std::cout << "uniquePtr1 = " << *uniquePtr1 << '\n';
        std::cout << "uniquePtr1 = " << *uniquePtr2 << '\n';

        *uniquePtr1 = 11;
        *uniquePtr2 = 42;

        std::cout << "uniquePtr1 = " << *uniquePtr1 << '\n';
        std::cout << "uniquePtr1 = " << *uniquePtr2 << '\n';

        uniquePtr1 = std::move(uniquePtr2);
    } // uniquePtr2 is dead

    std::cout << "&uniquePtr1 = " << uniquePtr1.get() << '\n';
    std::cout << "uniquePtr1 = " << *uniquePtr1 << '\n';
}
