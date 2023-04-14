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
template<class T1, class T2>
class pair {
public:
	T1 first;
	T2 second;
	constexpr pair() : first(), second() {}
	pair(const pair &other) = default;
	pair(pair &&other) = default;
	pair(const T1 &x, const T2 &y) : first(x), second(y) {}
	template<class U1, class U2>
	pair(U1 &&x, U2 &&y) : first(x), second(y) {}
	template<class U1, class U2>
	pair(const pair<U1, U2> &other) : first(other.first), second(other.second) {}
	template<class U1, class U2>
	pair(pair<U1, U2> &&other) : first(other.first), second(other.second) {}
};
}

#endif
