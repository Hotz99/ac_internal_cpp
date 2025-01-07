#pragma once

#include <windows.h>
#include "./ac_structs.h"
#include "./ac_state.h"

namespace ac_functions {
	// TODO can we rewrite these arg names or must they match the game source code ?
	void IntersectGeometry(const ac_structs::vec& from, ac_structs::vec& to);
}