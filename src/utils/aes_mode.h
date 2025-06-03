#ifndef AES_MODE_H
#define AES_MODE_H

#include <span>
#include <string_view>
#include <string>
#include <memory>

/**
* Interface to implement the concept of mode
*/
class AES_MODE {
public:
	virtual ~AES_MODE() = default;
	virtual std::string encrypt(std::string_view plain_text) = 0;
	virtual std::string decrypt(std::string_view cipher_text) = 0;

	// Factory Method
	static std::unique_ptr<AES_MODE> create(const std::string& mode);
};

#endif // !AES_MODE_H