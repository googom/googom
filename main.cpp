

using namespace seastar;

namespace {
    seastar::logger lg("main");
}

int main(int argc, char **argv) {
    seastar::app_template app;
    namespace po = boost::program_options;

    return app.run(argc, argv, [&]() -> seastar::future<int> {
        return 0;
    });
}