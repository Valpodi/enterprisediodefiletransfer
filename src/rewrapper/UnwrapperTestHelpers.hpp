// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef REWRAPPER_UNWRAPPERTESTHELPERS_HPP
#define REWRAPPER_UNWRAPPERTESTHELPERS_HPP

#include <filesystem>
#include <BytesBuffer.hpp>
#include <sstream>
#include "CloakedDagger.hpp"

struct TestPacket {
  std::array<char, CloakedDagger::headerSize()> header;
  BytesBuffer message;
};

TestPacket createTestWrappedString(const std::string& payload, const std::array<char, 8>& mask={0x12, 0x34, 0x56, 0x78, static_cast<char>(0x9a), static_cast<char>(0xbc), static_cast<char>(0xde),
    static_cast<char>(0xf0)});

bool isCloakDaggerEncoded(std::istream& inputStream);
bool isCloakDaggerEncoded(const BytesBuffer& inputData);
void cloakDaggerUnwrap(std::istream& inputStream, std::ostream& outputStream);
void unwrapFromStream(std::istream& inputStream, std::ostream& outputStream);
BytesBuffer unwrap(const BytesBuffer& inputData);

#endif //REWRAPPER_UNWRAPPERTESTHELPERS_HPP
