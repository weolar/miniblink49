#ifdef OBJECT_PRINT

#include "src/objects.h"

#include <iosfwd>

#include "src/objects/struct-inl.h"

namespace v8 {
namespace internal {

void EnumCache::EnumCachePrint(std::ostream& os) {
  PrintHeader(os, "EnumCache");
  os << "\n - keys: " << Brief(keys());
  os << "\n - indices: " << Brief(indices());
  os << "\n";
}

void Tuple2::Tuple2Print(std::ostream& os) {
  PrintHeader(os, "Tuple2");
  os << "\n - value1: " << Brief(value1());
  os << "\n - value2: " << Brief(value2());
  os << "\n";
}

void Tuple3::Tuple3Print(std::ostream& os) {
  PrintHeader(os, "Tuple3");
  os << "\n - value1: " << Brief(value1());
  os << "\n - value2: " << Brief(value2());
  os << "\n - value3: " << Brief(value3());
  os << "\n";
}

}  // namespace internal"
}  // namespace v8"

#endif  // OBJECT_PRINT

