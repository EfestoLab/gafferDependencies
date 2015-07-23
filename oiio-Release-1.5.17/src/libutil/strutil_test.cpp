/*
  Copyright 2010 Larry Gritz and the other authors and contributors.
  All Rights Reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of the software's owners nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  (This is the Modified BSD License)
*/

#include <cstdio>

#include "OpenImageIO/strutil.h"
#include "OpenImageIO/unittest.h"

OIIO_NAMESPACE_USING;
using namespace Strutil;



void test_format ()
{
    // Test formatting
    OIIO_CHECK_EQUAL (Strutil::format ("%d %f %g", int(3), 3.14f, 3.14f),
                      "3 3.140000 3.14");
    OIIO_CHECK_EQUAL (Strutil::format ("'%s' '%s'", "foo", std::string("foo")),
                      "'foo' 'foo'");
    OIIO_CHECK_EQUAL (Strutil::format ("'%3d' '%03d' '%-3d'", 3, 3, 3),
                      "'  3' '003' '3  '");

    // Test '+' format modifier
    OIIO_CHECK_EQUAL (Strutil::format ("%+d%+d%+d", 3, -3, 0), "+3-3+0");

    // FIXME -- we should make comprehensive tests here
}



void test_memformat ()
{
    OIIO_CHECK_EQUAL (Strutil::memformat (15), "15 B");
    OIIO_CHECK_EQUAL (Strutil::memformat (15LL*1024), "15 KB");
    OIIO_CHECK_EQUAL (Strutil::memformat (15LL*1024*1024), "15.0 MB");
    OIIO_CHECK_EQUAL (Strutil::memformat (15LL*1024*1024*1024), "15.0 GB");
    OIIO_CHECK_EQUAL (Strutil::memformat (15LL*1024*1024+200000), "15.2 MB");
    OIIO_CHECK_EQUAL (Strutil::memformat (15LL*1024*1024+200000, 3), "15.191 MB");
}



void test_timeintervalformat ()
{
    OIIO_CHECK_EQUAL (Strutil::timeintervalformat (15.321), "15.3s");
    OIIO_CHECK_EQUAL (Strutil::timeintervalformat (150.321), "2m 30.3s");
    OIIO_CHECK_EQUAL (Strutil::timeintervalformat (15000.321), "4h 10m 0.3s");
    OIIO_CHECK_EQUAL (Strutil::timeintervalformat (150000.321), "1d 17h 40m 0.3s");
    OIIO_CHECK_EQUAL (Strutil::timeintervalformat (150.321, 2), "2m 30.32s");
}



void test_get_rest_arguments ()
{
    int ret;
    std::map <std::string, std::string> result;
    std::string base;
    std::string url = "someplace?arg1=value1&arg2=value2";
    ret = Strutil::get_rest_arguments (url, base, result);
    OIIO_CHECK_EQUAL (ret, true);
    OIIO_CHECK_EQUAL (base, "someplace");
    OIIO_CHECK_EQUAL (result["arg1"], "value1");
    OIIO_CHECK_EQUAL (result["arg2"], "value2");
    OIIO_CHECK_EQUAL (result["arg3"], "");

    result.clear();
    url = "?arg1=value1&arg2=value2";
    ret = Strutil::get_rest_arguments (url, base, result);
    OIIO_CHECK_EQUAL (ret, true);
    OIIO_CHECK_EQUAL (base, "");
    OIIO_CHECK_EQUAL (result["arg1"], "value1");
    OIIO_CHECK_EQUAL (result["arg2"], "value2");

    result.clear();
    url = "arg1=value1&arg2=value2";
    ret = Strutil::get_rest_arguments (url, base, result);
    OIIO_CHECK_EQUAL (ret, true);
    OIIO_CHECK_EQUAL (base, "arg1=value1&arg2=value2");
    OIIO_CHECK_EQUAL (result["arg1"], "");
    OIIO_CHECK_EQUAL (result["arg2"], "");

    result.clear();
    url = "";
    ret = Strutil::get_rest_arguments (url, base, result);
    OIIO_CHECK_EQUAL (ret, true);
    OIIO_CHECK_EQUAL (base, "");
    OIIO_CHECK_EQUAL (result["arg1"], "");
    OIIO_CHECK_EQUAL (result["arg2"], "");

    result.clear();
    url = "sometextwithoutasense????&&&&&arg4=val1";
    ret = Strutil::get_rest_arguments (url, base, result);
    OIIO_CHECK_EQUAL (ret, false);
    OIIO_CHECK_EQUAL (base, "sometextwithoutasense");
    OIIO_CHECK_EQUAL (result["arg1"], "");
    OIIO_CHECK_EQUAL (result["arg2"], "");
    OIIO_CHECK_EQUAL (result["arg4"], "");

    result.clear();
    url = "atext?arg1value1&arg2value2";
    ret = Strutil::get_rest_arguments (url, base, result);
    OIIO_CHECK_EQUAL (ret, false);
    OIIO_CHECK_EQUAL (base, "atext");
    OIIO_CHECK_EQUAL (result["arg1"], "");
    OIIO_CHECK_EQUAL (result["arg2"], "");

    result.clear();
    url = "atext?arg1=value1&arg2value2";
    result["arg2"] = "somevalue";
    ret = Strutil::get_rest_arguments (url, base, result);
    OIIO_CHECK_EQUAL (ret, false);
    OIIO_CHECK_EQUAL (base, "atext");
    OIIO_CHECK_EQUAL (result["arg1"], "value1");
    OIIO_CHECK_EQUAL (result["arg2"], "somevalue");
}



