#include <functional>
#include <iostream>
#include <unordered_map>

struct Event {
    virtual ~Event() {}
    virtual int getId() const = 0;
};

struct Foo : Event {
    int getId() const override { return 1; }
    std::string name() const { return "Bob"; };
};

struct Bar : Event {
    int getId() const override { return 2; };
    std::string address() const { return "43"; };
};

using EventCall = std::function<void(Event const &)> ;

template<typename E, typename S>
EventCall eventCallback(S & service) {
    return [service] (Event const & event) mutable {service.accept(static_cast<E const &>(event));};
}

struct Service {
    void accept(Foo const & aFoo) {
        std::cout << "You gave me a Foo called " << aFoo.name() << '\n';
    }

    void accept(Bar const & aBar) {
        std::cout << "You gave me a Bar from " << aBar.address() << '\n';
    }
};

struct Dispatcher {
    Dispatcher(std::unordered_map<int, EventCall> & callbacks) : callbacks_(callbacks) {};
    std::unordered_map<int, EventCall> & callbacks_;

    void accept(Event const & anEvent) {
        auto callback = callbacks_.find(anEvent.getId());
        if (callback != callbacks_.end()) {
            callback->second(anEvent);
        }
        else {
            std::cout << "Unknown event: " << anEvent.getId() << '\n';
        }
    }
};

int main(int, char**) {
    std::cout << "Hello, world!\n";
    Foo foo;
    Bar bar;
    std::cout << "Foo id: " << foo.getId() << " Bar id: " << bar.getId() << '\n';
    Service theService;
    theService.accept(foo);
    theService.accept(bar);
    std::unordered_map<int, EventCall> callbacks;
    callbacks[1] = eventCallback<Foo>(theService);
    callbacks[2] = eventCallback<Bar>(theService);
    callbacks[1](foo);
    callbacks[2](bar);
    eventCallback<Foo>(theService)(foo);
    Dispatcher dispatcher(callbacks);
    dispatcher.accept(bar);
    dispatcher.accept(foo);
}
