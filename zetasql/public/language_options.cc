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

#include "zetasql/public/language_options.h"

#include <set>
#include <string>

#include "zetasql/base/logging.h"
#include "google/protobuf/descriptor.pb.h"
#include "zetasql/parser/keywords.h"
#include "zetasql/public/options.pb.h"
#include "zetasql/resolved_ast/resolved_node_kind.pb.h"
#include "absl/base/macros.h"
#include "absl/container/btree_set.h"
#include "absl/status/status.h"
#include "absl/strings/ascii.h"
#include "absl/strings/str_join.h"
#include "zetasql/base/status.h"
#include "zetasql/base/status_builder.h"

namespace zetasql {

LanguageOptions::LanguageFeatureSet
LanguageOptions::GetLanguageFeaturesForVersion(LanguageVersion version) {
  LanguageFeatureSet features;
  switch (version) {
    // Include versions in decreasing order here, falling through to include
    // all features from previous versions.
    case VERSION_CURRENT:
    case VERSION_1_4:
      // Add new features here that are "ideally_enabled" and not
      // "in_development". Add features here when removing "in_development".
      features.insert(FEATURE_V_1_4_UUID_TYPE);
      features.insert(FEATURE_V_1_4_ARRAY_AGGREGATION_FUNCTIONS);
      features.insert(FEATURE_V_1_4_BARE_ARRAY_ACCESS);
      features.insert(FEATURE_V_1_4_WITH_EXPRESSION);
      features.insert(FEATURE_V_1_4_SAFE_FUNCTION_CALL_WITH_LAMBDA_ARGS);
      features.insert(FEATURE_V_1_4_STRUCT_POSITIONAL_ACCESSOR);
      features.insert(FEATURE_V_1_4_LOAD_DATA_PARTITIONS);
      features.insert(FEATURE_V_1_4_LOAD_DATA_TEMP_TABLE);
      features.insert(FEATURE_V_1_4_SINGLE_TABLE_NAME_ARRAY_PATH);
      features.insert(FEATURE_V_1_4_CORRESPONDING);
      features.insert(FEATURE_V_1_4_FIRST_AND_LAST_N);
      features.insert(FEATURE_V_1_4_NULLIFZERO_ZEROIFNULL);
      features.insert(FEATURE_V_1_4_ARRAY_FIND_FUNCTIONS);
      features.insert(FEATURE_V_1_4_PI_FUNCTIONS);
      features.insert(FEATURE_V_1_4_CORRESPONDING_FULL);
      features.insert(FEATURE_V_1_4_BY_NAME);
      features.insert(FEATURE_V_1_4_GROUP_BY_ALL);
      features.insert(FEATURE_V_1_4_CREATE_MODEL_WITH_ALIASED_QUERY_LIST);
      features.insert(FEATURE_V_1_4_REMOTE_MODEL);
      features.insert(FEATURE_V_1_4_LITERAL_CONCATENATION);
      features.insert(FEATURE_V_1_4_ENABLE_FLOAT_DISTANCE_FUNCTIONS);
      features.insert(FEATURE_V_1_4_DOT_PRODUCT);
      features.insert(FEATURE_V_1_4_MANHATTAN_DISTANCE);
      features.insert(FEATURE_V_1_4_L1_NORM);
      features.insert(FEATURE_V_1_4_L2_NORM);
      features.insert(FEATURE_V_1_4_ARRAY_ZIP);
      features.insert(FEATURE_V_1_4_GROUPING_SETS);
      features.insert(FEATURE_V_1_4_GROUPING_BUILTIN);
      features.insert(FEATURE_V_1_4_MULTIWAY_UNNEST);
      features.insert(FEATURE_V_1_4_IMPLICIT_COERCION_STRING_LITERAL_TO_BYTES);
      features.insert(FEATURE_V_1_4_REPLACE_FIELDS_ALLOW_MULTI_ONEOF);
      features.insert(FEATURE_V_1_4_JSON_ARRAY_VALUE_EXTRACTION_FUNCTIONS);
      features.insert(FEATURE_V_1_4_JSON_MORE_VALUE_EXTRACTION_FUNCTIONS);
      features.insert(FEATURE_V_1_4_CREATE_FUNCTION_LANGUAGE_WITH_CONNECTION);
      features.insert(FEATURE_V_1_4_KLL_FLOAT64_PRIMARY_WITH_DOUBLE_ALIAS);
      features.insert(FEATURE_V_1_4_DISALLOW_PIVOT_AND_UNPIVOT_ON_ARRAY_SCANS);
      features.insert(FEATURE_V_1_4_SQL_GRAPH);
      features.insert(FEATURE_V_1_4_SQL_GRAPH_ADVANCED_QUERY);
      features.insert(FEATURE_V_1_4_SQL_GRAPH_EXPOSE_GRAPH_ELEMENT);
      features.insert(FEATURE_V_1_4_SQL_GRAPH_BOUNDED_PATH_QUANTIFICATION);
      features.insert(FEATURE_V_1_4_SQL_GRAPH_RETURN_EXTENSIONS);
      features.insert(FEATURE_V_1_4_SQL_GRAPH_PATH_MODE);
      features.insert(FEATURE_V_1_4_SQL_GRAPH_PATH_TYPE);
      features.insert(FEATURE_V_1_4_GROUP_BY_GRAPH_PATH);
      features.insert(FEATURE_V_1_4_FOR_UPDATE);
      features.insert(FEATURE_V_1_4_LIMIT_OFFSET_EXPRESSIONS);
      features.insert(FEATURE_V_1_4_MATCH_RECOGNIZE);
      features.insert(FEATURE_V_1_4_BITWISE_AGGREGATE_BYTES_SIGNATURES);
      features.insert(FEATURE_V_1_4_FROM_PROTO_DURATION);
      features.insert(FEATURE_V_1_4_SIMPLIFY_PIVOT_REWRITE);
      features.insert(FEATURE_V_1_4_MULTILEVEL_AGGREGATION);
      features.insert(FEATURE_V_1_4_PIPE_NAMED_WINDOWS);
      features.insert(FEATURE_V_1_4_PIPE_RECURSIVE_UNION);
      features.insert(FEATURE_V_1_4_MULTILEVEL_AGGREGATION_IN_UDAS);
      ABSL_FALLTHROUGH_INTENDED;
    case VERSION_1_3:
      // NO CHANGES SHOULD HAPPEN INSIDE THE VERSIONS BELOW, which are
      // supposed to be stable and frozen, except possibly for bug fixes.
      features.insert(FEATURE_V_1_3_PROTO_DEFAULT_IF_NULL);
      features.insert(FEATURE_V_1_3_EXTRACT_FROM_PROTO);
      features.insert(FEATURE_V_1_3_ARRAY_GREATEST_LEAST);
      features.insert(FEATURE_V_1_3_ARRAY_ORDERING);
      features.insert(FEATURE_V_1_3_OMIT_INSERT_COLUMN_LIST);
      features.insert(FEATURE_V_1_3_IGNORE_PROTO3_USE_DEFAULTS);
      features.insert(FEATURE_V_1_3_REPLACE_FIELDS);
      features.insert(FEATURE_V_1_3_NULLS_FIRST_LAST_IN_ORDER_BY);
      features.insert(FEATURE_V_1_3_ALLOW_DASHES_IN_TABLE_NAME);
      features.insert(FEATURE_V_1_3_CONCAT_MIXED_TYPES);
      features.insert(FEATURE_V_1_3_WITH_RECURSIVE);
      features.insert(FEATURE_V_1_3_PROTO_MAPS);
      features.insert(FEATURE_V_1_3_ENUM_VALUE_DESCRIPTOR_PROTO);
      features.insert(FEATURE_V_1_3_DECIMAL_ALIAS);
      features.insert(FEATURE_V_1_3_UNNEST_AND_FLATTEN_ARRAYS);
      features.insert(FEATURE_V_1_3_ALLOW_CONSECUTIVE_ON);
      features.insert(FEATURE_V_1_3_ALLOW_REGEXP_EXTRACT_OPTIONALS);
      features.insert(FEATURE_V_1_3_DATE_TIME_CONSTRUCTORS);
      features.insert(FEATURE_V_1_3_DATE_ARITHMETICS);
      features.insert(FEATURE_V_1_3_ADDITIONAL_STRING_FUNCTIONS);
      features.insert(FEATURE_V_1_3_WITH_GROUP_ROWS);
      features.insert(FEATURE_V_1_3_EXTENDED_DATE_TIME_SIGNATURES);
      features.insert(FEATURE_V_1_3_EXTENDED_GEOGRAPHY_PARSERS);
      features.insert(FEATURE_V_1_3_INLINE_LAMBDA_ARGUMENT);
      features.insert(FEATURE_V_1_3_PIVOT);
      features.insert(FEATURE_V_1_3_ANNOTATION_FRAMEWORK);
      features.insert(FEATURE_V_1_3_IS_DISTINCT);
      features.insert(FEATURE_V_1_3_FORMAT_IN_CAST);
      features.insert(FEATURE_V_1_3_UNPIVOT);
      features.insert(FEATURE_V_1_3_DML_RETURNING);
      features.insert(FEATURE_V_1_3_FILTER_FIELDS);
      features.insert(FEATURE_V_1_3_QUALIFY);
      features.insert(FEATURE_V_1_3_REPEAT);
      features.insert(FEATURE_V_1_3_COLUMN_DEFAULT_VALUE);
      features.insert(FEATURE_V_1_3_KLL_WEIGHTS);
      features.insert(FEATURE_V_1_3_FOR_IN);
      features.insert(FEATURE_V_1_3_CASE_STMT);
      features.insert(FEATURE_V_1_3_ALLOW_SLASH_PATHS);
      features.insert(FEATURE_V_1_3_TYPEOF_FUNCTION);
      features.insert(FEATURE_V_1_3_SCRIPT_LABEL);
      features.insert(FEATURE_V_1_3_REMOTE_FUNCTION);
      features.insert(FEATURE_V_1_3_BRACED_PROTO_CONSTRUCTORS);
      features.insert(FEATURE_V_1_3_LIKE_ANY_SOME_ALL);
      ABSL_FALLTHROUGH_INTENDED;
    case VERSION_1_2:
      features.insert(FEATURE_V_1_2_ARRAY_ELEMENTS_WITH_SET);
      features.insert(FEATURE_V_1_2_CIVIL_TIME);
      features.insert(FEATURE_V_1_2_CORRELATED_REFS_IN_NESTED_DML);
      features.insert(FEATURE_V_1_2_GENERATED_COLUMNS);
      features.insert(FEATURE_V_1_2_GROUP_BY_ARRAY);
      features.insert(FEATURE_V_1_2_GROUP_BY_STRUCT);
      features.insert(FEATURE_V_1_2_NESTED_UPDATE_DELETE_WITH_OFFSET);
      features.insert(FEATURE_V_1_2_PROTO_EXTENSIONS_WITH_NEW);
      features.insert(FEATURE_V_1_2_PROTO_EXTENSIONS_WITH_SET);
      features.insert(FEATURE_V_1_2_SAFE_FUNCTION_CALL);
      features.insert(FEATURE_V_1_2_WEEK_WITH_WEEKDAY);
      ABSL_FALLTHROUGH_INTENDED;
    case VERSION_1_1:
      features.insert(FEATURE_V_1_1_ORDER_BY_COLLATE);
      features.insert(FEATURE_V_1_1_WITH_ON_SUBQUERY);
      features.insert(FEATURE_V_1_1_SELECT_STAR_EXCEPT_REPLACE);
      features.insert(FEATURE_V_1_1_ORDER_BY_IN_AGGREGATE);
      features.insert(FEATURE_V_1_1_CAST_DIFFERENT_ARRAY_TYPES);
      features.insert(FEATURE_V_1_1_ARRAY_EQUALITY);
      features.insert(FEATURE_V_1_1_LIMIT_IN_AGGREGATE);
      features.insert(FEATURE_V_1_1_HAVING_IN_AGGREGATE);
      features.insert(FEATURE_V_1_1_NULL_HANDLING_MODIFIER_IN_ANALYTIC);
      features.insert(FEATURE_V_1_1_NULL_HANDLING_MODIFIER_IN_AGGREGATE);
      features.insert(FEATURE_V_1_1_FOR_SYSTEM_TIME_AS_OF);
      ABSL_FALLTHROUGH_INTENDED;
    case VERSION_1_0:
      break;
    case __LanguageVersion__switch_must_have_a_default__:
      ABSL_LOG(ERROR) << "GetLanguageFeaturesForVersion called with " << version;
      break;
  }
  return features;
}

void LanguageOptions::SetLanguageVersion(LanguageVersion version) {
  enabled_language_features_ = GetLanguageFeaturesForVersion(version);
}

LanguageOptions LanguageOptions::MaximumFeatures() {
  LanguageOptions options;
  options.EnableMaximumLanguageFeatures();
  return options;
}

std::string LanguageOptions::GetEnabledLanguageFeaturesAsString() const {
  return ToString(enabled_language_features_);
}

std::string LanguageOptions::ToString(const LanguageFeatureSet& features) {
  absl::btree_set<std::string> strings;
  for (LanguageFeature feature : features) {
    strings.insert(LanguageFeature_Name(feature));
  }
  return absl::StrJoin(strings.begin(), strings.end(), ", ");
}

LanguageOptions::LanguageOptions(const LanguageOptionsProto& proto)
    : name_resolution_mode_(proto.name_resolution_mode()),
      product_mode_(proto.product_mode()),
      error_on_deprecated_syntax_(proto.error_on_deprecated_syntax()) {
  supported_statement_kinds_.clear();
  for (int i = 0; i <  proto.supported_statement_kinds_size(); ++i) {
    supported_statement_kinds_.insert(proto.supported_statement_kinds(i));
  }
  if (proto.enabled_language_features_size() > 0) {
    enabled_language_features_.clear();
    for (int i = 0; i <  proto.enabled_language_features_size(); ++i) {
      enabled_language_features_.insert(proto.enabled_language_features(i));
    }
  }
  if (proto.supported_generic_entity_types_size() > 0) {
    supported_generic_entity_types_.clear();
    for (int i = 0; i <  proto.supported_generic_entity_types_size(); ++i) {
      supported_generic_entity_types_.insert(
          proto.supported_generic_entity_types(i));
    }
  }
  if (proto.supported_generic_sub_entity_types_size() > 0) {
    supported_generic_sub_entity_types_.clear();
    for (int i = 0; i < proto.supported_generic_sub_entity_types_size(); ++i) {
      supported_generic_sub_entity_types_.insert(
          proto.supported_generic_sub_entity_types(i));
    }
  }
  for (absl::string_view keyword : proto.reserved_keywords()) {
    // Failure is possible if the proto is invalid, but a constructor cannot
    // return a status. Crash in debug builds, but silently ignore the malformed
    // keyword in production.
    auto status = EnableReservableKeyword(keyword);
    ZETASQL_DCHECK_OK(status);
    status.IgnoreError();
  }
}

void LanguageOptions::Serialize(LanguageOptionsProto* proto) const {
  proto->set_name_resolution_mode(name_resolution_mode_);
  proto->set_product_mode(product_mode_);
  proto->set_error_on_deprecated_syntax(error_on_deprecated_syntax_);

  for (ResolvedNodeKind kind : supported_statement_kinds_) {
    proto->add_supported_statement_kinds(kind);
  }
  for (LanguageFeature feature : enabled_language_features_) {
    proto->add_enabled_language_features(feature);
  }
  for (const std::string& entity_type : supported_generic_entity_types_) {
    proto->add_supported_generic_entity_types(entity_type);
  }
  for (const std::string& entity_type : supported_generic_sub_entity_types_) {
    proto->add_supported_generic_sub_entity_types(entity_type);
  }
  for (absl::string_view keyword : reserved_keywords_) {
    proto->add_reserved_keywords(std::string(keyword));
  }
}

void LanguageOptions::EnableMaximumLanguageFeatures(bool for_development) {
  const google::protobuf::EnumDescriptor* descriptor =
      google::protobuf::GetEnumDescriptor<LanguageFeature>();
  for (int i = 0; i < descriptor->value_count(); ++i) {
    const google::protobuf::EnumValueDescriptor* value_descriptor = descriptor->value(i);
    const LanguageFeature feature =
        static_cast<LanguageFeature>(value_descriptor->number());
    const LanguageFeatureOptions& options =
        value_descriptor->options().GetExtension(language_feature_options);
    const bool enabled = options.ideally_enabled() &&
                         (for_development || !options.in_development());
    if (feature != __LanguageFeature__switch_must_have_a_default__ && enabled) {
      EnableLanguageFeature(feature);
    }
  }

  // TODO: This should be fleshed out fully when we have an approved
  // design for keyword maturity
  if (for_development) {
    EnableAllReservableKeywords();
  } else {
    // QUALIFY is the only exception as it's already launched.
    ZETASQL_CHECK_OK(EnableReservableKeyword("QUALIFY", /*reserved=*/true));
  }
}

const LanguageOptions::KeywordSet& LanguageOptions::GetReservableKeywords() {
  static auto* reservable_keywords =
      new KeywordSet{"QUALIFY", "MATCH_RECOGNIZE", "GRAPH_TABLE"};
  return *reservable_keywords;
}

bool LanguageOptions::IsReservedKeyword(absl::string_view keyword) const {
  if (reserved_keywords_.contains(keyword)) {
    return true;
  }
  const parser::KeywordInfo* keyword_info = parser::GetKeywordInfo(keyword);
  return keyword_info != nullptr && keyword_info->IsAlwaysReserved();
}

absl::Status LanguageOptions::EnableReservableKeyword(absl::string_view keyword,
                                                      bool reserved) {
  std::string keyword_uppercase = absl::AsciiStrToUpper(keyword);
  const auto& reservable_keywords = GetReservableKeywords();
  auto it = reservable_keywords.find(keyword_uppercase);
  if (it == reservable_keywords.end()) {
    return zetasql_base::InvalidArgumentErrorBuilder()
           << "Invalid keyword " << keyword
           << " passed to LanguageOptions::EnableReservableKeyword()";
  }

  if (reserved) {
    reserved_keywords_.insert(*it);
  } else {
    reserved_keywords_.erase(*it);
  }
  return absl::OkStatus();
}

void LanguageOptions::EnableAllReservableKeywords(bool reserved) {
  if (reserved) {
    reserved_keywords_ = GetReservableKeywords();
  } else {
    reserved_keywords_.clear();
  }
}
}  // namespace zetasql
