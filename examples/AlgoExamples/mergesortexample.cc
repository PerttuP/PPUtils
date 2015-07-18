/* MergeSortExample
 * This is an example program to demonstrate the PPUtils::mergeSort algorithm.
 * 
 * Author: Perttu Paarlahti     perttu.paarlahti@gmail.com
 * Created: 18-June-2015
 */

#include <iostream>
#include <vector>
#include <functional>
#include "../../source/PPUtils/algo.hh"


void printVector(const std::vector<int>& v);

int main()
{
    std::vector<int> v;
    for (int i=0; i<10; ++i){
        v.push_back(i);
    }
    printVector(v);
    
    PPUtils::mergeSort(v.begin(), v.end(), std::greater<int>());
    printVector(v);
    
    PPUtils::mergeSort(v.begin(), v.end()); // sorted by std::less by default.
    printVector(v);
    
    PPUtils::mergeSort(v.begin(), v.end(), 
                       [](int a, int b)
                        {
                            if (a%2 == 0 && b%2 != 0) return true;
                            if (a%2 != 0 && b%2 == 0) return false;
                            return a<b;
                        });
    printVector(v);
    
    return 0;
}


void printVector(const std::vector<int>& v)
{
    std::cout << "V contains: ";
    for (auto it=v.begin(); it!=v.end()-1; ++it){
        std::cout << *it << ", ";
    }
    std::cout << v.back() << std::endl;
}


/* Expected output:
 * 
 * V contains: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
 * V contains: 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
 * V contains: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
 * V contains: 0, 2, 4, 6, 8, 1, 3, 5, 7, 9
 */
