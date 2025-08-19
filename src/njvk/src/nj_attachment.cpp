#include "nj_attachment.h"

namespace nj::ren {

Attachment::Attachment(Type type) : type{type} {}
AttachmentData::AttachmentData(AttachmentH attachment, ImageH image,
                               ImageViewH view)
    : attachment{attachment}, image{image}, imageView{view} {}

} // namespace nj::ren
