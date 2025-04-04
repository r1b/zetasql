#
# Copyright 2019 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# This is written with SELECT after ORDER BY and LIMIT.
# The other order would work too since SELECT preserves order.
FROM
  customer,
  orders,
  lineitem,
  nation
|> WHERE
    c_custkey = o_custkey
    AND l_orderkey = o_orderkey
    AND o_orderdate >= date '1994-02-01'
    AND o_orderdate < date_add(date '1994-02-01', INTERVAL 3 month)
    AND l_returnflag = 'R'
    AND c_nationkey = n_nationkey
|> AGGREGATE
    sum(l_extendedprice * (1 - l_discount)) AS revenue,
   GROUP BY
    c_custkey,
    c_name,
    c_acctbal,
    c_phone,
    n_name,
    c_address,
    c_comment
|> ORDER BY revenue DESC
|> LIMIT 20
|> SELECT
    c_custkey,
    c_name,
    revenue,
    c_acctbal,
    n_name,
    c_address,
    c_phone,
    c_comment;
