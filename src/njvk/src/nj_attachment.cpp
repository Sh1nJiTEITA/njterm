#include "nj_attachment.h"

namespace nj::ren {

Attachment::Attachment(Type type) : type{type} {}

// AttachmentData::AttachmentData(AttachmentH attachment)
//     : attachment{attachment}, image{attachment->CreateImage()},
//       imageView{attachment->CreateImageView()} {}
//
AttachmentData::AttachmentData(AttachmentH attachment, vk::SharedImage image,
                               vk::SharedImageView view)
    : attachment{attachment}, image{image}, imageView{view} {}

} // namespace nj::ren
