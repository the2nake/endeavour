#include <vector>
#include <string>
#include <iostream>

void splitString(std::vector<std::string> &container, std::string s, std::string delim)
{
    container.clear();
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delim)) != std::string::npos)
    {
        token = s.substr(0, pos);
        std::cout << token << std::endl;
        s.erase(0, pos + delim.length());
    }
    token = s.substr(0, pos);
    std::cout << token << std::endl;
    s.erase(0, pos + delim.length());
}

int main()
{
    std::vector<std::string> container;
    splitString(container, "asdfoiasnfmaifnasdopfasdf", "o");
    for (std::string str : container)
    {
        std::cout << str << std::endl;
    }
    return container.size();
}