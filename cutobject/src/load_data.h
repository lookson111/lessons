#pragma once

#include <filesystem>
#include "model.h"
namespace load {

std::optional<std::pair<model::Plain, model::PolygonMash>>
LoadData(const std::filesystem::path& path);

} // namespace load