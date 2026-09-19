#pragma once

namespace ls {

  namespace debug {
    class Console;
  }

}  // namespace ls

namespace ls::asset {

  struct AssetContext {
    debug::Console* console{ nullptr };
  };

}  // namespace ls::asset
