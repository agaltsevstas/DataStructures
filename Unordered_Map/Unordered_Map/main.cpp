#include "Unordered_Map.h"

/*
 Сайты: https://github.com/VladimirBalun/Algorithms/blob/master/DataStructures/HashTableWithSeparateChaining.cpp
        https://github.com/carlaoutput/hashtable/blob/master/hashtable.hpp
 */


int main()
{
    Unordered_Map<int, std::string> map;
    try
    {
        map.At(1);
    }
    catch (const std::runtime_error& exception)
    {
        std::cout << exception.what() << std::endl;
    }
    try
    {
        map.Bucket_Size(1);
    }
    catch (const std::out_of_range& exception)
    {
        std::cout << exception.what() << std::endl;
    }
    [[maybe_unused]] auto& value = map[0] = "0";
    map.Insert({10, "10"});
    map.Insert({12, "12"});
    map.Insert({7, "7"});
    map.Insert({14, "14"});
    map.Insert({5, "5"});
    map.Insert({6, "6"});
    
    [[maybe_unused]] auto size = map.Size();
    [[maybe_unused]] auto buckets_count = map.Buckets_Count();
    [[maybe_unused]] auto bucket = map.Bucket(5);
    [[maybe_unused]] auto bucket_size = map.Bucket_Size(5);
    [[maybe_unused]] auto load_factor = map.Load_Factor();
    map.Max_Load_Factor(map.Max_Load_Factor());
    [[maybe_unused]] auto empty = map.Empty();
    [[maybe_unused]] auto contains1 = map.Contains(10);
    [[maybe_unused]] auto contains2 = map.Contains(11);
    [[maybe_unused]] auto count1 = map.Count(10);
    [[maybe_unused]] auto count2 = map.Count(11);
    [[maybe_unused]] auto find1 = map.Find(10);
    [[maybe_unused]] auto find2 = map.Find(11);
    std::cout << "Unordered_Map" << std::endl;
    for (Unordered_Map<int, std::string>::Iterator it = map.Begin(); it != map.End(); ++it)
    {
        // it->data.first = it->data.first; // ключ const!!!
        it->second = it->second;
        std::cout << "Key = " << it->first << ", Value = " << it->second << std::endl;
    }
    std::cout << std::endl;
    
    map.Swap(map);
    map = map;
    map = std::move(map);
    Unordered_Map<int, std::string> map2(map);
    Unordered_Map<int, std::string> map3;
    map3 = map;
    Unordered_Map<int, std::string> map4(std::move(map2));
    Unordered_Map<int, std::string> map5;
    map5 = std::move(map3);
    map4.Swap(map5);
    Unordered_Map<int, std::string> map6 = {{7, "7"}, {8, "8"}, {9, "9"}};
    auto map7 = map6;
    [[maybe_unused]] auto compare1 = (map6 == map7);
    [[maybe_unused]] auto compare2 = (map6 != map7);
    map6[10] = "10";
    [[maybe_unused]] auto compare5 = (map6 == map7);
    [[maybe_unused]] auto compare6 = (map6 != map7);
    
    std::cout << "Map: erase1" << std::endl;
    map.Erase(----map.End());
    for (auto it = map.Begin(); it != map.End(); ++it)
    {
        std::cout << "Key = " << it->first << ", Value = " << it->second << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Map: erase2" << std::endl;
    map.Erase(------map.End());
    for (Unordered_Map<int, std::string>::Iterator it = map.Begin(); it != map.End(); ++it)
    {
        std::cout << "Key = " << it->first << ", Value = " << it->second << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Map: erase 0" << std::endl;
    map.Erase(0);
    for (auto it = map.Begin(); it != map.End(); ++it)
    {
        std::cout << "Key = " << it->first << ", Value = " << it->second << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Map: erase all before end" << std::endl;
    map.Erase(map.Begin(), --map.End());
    for (auto it = map.Begin(); it != map.End(); ++it)
    {
        std::cout << "Key = " << it->first << ", Value = " << it->second << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Map: clear" << std::endl;
    map.Clear();
    for (auto it = map.Begin(); it != map.End(); ++it)
    {
        std::cout << "Key = " << it->first << ", Value = " << it->second << std::endl;
    }
    std::cout << std::endl;
    return 0;
}
