#include "nj_image_view.h"

namespace nj::ren {

ImageView::ImageView(vk::UniqueImageView &&image) { handle = std::move(image); }

auto ImageView::HandleName() const noexcept -> std::string {
    return "ImageView";
}

} // namespace nj::ren
