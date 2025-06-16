#pragma once
#ifndef NJ_ATTACHMENT_H
#define NJ_ATTACHMENT_H

#include "nj_device.h"

namespace nj::ren {

// clang-format off
//


//! Abstract class represents representation of vulkan render unit
//! Abstracts way of creating render pass attachments
//! To handle needed attachments it is needed to override methods
//! of this class
class Attachment : public std::enable_shared_from_this<Attachment> {
  public:
    enum Type { 
        Color,
        Depth,
        Stencil
    };

    Attachment(Type type);

    //! Attachemnt type must be specified during overriding stage
    const Type type;

    //! Returns description of attachment for render pass creation
    virtual auto Description() -> vk::AttachmentDescription = 0;

    //! Returns reference of attachment for render pass creation
    //! @param idx Index of attachment amoung others
    virtual auto Reference(uint32_t idx) -> vk::AttachmentReference = 0;

    //! Returns dependency for render pass creation
    virtual auto Dependency() -> vk::SubpassDependency = 0;

    //! Creates image pack (image + image view) for this attachment 
    //! @param frame_idx Frame index create to. Can be usefull if using with
    //! swapchain, where images are indexed
    virtual auto CreateData(uint32_t frame_idx, DeviceH device) -> class AttachmentData = 0;
};
using AttachmentH = std::shared_ptr<Attachment>;

//! Class for storing attachemnt variables like image and image view
//! This class is a good solution if multiple images & image views are
//! needed. This case can happen if each frame (on flight) must have 
//! own pack of vk-handles
struct AttachmentData {
    AttachmentData(AttachmentH attachment, vk::SharedImage image, vk::SharedImageView view);
    const vk::SharedImage image;
    const vk::SharedImageView imageView;
    const AttachmentH attachment;
};
using AttachmentDataH = std::shared_ptr<AttachmentData>;

// clang-format on

} // namespace nj::ren

#endif
