#ifndef _ENGINE_HELPER
#define _ENGINE_HELPER

#define emptyContainer(ptr) for(auto it = ptr->begin(); it != ptr->end(); it++) { delete *it; }
#define emptyMap(ptr) for(auto it = ptr->begin(); it != ptr->end(); it++) { delete it->second; }


#endif