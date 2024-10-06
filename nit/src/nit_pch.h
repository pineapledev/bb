#pragma once
#include <iostream>
#include <stdint.h>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <memory>
#include <assert.h>
#include <unordered_map>
#include <functional>
#include <cassert>
#include <array>
#include <random>
#include <chrono>
#include <filesystem>
#include <map>
#include <unordered_set>
#include <bitset>
#include <queue>
#include <set>
#include <imgui.h>
#include <yaml-cpp/yaml.h>

#include "nit/core/base.h"
#include "nit/core/event.h"
#include "nit/core/type.h"
#include "nit/core/pool.h"
#include "nit/core/asset.h"

#ifdef NIT_PLATFORM_LINUX
#include <linux/string.h>
#endif

#include "nit/math/math_common.h"
#include "nit/math/vector2.h"
#include "nit/math/vector3.h"
#include "nit/math/vector4.h"
#include "nit/math/matrix4.h"