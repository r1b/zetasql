//
// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// Helpers for constructing JSON value from ZetaSQL value.

#ifndef ZETASQL_PUBLIC_FUNCTIONS_TO_JSON_H_
#define ZETASQL_PUBLIC_FUNCTIONS_TO_JSON_H_

#include "zetasql/public/functions/unsupported_fields.pb.h"
#include "zetasql/public/json_value.h"
#include "zetasql/public/language_options.h"
#include "zetasql/public/value.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "zetasql/base/status.h"

namespace zetasql {
namespace functions {

// The threshold value of the json object nesting level to check if the nested
// json object causing out of stack space.
const int kNestingLevelStackCheckThreshold = 10;

// Constructs JSONValue from <value> with options of:
// - stringify_wide_numbers: which defines how numeric values outside of
//   DOUBLE type domain are encoded in the generated JSON document.
//   All non-double numerics are encoded as strings if
//   stringify_wide_numbers is true. Otherwise, JSON number
//   type is used to represent all values of ZetaSQL number types,
//   including values outside of DOUBLE domain.
// - canonicalize_zero: if true, the sign on a signed zero is removed
//   when converting numeric type to string.
// - unsupported_fields:
//   - FAIL (default): fail the query for any unsupported field
//   - IGNORE: treat unsupported fields as non-existent
//   - PLACEHOLDER: replace value with descriptive message for the type
// TODO : remove canonicalize_zero flag when all
// engines have rolled out this new behavior.
absl::StatusOr<JSONValue> ToJson(
    const Value& value, bool stringify_wide_numbers,
    const LanguageOptions& language_options, bool canonicalize_zero = false,
    UnsupportedFieldsEnum::UnsupportedFields unsupported_fields =
        UnsupportedFieldsEnum::FAIL);

}  // namespace functions
}  // namespace zetasql

#endif  // ZETASQL_PUBLIC_FUNCTIONS_TO_JSON_H_
