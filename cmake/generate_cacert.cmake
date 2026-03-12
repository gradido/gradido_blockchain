
# Read file
file(READ "${INPUT_FILE}" PEM_CONTENT)

# Escape for C string (quotes and newlines)
string(REPLACE "\\" "\\\\" PEM_CONTENT "${PEM_CONTENT}")
string(REPLACE "\"" "\\\"" PEM_CONTENT "${PEM_CONTENT}")
string(REGEX REPLACE "\n" "\\\\n\"\n\"" PEM_CONTENT "${PEM_CONTENT}")

# Write header
file(WRITE "${OUTPUT_FILE}" "// Auto-generated from cacert.pem
#ifndef GRADIDO_BLOCKCHAIN_CACERT_H
#define GRADIDO_BLOCKCHAIN_CACERT_H

#include <string_view>

static constexpr std::string_view CACERT_PEM =
\"${PEM_CONTENT}\";

#endif // GRADIDO_BLOCKCHAIN_CACERT_H
")