void test_escape_sequences ()
{
    OIIO_CHECK_EQUAL (Strutil::unescape_chars("\\\\ \\n \\r \\017"),
                      "\\ \n \r \017");
    OIIO_CHECK_EQUAL (Strutil::escape_chars("\\ \n \r"),
                      "\\\\ \\n \\r");
}



void test_wordwrap ()
{
    std::string words = "Now is the time for all good men to come to the aid of their party.";
    OIIO_CHECK_EQUAL (Strutil::wordwrap(words, 24),
                      "Now is the time for all\n"
                      "good men to come to the\n"
                      "aid of their party.");
}



void test_comparisons ()
{
    OIIO_CHECK_EQUAL (Strutil::iequals ("abc", "abc"), true);
    OIIO_CHECK_EQUAL (Strutil::iequals ("Abc", "aBc"), true);
    OIIO_CHECK_EQUAL (Strutil::iequals ("abc", "adc"), false);

    OIIO_CHECK_EQUAL (Strutil::istarts_with ("abcd", "ab"), true);
    OIIO_CHECK_EQUAL (Strutil::istarts_with ("aBcd", "Ab"), true);
    OIIO_CHECK_EQUAL (Strutil::istarts_with ("abcd", "ba"), false);
    OIIO_CHECK_EQUAL (Strutil::istarts_with ("abcd", "abcde"), false);
    OIIO_CHECK_EQUAL (Strutil::istarts_with ("", "a"), false);
    OIIO_CHECK_EQUAL (Strutil::istarts_with ("", ""), true);
    OIIO_CHECK_EQUAL (Strutil::istarts_with ("abc", ""), true);

    OIIO_CHECK_EQUAL (Strutil::iends_with ("abcd", "cd"), true);
    OIIO_CHECK_EQUAL (Strutil::iends_with ("aBCd", "cd"), true);
    OIIO_CHECK_EQUAL (Strutil::iends_with ("aBcd", "CD"), true);
    OIIO_CHECK_EQUAL (Strutil::iends_with ("abcd", "ba"), false);
    OIIO_CHECK_EQUAL (Strutil::iends_with ("abcd", "xabcd"), false);
    OIIO_CHECK_EQUAL (Strutil::iends_with ("", "a"), false);
    OIIO_CHECK_EQUAL (Strutil::iends_with ("", ""), true);
    OIIO_CHECK_EQUAL (Strutil::iends_with ("abc", ""), true);

    OIIO_CHECK_EQUAL (Strutil::contains ("abcde", "ab"), true);
    OIIO_CHECK_EQUAL (Strutil::contains ("abcde", "bcd"), true);
    OIIO_CHECK_EQUAL (Strutil::contains ("abcde", "de"), true);
    OIIO_CHECK_EQUAL (Strutil::contains ("abcde", "cdx"), false);
    OIIO_CHECK_EQUAL (Strutil::contains ("abcde", ""), true);
    OIIO_CHECK_EQUAL (Strutil::contains ("", ""), true);
    OIIO_CHECK_EQUAL (Strutil::contains ("", "x"), false);

    OIIO_CHECK_EQUAL (Strutil::icontains ("abcde", "ab"), true);
    OIIO_CHECK_EQUAL (Strutil::icontains ("Abcde", "aB"), true);
    OIIO_CHECK_EQUAL (Strutil::icontains ("abcde", "bcd"), true);
    OIIO_CHECK_EQUAL (Strutil::icontains ("Abcde", "bCd"), true);
    OIIO_CHECK_EQUAL (Strutil::icontains ("abcDe", "dE"), true);
    OIIO_CHECK_EQUAL (Strutil::icontains ("abcde", "cdx"), false);
    OIIO_CHECK_EQUAL (Strutil::icontains ("abcde", ""), true);
    OIIO_CHECK_EQUAL (Strutil::icontains ("", ""), true);
    OIIO_CHECK_EQUAL (Strutil::icontains ("", "x"), false);
}



