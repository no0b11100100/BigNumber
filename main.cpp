#include <iostream>
#include <chrono>
#include "bignumber.hpp"

using namespace std;
using namespace chrono;

namespace
{
size_t sub (BigInt::BigInt a, BigInt::BigInt b)
{
    std::string num_1 = "";
    std::string num_2 = "";

    for(const auto& bit : a.List()) num_1 += bit + '0';
    for(const auto& bit : b.List()) num_2 += bit + '0';

    return (std::strtol(num_1.c_str(), NULL, 2) - std::strtol(num_2.c_str(), NULL, 2));
}

size_t add (BigInt::BigInt a, BigInt::BigInt b)
{
    std::string num_1 = "";
    std::string num_2 = "";

    for(const auto& bit : a.List()) num_1 += bit + '0';
    for(const auto& bit : b.List()) num_2 += bit + '0';

    return (std::strtol(num_1.c_str(), NULL, 2) + std::strtol(num_2.c_str(), NULL, 2));
}

// https://www.cyberforum.ru/delphi-beginners/thread1023895.html
void mult(BigInt::BigInt a, BigInt::BigInt b)
{
    cout << "input " << b.SetedBits() << " " << b.List().size() << endl;
    if(b.is2Pow())
    { // test 4
        cout << "pow 2 " << (a<<b.toLow2Pow()-1).Decimal() << endl;
    } else if(*(b.List().rbegin()) == 0 && b.SetedBits()+1 == b.List().size())
    { // test 14 // ~60 microseconds
//        cout << "fisrt " << (a<<b.List().size()).Decimal() << endl;
        cout << "fisrt " << sub((a<<b.List().size()), add(a,a)) << endl;
    } else if(b.SetedBits() == b.List().size()) {
        cout << sub((a<<b.List().size()), a) << endl; // test 31 // ~50 microseconds
    } else if(b.SetedBits() > (b.List().size() / 2)) // 1 count is bigger than 0
    { // test 13 // ~80 microseconds
        auto temp = a;
        auto lastSub = a;
        a <<= b.List().size();
//        cout << a.Decimal() << endl;
        for(size_t i = 0; i < b.List().size(); ++i)
        {
            if(*(std::next(b.List().rbegin(), i)) == 0) a = sub(a, temp);
            temp <<= 1;
        }
        a = sub(a, lastSub);
        cout << "second " << a.Decimal() << endl;
    } else
    { // test 32 // ~70 microseconds
        auto temp = a;
        auto lastSub = a;
        for(size_t i = 0; i < b.List().size(); ++i)
        {
            if(*(std::next(b.List().rbegin(), i)) == 1) a = add(a, temp);
            temp <<= 1;
        }
        a = sub(a, lastSub);
        cout << "third " << a.Decimal() << endl;
    }
}
}


int main() {
//    BigInt::BigInt a(5);
//    BigInt::BigInt b(16);

//    auto start = high_resolution_clock::now();
////    auto res = a*b.List();
////    mult(a,b);
////    auto res = BigInt::BigInt::Pow(a, 4);
////    auto res = a*b;
////    b.isPerfectSquare();
//    auto stop = high_resolution_clock::now();
//    auto duration = duration_cast<microseconds>(stop - start);
//    cout << "\nmult duration " << duration.count() << endl << endl;
////    cout << res.Decimal() << endl;

    vector<bool> f;
    BigInt::BigInt c(f, BigInt::BASE::BINARY);


    return 0;
}

//template <class T, class U>
//struct same_type
//{
//    static const bool value = false;
//};

////specialization for types that are the same
//template <class T>
//struct same_type<T, T>
//{
//    static const bool value = true;
//};

////sample usage:
//template <class FirstContainer, class Type>
//bool containers_of_same_type(const FirstContainer&, const Type&)
//{
//    return same_type<
//        typename FirstContainer::value_type,
//        Type
//    >::value;
//}

//#include <vector>
//#include <list>
//#include <iostream>

//namespace
//{

//template <typename T>
//struct identity
//{
//    using type = T;
//};

//template<typename T>
//struct remove_all_pointers : std::conditional_t<
//    std::is_pointer_v<T>,
//    remove_all_pointers<
//        std::remove_pointer_t<T>
//    >,
//    identity<T>
//>
//{};

//template<typename T>
//using remove_all_t = typename remove_all_pointers<std::remove_cvref_t <T>>::type;

//template<typename T>
//constexpr bool is_integer()
//{
//    using Type = remove_all_t<T>;
//    return std::is_same_v<Type, int> ||
//            std::is_same_v<Type, unsigned> ||
//            std::is_same_v<Type, signed> ||
//            std::is_same_v<Type, short> ||
//            std::is_same_v<Type, unsigned short> ||
//            std::is_same_v<Type, signed short> ||
//            std::is_same_v<Type, long> ||
//            std::is_same_v<Type, signed long> ||
//            std::is_same_v<Type, unsigned long> ||
//            std::is_same_v<Type, long long> ||
//            std::is_same_v<Type, unsigned long long> ||
//            std::is_same_v<Type, signed long long> ||
//            std::is_same_v<Type, uint8_t> ||
//            std::is_same_v<Type, uint16_t> ||
//            std::is_same_v<Type, uint32_t> ||
//            std::is_same_v<Type, uint64_t> ||
//            std::is_same_v<Type, int8_t> ||
//            std::is_same_v<Type, int16_t> ||
//            std::is_same_v<Type, int32_t> ||
//            std::is_same_v<Type, int64_t> ||
//            std::is_same_v<Type, std::size_t> ||
//            std::is_same_v<Type, bool>;
//}

//template<template<class, class> class Container, class Type>
//constexpr bool is_allow_container()
//{
//    return std::is_same_v<Container, std::string> ||
//           (std::is_same_v<Container, std::vector<Type, std::allocator<Type>>> && is_integer<Type>() );
//}

//} // namespace


//class S
//{
//public:
//    template<typename Type,
//        class = typename std::enable_if_t< std::is_same_v<is_allow_container<Type, typename Type::value_type>(), true > > >
//    S(Type&&){}

//    template<typename Type,
//        class = typename std::enable_if_t< std::is_same_v<is_allow_container<Type, typename Type::value_type>(), true > > >
//    S(Type&){}

//    template<typename Type,
//        class = typename std::enable_if_t< std::is_same_v<is_allow_container<Type, typename Type::value_type>(), true > > >
//    S(const Type&){}

////    template<typename Type,
////        class = typename std::enable_if_t< std::is_same_v<int, typename Type::value_type> > >
//    template<typename Type,
//        class = typename std::enable_if_t< std::is_same_v<is_allow_container<Type, typename Type::value_type>(), true > > >
//    S(const Type&&){}
//};

//int main()
//{
//    std::vector<int>c;

//    S s(c);
//}
