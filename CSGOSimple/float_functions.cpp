// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "float_functions.h"
#include <limits>

namespace float_functions
{
	bool is_equal(const double& x, const double& y)
	{
		return fabs(x - y) < std::numeric_limits<double>::epsilon();
	}

	bool is_equal(const float& x, const float& y)
	{
		return fabsf(x - y) < std::numeric_limits<float>::epsilon();
	}
}