void test_case ()
{
    std::string s;
    s = "abcDEF,*1";
    Strutil::to_lower (s);
    OIIO_CHECK_EQUAL (s, "abcdef,*1");
    s = "abcDEF,*1";
    Strutil::to_upper (s);
    OIIO_CHECK_EQUAL (s, "ABCDEF,*1");
}



void test_strip ()
{
    OIIO_CHECK_EQUAL (Strutil::strip ("abcdefbac", "abc"), "def");
    OIIO_CHECK_EQUAL (Strutil::strip ("defghi", "abc"), "defghi");
    OIIO_CHECK_EQUAL (Strutil::strip ("  \tHello, world\n"), "Hello, world");
    OIIO_CHECK_EQUAL (Strutil::strip (" \t"), "");
    OIIO_CHECK_EQUAL (Strutil::strip (""), "");
}



void test_split ()
{
    std::string s ("Now\nis the  time!");
    std::vector<string_view> splits;

    // test default -- split at whitespace
    Strutil::split (s, splits);
    OIIO_CHECK_EQUAL (splits.size(), 4);
    OIIO_CHECK_EQUAL (splits[0], "Now");
    OIIO_CHECK_EQUAL (splits[1], "is");
    OIIO_CHECK_EQUAL (splits[2], "the");
    OIIO_CHECK_EQUAL (splits[3], "time!");

    // test custom split string
    Strutil::split (s, splits, " t");
    OIIO_CHECK_EQUAL (splits.size(), 3);
    OIIO_CHECK_EQUAL (splits[0], "Now\nis");
    OIIO_CHECK_EQUAL (splits[1], "he ");
    OIIO_CHECK_EQUAL (splits[2], "ime!");

    // test maxsplit
    Strutil::split (s, splits, "", 2);
    OIIO_CHECK_EQUAL (splits.size(), 2);
    OIIO_CHECK_EQUAL (splits[0], "Now");
    OIIO_CHECK_EQUAL (splits[1], "is the  time!");

    // test maxsplit with non-default sep
    Strutil::split (s, splits, " ", 2);
    OIIO_CHECK_EQUAL (splits.size(), 2);
    OIIO_CHECK_EQUAL (splits[0], "Now\nis");
    OIIO_CHECK_EQUAL (splits[1], "the  time!");
}



void test_join ()
{
    std::vector<std::string> seq;
    seq.push_back ("Now");
    seq.push_back ("is");
    seq.push_back ("the");
    seq.push_back ("time");
    OIIO_CHECK_EQUAL (Strutil::join (seq, ". "),
                      "Now. is. the. time");
}



void test_conversion ()
{
    OIIO_CHECK_EQUAL (Strutil::from_string<int>("hi"), 0);
    OIIO_CHECK_EQUAL (Strutil::from_string<int>("123"), 123);
    OIIO_CHECK_EQUAL (Strutil::from_string<int>("-123"), -123);
    OIIO_CHECK_EQUAL (Strutil::from_string<int>(" 123 "), 123);
    OIIO_CHECK_EQUAL (Strutil::from_string<int>("123.45"), 123);

    OIIO_CHECK_EQUAL (Strutil::from_string<float>("hi"), 0.0f);
    OIIO_CHECK_EQUAL (Strutil::from_string<float>("123"), 123.0f);
    OIIO_CHECK_EQUAL (Strutil::from_string<float>("-123"), -123.0f);
    OIIO_CHECK_EQUAL (Strutil::from_string<float>("123.45"), 123.45f);
    OIIO_CHECK_EQUAL (Strutil::from_string<float>(" 123.45 "), 123.45f);
    OIIO_CHECK_EQUAL (Strutil::from_string<float>("123.45+12"), 123.45f);
    OIIO_CHECK_EQUAL (Strutil::from_string<float>("1.2345e+2"), 123.45f);
}



