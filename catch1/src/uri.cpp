#include "catch.hpp"
#include <yat/utils/URI.h>


const yat::String uri_01_str = "foo://example.com:8042/over/there?name=ferret#nose";
const yat::String uri_02_str = "file:/nfs/ruche/project/file.nxs?entry=scan,uig=12,gid=125,dest_path=/nfs/tempdata/";
const yat::String uri_03_str = "http://[fe80:3::1ff:fe23:4567:890a]:443/over/there?name=ferret#nose";
const yat::String uri_04_str = "tango://acs.esrf.fr:10000/my/funny/dev/state.change";
const yat::String uri_05_str = "tel:+145349836";
const yat::String uri_06_str = "mailto:paf092@gmail.com";
const yat::String uri_07_str = "http://192.168.0.1:80/blabla_(blu)";
const yat::String uri_08_str = "https://wandbox.org/";
const yat::String uri_09_str = "news:comp.infosystems.www.servers.unix";
const yat::String uri_10_str = "https://john.doe@www.example.com:123/forum/questions/?tag=networking&order=newest#top";

TEST_CASE("uri_valid", "[URI]")
{
  yat::URI uri;
  CHECK_NOTHROW(uri.set(uri_01_str));
  CHECK_NOTHROW(uri.set(uri_02_str));
  CHECK_NOTHROW(uri.set(uri_03_str));
  CHECK_NOTHROW(uri.set(uri_04_str));
  CHECK_NOTHROW(uri.set(uri_05_str));
  CHECK_NOTHROW(uri.set(uri_06_str));
  CHECK_NOTHROW(uri.set(uri_07_str));
  CHECK_NOTHROW(uri.set(uri_08_str));
  CHECK_NOTHROW(uri.set(uri_09_str));
  CHECK_NOTHROW(uri.set(uri_10_str));
}

