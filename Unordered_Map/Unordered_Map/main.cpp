#include "Unordered_Map.h"

/*
 Сайты: https://github.com/VladimirBalun/Algorithms/blob/master/DataStructures/HashTableWithSeparateChaining.cpp
        https://github.com/carlaoutput/hashtable/blob/master/hashtable.hpp
 */

#include <unordered_map>
int main()
{
    std::unordered_map<int, std::string> map1;
    map1 = map1;
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
    auto value = map[0] = "0";
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
    for (auto it = map.Begin(); it != map.End(); ++it)
    {
        std::cout << "Key = " << it->data.first << ", Value = " << it->data.second << std::endl;
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
    
    map.Erase(----map.End());
    std::cout << "Map: erase1" << std::endl;
    for (auto it = map.Begin(); it != map.End(); ++it)
    {
        std::cout << "Key = " << it->data.first << ", Value = " << it->data.second << std::endl;
    }
    std::cout << std::endl;
    map.Erase(------map.End());
    std::cout << "Map: erase1" << std::endl;
    for (auto it = map.Begin(); it != map.End(); ++it)
    {
        std::cout << "Key = " << it->data.first << ", Value = " << it->data.second << std::endl;
    }
    std::cout << std::endl;
    map.Erase(map.Begin(), --map.End());
    map.Clear();
    return 0;
}