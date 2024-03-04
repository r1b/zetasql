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

#include "zetasql/parser/macros/standalone_macro_expansion.h"

#include <cctype>
#include <string>
#include <vector>

#include "zetasql/parser/bison_token_codes.h"
#include "zetasql/parser/macros/token_splicing_utils.h"
#include "zetasql/parser/macros/token_with_location.h"
#include "zetasql/base/check.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "absl/types/span.h"

namespace zetasql {
namespace parser {
namespace macros {

static bool IsIntegerOrFloatingPointLiteral(const TokenWithLocation& token) {
  return token.kind == INTEGER_LITERAL || token.kind == FLOATING_POINT_LITERAL;
}

static bool SplicingTokensCouldStartComment(
    const TokenWithLocation& previous_token,
    const TokenWithLocation& current_token) {
  return (previous_token.kind == '-' && current_token.kind == '-') ||
         (previous_token.kind == '/' && current_token.kind == '/') ||
         (previous_token.kind == '/' && current_token.kind == '*');
}

static bool TokensRequireExplicitSeparation(
    const TokenWithLocation& previous_token,
    const TokenWithLocation& current_token) {
  if (current_token.text.empty()) {
    // YYEOF doesn't need separation.
    return false;
  }

  // Macro invocation, keyword or unquoted identifier followed by a character
  // that can continue it.
  if (previous_token.kind == MACRO_INVOCATION ||
      IsKeywordOrUnquotedIdentifier(previous_token)) {
    return IsIdentifierCharacter(current_token.text.front());
  }
  // Macro argument reference followed by a decimal digit.
  if (previous_token.kind == MACRO_ARGUMENT_REFERENCE) {
    return std::isdigit(current_token.text.front());
  }

  // Avoid comment-outs, where symbols inadvertently become the start of a
  // comment.
  if (SplicingTokensCouldStartComment(previous_token, current_token)) {
    return true;
  }

  // Integer and floating-point literals should not splice
  if (IsIntegerOrFloatingPointLiteral(previous_token) &&
      IsIntegerOrFloatingPointLiteral(current_token)) {
    return true;
  }

  // OK to have no space.
  return false;
}

std::string TokensToString(absl::Span<const TokenWithLocation> tokens,
                           bool standardize_to_single_whitespace) {
  std::string expanded_sql;
  for (auto it = tokens.begin(); it != tokens.end(); ++it) {
    const auto& current_token = *it;
    absl::string_view whitespace = current_token.preceding_whitespaces;
    if (standardize_to_single_whitespace) {
      whitespace = " ";
      if (it == tokens.begin() || it == tokens.end() - 1) {
        // No space before the first token.
        // Also at the end it's YYEOF, so we also drop spaces before it (which
        // would be trailing to the content).
        ABSL_DCHECK(it != tokens.rbegin().base() || current_token.text.empty());
        whitespace = "";
      }
    }
    if (whitespace.empty() && it != tokens.begin()) {
      const TokenWithLocation& previous_token = *(it - 1);
      if (TokensRequireExplicitSeparation(previous_token, current_token)) {
        // Prevent token splicing by forcing an extra space.
        whitespace = " ";
      }
    }
    absl::StrAppend(&expanded_sql, whitespace, current_token.text);
  }
  return expanded_sql;
}

}  // namespace macros
}  // namespace parser
}  // namespace zetasql
