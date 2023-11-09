#ifndef GENERAL_HPP
#define GENERAL_HPP

#include <iostream>
#include <iomanip>
#include <openssl/md5.h>

#pragma warning(disable : 4996)

struct RenderableData;
struct RenderableObject;

std::string GenerateMD5(const std::string& data)
{
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), digest);

    std::ostringstream oss;

    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);

    return oss.str();
}

static std::string LoadFile(const std::string& path)
{
    std::ifstream shaderFile(path);
    std::stringstream shaderStream;

    if (!shaderFile.is_open())
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << path << std::endl;
        return "";
    }

    shaderStream << shaderFile.rdbuf();
    shaderFile.close();

    return shaderStream.str();
}

struct IPackagable
{
	void* Package()
	{
		void* out = this;

		return out;
	}
};

template<typename T>
struct NameIDTag : public IPackagable
{
    std::string name;
    std::string hash;
    T object;

    bool IsMatch(const std::string& hash) const
    {
        return this->hash == hash;
    }

    bool IsMatch(const std::string& name, const T& object) const
    {
        return this->name == name && this->object == object;
    }

    bool operator==(const NameIDTag<T>& other) const
    {
        return hash == other.hash && object == other.object;
    }

    static NameIDTag Register(const std::string& name, const T& object)
    {
        NameIDTag out;

        out.name = name;
        out.hash = GenerateMD5(name);  // Assuming GenerateMD5 is defined elsewhere.
        out.object = object;

        return out;
    }
};

#endif // !GENERAL_HPP