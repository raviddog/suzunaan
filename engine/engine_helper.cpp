#include "engine_helper.h"

namespace engine {
    template <class T, class U> void deleteUnorderedMap(std::unordered_map<T, U> *ptr) {
        for(auto it = ptr->begin(); it != ptr->end(); it++) {
            delete *it;
            // it = ptr->erase(it);
        }
        delete ptr;
    }

    template <class T, class U> void deleteUnorderedMultiMap(std::unordered_multimap<T, U> *ptr) {
        for(auto it = ptr->begin(); it != ptr->end(); it++) {
            delete *it;
            // it = ptr->erase(it);
        }
        delete ptr;
    }

    template <class T, class U> void deletemap(std::map<T, U> *ptr) {
        for(auto it = ptr->begin(); it != ptr->end(); it++) {
            delete *it;
            // it = ptr->erase(it);
        }
        delete ptr;
    }

    template <class T, class U> void deleteMultimap(std::multimap<T, U> *ptr) {
        for(auto it = ptr->begin(); it != ptr->end(); it++) {
            delete *it;
            // it = ptr->erase(it);
        }
        delete ptr;
    }

    template <class T> void deleteVector(std::vector<T> *ptr) {
        for(auto it = ptr->begin(); it != ptr->end(); it++) {
            delete *it;
        }
        delete ptr;
    }
}