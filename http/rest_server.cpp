#include "rest_server.h"

rest_server::rest_server(message_store &store)
        : _store(store) {}

void rest_server::start(uint16_t port) {
    _server.start().then([this, port] {
        setup_routes();
        _server.listen(seastar::socket_address(seastar::ipv4_addr{port}));
        std::cout << "REST server listening on port " << port << std::endl;
    }).or_terminate();
}

seastar::future<> rest_server::stop() {
    return _server.stop();
}


void rest_server::setup_routes() {
    using namespace seastar;
    _server.set_routes([this](httpd::routes &r) {
        r.add(httpd::GET, httpd::url(
                "/message").remainder("id"), new httpd::function_handler(
                [this](std::unique_ptr<http::request> req, std::unique_ptr<http::reply> rep) {
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
                  int id = _store.store_message(content);
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
    });
}
