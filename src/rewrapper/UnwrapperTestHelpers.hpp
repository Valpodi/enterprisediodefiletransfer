// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef REWRAPPER_UNWRAPPERTESTHELPERS_HPP
#define REWRAPPER_UNWRAPPERTESTHELPERS_HPP

#include <filesystem>
#include <BytesBuffer.hpp>
#include <sstream>


std::stringstream createTestWrappedString(const std::string& payload, const BytesBuffer& mask={0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0});
BytesBuffer createTestWrappedBytesBuffer(const std::string& payload, const BytesBuffer& mask={0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0});

bool isCloakDaggerEncoded(std::istream& inputStream);
bool isCloakDaggerEncoded(const BytesBuffer& inputData);
void cloakDaggerUnwrap(std::istream& inputStream, std::ostream& outputStream);
void unwrapFromStream(std::istream& inputStream, std::ostream& outputStream);
BytesBuffer unwrap(const BytesBuffer& inputData);

#endif //REWRAPPER_UNWRAPPERTESTHELPERS_HPP
