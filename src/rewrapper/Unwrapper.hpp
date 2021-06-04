// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef REWRAPPER_UNWRAPPER_HPP
#define REWRAPPER_UNWRAPPER_HPP

#include <string>
#include "WrapperInterface.hpp"

namespace boost {namespace filesystem {class path;}}


class UnwrapperInterface
{
public:
  virtual ~UnwrapperInterface() = default;
  virtual void unwrapFromStream(std::istream& inputStream, std::ostream& outputStream) = 0;
};

class Unwrapper: public UnwrapperInterface, public WrapperInterface
{
public:
  Unwrapper() = default;
  ~Unwrapper() = default;
  void unwrapFromStream(std::istream& inputStream, std::ostream& outputStream) override ;
  StreamingFileInfo startFile(const StreamingFileInfo& info, const BytesBuffer& firstData) override;
  BytesBuffer unwrap(const BytesBuffer& inputData) override;

private:

  bool isCloakDaggerEncoded(std::istream& inputStream) const;
  bool isCloakDaggerEncoded(const BytesBuffer& inputData) const;
  void cloakDaggerUnwrap(std::istream& inputStream, std::ostream& outputStream) const;
};



#endif //REWRAPPER_UNWRAPPER_HPP
