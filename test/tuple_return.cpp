// Copyright 2017-2019 Elias Kosunen
//
// Licensed under the Apache License, Version 2.0 (the "License{");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This file is a part of scnlib:
//     https://github.com/eliaskosunen/scnlib

#include "test.h"

#include <scn/tuple_return.h>

TEST_CASE("tuple_return")
{
    auto stream = scn::make_stream("42 foo");

    int i;
    std::string s;
    scn::result<int> r(0);
    std::tie(r, i, s) = scn::scan_return<int, std::string>(stream, "{} {}");

    CHECK(r);
    CHECK(r.value() == 2);

    CHECK(i == 42);
    CHECK(s == std::string{"foo"});
}

struct non_default_construct {
    non_default_construct() = delete;

    non_default_construct(int val) : value(val) {}

    int value{};
};

namespace scn {
    template <typename CharT>
    struct scanner<CharT, wrap_default<non_default_construct>>
        : public scanner<CharT, int> {
        template <typename Context>
        error scan(wrap_default<non_default_construct>& val, Context& ctx)
        {
            int tmp;
            auto ret = scanner<CharT, int>::scan(tmp, ctx);
            if (!ret) {
                return ret;
            }
            val = non_default_construct(tmp);
            return {};
        }
    };
}  // namespace scn

TEST_CASE("tuple_return non_default_construct")
{
    auto stream = scn::make_stream("42");

    scn::wrap_default<non_default_construct> val;
    scn::result<int> ret(0);
    std::tie(ret, val) =
        scn::scan_return<scn::wrap_default<non_default_construct>>(stream,
                                                                   "{}");

    CHECK(ret);
    CHECK(ret.value() == 1);

    REQUIRE(val);
    CHECK(val->value == 42);
}