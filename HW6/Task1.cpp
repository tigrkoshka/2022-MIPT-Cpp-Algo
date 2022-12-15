#include <coroutine>
#include <iostream>

struct promise;
struct coroutine : std::coroutine_handle<promise> {
    using promise_type = struct promise;
};

struct promise {
    coroutine get_return_object() { return {coroutine::from_promise(*this)}; }

    std::suspend_never initial_suspend() noexcept { return {}; }

    std::suspend_always final_suspend() noexcept { return {}; }

    void return_void() {}

    void unhandled_exception() {}
};

int main() {
    coroutine h = []() -> coroutine {
        std::cout << "Hello, world!" << std::endl;
        co_await std::suspend_always{};
        std::cout << "I am coroutine!" << std::endl;
        co_await std::suspend_always{};
        std::cout << "Nice to meet you!" << std::endl;
        co_await std::suspend_always{};
        std::cout << "Goodbye!" << std::endl;
        co_return;
    }();

    h.resume();
    h.resume();
    h.resume();
    h.destroy();
}