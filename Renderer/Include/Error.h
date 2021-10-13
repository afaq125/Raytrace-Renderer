#pragma once

namespace Renderer
{
#define ASSERT(predicate, error) if (predicate) { throw std::runtime_error(error); }
}