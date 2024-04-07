#include "Map.h"


/*
 Видео: https://www.youtube.com/watch?v=oYyEqfi_4fo&ab_channel=selfedu
 */

/*
 Сайты: https://github.com/VladimirBalun/Algorithms/blob/master/DataStructures/BinaryTree.cpp
        https://github.com/MikhailPimenov/s21_containers/blob/develop/src/s21_tree.h
        https://github.com/Nikhil14/Map/blob/master/map.hpp
        https://github.com/pradykaushik/Map-Container/blob/master/Map.hpp
 */

int main()
{
    Map<int, std::string> map;
    try
    {
        map.At(1);
    }
    catch (const std::runtime_error& exception)
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
    
    [[maybe_unused]] auto depth = map.Depth();
    [[maybe_unused]] auto empty = map.Empty();
    [[maybe_unused]] auto contains1 = map.Contains(10);
    [[maybe_unused]] auto contains2 = map.Contains(11);
    [[maybe_unused]] auto count1 = map.Count(10);
    [[maybe_unused]] auto count2 = map.Count(11);
    [[maybe_unused]] auto find1 = map.Find(10);
    [[maybe_unused]] auto find2 = map.Find(11);
    std::cout << "Map" << std::endl;
    for (Map<int, std::string>::Iterator it = map.Begin(); it != map.End(); ++it)
    {
        // it->first = it->first; // ключ const!!!
        it->second = it->second;
        std::cout << "Key = " << it->first << ", Value = " << it->second << std::endl;
    }
    std::cout << std::endl;
    
    map.Swap(map);
    map = map;
    map = std::move(map);
    Map<int, std::string> map2(map);
    Map<int, std::string> map3;
    map3 = map;
    Map<int, std::string> map4(std::move(map2));
    Map<int, std::string> map5;
    map5 = std::move(map3);
    map4.Swap(map5);
    Map<int, std::string> map6 = {{7, "7"}, {8, "8"}, {9, "9"}};
    auto map7 = map6;
    [[maybe_unused]] auto compare1 = (map6 == map7);
    [[maybe_unused]] auto compare2 = (map6 != map7);
    map6[10] = "10";
    [[maybe_unused]] auto compare5 = (map6 == map7);
    [[maybe_unused]] auto compare6 = (map6 != map7);
    
    map.Erase(map.Begin());
    std::cout << "Map: erase begin" << std::endl;
    for (auto it = map.Begin(); it != map.End(); ++it)
    {
        std::cout << "Key = " << it->first << ", Value = " << it->second << std::endl;
    }
    std::cout << std::endl;
    map.Erase(--map.End());
    std::cout << "Map: erase end" << std::endl;
    for (auto it = map.Begin(); it != map.End(); ++it)
    {
        std::cout << "Key = " << it->first << ", Value = " << it->second << std::endl;
    }
    std::cout << std::endl;
    map.Erase(7);
    std::cout << "Map: erase 7" << std::endl;
    for (auto it = map.Begin(); it != map.End(); ++it)
    {
        std::cout << "Key = " << it->first << ", Value = " << it->second << std::endl;
    }
    std::cout << std::endl;
    map.Erase(map.Begin(), --map.End());
    std::cout << "Map: erase all before end" << std::endl;
    for (auto it = map.Begin(); it != map.End(); ++it)
    {
        std::cout << "Key = " << it->first << ", Value = " << it->second << std::endl;
    }
    std::cout << std::endl;
    map.Clear();
    std::cout << "Map: clear" << std::endl;
    for (auto it = map.Begin(); it != map.End(); ++it)
    {
        std::cout << "Key = " << it->first << ", Value = " << it->second << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "Numbers" << std::endl;
    Map<int, int> numbers = {{100, 100},
                            {50, 50},
                            {200, 200},
                            {10, 10},
                            {70, 70},
                            {60, 60},
                            {150, 150},
                            {140, 140},
                            {220, 220},
                            {230, 230}};
    
    std::cout << "Inorder: ";
    for (const auto& value: numbers.InorderTraversal())
        std::cout << value << ", ";
    std::cout << std::endl;
    
    std::cout << "Preorder: ";
    for (const auto& value: numbers.PreorderTraversal())
        std::cout << value << ", ";
    std::cout << std::endl;
    
    std::cout << "Postorder: ";
    for (const auto& value: numbers.PostorderTraversal())
        std::cout << value << ", ";
    std::cout << std::endl;
    
    for (auto it = numbers.Begin(); it != numbers.End(); ++it)
    {
        std::cout << "Key = " << it->first << ", Value = " << it->second << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Numbers: erase 60" << std::endl;
    numbers.Erase(60);
    for (auto it = numbers.Begin(); it != numbers.End(); ++it)
    {
        std::cout << "Key = " << it->first << ", Value = " << it->second << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Numbers: erase 150" << std::endl;
    numbers.Erase(150);
    for (auto it = numbers.Begin(); it != numbers.End(); ++it)
    {
        std::cout << "Key = " << it->first << ", Value = " << it->second << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Numbers: erase 200" << std::endl;
    numbers.Erase(200);
    for (auto it = numbers.Begin(); it != numbers.End(); ++it)
    {
        std::cout << "Key = " << it->first << ", Value = " << it->second << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Numbers: erase 230" << std::endl;
    numbers.Erase(230);
    for (auto it = numbers.Begin(); it != numbers.End(); ++it)
    {
        std::cout << "Key = " << it->first << ", Value = " << it->second << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Numbers: erase 100" << std::endl;
    numbers.Erase(100);
    for (auto it = numbers.Begin(); it != numbers.End(); ++it)
    {
        std::cout << "Key = " << it->first << ", Value = " << it->second << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Numbers: erase 10" << std::endl;
    numbers.Erase(10);
    for (auto it = numbers.Begin(); it != numbers.End(); ++it)
    {
        std::cout << "Key = " << it->first << ", Value = " << it->second << std::endl;
    }
    std::cout << std::endl;
    
    return 0;
}
