#include "rest_server.h"
#include "../../topics/topic_private/topic_user/topic_private_user_definition.h"
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>

std::string RestServer::decode64(const std::string &val) {
    using namespace boost::archive::iterators;
    using It = transform_width<binary_from_base64<std::string::const_iterator>, 8, 6>;
    return boost::algorithm::trim_right_copy_if(std::string(It(std::begin(val)), It(std::end(val))), [](char c) {
        return c == '\0';
    });
}

std::string RestServer::encode64(const std::string &val) {
    using namespace boost::archive::iterators;
    using It = base64_from_binary<transform_width<std::string::const_iterator, 6, 8>>;
    auto tmp = std::string(It(std::begin(val)), It(std::end(val)));
    return tmp.append((3 - val.size() % 3) % 3, '=');
}

RestServer::RestServer(){}

void RestServer::start(uint16_t port) {
    _server.start().then([this, port] {
        setup_routes();
        _server.listen(seastar::socket_address(seastar::ipv4_addr{port}));
        std::cout << "REST server listening on port " << port << std::endl;
    }).or_terminate();
}

seastar::future<> RestServer::stop() {
    return _server.stop();
}

/*
 * Required endpoints
 * message -> send, consume, delete - (Web socket?)
 * user -> create, disable, delete, list
 * json configuration -> add, update, list
 */
void RestServer::setup_routes() {
    using namespace seastar;
    _server.set_routes([this](httpd::routes &r) {
        r.add(httpd::GET, httpd::url(
                "/message").remainder("id"), new httpd::function_handler(
                [this](std::unique_ptr<http::request> req, std::unique_ptr<http::reply> rep) {
                    auto auth_header = req->get_header("Authorization");

                    std::string encoded = auth_header.substr(6);
                    std::string credentials = decode64(encoded);

                    auto separator_pos = credentials.find(':');
                    if (separator_pos == std::string::npos) {
                        // Modify the response status to indicate a bad request
                        rep->_status = seastar::http::reply::status_type::bad_request;

                        // Write a detailed error message in JSON format
                        rep->write_body("json", seastar::json::stream_object("Invalid credentials format"));

                        // Finalize the response
                        rep->done("json");

                        // Return a future containing the modified reply
                        return seastar::make_ready_future<std::unique_ptr<seastar::http::reply>>(std::move(rep));
                    }

                    auto username = credentials.substr(0, separator_pos);
                    auto password = credentials.substr(separator_pos + 1);

                    //TODO check auth

                    try {
                        auto id_str = req->param.at("id");
                        //THIS WAS NEEDED TO CLEAR / char
                        id_str = id_str.substr(1);
                        int id = std::stoi(id_str);
                        return _store.get_message(id).then(
                                [rep = std::move(rep)](
                                        std::optional<std::string> msg) mutable -> std::unique_ptr<http::reply> {
                                    if (msg) {
                                        rep->write_body("json", seastar::json::stream_object(std::move(*msg)));
                                    } else {
                                        rep->write_body("json", seastar::json::stream_object("Message not found"));
                                    }
                                    rep->done("json");
                                    return std::move(rep);
                                });
                    } catch (const std::exception &e) {
                        rep->_status = http::reply::status_type::bad_request;
                        rep->write_body("json", seastar::json::stream_object("Invalid ID format"));
                        rep->done("json");
                        return seastar::make_ready_future<std::unique_ptr<http::reply>>(std::move(rep));
                    }
                }, "json"));

        r.add(httpd::POST, httpd::url("/message"),
              new httpd::function_handler([this](std::unique_ptr<http::request> req, std::unique_ptr<http::reply> rep) {
                  auto content = req->content;
                  int id = _store.store_message(content,"test");
                  rep->write_body("json", seastar::json::stream_object(std::to_string(id)));
                  rep->done("json");
                  return seastar::make_ready_future<std::unique_ptr<http::reply>>(std::move(rep));
              }, "json"));

        r.add(httpd::DELETE, httpd::url(
                      "/message").remainder("id"),
              new httpd::function_handler([this](std::unique_ptr<http::request> req, std::unique_ptr<http::reply> rep) {
                  try {
                      auto id_str = req->param.at("id");
                      //THIS WAS NEEDED TO CLEAR / char
                      id_str = id_str.substr(1);
                      int id = std::stoi(id_str);
                      if (_store.delete_message(id)) {
                          rep->write_body("json", seastar::json::stream_object("Message deleted"));
                      } else {
                          rep->write_body("json", seastar::json::stream_object("Message not found or already deleted"));
                      }
                      rep->done("json");
                      return seastar::make_ready_future<std::unique_ptr<http::reply>>(std::move(rep));
                  } catch (const std::exception &e) {
                      rep->_status = http::reply::status_type::bad_request;
                      rep->write_body("json", seastar::json::stream_object("Invalid ID format"));
                      rep->done("json");
                      return seastar::make_ready_future<std::unique_ptr<http::reply>>(std::move(rep));
                  }
              }, "json"));

        //USER
        r.add(httpd::POST, httpd::url("/user"),
              new httpd::function_handler([this](std::unique_ptr<http::request> req, std::unique_ptr<http::reply> rep) {
                  try {
                      // Read and parse JSON
                      std::stringstream json_stream(req->content);
                      boost::property_tree::ptree pt;
                      boost::property_tree::read_json(json_stream, pt);

                      std::string username = pt.get<std::string>("username");
                      std::string password = pt.get<std::string>("password");
                      std::string email = pt.get<std::string>("email");

                      auto user= TopicPrivateUserDefinition::getInstance()->insert(username,password,email);

                      rep->write_body("json", user.to_json());
                  } catch (const std::exception& e) {
                      // Handle JSON parsing errors
                      boost::property_tree::ptree error;
                      error.put("error", e.what());
                      std::ostringstream error_stream;
                      boost::property_tree::write_json(error_stream, error);

                      rep->set_status(http::reply::status_type::bad_request);
                      rep->write_body("json", error_stream.str());
                  }

                  rep->done("json");
                  return seastar::make_ready_future<std::unique_ptr<http::reply>>(std::move(rep));
              }, "json"));

        r.add(httpd::GET, httpd::url("/users"),
              new httpd::function_handler([this](std::unique_ptr<http::request> req, std::unique_ptr<http::reply> rep) {
                  try {
                      // Fetch all users
                      const std::vector<TopicPrivateUserStructure>& users = TopicPrivateUserDefinition::getInstance()-> getAllUsers();

                      // Create JSON array
                      boost::property_tree::ptree users_array;
                      for (const auto& user : users) {
                          boost::property_tree::ptree user_ptree = user.to_ptree();
                          users_array.push_back(std::make_pair("", user_ptree));
                      }

                      // Create response JSON object
                      boost::property_tree::ptree response;
                      response.add_child("users", users_array);

                      std::ostringstream response_stream;
                      boost::property_tree::write_json(response_stream, response);

                      rep->write_body("json", response_stream.str());
                  } catch (const std::exception& e) {
                      // Handle errors
                      boost::property_tree::ptree error;
                      error.put("error", e.what());
                      std::ostringstream error_stream;
                      boost::property_tree::write_json(error_stream, error);

                      rep->set_status(http::reply::status_type::internal_server_error);
                      rep->write_body("json", error_stream.str());
                  }

                  rep->done("json");
                  return seastar::make_ready_future<std::unique_ptr<http::reply>>(std::move(rep));
              }, "json"));

    });
}