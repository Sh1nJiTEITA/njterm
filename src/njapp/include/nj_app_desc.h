#ifndef NJ_APP_DESCRIPTORS_H
#define NJ_APP_DESCRIPTORS_H

#include "nj_app_context.h"
#include <cstdint>

namespace nj::app {

class App;

enum Layout : uint8_t { Basic = 0, AtlasPages = 1 };

void CreateBasicDescriptors(Context* ctx);
void CreateAtlasPagesDescriptors(Context* ctx);

} // namespace nj::app

#endif