void test_extract ()
{
    std::vector<int> vals;
    int n;

    vals.clear(); vals.resize (3, -1);
    n = Strutil::extract_from_list_string (vals, "1");
    OIIO_CHECK_EQUAL (vals.size(), 3);
    OIIO_CHECK_EQUAL (vals[0], 1);
    OIIO_CHECK_EQUAL (vals[1], 1);
    OIIO_CHECK_EQUAL (vals[2], 1);
    OIIO_CHECK_EQUAL (n, 1);

    vals.clear(); vals.resize (3, -1);
    n = Strutil::extract_from_list_string (vals, "1,3,5");
    OIIO_CHECK_EQUAL (vals.size(), 3);
    OIIO_CHECK_EQUAL (vals[0], 1);
    OIIO_CHECK_EQUAL (vals[1], 3);
    OIIO_CHECK_EQUAL (vals[2], 5);
    OIIO_CHECK_EQUAL (n, 3);

    vals.clear(); vals.resize (3, -1);
    n = Strutil::extract_from_list_string (vals, "1,,5");
    OIIO_CHECK_EQUAL (vals.size(), 3);
    OIIO_CHECK_EQUAL (vals[0], 1);
    OIIO_CHECK_EQUAL (vals[1], -1);
    OIIO_CHECK_EQUAL (vals[2], 5);
    OIIO_CHECK_EQUAL (n, 3);

    vals.clear(); vals.resize (3, -1);
    n = Strutil::extract_from_list_string (vals, "abc");
    OIIO_CHECK_EQUAL (vals.size(), 3);
    OIIO_CHECK_EQUAL (vals[0], 0);
    OIIO_CHECK_EQUAL (vals[1], 0);
    OIIO_CHECK_EQUAL (vals[2], 0);
    OIIO_CHECK_EQUAL (n, 1);

    vals.clear(); vals.resize (3, -1);
    n = Strutil::extract_from_list_string (vals, "");
    OIIO_CHECK_EQUAL (vals.size(), 3);
    OIIO_CHECK_EQUAL (vals[0], -1);
    OIIO_CHECK_EQUAL (vals[1], -1);
    OIIO_CHECK_EQUAL (vals[2], -1);
    OIIO_CHECK_EQUAL (n, 0);
}



void test_safe_strcpy ()
{
    { // test in-bounds copy
        char result[4] = { '0', '1', '2', '3' };
        Strutil::safe_strcpy (result, "A", 3);
        OIIO_CHECK_EQUAL (result[0], 'A');
        OIIO_CHECK_EQUAL (result[1],  0);
        OIIO_CHECK_EQUAL (result[2], '2'); // should not overwrite
        OIIO_CHECK_EQUAL (result[3], '3'); // should not overwrite
    }
    { // test over-bounds copy
        char result[4] = { '0', '1', '2', '3' };
        Strutil::safe_strcpy (result, "ABC", 3);
        OIIO_CHECK_EQUAL (result[0], 'A');
        OIIO_CHECK_EQUAL (result[1], 'B');
        OIIO_CHECK_EQUAL (result[2],  0);
        OIIO_CHECK_EQUAL (result[3], '3'); // should not overwrite
    }
    { // test empty string copy
        char result[4] = { '0', '1', '2', '3' };
        Strutil::safe_strcpy (result, "", 3);
        OIIO_CHECK_EQUAL (result[0], 0);
        OIIO_CHECK_EQUAL (result[1], '1'); // should not overwrite
        OIIO_CHECK_EQUAL (result[2], '2'); // should not overwrite
        OIIO_CHECK_EQUAL (result[3], '3'); // should not overwrite
    }
    { // test NULL case
        char result[4] = { '0', '1', '2', '3' };
        Strutil::safe_strcpy (result, NULL, 3);
        OIIO_CHECK_EQUAL (result[0], 0);
        OIIO_CHECK_EQUAL (result[1], '1'); // should not overwrite
        OIIO_CHECK_EQUAL (result[2], '2'); // should not overwrite
        OIIO_CHECK_EQUAL (result[3], '3'); // should not overwrite
    }
}



