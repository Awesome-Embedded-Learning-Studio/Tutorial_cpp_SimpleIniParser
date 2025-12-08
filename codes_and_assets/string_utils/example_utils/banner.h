/**
 * @file banner.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Banner Public Interfaces
 * @version 0.1
 * @date 2025-11-17
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include <string>
#include <string_view>

namespace cxx_utils {
namespace example_utils {
	/**
	 * @brief   BannerAlignment indicated the alignment of
	 *          string position
	 *
	 */
	enum class BannerAlignment {
		LEFT,
		CENTER,
		RIGHT
	};

	/**
	 * @brief   Create A String Sources for banner, you can use it to
	 *          Redirect to any IO
	 *
	 * @param view
	 * @param alignment
	 * @param banner_ch
	 * @return std::string
	 */
	[[nodiscard("Don't Ignore the Return Value of"
	            " Banner String, else why shell you create it :)")]]
	std::string banner_src(std::string_view view,
	                       const BannerAlignment alignment = BannerAlignment::CENTER,
	                       const char banner_ch = '=');
}
}