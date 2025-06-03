#include "aes_mode.h"
#include "aes_ctr.h"
#include "aes_gcm.h"

std::unique_ptr<AES_MODE> AES_MODE::create(const std::string& mode) {
    if (mode == "CTR") return std::make_unique<AES_CTR>();
    if (mode == "GCM") return std::make_unique<AES_GCM>();
    throw std::invalid_argument("Unsupported AES mode: " + mode);
}