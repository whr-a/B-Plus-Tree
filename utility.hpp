#ifndef SJTU_UTILITY_HPP
#define SJTU_UTILITY_HPP

#include <utility>

namespace sjtu {


template <class T>
T* upper_bound(T* first,T* last, T key) {
    last--;
    if(key>=*last)return last+1;
    while (first < last) {
        T* mid = first + (last - first) / 2;
        if (key < *mid) {
            last = mid;
        } else {
            first = mid + 1;
        }
    }
    return first;
}
template <class T>
T* lower_bound(T *first,T *last, T key) {
    last--;
    if(key>*last)return last+1;
    while (first < last) {
        T* mid = first + (last - first) / 2;
        if (*mid < key) {
            first = mid + 1;
        } else {
            last = mid;
        }
    }
    return first;
}

}

#endif
