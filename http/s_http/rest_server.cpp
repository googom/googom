#include "rest_server.h"

rest_server::rest_server(message_store& store)
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
    _server.set_routes([this](httpd::routes& r) {
        r.put(httpd::GET, "/message/{id}", new httpd::function_handler([this](std::unique_ptr<http::request> req, std::unique_ptr<http::reply> rep) {
            auto id = std::stoi(req->param.at("id"));
            return _store.get_message(id).then([rep = std::move(rep)](std::optional<std::string> msg) mutable -> std::unique_ptr<http::reply> {
                if (msg) {
                    rep->write_body("json", seastar::json::stream_object(std::move(*msg)));
                } else {
                    rep->write_body("json", seastar::json::stream_object("Message not found"));
                }
                rep->done("json");
                return std::move(rep);
            });
        }, "json"));

        r.put(httpd::POST, "/message", new httpd::function_handler([this](std::unique_ptr<http::request> req, std::unique_ptr<http::reply> rep) {
            auto content = req->content;
            int id = _store.store_message(content);
            rep->write_body("json", seastar::json::stream_object(std::to_string(id)));
            rep->done("json");
            return seastar::make_ready_future<std::unique_ptr<http::reply>>(std::move(rep));
        }, "json"));

        r.put(httpd::DELETE, "/message/{id}", new httpd::function_handler([this](std::unique_ptr<http::request> req, std::unique_ptr<http::reply> rep) {
            auto id = std::stoi(req->param.at("id"));
            if (_store.delete_message(id)) {
                rep->write_body("json", seastar::json::stream_object("Message deleted"));
            } else {
                rep->write_body("json", seastar::json::stream_object("Message not found or already deleted"));
            }
            rep->done("json");
            return seastar::make_ready_future<std::unique_ptr<http::reply>>(std::move(rep));
        }, "json"));
    });
}