TEST_CASE("uri_parse", "[URI]")
{
  yat::URI uri;

  uri.set(uri_01_str);
  CHECK(uri.get(yat::URI::SCHEME) == "foo");
  CHECK(uri.get(yat::URI::AUTHORITY) == "example.com:8042");
  CHECK(uri.get(yat::URI::USERINFO) == "");
  CHECK(uri.get(yat::URI::HOST) == "example.com");
  CHECK(uri.get(yat::URI::PORT) == "8042");
  CHECK(uri.get(yat::URI::PATH) == "/over/there");
  CHECK(uri.get(yat::URI::QUERY) == "name=ferret");
  CHECK(uri.get(yat::URI::FRAGMENT) == "nose");

  uri.set(uri_02_str);
  CHECK(uri.get(yat::URI::SCHEME) == "file");
  CHECK(uri.get(yat::URI::AUTHORITY) == "");
  CHECK(uri.get(yat::URI::USERINFO) == "");
  CHECK(uri.get(yat::URI::HOST) == "");
  CHECK(uri.get(yat::URI::PORT) == "");
  CHECK(uri.get(yat::URI::PATH) == "/nfs/ruche/project/file.nxs");
  CHECK(uri.get(yat::URI::QUERY) == "entry=scan,uig=12,gid=125,dest_path=/nfs/tempdata/");
  CHECK(uri.get(yat::URI::FRAGMENT) == "");

  uri.set(uri_03_str);
  CHECK(uri.get(yat::URI::SCHEME) == "http");
  CHECK(uri.get(yat::URI::AUTHORITY) == "[fe80:3::1ff:fe23:4567:890a]:443");
  CHECK(uri.get(yat::URI::USERINFO) == "");
  CHECK(uri.get(yat::URI::HOST) == "[fe80:3::1ff:fe23:4567:890a]");
  CHECK(uri.get(yat::URI::PORT) == "443");
  CHECK(uri.get(yat::URI::PATH) == "/over/there");
  CHECK(uri.get(yat::URI::QUERY) == "name=ferret");
  CHECK(uri.get(yat::URI::FRAGMENT) == "nose");

  uri.set(uri_04_str);
  CHECK(uri.get(yat::URI::SCHEME) == "tango");
  CHECK(uri.get(yat::URI::AUTHORITY) == "acs.esrf.fr:10000");
  CHECK(uri.get(yat::URI::USERINFO) == "");
  CHECK(uri.get(yat::URI::HOST) == "acs.esrf.fr");
  CHECK(uri.get(yat::URI::PORT) == "10000");
  CHECK(uri.get(yat::URI::PATH) == "/my/funny/dev/state.change");
  CHECK(uri.get(yat::URI::QUERY) == "");
  CHECK(uri.get(yat::URI::FRAGMENT) == "");

  uri.set(uri_05_str);
  CHECK(uri.get(yat::URI::SCHEME) == "tel");
  CHECK(uri.get(yat::URI::AUTHORITY) == "");
  CHECK(uri.get(yat::URI::USERINFO) == "");
  CHECK(uri.get(yat::URI::HOST) == "");
  CHECK(uri.get(yat::URI::PORT) == "");
  CHECK(uri.get(yat::URI::PATH) == "+145349836");
  CHECK(uri.get(yat::URI::QUERY) == "");
  CHECK(uri.get(yat::URI::FRAGMENT) == "");

  uri.set(uri_06_str);
  CHECK(uri.get(yat::URI::SCHEME) == "mailto");
  CHECK(uri.get(yat::URI::AUTHORITY) == "");
  CHECK(uri.get(yat::URI::USERINFO) == "");
  CHECK(uri.get(yat::URI::HOST) == "");
  CHECK(uri.get(yat::URI::PORT) == "");
  CHECK(uri.get(yat::URI::PATH) == "paf092@gmail.com");
  CHECK(uri.get(yat::URI::QUERY) == "");
  CHECK(uri.get(yat::URI::FRAGMENT) == "");

  uri.set(uri_07_str);
  CHECK(uri.get(yat::URI::SCHEME) == "http");
  CHECK(uri.get(yat::URI::AUTHORITY) == "192.168.0.1:80");
  CHECK(uri.get(yat::URI::USERINFO) == "");
  CHECK(uri.get(yat::URI::HOST) == "192.168.0.1");
  CHECK(uri.get(yat::URI::PORT) == "80");
  CHECK(uri.get(yat::URI::PATH) == "/blabla_(blu)");
  CHECK(uri.get(yat::URI::QUERY) == "");
  CHECK(uri.get(yat::URI::FRAGMENT) == "");

  uri.set(uri_08_str);
  CHECK(uri.get(yat::URI::SCHEME) == "https");
  CHECK(uri.get(yat::URI::AUTHORITY) == "wandbox.org");
  CHECK(uri.get(yat::URI::USERINFO) == "");
  CHECK(uri.get(yat::URI::HOST) == "wandbox.org");
  CHECK(uri.get(yat::URI::PORT) == "");
  CHECK(uri.get(yat::URI::PATH) == "/");
  CHECK(uri.get(yat::URI::QUERY) == "");
  CHECK(uri.get(yat::URI::FRAGMENT) == "");

  uri.set(uri_09_str);
  CHECK(uri.get(yat::URI::SCHEME) == "news");
  CHECK(uri.get(yat::URI::AUTHORITY) == "");
  CHECK(uri.get(yat::URI::USERINFO) == "");
  CHECK(uri.get(yat::URI::HOST) == "");
  CHECK(uri.get(yat::URI::PORT) == "");
  CHECK(uri.get(yat::URI::PATH) == "comp.infosystems.www.servers.unix");
  CHECK(uri.get(yat::URI::QUERY) == "");
  CHECK(uri.get(yat::URI::FRAGMENT) == "");

  uri.set(uri_10_str);
  CHECK(uri.get(yat::URI::SCHEME) == "https");
  CHECK(uri.get(yat::URI::AUTHORITY) == "john.doe@www.example.com:123");
  CHECK(uri.get(yat::URI::USERINFO) == "john.doe");
  CHECK(uri.get(yat::URI::HOST) == "www.example.com");
  CHECK(uri.get(yat::URI::PORT) == "123");
  CHECK(uri.get(yat::URI::PATH) == "/forum/questions/");
  CHECK(uri.get(yat::URI::QUERY) == "tag=networking&order=newest");
  CHECK(uri.get(yat::URI::FRAGMENT) == "top");
}