// test some of the trickier methods in string_view.
void test_string_view ()
{
    std::string s("0123401234");
    string_view sr (s);

    OIIO_CHECK_EQUAL (sr.find("123"), s.find("123"));
    OIIO_CHECK_EQUAL (sr.find("123"), 1);
    OIIO_CHECK_EQUAL (sr.find("143"), string_view::npos);
    OIIO_CHECK_EQUAL (sr.find("123", 4), s.find("123", 4));
    OIIO_CHECK_EQUAL (sr.find("123", 4), 6);
    OIIO_CHECK_EQUAL (sr.find("143", 4), string_view::npos);

    OIIO_CHECK_EQUAL (sr.find('1'), s.find('1'));
    OIIO_CHECK_EQUAL (sr.find('1'), 1);
    OIIO_CHECK_EQUAL (sr.find('5'), string_view::npos);
    OIIO_CHECK_EQUAL (sr.find('1', 4), s.find('1', 4));
    OIIO_CHECK_EQUAL (sr.find('1', 4), 6);
    OIIO_CHECK_EQUAL (sr.find('5', 4), string_view::npos);

    OIIO_CHECK_EQUAL (sr.rfind("123"), s.rfind("123"));
    OIIO_CHECK_EQUAL (sr.rfind("123"), 6);
    OIIO_CHECK_EQUAL (sr.rfind("1234"), 6);
    OIIO_CHECK_EQUAL (sr.rfind("143"), string_view::npos);
    OIIO_CHECK_EQUAL (sr.rfind("123", 5), s.rfind("123", 5));
    OIIO_CHECK_EQUAL (sr.rfind("123", 5), 1);
    OIIO_CHECK_EQUAL (sr.rfind("123", 4), 1);
    OIIO_CHECK_EQUAL (sr.rfind("143", 5), string_view::npos);
    OIIO_CHECK_EQUAL (sr.rfind("012", 4), 0);

    OIIO_CHECK_EQUAL (sr.rfind('1'), s.rfind('1'));
    OIIO_CHECK_EQUAL (sr.rfind('1'), 6);
    OIIO_CHECK_EQUAL (sr.rfind('5'), string_view::npos);
    OIIO_CHECK_EQUAL (sr.rfind('1', 4), s.rfind('1', 4));
    OIIO_CHECK_EQUAL (sr.rfind('1', 4), 1);
    OIIO_CHECK_EQUAL (sr.rfind('5', 4), string_view::npos);
}



