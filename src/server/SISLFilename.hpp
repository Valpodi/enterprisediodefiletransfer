// Copyright PA Knowledge 2021

#ifndef ENTERPRISEDIODETESTER_SISLFILENAME_H
#define ENTERPRISEDIODETESTER_SISLFILENAME_H

#include <BytesBuffer.hpp>
#include <optional>
#include <string>

class SISLFilename
{
public:
  explicit SISLFilename(std::uint32_t maxSislLength, std::uint32_t maxFilenameLength=1000);

public:
  [[nodiscard]] std::optional<std::string> extractFilename(const BytesBuffer& eofFrame) const;

private:
  static std::string convertFromSisl(std::string sislFilename);

  const std::uint32_t maxSislLength;
  const std::uint32_t maxFilenameLength;
};

#endif // ENTERPRISEDIODETESTER_SISLFILENAME_H
