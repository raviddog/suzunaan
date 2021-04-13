#define emptyContainer(ptr) for(auto it = ptr->begin(); it != ptr->end(); it++) { delete *it; }
#define emptyMap(ptr) for(auto it = ptr->begin(); it != ptr->end(); it++) { delete it->second; }