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

#include <string>
#include <vector>

#include "zetasql/common/builtin_function_internal.h"
#include "zetasql/public/builtin_function_options.h"
#include "zetasql/public/function.h"
#include "zetasql/public/function_signature.h"
#include "zetasql/public/types/array_type.h"
#include "zetasql/public/types/struct_type.h"
#include "zetasql/public/types/type.h"
#include "zetasql/public/types/type_factory.h"
#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "absl/strings/substitute.h"
#include "zetasql/base/status_macros.h"

namespace zetasql {

// Create a `FunctionSignatureOptions` that configures a SQL definition that
// will be inlined by `REWRITE_BUILTIN_FUNCTION_INLINER`.
static FunctionSignatureOptions SetDefinitionForInlining(absl::string_view sql,
                                                         bool enabled = true) {
  return FunctionSignatureOptions().set_rewrite_options(
      FunctionSignatureRewriteOptions()
          .set_enabled(enabled)
          .set_rewriter(REWRITE_BUILTIN_FUNCTION_INLINER)
          .set_sql(sql));
}

absl::Status GetDistanceFunctions(TypeFactory* type_factory,
                                  const BuiltinFunctionOptions& options,
                                  NameToFunctionMap* functions) {
  std::vector<StructType::StructField> input_struct_fields_int64 = {
      {"key", types::Int64Type()}, {"value", types::DoubleType()}};
  const StructType* struct_int64 = nullptr;
  ZETASQL_RETURN_IF_ERROR(
      type_factory->MakeStructType({input_struct_fields_int64}, &struct_int64));
  const ArrayType* array_struct_int64_key_type;
  ZETASQL_RETURN_IF_ERROR(
      type_factory->MakeArrayType(struct_int64, &array_struct_int64_key_type));

  std::vector<StructType::StructField> input_struct_fields_string = {
      {"key", types::StringType()}, {"value", types::DoubleType()}};
  const StructType* struct_string = nullptr;
  ZETASQL_RETURN_IF_ERROR(type_factory->MakeStructType({input_struct_fields_string},
                                               &struct_string));
  const ArrayType* array_struct_string_key_type;
  ZETASQL_RETURN_IF_ERROR(type_factory->MakeArrayType(struct_string,
                                              &array_struct_string_key_type));

  FunctionOptions function_options;
  std::vector<FunctionSignatureOnHeap> cosine_signatures = {
      {types::DoubleType(),
       {types::DoubleArrayType(), types::DoubleArrayType()},
       FN_COSINE_DISTANCE_DENSE_DOUBLE},
      {types::DoubleType(),
       {array_struct_int64_key_type, array_struct_int64_key_type},
       FN_COSINE_DISTANCE_SPARSE_INT64},
      {types::DoubleType(),
       {array_struct_string_key_type, array_struct_string_key_type},
       FN_COSINE_DISTANCE_SPARSE_STRING}};

  if (options.language_options.LanguageFeatureEnabled(
          FEATURE_V_1_4_ENABLE_FLOAT_DISTANCE_FUNCTIONS)) {
    cosine_signatures.push_back(
        {types::DoubleType(),
         {types::FloatArrayType(), types::FloatArrayType()},
         FN_COSINE_DISTANCE_DENSE_FLOAT});
  }

  InsertFunction(functions, options, "cosine_distance", Function::SCALAR,
                 cosine_signatures, function_options);

  FunctionArgumentType options_arg = FunctionArgumentType(
      types::JsonType(),
      FunctionArgumentTypeOptions(FunctionArgumentType::REQUIRED)
          .set_argument_name("options", kNamedOnly));

  std::vector<FunctionSignatureOnHeap> approx_cosine_signatures = {
      {types::DoubleType(),
       {types::DoubleArrayType(), types::DoubleArrayType()},
       FN_APPROX_COSINE_DISTANCE_DOUBLE},
      {types::DoubleType(),
       {types::DoubleArrayType(), types::DoubleArrayType(), options_arg},
       FN_APPROX_COSINE_DISTANCE_DOUBLE_WITH_OPTIONS},
      {types::DoubleType(),
       {types::FloatArrayType(), types::FloatArrayType()},
       FN_APPROX_COSINE_DISTANCE_FLOAT},
      {types::DoubleType(),
       {types::FloatArrayType(), types::FloatArrayType(), options_arg},
       FN_APPROX_COSINE_DISTANCE_FLOAT_WITH_OPTIONS}};

  InsertFunction(functions, options, "approx_cosine_distance", Function::SCALAR,
                 approx_cosine_signatures, /*function_options=*/{});

  std::vector<FunctionSignatureOnHeap> euclidean_signatures = {
      {types::DoubleType(),
       {types::DoubleArrayType(), types::DoubleArrayType()},
       FN_EUCLIDEAN_DISTANCE_DENSE_DOUBLE},
      {types::DoubleType(),
       {array_struct_int64_key_type, array_struct_int64_key_type},
       FN_EUCLIDEAN_DISTANCE_SPARSE_INT64},
      {types::DoubleType(),
       {array_struct_string_key_type, array_struct_string_key_type},
       FN_EUCLIDEAN_DISTANCE_SPARSE_STRING}};

  if (options.language_options.LanguageFeatureEnabled(
          FEATURE_V_1_4_ENABLE_FLOAT_DISTANCE_FUNCTIONS)) {
    euclidean_signatures.push_back(
        {types::DoubleType(),
         {types::FloatArrayType(), types::FloatArrayType()},
         FN_EUCLIDEAN_DISTANCE_DENSE_FLOAT});
  }

  InsertFunction(functions, options, "euclidean_distance", Function::SCALAR,
                 euclidean_signatures, function_options);

  std::vector<FunctionSignatureOnHeap> approx_euclidean_signatures = {
      {types::DoubleType(),
       {types::DoubleArrayType(), types::DoubleArrayType()},
       FN_APPROX_EUCLIDEAN_DISTANCE_DOUBLE},
      {types::DoubleType(),
       {types::DoubleArrayType(), types::DoubleArrayType(), options_arg},
       FN_APPROX_EUCLIDEAN_DISTANCE_DOUBLE_WITH_OPTIONS},
      {types::DoubleType(),
       {types::FloatArrayType(), types::FloatArrayType()},
       FN_APPROX_EUCLIDEAN_DISTANCE_FLOAT},
      {types::DoubleType(),
       {types::FloatArrayType(), types::FloatArrayType(), options_arg},
       FN_APPROX_EUCLIDEAN_DISTANCE_FLOAT_WITH_OPTIONS}};

  InsertFunction(functions, options, "approx_euclidean_distance",
                 Function::SCALAR, approx_euclidean_signatures,
                 /*function_options=*/{});

  // Lambdas for a common error message amongst function rewriters.
  auto null_element_err_msg_base = [](absl::string_view name) {
    return absl::Substitute(
        "Cannot compute $0 with a NULL element, since it is unclear if NULLs "
        "should be ignored, counted as a zero value, or another "
        "interpretation.",
        name);
  };

  // Lambda with argument-checking SQL common to distance function rewriters.
  auto distance_fn_rewrite_sql = [&null_element_err_msg_base](
                                     absl::string_view name,
                                     absl::string_view sql) {
    return absl::Substitute(R"sql(
      CASE
        WHEN input_array_1 IS NULL OR input_array_2 IS NULL
          THEN NULL
        WHEN ARRAY_LENGTH(input_array_1) = 0 AND ARRAY_LENGTH(input_array_2) = 0
          THEN CAST(0 AS FLOAT64)
        WHEN ARRAY_LENGTH(input_array_1) != ARRAY_LENGTH(input_array_2)
          THEN ERROR(FORMAT(
            "Array arguments to %s must have equal length. The given arrays have lengths of %d and %d",
            "$0", ARRAY_LENGTH(input_array_1), ARRAY_LENGTH(input_array_2)))
        ELSE
          $1
          WHERE
            IF(e1 IS NULL, ERROR(FORMAT(
              "%s The NULL element was found in the first array argument at OFFSET %d",
              "$2", index)), TRUE) AND
            IF(input_array_2[OFFSET(index)] IS NULL, ERROR(FORMAT(
              "%s The NULL element was found in the second array argument at OFFSET %d",
              "$2", index)), TRUE))
        END
    )sql",
                            name, sql, null_element_err_msg_base(name));
  };

  // Lambda for defining named arguments for distance function rewriters.
  auto distance_fn_named_arg = [](const Type* arg_type,
                                  absl::string_view name) {
    return FunctionArgumentType(
        arg_type,
        FunctionArgumentTypeOptions().set_argument_name(name, kPositionalOnly));
  };

  // Use a Rewriter for DOT_PRODUCT.
  std::string dot_product_sql = distance_fn_rewrite_sql("DOT_PRODUCT", R"sql(
      (SELECT
            SUM(
              CAST(e1 AS FLOAT64) *
              CAST(input_array_2[OFFSET(index)] AS FLOAT64))
          FROM UNNEST(input_array_1) AS e1 WITH OFFSET index
    )sql");

  FunctionSignatureOptions dot_product_signature_options =
      SetDefinitionForInlining(dot_product_sql, true)
          .add_required_language_feature(FEATURE_V_1_4_DOT_PRODUCT);

  std::vector<FunctionSignatureOnHeap> dot_product_signatures = {
      {types::DoubleType(),
       {distance_fn_named_arg(types::Int64ArrayType(), "input_array_1"),
        distance_fn_named_arg(types::Int64ArrayType(), "input_array_2")},
       FN_DOT_PRODUCT_INT64,
       dot_product_signature_options},
      {types::DoubleType(),
       {distance_fn_named_arg(types::FloatArrayType(), "input_array_1"),
        distance_fn_named_arg(types::FloatArrayType(), "input_array_2")},
       FN_DOT_PRODUCT_FLOAT,
       dot_product_signature_options},
      {types::DoubleType(),
       {distance_fn_named_arg(types::DoubleArrayType(), "input_array_1"),
        distance_fn_named_arg(types::DoubleArrayType(), "input_array_2")},
       FN_DOT_PRODUCT_DOUBLE,
       dot_product_signature_options}};

  InsertFunction(functions, options, "dot_product", Function::SCALAR,
                 dot_product_signatures, function_options);

  std::vector<FunctionSignatureOnHeap> approx_dot_product_signatures = {
      {types::DoubleType(),
       {types::Int64ArrayType(), types::Int64ArrayType()},
       FN_APPROX_DOT_PRODUCT_INT64},
      {types::DoubleType(),
       {types::Int64ArrayType(), types::Int64ArrayType(), options_arg},
       FN_APPROX_DOT_PRODUCT_INT64_WITH_OPTIONS},
      {types::DoubleType(),
       {types::FloatArrayType(), types::FloatArrayType()},
       FN_APPROX_DOT_PRODUCT_FLOAT},
      {types::DoubleType(),
       {types::FloatArrayType(), types::FloatArrayType(), options_arg},
       FN_APPROX_DOT_PRODUCT_FLOAT_WITH_OPTIONS},
      {types::DoubleType(),
       {types::DoubleArrayType(), types::DoubleArrayType()},
       FN_APPROX_DOT_PRODUCT_DOUBLE},
      {types::DoubleType(),
       {types::DoubleArrayType(), types::DoubleArrayType(), options_arg},
       FN_APPROX_DOT_PRODUCT_DOUBLE_WITH_OPTIONS}};

  InsertFunction(functions, options, "approx_dot_product", Function::SCALAR,
                 approx_dot_product_signatures, /*function_options=*/{});

  // Use a Rewriter for MANHATTAN_DISTANCE.
  std::string manhattan_distance_sql =
      distance_fn_rewrite_sql("MANHATTAN_DISTANCE", R"sql(
      (SELECT
            SUM(ABS(
              CAST(e1 AS FLOAT64) -
              CAST(input_array_2[OFFSET(index)] AS FLOAT64)))
          FROM UNNEST(input_array_1) AS e1 WITH OFFSET index
    )sql");

  FunctionSignatureOptions manhattan_distance_signature_options =
      SetDefinitionForInlining(manhattan_distance_sql, true)
          .add_required_language_feature(FEATURE_V_1_4_MANHATTAN_DISTANCE);

  std::vector<FunctionSignatureOnHeap> manhattan_distance_signatures = {
      {types::DoubleType(),
       {distance_fn_named_arg(types::Int64ArrayType(), "input_array_1"),
        distance_fn_named_arg(types::Int64ArrayType(), "input_array_2")},
       FN_MANHATTAN_DISTANCE_INT64,
       manhattan_distance_signature_options},
      {types::DoubleType(),
       {distance_fn_named_arg(types::FloatArrayType(), "input_array_1"),
        distance_fn_named_arg(types::FloatArrayType(), "input_array_2")},
       FN_MANHATTAN_DISTANCE_FLOAT,
       manhattan_distance_signature_options},
      {types::DoubleType(),
       {distance_fn_named_arg(types::DoubleArrayType(), "input_array_1"),
        distance_fn_named_arg(types::DoubleArrayType(), "input_array_2")},
       FN_MANHATTAN_DISTANCE_DOUBLE,
       manhattan_distance_signature_options}};

  InsertFunction(functions, options, "manhattan_distance", Function::SCALAR,
                 manhattan_distance_signatures, function_options);

  // Lambda with argument-checking SQL common to norm function rewriters.
  auto norm_fn_rewrite_sql = [&null_element_err_msg_base](
                                 absl::string_view name,
                                 absl::string_view sql) {
    return absl::Substitute(R"sql(
      CASE
        WHEN input_array IS NULL
          THEN NULL
        WHEN ARRAY_LENGTH(input_array) = 0
          THEN CAST(0 AS FLOAT64)
        ELSE
          $0
          WHERE
            IF(e IS NULL, ERROR(FORMAT(
              "%s The NULL element was found in the array argument at OFFSET %d",
              "$1", index)), TRUE))
        END
    )sql",
                            sql, null_element_err_msg_base(name));
  };

  // Use a Rewriter for L1_NORM.
  std::string l1_norm_sql = norm_fn_rewrite_sql("L1_NORM", R"sql(
      (SELECT SUM(ABS(CAST(e AS FLOAT64)))
       FROM UNNEST(input_array) AS e WITH OFFSET index
    )sql");

  FunctionSignatureOptions l1_norm_signature_options =
      SetDefinitionForInlining(l1_norm_sql, true)
          .add_required_language_feature(FEATURE_V_1_4_L1_NORM);

  std::vector<FunctionSignatureOnHeap> l1_norm_signatures = {
      {types::DoubleType(),
       {distance_fn_named_arg(types::Int64ArrayType(), "input_array")},
       FN_L1_NORM_INT64,
       l1_norm_signature_options},
      {types::DoubleType(),
       {distance_fn_named_arg(types::FloatArrayType(), "input_array")},
       FN_L1_NORM_FLOAT,
       l1_norm_signature_options},
      {types::DoubleType(),
       {distance_fn_named_arg(types::DoubleArrayType(), "input_array")},
       FN_L1_NORM_DOUBLE,
       l1_norm_signature_options}};

  InsertFunction(functions, options, "l1_norm", Function::SCALAR,
                 l1_norm_signatures, function_options);

  // Use a Rewriter for L2_NORM.
  std::string l2_norm_sql = norm_fn_rewrite_sql("L2_NORM", R"sql(
      (SELECT SQRT(SUM(CAST(e AS FLOAT64) * CAST(e AS FLOAT64)))
       FROM UNNEST(input_array) AS e WITH OFFSET index
    )sql");

  FunctionSignatureOptions l2_norm_signature_options =
      SetDefinitionForInlining(l2_norm_sql, true)
          .add_required_language_feature(FEATURE_V_1_4_L2_NORM);

  std::vector<FunctionSignatureOnHeap> l2_norm_signatures = {
      {types::DoubleType(),
       {distance_fn_named_arg(types::Int64ArrayType(), "input_array")},
       FN_L2_NORM_INT64,
       l2_norm_signature_options},
      {types::DoubleType(),
       {distance_fn_named_arg(types::FloatArrayType(), "input_array")},
       FN_L2_NORM_FLOAT,
       l2_norm_signature_options},
      {types::DoubleType(),
       {distance_fn_named_arg(types::DoubleArrayType(), "input_array")},
       FN_L2_NORM_DOUBLE,
       l2_norm_signature_options}};

  InsertFunction(functions, options, "l2_norm", Function::SCALAR,
                 l2_norm_signatures, function_options);

  return absl::OkStatus();
}

}  // namespace zetasql
