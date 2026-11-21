#include "System/Registry/RegistryCmp.h"

#include "System/Hash64.h"

namespace Atrum {

bool RegistryCmp::HasUpdRegistration(const std::string &cmpName) const {
  return registryCmpUpd_.find(Hash64(cmpName)) != registryCmpUpd_.end();
}

bool RegistryCmp::HasDrawRegistration(const std::string &cmpName) const {
  return registryCmpDraw_.find(Hash64(cmpName)) != registryCmpDraw_.end();
}

bool RegistryCmp::HasDataRegistration(const std::string &cmpName) const {
  return registryCmpData_.find(Hash64(cmpName)) != registryCmpData_.end();
}

} // namespace Atrum