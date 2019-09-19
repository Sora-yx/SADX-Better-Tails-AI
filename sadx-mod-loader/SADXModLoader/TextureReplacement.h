#pragma once
#include <string>
#include <vector>

struct TexPackEntry
{
	uint32_t global_index;
	std::string name;
	uint32_t width, height;
};

namespace texpack
{
	/**
	 * \brief Parses custom texture index.
	 * \param path A valid path to a texture pack directory containing index.txt
	 * \param out A vector to populate.
	 * \return \c true on success.
	 */
	bool parse_index(const std::string& path, std::vector<TexPackEntry>& out);

	/**
	 * \brief Initializes function hooks for texture replacement.
	 */
	void init();
}
