#include <gmock/gmock.h>

#include "pdk/pdk.hpp"

using ::testing::ElementsAre;

// check nothing mad happens with namespace resolution
namespace my_namespace {

using namespace lmadb::pdk;

template <typename R, typename P>
constexpr auto parser_returns(P)
{
  return std::is_same_v<parse_t<P>, R>;
}

// check literal parsers.
constexpr auto hello_parser = lit<'h', 'e', 'l', 'l', 'o'>;
static_assert(parser_returns<std::monostate>(hello_parser));

constexpr auto ihello_parser = ilit<'h', 'e', 'l', 'l', 'o'>;
static_assert(parser_returns<std::monostate>(ihello_parser));

namespace udl_test {

using namespace lmadb::pdk::literals;

constexpr auto hello_udl_parser = "hello"_lit;
constexpr auto ihello_udl_parser = "hello"_ilit;

static_assert(std::is_same_v<decltype(hello_parser), decltype(hello_udl_parser)>);
static_assert(std::is_same_v<decltype(ihello_parser), decltype(ihello_udl_parser)>);

} // namespace udl_test

static_assert(parser_returns<std::string_view>(alpha));
static_assert(parser_returns<std::string_view>(digit));
static_assert(parser_returns<std::string_view>(space));

constexpr auto get_size = [](auto x) {
  return x.size();
};

constexpr auto count_parser = alpha / get_size;
constexpr auto get_42_parser = hello_parser / [] { return 42; };

static_assert(parser_returns<std::size_t>(count_parser));
static_assert(parser_returns<int>(get_42_parser));

constexpr auto discard_parser = ~alpha;
static_assert(parser_returns<std::monostate>(discard_parser));

static_assert(parser_returns<std::int32_t>(int32));

constexpr auto hello_number_parser = hello_parser >> int32;
constexpr auto number_hello_parser = int32 >> hello_parser;

static_assert(parser_returns<std::int32_t>(hello_number_parser));
static_assert(parser_returns<std::int32_t>(number_hello_parser));

constexpr auto hello_world_parser = hello_parser >> lit<' ', 'w', 'o', 'r', 'l', 'd'>;
static_assert(parser_returns<std::monostate>(hello_world_parser));

constexpr auto two_words_parser = alpha >> lit<' '> >> alpha;
constexpr auto two_words_number_parser = two_words_parser >> lit<' '> >> int32;
constexpr auto number_two_words_parser = int32 >> lit<' '> >> two_words_parser;
constexpr auto two_words_number_two_words_parser = two_words_number_parser >> lit<' '> >> two_words_parser;

static_assert(parser_returns<std::tuple<std::string_view, std::string_view>>(two_words_parser));
static_assert(parser_returns<std::tuple<std::string_view, std::string_view, std::int32_t>>(two_words_number_parser));
static_assert(parser_returns<std::tuple<std::int32_t, std::string_view, std::string_view>>(number_two_words_parser));

using wwnww = std::tuple<std::string_view, std::string_view, std::int32_t, std::string_view, std::string_view>;
static_assert(parser_returns<wwnww>(two_words_number_two_words_parser));

constexpr auto maybe_word_parser = -alpha;
constexpr auto maybe_maybe_word_parser = -maybe_word_parser;

static_assert(parser_returns<std::optional<std::string_view>>(maybe_word_parser));
static_assert(parser_returns<std::optional<std::string_view>>(maybe_maybe_word_parser));

constexpr auto name_age_parser = alpha > int32;
static_assert(parser_returns<std::tuple<std::string_view, std::int32_t>>(name_age_parser));

constexpr auto alpha_or_digit_parser = alpha | digit;
static_assert(parser_returns<std::string_view>(alpha_or_digit_parser));

constexpr auto alpha_or_number_parser = alpha | int32;
static_assert(parser_returns<std::variant<std::string_view, std::int32_t>>(alpha_or_number_parser));

constexpr auto true_or_false_parser = lit<'t', 'r', 'u', 'e'> / to<true> | lit<'f', 'a', 'l', 's', 'e'> / to<false>;
constexpr auto alpha_or_number_or_bool_parser = alpha_or_number_parser | true_or_false_parser;
constexpr auto bool_or_alpha_or_number_parser = true_or_false_parser | alpha_or_number_parser;

static_assert(parser_returns<std::variant<std::string_view, std::int32_t, bool>>(alpha_or_number_or_bool_parser));
static_assert(parser_returns<std::variant<bool, std::string_view, std::int32_t>>(bool_or_alpha_or_number_parser));

constexpr auto bool_nil_alpha_or_number_parser = (true_or_false_parser | lit<'n', 'i', 'l'>) | alpha_or_number_parser;

static_assert(parser_returns<std::variant<bool, std::monostate, std::string_view, std::int32_t>>(bool_nil_alpha_or_number_parser));

enum class roman_numerals {
  one, two, three, four, five
};

constexpr auto roman_numeral_parser = [] {
  using namespace lmadb::pdk::literals;

  return "V"_lit / to<roman_numerals::five>
    | "IV"_lit / to<roman_numerals::four>
    | "III"_lit / to<roman_numerals::three>
    | "II"_lit / to<roman_numerals::two>
    | "I"_lit / to<roman_numerals::one>;
}();

static_assert(parser_returns<roman_numerals>(roman_numeral_parser));

constexpr auto bools_parser = +true_or_false_parser;
constexpr auto kleen_bools_parser = *true_or_false_parser;

static_assert(parser_returns<std::vector<bool>>(bools_parser));
static_assert(parser_returns<std::vector<bool>>(kleen_bools_parser));

constexpr auto csv_parser = alpha % lit<','>;

static_assert(parser_returns<std::vector<std::string_view>>(csv_parser));

// TODO:
// constexpr auto complex_csv_parser = name_age_parser % lit<','>
// static_assert(parser_returns<std::vector<std::tuple<std::string_view, std::int32_t>>>(csv_parser));

TEST(HelloParser, Matches)
{
  const auto input = "hello world!";
  const auto result = hello_parser(input);

  ASSERT_TRUE(result);
  ASSERT_EQ(result->first, std::monostate{});
  ASSERT_EQ(result->second, " world!");
}

// std::tuple<std::string_view, int, std::vector<std::tuple<std::string_view, int>>>

TEST(HelloParser, DoesntMatch)
{
  const auto input = "hElLO world!";
  const auto result = hello_parser(input);

  ASSERT_FALSE(result);
}

TEST(CaseInsensitiveHelloParser, Matches)
{
  const auto input = "hElLO world!";
  const auto result = ihello_parser(input);

  ASSERT_TRUE(result);
  ASSERT_EQ(result->first, std::monostate{});
  ASSERT_EQ(result->second, " world!");
}

TEST(CaseInsensitiveHelloParser, DoesntMatch)
{
  const auto input = "hell0 world!";
  const auto result = ihello_parser(input);

  ASSERT_FALSE(result);
}

TEST(AlphaParser, Matches)
{
  const auto input = "hello world!";
  const auto result = alpha(input);

  ASSERT_TRUE(result);
  ASSERT_EQ(result->first, "hello");
  ASSERT_EQ(result->second, " world!");
}

TEST(AlphaParser, DoesntMatch)
{
  const auto input = " hello world!";
  const auto result = alpha(input);

  ASSERT_FALSE(result);
}

TEST(SpaceParser, Matches)
{
  const auto input = "\t hello world!";
  const auto result = space(input);

  ASSERT_TRUE(result);
  ASSERT_EQ(result->first, "\t ");
  ASSERT_EQ(result->second, "hello world!");
}

TEST(SpaceParser, DoesntMatch)
{
  const auto input = "hello world!";
  const auto result = space(input);

  ASSERT_FALSE(result);
}

TEST(CountParser, Matches)
{
  const auto input = "hello world!";
  const auto result = count_parser(input);

  ASSERT_TRUE(result);
  ASSERT_EQ(result->first, 5);
  ASSERT_EQ(result->second, " world!");
}

TEST(CountParser, DoesntMatch)
{
  const auto input = " hello world!";
  const auto result = count_parser(input);

  ASSERT_FALSE(result);
}

TEST(Get42Parser, Matches)
{
  const auto input = "hello world!";
  const auto result = get_42_parser(input);

  ASSERT_TRUE(result);
  ASSERT_EQ(result->first, 42);
  ASSERT_EQ(result->second, " world!");
}

TEST(Get42Parser, DoesntMatch)
{
  const auto input = " hello world!";
  const auto result = get_42_parser(input);

  ASSERT_FALSE(result);
}

TEST(DiscardParser, Matches)
{
  const auto input = "hello world!";
  const auto result = discard_parser(input);

  ASSERT_TRUE(result);
  ASSERT_EQ(result->first, std::monostate{});
  ASSERT_EQ(result->second, " world!");
}

TEST(DiscardParser, DoesntMatch)
{
  const auto input = "42 world!";
  const auto result = discard_parser(input);

  ASSERT_FALSE(result);
}

TEST(Int32Parser, Matches)
{
  const auto input = "42 world!";
  const auto result = int32(input);

  ASSERT_TRUE(result);
  ASSERT_EQ(result->first, 42);
  ASSERT_EQ(result->second, " world!");
}

TEST(Int32Parser, DoesntMatch)
{
  const auto input = "hello world!";
  const auto result = int32(input);

  ASSERT_FALSE(result);
}

TEST(HelloNumberParser, Matches)
{
  const auto input = "hello99!";
  const auto result = hello_number_parser(input);

  ASSERT_TRUE(result);
  ASSERT_EQ(result->first, 99);
  ASSERT_EQ(result->second, "!");
}

TEST(HelloNumberParser, DoesntMatch)
{
  const auto input = "hello 99!";
  const auto result = hello_number_parser(input);

  ASSERT_FALSE(result);
}

TEST(NumberHelloParser, Matches)
{
  const auto input = "42hello!";
  const auto result = number_hello_parser(input);

  ASSERT_TRUE(result);
  ASSERT_EQ(result->first, 42);
  ASSERT_EQ(result->second, "!");
}

TEST(NumberHelloParser, DoesntMatch)
{
  const auto input = "hello99!";
  const auto result = number_hello_parser(input);

  ASSERT_FALSE(result);
}

TEST(HelloWorldParser, Matches)
{
  const auto input = "hello world!";
  const auto result = hello_world_parser(input);

  ASSERT_TRUE(result);
  ASSERT_EQ(result->first, std::monostate{});
  ASSERT_EQ(result->second, "!");
}

TEST(HelloWorldParser, DoesntMatch)
{
  const auto input = "helloworld!";
  const auto result = hello_world_parser(input);

  ASSERT_FALSE(result);
}

TEST(MaybeWordParser, Matches)
{
  const auto input = "hello world!";
  const auto result = maybe_word_parser(input);

  ASSERT_TRUE(result);
  ASSERT_EQ(result->first, "hello");
  ASSERT_EQ(result->second, " world!");
}

TEST(MaybeWordParser, MatchesEmpty)
{
  const auto input = "42 world!";
  const auto result = maybe_word_parser(input);

  ASSERT_TRUE(result);
  ASSERT_EQ(result->first, std::nullopt);
  ASSERT_EQ(result->second, "42 world!");
}

TEST(MaybeMaybeWordParser, Matches)
{
  const auto input = "hello world!";
  const auto result = maybe_maybe_word_parser(input);

  ASSERT_TRUE(result);
  ASSERT_EQ(result->first, "hello");
  ASSERT_EQ(result->second, " world!");
}

TEST(MaybeMaybeWordParser, MatchesEmpty)
{
  const auto input = "42 world!";
  const auto result = maybe_maybe_word_parser(input);

  ASSERT_TRUE(result);
  ASSERT_EQ(result->first, std::nullopt);
  ASSERT_EQ(result->second, "42 world!");
}

TEST(NameAgeParser, Matches)
{
  const auto input = "sam 42!";
  const auto result = name_age_parser(input);

  ASSERT_TRUE(result);
  ASSERT_EQ(result->first, std::make_tuple("sam", 42));
  ASSERT_EQ(result->second, "!");
}

TEST(NameAgeParser, MatchesWithoutSpaces)
{
  const auto input = "sam42!";
  const auto result = name_age_parser(input);

  ASSERT_TRUE(result);
  ASSERT_EQ(result->first, std::make_tuple("sam", 42));
  ASSERT_EQ(result->second, "!");
}

TEST(AlphaOrDigitParser, MatchesAlpha)
{
  const auto input = "sam42!";
  const auto result = alpha_or_digit_parser(input);

  ASSERT_TRUE(result);
  ASSERT_EQ(result->first, "sam");
  ASSERT_EQ(result->second, "42!");
}

TEST(AlphaOrDigitParser, MatchesDigit)
{
  const auto input = "42!";
  const auto result = alpha_or_digit_parser(input);

  ASSERT_TRUE(result);
  ASSERT_EQ(result->first, "42");
  ASSERT_EQ(result->second, "!");
}

TEST(AlphaOrDigitParser, DoesntMatch)
{
  const auto input = "!42!";
  const auto result = alpha_or_digit_parser(input);

  ASSERT_FALSE(result);
}

TEST(AlphaOrNumberParser, MatchesAlpha)
{
  const auto input = "sam42!";
  const auto result = alpha_or_number_parser(input);

  ASSERT_TRUE(result);
  ASSERT_EQ(std::get<0>(result->first), "sam");
  ASSERT_EQ(result->second, "42!");
}

TEST(AlphaOrNumberParser, MatchesDigit)
{
  const auto input = "42!";
  const auto result = alpha_or_number_parser(input);

  ASSERT_TRUE(result);
  ASSERT_EQ(std::get<1>(result->first), 42);
  ASSERT_EQ(result->second, "!");
}

TEST(AlphaOrNumberParser, DoesntMatch)
{
  const auto input = "!42!";
  const auto result = alpha_or_number_parser(input);

  ASSERT_FALSE(result);
}

TEST(BoolNilAlphaOrNumberParser, MatchesBool)
{
  const auto input = "true!";
  const auto result = bool_nil_alpha_or_number_parser(input);

  ASSERT_TRUE(result);
  ASSERT_EQ(std::get<0>(result->first), true);
  ASSERT_EQ(result->second, "!");
}

TEST(BoolNilAlphaOrNumberParser, MatchesNil)
{
  const auto input = "nil!";
  const auto result = bool_nil_alpha_or_number_parser(input);

  ASSERT_TRUE(result);
  ASSERT_EQ(std::get<1>(result->first), std::monostate{});
  ASSERT_EQ(result->second, "!");
}

TEST(BoolNilAlphaOrNumberParser, MatchesAlpha)
{
  const auto input = "sam!";
  const auto result = bool_nil_alpha_or_number_parser(input);

  ASSERT_TRUE(result);
  ASSERT_EQ(std::get<2>(result->first), "sam");
  ASSERT_EQ(result->second, "!");
}

TEST(BoolNilAlphaOrNumberParser, MatchesNumber)
{
  const auto input = "42!";
  const auto result = bool_nil_alpha_or_number_parser(input);

  ASSERT_TRUE(result);
  ASSERT_EQ(std::get<3>(result->first), 42);
  ASSERT_EQ(result->second, "!");
}

TEST(BoolsParser, MatchesOne)
{
  const auto input = "true!";
  const auto result = bools_parser(input);

  ASSERT_TRUE(result);
  ASSERT_THAT(result->first, ElementsAre(true));
  ASSERT_EQ(result->second, "!");
}

TEST(BoolsParser, MatchesTwo)
{
  const auto input = "falsetrue!";
  const auto result = bools_parser(input);

  ASSERT_TRUE(result);
  ASSERT_THAT(result->first, ElementsAre(false, true));
  ASSERT_EQ(result->second, "!");
}

TEST(BoolsParser, MatchesNone)
{
  const auto input = "1falsetrue!";
  const auto result = bools_parser(input);

  ASSERT_FALSE(result);
}

TEST(KleenBoolsParser, MatchesOne)
{
  const auto input = "true!";
  const auto result = kleen_bools_parser(input);

  ASSERT_TRUE(result);
  ASSERT_THAT(result->first, ElementsAre(true));
  ASSERT_EQ(result->second, "!");
}

TEST(KleenBoolsParser, MatchesTwo)
{
  const auto input = "falsetrue!";
  const auto result = kleen_bools_parser(input);

  ASSERT_TRUE(result);
  ASSERT_THAT(result->first, ElementsAre(false, true));
  ASSERT_EQ(result->second, "!");
}

TEST(KleenBoolsParser, MatchesNone)
{
  const auto input = "1falsetrue!";
  const auto result = kleen_bools_parser(input);

  ASSERT_TRUE(result);
  ASSERT_TRUE(result->first.empty());
  ASSERT_EQ(result->second, "1falsetrue!");
}

// TEST(CsvParser, MatchesOne)
// {
//   const auto input = "sam!";
//   const auto result = csv_parser(input);

//   ASSERT_TRUE(result);
//   ASSERT_THAT(result->first, ElementsAre("sam"));
//   ASSERT_EQ(result->second, "!");
// }

// TEST(CsvParser, MatchesTwo)
// {
//   const auto input = "sam, tom!";
//   const auto result = csv_parser(input);

//   ASSERT_TRUE(result);
//   ASSERT_THAT(result->first, ElementsAre("sam", "tom"));
//   ASSERT_EQ(result->second, "!");
// }

// TEST(CsvParser, MatchesNone)
// {
//   const auto input = "42, sam!";
//   const auto result = csv_parser(input);

//   ASSERT_FALSE(result);
// }

} // namespace my_namespace