#include <unordered_map>
#include <map>
#include <vector>

namespace engine{
    template<class T, class U> void deleteUnorderedMap(std::unordered_map<T, U>*);
    template<class T, class U> void deleteUnorderedMultimap(std::unordered_multimap<T, U>*);
    template<class T, class U> void deleteMap(std::map<T, U>*);
    template<class T, class U> void deleteMultimap(std::multimap<T, U>*);
    template<class T> void deleteVector(std::vector<T>*);
}