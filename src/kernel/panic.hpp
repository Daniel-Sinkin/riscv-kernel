#pragma once

namespace kernel {

[[noreturn]] auto panic(const char *msg) -> void;

}