void test_parse ()
{
    std::cout << "Testing parse functions\n";
    string_view s;
    s = "";        skip_whitespace(s);  OIIO_CHECK_EQUAL (s, "");
    s = "   ";     skip_whitespace(s);  OIIO_CHECK_EQUAL (s, "");
    s = "foo";     skip_whitespace(s);  OIIO_CHECK_EQUAL (s, "foo");
    s = "  foo  "; skip_whitespace(s);  OIIO_CHECK_EQUAL (s, "foo  ");

    s = "abc"; OIIO_CHECK_ASSERT (! parse_char (s, 'd') && s == "abc");

    s = "abc"; OIIO_CHECK_ASSERT (parse_char (s, 'a', true, false) && s == "abc");
    s = "abc"; OIIO_CHECK_ASSERT (parse_char (s, 'a') && s == "bc");

    s = "abc"; OIIO_CHECK_ASSERT (parse_until_char (s, 'c', false) && s == "abc");
    s = "abc"; OIIO_CHECK_ASSERT (parse_until_char (s, 'c') && s == "c");
    s = "abc"; OIIO_CHECK_ASSERT (! parse_until_char (s, 'd') && s == "");

    s = "abcdef";
    OIIO_CHECK_ASSERT (! parse_prefix (s, "def", false) && s == "abcdef");
    OIIO_CHECK_ASSERT (parse_prefix (s, "abc", false) && s == "abcdef");
    OIIO_CHECK_ASSERT (parse_prefix (s, "abc") && s == "def");

    int i = 0;
    s = "abc"; OIIO_CHECK_ASSERT (! parse_int (s, i) && s == "abc");
    s = " 143 abc"; OIIO_CHECK_ASSERT (parse_int (s, i) && i == 143 && s == " abc");
    s = " 143 abc"; OIIO_CHECK_ASSERT (parse_int (s, i, false) && i == 143 && s == " 143 abc");

    float f = 0;
    s = "abc"; OIIO_CHECK_ASSERT (! parse_float (s, f) && s == "abc");
    s = " 42.1 abc"; OIIO_CHECK_ASSERT (parse_float (s, f) && f == 42.1f && s == " abc");
    s = " 42.1 abc"; OIIO_CHECK_ASSERT (parse_float (s, f, false) && f == 42.1f && s == " 42.1 abc");

    string_view ss;
    s = "foo bar";
    OIIO_CHECK_ASSERT (parse_string (s, ss) && ss == "foo" && s == " bar");
    s = "\"foo bar\" baz";
    OIIO_CHECK_ASSERT (parse_string (s, ss) && ss == "foo bar" && s == " baz");
    s = "\"foo bar\" baz";
    OIIO_CHECK_ASSERT (parse_string (s, ss, false) && ss == "foo bar" && s == "\"foo bar\" baz");
    s = "\"foo bar\" baz";
    OIIO_CHECK_ASSERT (parse_string (s, ss, true, KeepQuotes) && ss == "\"foo bar\"" && s == " baz");

    s = " foo bar"; ss = parse_word (s);
    OIIO_CHECK_ASSERT (ss == "foo" && s == " bar");
    s = " 14 foo bar"; ss = parse_word (s);
    OIIO_CHECK_ASSERT (ss.size() == 0 && s == " 14 foo bar");
    s = "foo14 bar"; ss = parse_word (s);
    OIIO_CHECK_ASSERT (ss == "foo" && s == "14 bar");
    s = " foo bar"; ss = parse_word (s, false);
    OIIO_CHECK_ASSERT (ss == "foo" && s == " foo bar");

    s = " foo bar"; ss = parse_identifier (s);
    OIIO_CHECK_ASSERT (ss == "foo" && s == " bar");
    s = " 14 foo bar"; ss = parse_identifier (s);
    OIIO_CHECK_ASSERT (ss.size() == 0 && s == " 14 foo bar");
    s = " foo_14 bar"; ss = parse_identifier (s);
    OIIO_CHECK_ASSERT (ss == "foo_14" && s == " bar");
    s = " foo_14 bar"; ss = parse_identifier (s, false);
    OIIO_CHECK_ASSERT (ss == "foo_14" && s == " foo_14 bar");
    s = "fl$orp 14";  ss = parse_identifier (s);
    OIIO_CHECK_ASSERT (ss == "fl" && s == "$orp 14");
    s = "fl$orp 14";  ss = parse_identifier (s, "$:", true);
    OIIO_CHECK_ASSERT (ss == "fl$orp" && s == " 14");

    s = "foo;bar blow"; ss = parse_until (s, ";");
    OIIO_CHECK_ASSERT (ss == "foo" && s == ";bar blow");
    s = "foo;bar blow"; ss = parse_until (s, "\t ");
    OIIO_CHECK_ASSERT (ss == "foo;bar" && s == " blow");
    s = "foo;bar blow"; ss = parse_until (s, "/");
    OIIO_CHECK_ASSERT (ss == "foo;bar blow" && s == "");
}




void
test_float_formatting ()
{
    // For every possible float value, test that printf("%.9g"), which
    // we are sure preserves full precision as text, exactly matches
    // Strutil::format("%.9g") and also matches stream output with
    // precision(9).  VERY EXPENSIVE!  Takes tens of minutes to run.
    // Don't do this unless you really need to test it.
    for (unsigned long long i = 0;  i <= (unsigned long long)0xffffffff;  ++i) {
        unsigned int i32 = (unsigned int)i;
        float *f = (float *)&i32;
        std::ostringstream sstream;
        sstream.precision (9);
        sstream << *f;
        char buffer[64];
        sprintf (buffer, "%.9g", *f);
        std::string tiny = Strutil::format ("%.9g", *f);
        if (sstream.str() != tiny || tiny != buffer)
            printf ("%x  stream '%s'  printf '%s'  Strutil::format '%s'\n",
                    i32, sstream.str().c_str(), buffer, tiny.c_str());
        if ((i32 & 0xfffffff) == 0xfffffff) {
            printf ("%x\n", i32);
            fflush (stdout);
        }
    }
}



int
main (int argc, char *argv[])
{
    test_format ();
    test_memformat ();
    test_timeintervalformat ();
    test_get_rest_arguments ();
    test_escape_sequences ();
    test_wordwrap ();
    test_comparisons ();
    test_case ();
    test_strip ();
    test_split ();
    test_join ();
    test_conversion ();
    test_extract ();
    test_safe_strcpy ();
    test_string_view ();
    test_parse ();
    // test_float_formatting ();

    return unit_test_failures;
}